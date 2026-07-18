#define MiGetPteAddress(_PteBase, _VirtualAddress) (ULONG64*)(((_VirtualAddress >> 9) & 0x7FFFFFFFF8) + _PteBase);

KIRQL RaiseIrql(KIRQL NewIrql) {
    KIRQL result = (KIRQL)__readcr8();
    __writecr8(NewIrql);
    return result;
}

VOID LowerIrql(KIRQL NewIrql) {
    __writecr8(NewIrql);
}

INT InvlpgGetThreadUniqueIndexFromMap() {
    INT threadIndex = -1;

    for (INT i = 0; i < 512; ++i) {
        if (!_InterlockedCompareExchange8(&pAPI_information->Invlpg.threadUseMap[i], 1, 0)) {
            threadIndex = i;
            break;
        }
    }

    return threadIndex;
}

__forceinline VOID InvlpgCopyPhysicalAddress(ULONG64 page, ULONG64* page_pteaddress, ULONG64 address, PVOID buffer, ULONG64 size, ULONG64 write) {
    *page_pteaddress = ((address >> 12) << 12) | (write == 2 ? 0b111111111 : 0b111100011) & (~(1 << 9));

    __invlpg((VOID*)page);
    UCHAR* target = (UCHAR*)(page + (address & 0xFFF));

    if (!write) {
        __movsb((UCHAR*)buffer, target, size);
    }
    else {
        __movsb(target, (UCHAR*)buffer, size);
    }
}

__forceinline ULONG64 InvlpgTranslateLinearAddress(ULONG64 page, ULONG64* page_pteaddress, ULONG64 currentcr3, ULONG64 virtualaddress, ULONG64* pxeaddress) {
    ULONG64 pml4 = 0;
    ULONG64 pml4_address = (currentcr3 & 0xFFFFFFFFFF000) + 8 * ((virtualaddress >> 39) & 0x1FF);

    InvlpgCopyPhysicalAddress(page, page_pteaddress, pml4_address, &pml4, 8, 0);

    if (!(pml4 & 1)) return 0;
    if (((pml4 >> 12) & 0xFFFFFFFFFF) == ((currentcr3 >> 12) & 0xFFFFFFFFFF)) return 0;

    ULONG64 pdpte = 0;
    ULONG64 pdpte_address = (pml4 & 0xFFFFFFFFFF000) + 8 * ((virtualaddress >> 30) & 0x1FF);

    InvlpgCopyPhysicalAddress(page, page_pteaddress, pdpte_address, &pdpte, 8, 0);

    if (!(pdpte & 1)) return 0;
    if (((pdpte >> 7) & 1)) {
        if (!(pdpte & 0b10)) *pxeaddress = pdpte_address;
        return (pdpte & 0xFFFFFC0000000) + (virtualaddress & 0x3FFFFFFF);
    }

    ULONG64 pde = 0;
    ULONG64 pde_address = (pdpte & 0xFFFFFFFFFF000) + 8 * ((virtualaddress >> 21) & 0x1FF);

    InvlpgCopyPhysicalAddress(page, page_pteaddress, pde_address, &pde, 8, 0);

    if (!(pde & 1)) return 0;
    if (((pde >> 7) & 1)) {
        if (!(pde & 0b10)) *pxeaddress = pde_address;
        return (pde & 0xFFFFFFFE00000) + (virtualaddress & 0x1FFFFF);
    }

    ULONG64 pte = 0;
    ULONG64 pte_address = (pde & 0xFFFFFFFFFF000) + 8 * ((virtualaddress >> 12) & 0x1FF);

    InvlpgCopyPhysicalAddress(page, page_pteaddress, pte_address, &pte, 8, 0);

    if (!(pte & 1)) return 0;
    if (!(pte & 0b10)) *pxeaddress = pte_address;
    if (pte & 0x200000000000) return (pte & 0xFFFFFFFF000) + (virtualaddress & 0xFFF);

    return (pte & 0xFFFFFFFFFF000) + (virtualaddress & 0xFFF);
}

