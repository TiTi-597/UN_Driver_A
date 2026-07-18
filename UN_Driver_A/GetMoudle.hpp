__declspec(noinline)ULONG64 GetProcessPEB(_In_opt_ int* pIsX86) {
    ULONG64 peb = 0;
    if (pIsX86) *pIsX86 = -1;

    peb = ((ULONG64(*)(PEPROCESS))pAPI_information->ImportTable.pPsGetProcessWow64Process)(pAPI_information->gProcess.Process);
    if (peb) {
        if (pIsX86) *pIsX86 = 1;
    }
    else {
        if (pAPI_information->ImportTable.pPsGetProcessPeb) {
            peb = ((ULONG64(*)(PEPROCESS))pAPI_information->ImportTable.pPsGetProcessPeb)(pAPI_information->gProcess.Process);
            if (peb && pIsX86) *pIsX86 = 0;
        }
    }

    if (peb && ((peb & 0xF000000000000000) != 0)) peb = peb & 0x0FFFFFFFFFFFFFFF;
    return peb;
}

__declspec(noinline) ULONG64 GetModuleBaseInternal(UNICODE_STRING ModuleName, ULONG64 peb, INT isX86, ULONG64 Type) {
    if (!ModuleName.Buffer || !peb) return 0;

    typedef NTSTATUS(*MemoryOpFunc)(PDriverControl pDriver);
    MemoryOpFunc MemoryOperation = NULL;

    switch (Type) {
    case 0:
        MemoryOperation = (MemoryOpFunc)ManipulateMemory;
        break;
    case 1:
        MemoryOperation = (MemoryOpFunc)VirtualMemory;
        break;
    default:
        return 0;
    }

    if (!MemoryOperation) return 0;

    DriverControl driverOp = { 0 };

    if (isX86 == 0) {
        PEB64 pebData = { 0 };

        driverOp.Address = peb;
        driverOp.Buffer = (ULONG64)&pebData;
        driverOp.Size = sizeof(pebData);
        driverOp.Type = 0;

        if (!NT_SUCCESS(MemoryOperation(&driverOp))) return 0;
        if (!pebData.Ldr) return 0;

        PEB_LDR_DATA64 ldrData = { 0 };

        driverOp.Address = (ULONG64)pebData.Ldr;
        driverOp.Buffer = (ULONG64)&ldrData;
        driverOp.Size = sizeof(ldrData);
        driverOp.Type = 0;

        if (!NT_SUCCESS(MemoryOperation(&driverOp))) return 0;

        ULONG64 currentEntry = (ULONG64)ldrData.InMemoryOrderModuleList.Flink;
        ULONG64 listHead = (ULONG64)pebData.Ldr + FIELD_OFFSET(PEB_LDR_DATA64, InMemoryOrderModuleList);
        int moduleCount = 0;

        while (currentEntry != 0 && currentEntry != listHead && moduleCount < 300) {
            moduleCount++;

            ULONG64 ldrEntryAddr = currentEntry - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY64, InMemoryOrderLinks);
            LDR_DATA_TABLE_ENTRY64 ldrEntry = { 0 };

            driverOp.Address = ldrEntryAddr;
            driverOp.Buffer = (ULONG64)&ldrEntry;
            driverOp.Size = sizeof(ldrEntry);
            driverOp.Type = 0;

            if (!NT_SUCCESS(MemoryOperation(&driverOp))) break;

            if (ldrEntry.BaseDllName.Buffer && ldrEntry.BaseDllName.Length > 0) {
                WCHAR moduleName[260] = { 0 };
                ULONG nameLength = min(ldrEntry.BaseDllName.Length, sizeof(moduleName) - sizeof(WCHAR));

                driverOp.Address = (ULONG64)ldrEntry.BaseDllName.Buffer;
                driverOp.Buffer = (ULONG64)moduleName;
                driverOp.Size = nameLength;
                driverOp.Type = 0;

                if (NT_SUCCESS(MemoryOperation(&driverOp))) {
                    moduleName[nameLength / sizeof(WCHAR)] = L'\0';

                    UNICODE_STRING currentModuleName;
                    currentModuleName.Buffer = moduleName;
                    currentModuleName.Length = (USHORT)nameLength;
                    currentModuleName.MaximumLength = (USHORT)(nameLength + sizeof(WCHAR));

                    if (MyRtlCompareUnicodeString(&ModuleName, &currentModuleName, TRUE) == 0) return (ULONG64)ldrEntry.DllBase;
                }
            }

            if (!ldrEntry.InMemoryOrderLinks.Flink || ldrEntry.InMemoryOrderLinks.Flink == currentEntry) break;
            currentEntry = (ULONG64)ldrEntry.InMemoryOrderLinks.Flink;
        }
    }
    else {
        PEB32 pebData = { 0 };

        driverOp.Address = peb;
        driverOp.Buffer = (ULONG64)&pebData;
        driverOp.Size = sizeof(pebData);
        driverOp.Type = 0;

        if (!NT_SUCCESS(MemoryOperation(&driverOp))) return 0;
        if (!pebData.Ldr) return 0;

        PEB_LDR_DATA32 ldrData = { 0 };

        driverOp.Address = (ULONG64)(ULONG_PTR)pebData.Ldr;
        driverOp.Buffer = (ULONG64)&ldrData;
        driverOp.Size = sizeof(ldrData);
        driverOp.Type = 0;

        if (!NT_SUCCESS(MemoryOperation(&driverOp))) return 0;

        ULONG64 listHead = (ULONG64)(ULONG_PTR)pebData.Ldr + FIELD_OFFSET(PEB_LDR_DATA32, InMemoryOrderModuleList);
        ULONG64 currentEntry = (ULONG64)(ULONG_PTR)ldrData.InMemoryOrderModuleList.Flink;
        int moduleCount = 0;

        while (currentEntry != 0 && currentEntry != listHead && moduleCount < 300) {
            moduleCount++;

            ULONG32 ldrEntryAddr32 = (ULONG32)((ULONG_PTR)currentEntry - FIELD_OFFSET(LDR_DATA_TABLE_ENTRY32, InMemoryOrderLinks));
            ULONG64 ldrEntryAddr = (ULONG64)(ULONG_PTR)ldrEntryAddr32;

            LDR_DATA_TABLE_ENTRY32 ldrEntry = { 0 };

            driverOp.Address = ldrEntryAddr;
            driverOp.Buffer = (ULONG64)&ldrEntry;
            driverOp.Size = sizeof(ldrEntry);
            driverOp.Type = 0;

            if (!NT_SUCCESS(MemoryOperation(&driverOp))) break;

            if (ldrEntry.BaseDllName.Buffer && ldrEntry.BaseDllName.Length > 0) {
                WCHAR moduleName[260] = { 0 };
                ULONG nameLength = min(ldrEntry.BaseDllName.Length, sizeof(moduleName) - sizeof(WCHAR));

                driverOp.Address = (ULONG64)(ULONG_PTR)ldrEntry.BaseDllName.Buffer;
                driverOp.Buffer = (ULONG64)moduleName;
                driverOp.Size = nameLength;
                driverOp.Type = 0;

                if (NT_SUCCESS(MemoryOperation(&driverOp))) {
                    moduleName[nameLength / sizeof(WCHAR)] = L'\0';

                    UNICODE_STRING currentModuleName;
                    currentModuleName.Buffer = moduleName;
                    currentModuleName.Length = (USHORT)nameLength;
                    currentModuleName.MaximumLength = (USHORT)(nameLength + sizeof(WCHAR));

                    if (MyRtlCompareUnicodeString(&ModuleName, &currentModuleName, TRUE) == 0) return (ULONG64)(ULONG_PTR)ldrEntry.DllBase;
                }
            }

            if (ldrEntry.InMemoryOrderLinks.Flink == 0 || (ULONG64)(ULONG_PTR)ldrEntry.InMemoryOrderLinks.Flink == currentEntry) break;
            currentEntry = (ULONG64)(ULONG_PTR)ldrEntry.InMemoryOrderLinks.Flink;
        }
    }

    return 0;
}

