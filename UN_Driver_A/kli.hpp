#define KERNEL_SPACE_START 0xFFFF800000000000
#define KERNEL_SPACE_END 0xFFFFFFFFFFFFFFFF

__declspec(noinline) wchar_t* my_wcsstr(wchar_t* str, wchar_t* substr) {
    if (str == NULL || substr == NULL) return NULL;
    if (*substr == L'\0') return str;
    wchar_t* p1 = str;
    wchar_t* p2 = substr;
    while (*p1 != L'\0') {
        if (*p1 == *p2) {
            wchar_t* p1_temp = p1;
            wchar_t* p2_temp = p2;
            while (*p1_temp != L'\0' && *p2_temp != L'\0' && *p1_temp == *p2_temp) {
                p1_temp++; p2_temp++;
            }
            if (*p2_temp == L'\0') return p1;
        }
        p1++;
    }
    return NULL;
}

__declspec(noinline) PVOID FindBase(PDRIVER_OBJECT pDriverObject, wchar_t* Name, PLDR_DATA_TABLE_ENTRY* LdrEntry) {
    if (!pDriverObject || !pDriverObject->DriverSection || !Name) return NULL;

    PLDR_DATA_TABLE_ENTRY pLdrEntry = (PLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
    PLDR_DATA_TABLE_ENTRY pCurEntry = pLdrEntry;
    const ULONG MAX_MODULES = 1024;
    ULONG count = 0;

    do {
        if (pCurEntry != NULL &&
            pCurEntry->BaseDllName.Buffer != NULL &&
            pCurEntry->BaseDllName.Length > 0 &&
            pCurEntry->BaseDllName.MaximumLength >= pCurEntry->BaseDllName.Length &&
            (ULONG64)pCurEntry->BaseDllName.Buffer >= KERNEL_SPACE_START &&
            (ULONG64)pCurEntry->BaseDllName.Buffer < KERNEL_SPACE_END &&
            (ULONG64)pCurEntry->BaseDllName.Buffer + pCurEntry->BaseDllName.MaximumLength <= KERNEL_SPACE_END) {

            if (my_wcsstr(pCurEntry->BaseDllName.Buffer, Name) != NULL) {
                if (pCurEntry->DllBase && (ULONG64)pCurEntry->DllBase >= KERNEL_SPACE_START) {
                    if (LdrEntry) *LdrEntry = pCurEntry;
                    return pCurEntry->DllBase;
                }
            }
        }

        PLDR_DATA_TABLE_ENTRY pNext = (PLDR_DATA_TABLE_ENTRY)pCurEntry->InLoadOrderLinks.Flink;
        if (!pNext || (ULONG64)pNext < KERNEL_SPACE_START || (ULONG64)pNext >= KERNEL_SPACE_END) break;
        pCurEntry = pNext;

    } while (pCurEntry != pLdrEntry && ++count < MAX_MODULES);

    return NULL;
}

__declspec(noinline) ULONG64 GetFunByExportName(ULONG64 imageBase, const wchar_t* funcName) {
    PIMAGE_DOS_HEADER lpDosHeader = (PIMAGE_DOS_HEADER)imageBase;
    PIMAGE_NT_HEADERS64 lpNtHeader = (PIMAGE_NT_HEADERS64)(lpDosHeader->e_lfanew + (ULONG64)imageBase);
    PIMAGE_EXPORT_DIRECTORY lpExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG64)imageBase + lpNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    ULONG32* lpNameArr = (ULONG32*)(lpExportDir->AddressOfNames + (ULONG64)imageBase);
    ULONG32* lpFuncs = (ULONG32*)(lpExportDir->AddressOfFunctions + (ULONG64)imageBase);
    USHORT* lpOrdinals = (USHORT*)(lpExportDir->AddressOfNameOrdinals + (ULONG64)imageBase);

    for (ULONG nIdx = 0; nIdx < lpExportDir->NumberOfFunctions; ++nIdx) {
        if (!lpNameArr[nIdx] || !lpOrdinals[nIdx]) continue;

        const CHAR* ansiFuncName = (const CHAR*)(imageBase + lpNameArr[nIdx]);
        const wchar_t* wideName = funcName;
        const CHAR* ansiName = ansiFuncName;
        BOOL match = TRUE;

        while (*wideName && *ansiName) {
            wchar_t wideChar = *wideName;
            wchar_t ansiChar = (wchar_t)(unsigned char)*ansiName;
            if (wideChar >= L'A' && wideChar <= L'Z') wideChar = wideChar + (L'a' - L'A');
            if (ansiChar >= L'A' && ansiChar <= L'Z') ansiChar = ansiChar + (L'a' - L'A');

            if (wideChar != ansiChar) {
                match = FALSE;
                break;
            }
            wideName++;
            ansiName++;
        }
        if (match && *wideName == L'\0' && *ansiName == '\0') return imageBase + lpFuncs[lpOrdinals[nIdx]];
    }
    return 0;
}