__declspec(noinline) NTSTATUS ManipulateMemory(PDriverControl pDriver) {
    if (!pDriver->Address || !pDriver->Buffer || !pDriver->Size || !pAPI_information->CR3.CurrentCr3) return STATUS_INVALID_PARAMETER;

    if (!pAPI_information->Invlpg.Ptes) pAPI_information->Invlpg.Ptes = (ULONG64)((decltype(MmAllocateMappingAddress)*)pAPI_information->ImportTable.pMmAllocateMappingAddress)(512 * PAGE_SIZE, 'MEMR');
    if (!pAPI_information->Invlpg.MapMemory) pAPI_information->Invlpg.MapMemory = (ULONG64)((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPool, 512 * PAGE_SIZE, 'MEMR');
    if (!pAPI_information->Invlpg.PteBase) pAPI_information->Invlpg.PteBase = *(ULONG64*)((ULONG64)((decltype(MmGetVirtualForPhysical)*)pAPI_information->ImportTable.pMmGetVirtualForPhysical) + 0x22);

    INT threadIndex = InvlpgGetThreadUniqueIndexFromMap();
    if (threadIndex == -1)return STATUS_INSUFFICIENT_RESOURCES;

    ULONG64 NonPagedBuffer = pAPI_information->Invlpg.MapMemory + threadIndex * PAGE_SIZE;
    if (pDriver->Type)__movsb((PUCHAR)NonPagedBuffer, (PUCHAR)pDriver->Buffer, pDriver->Size);

    ULONG64 PageAddress = pAPI_information->Invlpg.Ptes + threadIndex * PAGE_SIZE;
    ULONG64* PagePTE = MiGetPteAddress(pAPI_information->Invlpg.PteBase, PageAddress);
    ULONG64 OriginalPTE = *PagePTE;

    ULONG64 RemainSize = pDriver->Size;
    ULONG64 PageOffset = 0;

    _disable();
    KIRQL oldIrql = RaiseIrql(DISPATCH_LEVEL);

    while (RemainSize > 0) {
        ULONG64 PxeAddress = 0;
        ULONG64 PhysicalAddress = InvlpgTranslateLinearAddress(PageAddress, PagePTE, pAPI_information->CR3.CurrentCr3, (ULONG64)pDriver->Address + PageOffset, &PxeAddress);
        if (!PhysicalAddress)break;
        if (PxeAddress && (pDriver->Type == 1))break;

        ULONG64 BytesToCopy = min(PAGE_SIZE - (PhysicalAddress & 0xFFF), RemainSize);
        if (!BytesToCopy)break;

        InvlpgCopyPhysicalAddress(PageAddress, PagePTE, PhysicalAddress, (PVOID)(NonPagedBuffer + PageOffset), BytesToCopy, pDriver->Type);

        RemainSize -= BytesToCopy;
        PageOffset += BytesToCopy;
    }

    *PagePTE = OriginalPTE;
    __invlpg((VOID*)PageAddress);
    LowerIrql(oldIrql);
    _enable();

    if (!pDriver->Type) __movsb((PUCHAR)pDriver->Buffer, (PUCHAR)NonPagedBuffer, pDriver->Size - RemainSize);

    __stosb((PUCHAR)NonPagedBuffer, 0, pDriver->Size);
    _InterlockedExchange8(&pAPI_information->Invlpg.threadUseMap[threadIndex], 0);
    if (RemainSize > 0)return STATUS_PARTIAL_COPY;
    return STATUS_SUCCESS;
}

__declspec(noinline) NTSTATUS SetProcess(ULONG64 ProcessId, ULONG64 DecryptCR3) {
    NTSTATUS Status = ((decltype(PsLookupProcessByProcessId)*)pAPI_information->ImportTable.pPsLookupProcessByProcessId)((HANDLE)ProcessId, &pAPI_information->gProcess.Process);
    if (!NT_SUCCESS(Status)) return Status;

    pAPI_information->CR3.CurrentCr3 = *(PULONG_PTR)((PUCHAR)pAPI_information->gProcess.Process + 0x28);

    if (!pAPI_information->Invlpg.Ptes)pAPI_information->Invlpg.Ptes = (ULONG64)((decltype(MmAllocateMappingAddress)*)pAPI_information->ImportTable.pMmAllocateMappingAddress)(512 * PAGE_SIZE, 1);
    if (!pAPI_information->Invlpg.MapMemory) pAPI_information->Invlpg.MapMemory = (ULONG64)((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPoolNx, 512 * PAGE_SIZE, 1);
    if (!pAPI_information->Invlpg.PteBase) pAPI_information->Invlpg.PteBase = *(ULONG64*)((ULONG64)((decltype(MmGetVirtualForPhysical)*)pAPI_information->ImportTable.pMmGetVirtualForPhysical) + 0x22);

    PVOID Wow64Info = ((decltype(PsGetProcessWow64Process)*)pAPI_information->ImportTable.pPsGetProcessWow64Process)(pAPI_information->gProcess.Process);
    if (Wow64Info != NULL) DecryptCR3 = 0;
    if (DecryptCR3) {
        void* process_base = ((decltype(PsGetProcessSectionBaseAddress)*)pAPI_information->ImportTable.pPsGetProcessSectionBaseAddress)(pAPI_information->gProcess.Process);
        if (process_base) {
            if (!pAPI_information->CR3.MmPfnDataBase) {
                Status = init_mmpfn_database();
                if (!NT_SUCCESS(Status)) {
                    ((decltype(ObfDereferenceObject)*)pAPI_information->ImportTable.pObfDereferenceObject)(pAPI_information->gProcess.Process);
                    return Status;
                }
            }
            uintptr_t real_cr3 = dirbase_from_base_address(process_base);
            if (real_cr3) pAPI_information->CR3.CurrentCr3 = real_cr3;
        }
    }

    ((decltype(ObfDereferenceObject)*)pAPI_information->ImportTable.pObfDereferenceObject)(pAPI_information->gProcess.Process);
    return STATUS_SUCCESS;
}