__declspec(noinline) NTSTATUS GetModuleBase(UNICODE_STRING ModuleName, ULONG64& Buffer, ULONG64 Type) {
    INT isX86 = -1;
    ULONG64 peb = GetProcessPEB(&isX86);
    if (isX86 == -1) return STATUS_UNSUCCESSFUL;

    Buffer = GetModuleBaseInternal(ModuleName, peb, isX86, Type);
    if (Buffer == 0) return STATUS_NOT_FOUND;

    return STATUS_SUCCESS;
}

__declspec(noinline) NTSTATUS GetKernelModuleAddress(UNICODE_STRING ModuleName, ULONG64& Buffer) {
    if (!ModuleName.Buffer || ModuleName.Length == 0) {
        Buffer = 0;
        return STATUS_INVALID_PARAMETER;
    }

    if (ModuleName.Length % 2 != 0) {
        Buffer = 0;
        return STATUS_INVALID_PARAMETER_2;
    }

    PLDR_DATA_TABLE_ENTRY pCurEntry = pAPI_information->NtoskrnlLdr;
    PLDR_DATA_TABLE_ENTRY pStartEntry = pCurEntry;

    do {
        if (!pCurEntry || !pCurEntry->BaseDllName.Buffer || (ULONG64)pCurEntry->BaseDllName.Buffer <= KERNEL_SPACE_START || (ULONG64)pCurEntry->BaseDllName.Buffer >= KERNEL_SPACE_END) continue;

        if (MyRtlCompareUnicodeString(&pCurEntry->BaseDllName, &ModuleName, TRUE) == 0) {
            Buffer = (ULONG64)pCurEntry->DllBase;
            return STATUS_SUCCESS;
        }

        pCurEntry = (PLDR_DATA_TABLE_ENTRY)pCurEntry->InLoadOrderLinks.Flink;

        if ((ULONG64)pCurEntry < KERNEL_SPACE_START || (ULONG64)pCurEntry >= KERNEL_SPACE_END) {
            Buffer = 0;
            return STATUS_INVALID_ADDRESS;
        }

    } while (pCurEntry != pStartEntry);

    Buffer = 0;
    return STATUS_NOT_FOUND;
}