__declspec(noinline) NTSTATUS SearchMemoryInProcessEx(ULONG64 StartAddress, ULONG64 Pattern, ULONG64 PatternSize, ULONG64 Attribute, ULONG64 ResultAddress) {
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    HANDLE hTargetProcess = NULL;
    PBYTE pMemBuffer = NULL;

    if (ResultAddress == NULL) return STATUS_INVALID_PARAMETER;
    if (PatternSize == 0 || Pattern == 0) return STATUS_INVALID_PARAMETER;

    const auto pMmIsAddressValid = (decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid;
    if (!pMmIsAddressValid((PVOID)Pattern)) return STATUS_ACCESS_VIOLATION;

    const auto pObOpenObjectByPointer = (decltype(ObOpenObjectByPointer)*)pAPI_information->ImportTable.pObOpenObjectByPointer;
    const auto pZwQueryVirtualMemory = (decltype(ZwQueryVirtualMemory)*)pAPI_information->ImportTable.pZwQueryVirtualMemory;
    const auto pExAllocatePoolWithTag = (decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag;
    const auto pExFreePoolWithTag = (decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag;
    const auto pMmCopyVirtualMemory = (decltype(MmCopyVirtualMemory)*)pAPI_information->ImportTable.pMmCopyVirtualMemory;
    const auto pIoGetCurrentProcess = (decltype(IoGetCurrentProcess)*)pAPI_information->ImportTable.pIoGetCurrentProcess;
    const auto pObCloseHandle = (decltype(ObCloseHandle)*)pAPI_information->ImportTable.pObCloseHandle;

    for (ULONG64 i = 0; i < PatternSize; i++) {
        if (!pMmIsAddressValid((PVOID)(Pattern + i))) return STATUS_ACCESS_VIOLATION;
    }

    Status = pObOpenObjectByPointer(pAPI_information->gProcess.Process, OBJ_KERNEL_HANDLE, NULL, PROCESS_ALL_ACCESS, NULL, KernelMode, &hTargetProcess);
    if (!NT_SUCCESS(Status)) return Status;

    PBYTE CurrentSearchAddress = (PBYTE)StartAddress;
    PBYTE FoundAddress = NULL;
    MEMORY_BASIC_INFORMATION MemInfo = { 0 };
    BOOLEAN bContinue = TRUE;

    const ULONG64 ReadableMask = PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_WRITECOPY;
    const ULONG64 InaccessibleMask = PAGE_NOACCESS | PAGE_GUARD;
    const PBYTE USER_SPACE_LIMIT = (PBYTE)0x7FFFFFFFFFFF;

    PUCHAR pPatternBytes = (PUCHAR)Pattern;
    BOOLEAN bHasWildcards = FALSE;
    UCHAR FirstPatternByte = 0;
    BOOLEAN bFirstByteIsWildcard = FALSE;

    if (PatternSize > 0) {
        FirstPatternByte = pPatternBytes[0];
        bFirstByteIsWildcard = (FirstPatternByte == 0xCC);

        for (SIZE_T i = 0; i < PatternSize && !bHasWildcards; i++) {
            if (pPatternBytes[i] == 0xCC) bHasWildcards = TRUE;
        }
    }

    while (bContinue && CurrentSearchAddress < USER_SPACE_LIMIT) {
        ZeroMemory_CPU(&MemInfo, sizeof(MemInfo));

        Status = pZwQueryVirtualMemory(hTargetProcess, CurrentSearchAddress, MemoryBasicInformation, &MemInfo, sizeof(MemInfo), NULL);
        if (!NT_SUCCESS(Status) || MemInfo.RegionSize == 0) break;

        PBYTE RegionStart = (PBYTE)MemInfo.BaseAddress;
        PBYTE RegionEnd = RegionStart + MemInfo.RegionSize;

        if (MemInfo.State != MEM_COMMIT || (MemInfo.Protect & InaccessibleMask) || !(MemInfo.Protect & ReadableMask)) {
            CurrentSearchAddress = RegionEnd;
            continue;
        }

        if (Attribute != 0 && (MemInfo.Protect & Attribute) == 0) {
            CurrentSearchAddress = RegionEnd;
            continue;
        }

        PBYTE SearchStart = (CurrentSearchAddress > RegionStart) ? CurrentSearchAddress : RegionStart;
        if (SearchStart >= RegionEnd) {
            CurrentSearchAddress = RegionEnd;
            continue;
        }

        SIZE_T RegionSize = (SIZE_T)(RegionEnd - SearchStart);
        if (RegionSize < PatternSize) {
            CurrentSearchAddress = RegionEnd;
            continue;
        }

        const SIZE_T BLOCK_SIZE = 0x10000;
        SIZE_T BytesRemaining = RegionSize;
        PBYTE BlockStart = SearchStart;

        while (BytesRemaining >= PatternSize && bContinue) {
            SIZE_T BlockReadSize = (BytesRemaining < BLOCK_SIZE) ? BytesRemaining : BLOCK_SIZE;
            if (BlockReadSize < PatternSize) break;

            pMemBuffer = (PBYTE)pExAllocatePoolWithTag(NonPagedPoolNx, BlockReadSize, pAPI_information->g_TAG.SGOC);
            if (!pMemBuffer) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            SIZE_T BytesRead = 0;
            Status = pMmCopyVirtualMemory(pAPI_information->gProcess.Process, BlockStart, pIoGetCurrentProcess(), pMemBuffer, BlockReadSize, KernelMode, &BytesRead);
            if (NT_SUCCESS(Status) && BytesRead >= PatternSize) {
                SIZE_T SearchLimit = BytesRead - PatternSize;

                for (SIZE_T i = 0; i <= SearchLimit && bContinue; i++) {
                    UCHAR TargetByte = pMemBuffer[i];
                    if (!bFirstByteIsWildcard && TargetByte != FirstPatternByte) continue;

                    BOOLEAN bMatch = TRUE;
                    SIZE_T j = 0;

                    if (bHasWildcards) {
                        for (; j < PatternSize && bMatch; j++) {
                            UCHAR PatternByte = pPatternBytes[j];
                            if (PatternByte != 0xCC && pMemBuffer[i + j] != PatternByte) {
                                bMatch = FALSE;
                                break;
                            }
                        }
                    }
                    else {
                        for (; j < PatternSize && bMatch; j++) {
                            if (pMemBuffer[i + j] != pPatternBytes[j]) {
                                bMatch = FALSE;
                                break;
                            }
                        }
                    }

                    if (bMatch) {
                        FoundAddress = BlockStart + i;
                        if (FoundAddress != (PBYTE)StartAddress) {
                            bContinue = FALSE;
                            break;
                        }
                    }
                }
            }

            pExFreePoolWithTag(pMemBuffer, pAPI_information->g_TAG.SGOC);
            pMemBuffer = NULL;

            if (!NT_SUCCESS(Status)) break;

            SIZE_T Advance = BlockReadSize - PatternSize + 1;
            if (Advance == 0) Advance = 1;

            BlockStart += Advance;
            BytesRemaining = (BytesRemaining > Advance) ? (BytesRemaining - Advance) : 0;
        }

        CurrentSearchAddress = RegionEnd;
    }

    if (hTargetProcess != NULL) pObCloseHandle(hTargetProcess, KernelMode);
    if (FoundAddress != NULL && FoundAddress != (PBYTE)StartAddress) {
        if (pMmIsAddressValid((PVOID)ResultAddress)) {
            *(PULONG64)ResultAddress = (ULONG64)FoundAddress;
            Status = STATUS_SUCCESS;
        }
        else {
            Status = STATUS_ACCESS_VIOLATION;
        }
    }
    else {
        Status = STATUS_NOT_FOUND;
    }

    return Status;
}