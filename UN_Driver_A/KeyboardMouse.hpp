__declspec(noinline) NTSTATUS SearchMouServiceCallBack() {
    UNICODE_STRING uniMouClass = { 0 };
    WCHAR wszMouClass[] = { L'\\', L'D', L'r', L'i', L'v', L'e', L'r', L'\\', L'M', L'o', L'u', L'C', L'l', L'a', L's', L's', L'\0' };
    MyRtlInitUnicodeString(&uniMouClass, wszMouClass);

    PDRIVER_OBJECT pMouClassObject = NULL;
    NTSTATUS status = ((decltype(ObReferenceObjectByName)*)pAPI_information->ImportTable.pObReferenceObjectByName)
        (&uniMouClass, OBJ_CASE_INSENSITIVE, NULL, 0, *((decltype(IoDriverObjectType))pAPI_information->ImportTable.pIoDriverObjectType), KernelMode, NULL, &pMouClassObject);

    if (!NT_SUCCESS(status)) return status;

    RTL_OSVERSIONINFOW ver = { 0 };
    ((decltype(RtlGetVersion)*)pAPI_information->ImportTable.pRtlGetVersion)(&ver);

    if (pMouClassObject->DriverStart && pMouClassObject->DriverSize > 100) {
        PUCHAR signature = NULL;
        ULONG sigLen = 0;
        PUCHAR start = (PUCHAR)pMouClassObject->DriverStart;
        PUCHAR end = NULL;
        PUCHAR p = NULL;
        BOOLEAN validFunc = FALSE;
        UCHAR key[] = { 0xA1, 0xD2, 0xB3, 0xC4 };

        if (ver.dwBuildNumber >= 26100) {
            UCHAR ShellCode[] = { 0xE9, 0x5B, 0xEF, 0xE0, 0xB1,0x9A, 0x3A, 0xA8, 0x85, 0xCA,0xFB, 0x4D, 0xD5, 0xF6, 0x93, 0x8C, 0x28, 0x9E, 0x97, 0xCC };
            Xor(ShellCode, sizeof(ShellCode), key, sizeof(key));
            signature = ShellCode;
            sigLen = sizeof(ShellCode);
        }
        else {
            UCHAR ShellCode[] = { 0xE9, 0x59, 0x77, 0x8C, 0x28, 0x8A, 0xBB, 0x8C, 0x28, 0xA2,0xA3, 0x8C, 0x28, 0xAA, 0xAB, 0x88, 0x28, 0x9A, 0x93 };
            Xor(ShellCode, sizeof(ShellCode), key, sizeof(key));
            signature = ShellCode;
            sigLen = sizeof(ShellCode);
        }

        end = start + pMouClassObject->DriverSize - sigLen;

        for (p = start; p < end; p++) {
            if (MyRtlCompareMemory(p, signature, sigLen) == sigLen) {
                validFunc = FALSE;
                for (int i = 0; i < 50; i++) {
                    if (!((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i)) break;

                    if (ver.dwBuildNumber >= 26100) {
                        if (p[i] == 0x57) {
                            if (i + 3 < 50 && ((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i + 3)) {
                                if (p[i + 1] == 0x48 && p[i + 2] == 0x83 && p[i + 3] == 0xEC) {
                                    validFunc = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                    else {
                        if (p[i] == 0x55) {
                            if (i + 3 < 50 && ((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i + 3)) {
                                if (p[i + 1] == 0x48 && p[i + 2] == 0x8B && p[i + 3] == 0xEC) {
                                    validFunc = TRUE;
                                    break;
                                }
                            }
                        }

                        if (p[i] == 0x48 && p[i + 1] == 0x83 && p[i + 2] == 0xEC) {
                            validFunc = TRUE;
                            break;
                        }
                    }
                }

                if (validFunc) {
                    pAPI_information->MouseClassServiceCallback = (_MouseClassServiceCallback)p;
                    break;
                }
            }
        }
    }

    PDEVICE_OBJECT pLastDevice = pMouClassObject->DeviceObject;
    while (pLastDevice) {
        if (!pLastDevice->NextDevice) {
            pAPI_information->MouseDeviceObject = pLastDevice;
            break;
        }
        pLastDevice = pLastDevice->NextDevice;
    }

    ((decltype(ObfDereferenceObject)*)pAPI_information->ImportTable.pObfDereferenceObject)(pMouClassObject);

    return STATUS_SUCCESS;
}

__declspec(noinline) NTSTATUS SearchKdbServiceCallBack() {
    UNICODE_STRING uniKbdClass = { 0 };
    WCHAR wszKbdClass[] = { L'\\', L'D', L'r', L'i', L'v', L'e', L'r', L'\\', L'K', L'b', L'd', L'c', L'l', L'a', L's', L's', L'\0' };
    MyRtlInitUnicodeString(&uniKbdClass, wszKbdClass);

    PDRIVER_OBJECT pKbdClassObject = NULL;
    NTSTATUS status = ((decltype(ObReferenceObjectByName)*)pAPI_information->ImportTable.pObReferenceObjectByName)
        (&uniKbdClass, OBJ_CASE_INSENSITIVE, NULL, 0, *((decltype(IoDriverObjectType))pAPI_information->ImportTable.pIoDriverObjectType), KernelMode, NULL, &pKbdClassObject);

    if (!NT_SUCCESS(status)) return status;

    RTL_OSVERSIONINFOW ver = { 0 };
    ((decltype(RtlGetVersion)*)pAPI_information->ImportTable.pRtlGetVersion)(&ver);

    if (pKbdClassObject->DriverStart && pKbdClassObject->DriverSize > 100) {
        PUCHAR signature = NULL;
        ULONG sigLen = 0;
        PUCHAR start = (PUCHAR)pKbdClassObject->DriverStart;
        PUCHAR end = NULL;
        PUCHAR p = NULL;
        BOOLEAN validFunc = FALSE;
        UCHAR key[] = { 0xA1, 0xD2, 0xB3, 0xC4 };

        if (ver.dwBuildNumber >= 26100) {
            UCHAR ShellCode[] = { 0xE9, 0x5B, 0xEF, 0xE0, 0xB1,0x9A, 0x3A, 0xA8, 0x85, 0xCA,0xFB, 0x4D, 0xD5, 0xF6, 0x93, 0x8C, 0x28, 0x9E, 0x97, 0xCC };
            Xor(ShellCode, sizeof(ShellCode), key, sizeof(key));
            signature = ShellCode;
            sigLen = sizeof(ShellCode);
        }
        else {
            UCHAR ShellCode[] = { 0xE9, 0x59, 0x77, 0x8C, 0x28, 0x8A, 0xBB, 0x8C, 0x28, 0xA2,0xA3, 0x8C, 0x28, 0xAA, 0xAB, 0x88, 0x28, 0x9A, 0x93 };
            Xor(ShellCode, sizeof(ShellCode), key, sizeof(key));
            signature = ShellCode;
            sigLen = sizeof(ShellCode);
        }

        end = start + pKbdClassObject->DriverSize - sigLen; 

        for (p = start; p < end; p++) {
            if (MyRtlCompareMemory(p, signature, sigLen) == sigLen) {
                validFunc = FALSE;
                for (int i = 0; i < 50; i++) {
                    if (!((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i)) break;

                    if (ver.dwBuildNumber >= 26100) {
                        if (p[i] == 0x57) {
                            if (i + 3 < 50 && ((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i + 3)) {
                                if (p[i + 1] == 0x48 && p[i + 2] == 0x83 && p[i + 3] == 0xEC) {
                                    validFunc = TRUE;
                                    break;
                                }
                            }
                        }
                    }
                    else {
                        if (p[i] == 0x55) {
                            if (i + 3 < 50 && ((decltype(MmIsAddressValid)*)pAPI_information->ImportTable.pMmIsAddressValid)(p + i + 3)) {
                                if (p[i + 1] == 0x48 && p[i + 2] == 0x8B && p[i + 3] == 0xEC) {
                                    validFunc = TRUE;
                                    break;
                                }
                            }
                        }

                        if (p[i] == 0x48 && p[i + 1] == 0x83 && p[i + 2] == 0xEC) {
                            validFunc = TRUE;
                            break;
                        }
                    }
                }

                if (validFunc) {
                    pAPI_information->KeyboardClassServiceCallback = (_KeyboardClassServiceCallback)p;
                    break;
                }
            }
        }
    }

    PDEVICE_OBJECT pLastDevice = pKbdClassObject->DeviceObject;
    while (pLastDevice) {
        if (!pLastDevice->NextDevice) {
            pAPI_information->KeyboardDeviceObject = pLastDevice;
            break;
        }
        pLastDevice = pLastDevice->NextDevice;
    }

    ((decltype(ObfDereferenceObject)*)pAPI_information->ImportTable.pObfDereferenceObject)(pKbdClassObject);

    return STATUS_SUCCESS;
}