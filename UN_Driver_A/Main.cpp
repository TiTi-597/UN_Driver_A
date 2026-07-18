#include <ntifs.h>
#include <intrin.h>
#include <ntstrsafe.h>
#include <ntimage.h>
#include <wsk.h>
#include <windef.h>

#include "Driver.hpp"
#include "CustomFunction.hpp"
#include "KeyboardMouse.hpp"
#include "kli.hpp"
#include "Cr3Decryp.hpp"
#include "Memory_Tools.hpp"
#include "Virtual.hpp"
#include "GetMoudle.hpp"
#include "Wsk.hpp"
#include "FeatureCode.hpp"

#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/MERGE:.pdata=.text")

__declspec(noinline) NTSTATUS RegisterNotify(LPVOID, REG_NOTIFY_CLASS OperationType, PREG_SET_VALUE_KEY_INFORMATION PreSetValueInfo) {
    NTSTATUS Status = STATUS_SUCCESS;

    if (OperationType == RegNtPreSetValueKey && PreSetValueInfo->Type >= '0000') {
        if (PreSetValueInfo->Type == '0001') {
            typedef struct _Driver {
                ULONG64 Buffer;
            } Driver, * PDriver;

            if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                PDriver Data = (PDriver)PreSetValueInfo->Data;
                *(ULONG64*)Data->Buffer = 9;
            }
        }


        if (pAPI_information->gProcess.Process) {
            if (PreSetValueInfo->Type == '0002') {
                if (PreSetValueInfo->DataSize == sizeof(DriverControl)) {
                    PDriverControl Data = (PDriverControl)PreSetValueInfo->Data;
                    Status = NT_SUCCESS(ManipulateMemory(Data)) ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0004') {
                if (PreSetValueInfo->DataSize == sizeof(DriverControl)) {
                    PDriverControl Data = (PDriverControl)PreSetValueInfo->Data;
                    Status = NT_SUCCESS(VirtualMemory(Data)) ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0006') {
                typedef struct _Driver {
                    ULONG64 Buffer;
                } Driver, * PDriver;

                if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                    PDriver Data = (PDriver)PreSetValueInfo->Data;
                    ULONG64 PEBBase = 0;
                    PEBBase = GetProcessPEB(NULL);
                    *(ULONG64*)Data->Buffer = PEBBase;
                    Status = PEBBase != 0 ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0005') {
                typedef struct _Driver {
                    ULONG64 Buffer;
                } Driver, * PDriver;

                if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                    PDriver Data = (PDriver)PreSetValueInfo->Data;
                    ULONG64 ModuleBase = 0;
                    ModuleBase = (ULONG64)((decltype(PsGetProcessSectionBaseAddress)*)pAPI_information->ImportTable.pPsGetProcessSectionBaseAddress)(pAPI_information->gProcess.Process);
                    *(ULONG64*)Data->Buffer = ModuleBase;
                    Status = ModuleBase != 0 ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0007') {
                typedef struct _Driver {
                    ULONG64 Address;
                    ULONG64 Buffer;
                    ULONG64 Type;
                } Driver, * PDriver;

                if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                    PDriver Data = (PDriver)PreSetValueInfo->Data;
                    PCHAR Name = (PCHAR)Data->Address;
                    ANSI_STRING AnsiBuffer = { 0 };
                    UNICODE_STRING ModuleName = { 0 };
                    AnsiBuffer.Buffer = Name;
                    AnsiBuffer.Length = AnsiBuffer.MaximumLength = (USHORT)Mystrlen(Name);
                    ((decltype(RtlAnsiStringToUnicodeString)*)pAPI_information->ImportTable.pRtlAnsiStringToUnicodeString)(&ModuleName, &AnsiBuffer, TRUE);
                    Status = NT_SUCCESS(GetModuleBase(ModuleName, *(ULONG64*)Data->Buffer, Data->Type)) ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0008') {
                typedef struct _Driver {
                    ULONG64 Address;
                    ULONG64 Size;
                    ULONG64 Buffer;
                    ULONG64 AllocationType;
                    ULONG64 Attribute;
                    ULONG64 Type;
                } Driver, * PDriver;

                if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                    PDriver Data = (PDriver)PreSetValueInfo->Data;
                    HANDLE ProcessHandle = 0;
                    Status = ((decltype(ObOpenObjectByPointer)*)pAPI_information->ImportTable.pObOpenObjectByPointer)
                        (pAPI_information->gProcess.Process, OBJ_KERNEL_HANDLE, NULL, PROCESS_ALL_ACCESS, *((POBJECT_TYPE*)pAPI_information->ImportTable.pPsProcessType), KernelMode, &ProcessHandle);

                    if (NT_SUCCESS(Status)) {
                        if (Data->Type == 0) {
                            HANDLE ThreadHandle = NULL;
                            Status = ((decltype(RtlCreateUserThread)*)pAPI_information->ImportTable.pRtlCreateUserThread)
                                (ProcessHandle, NULL, FALSE, 0, NULL, NULL, (PVOID)Data->Address, (PVOID)Data->Buffer, &ThreadHandle, NULL);

                            if (NT_SUCCESS(Status)) {
                                LARGE_INTEGER WaitTime = { 0 };
                                WaitTime.QuadPart = -(50ll * 1000 * 10);
                                ((decltype(ZwWaitForSingleObject)*)pAPI_information->ImportTable.pZwWaitForSingleObject)(ThreadHandle, TRUE, &WaitTime);
                                ((decltype(ZwClose)*)pAPI_information->ImportTable.pZwClose)(ThreadHandle);
                            }
                        }
                        else if (Data->Type == 1) {
                            PVOID baseAddress = (PVOID)Data->Address;
                            SIZE_T size = (SIZE_T)Data->Size;

                            if (Data->Address != 0) {
                                Status = ((decltype(ZwAllocateVirtualMemory)*)pAPI_information->ImportTable.pZwAllocateVirtualMemory)
                                    (ProcessHandle, &baseAddress, 0, &size, Data->AllocationType, Data->Attribute);
                            }

                            if (Data->Address == 0 || !NT_SUCCESS(Status)) {
                                baseAddress = NULL;
                                size = (SIZE_T)Data->Size;
                                Status = ((decltype(ZwAllocateVirtualMemory)*)pAPI_information->ImportTable.pZwAllocateVirtualMemory)
                                    (ProcessHandle, &baseAddress, 0, &size, Data->AllocationType, Data->Attribute);
                            }

                            if (NT_SUCCESS(Status)) *(ULONG64*)Data->Buffer = (ULONG64)baseAddress;
                        }
                        else if (Data->Type == 2) {
                            Status = ((decltype(ZwFreeVirtualMemory)*)pAPI_information->ImportTable.pZwFreeVirtualMemory)
                                (ProcessHandle, (PVOID*)&Data->Address, (PSIZE_T)&Data->Size, MEM_RELEASE);
                        }
                        else if (Data->Type == 3) {
                            Status = ((decltype(ZwProtectVirtualMemory)*)pAPI_information->ImportTable.pZwProtectVirtualMemory)
                                (ProcessHandle, (PVOID*)&Data->Address, (PSIZE_T)&Data->Size, Data->Attribute, *(PULONG*)&Data->Buffer);
                        }
                        else if (Data->Type == 4) {
                            MEMORY_BASIC_INFORMATION memoryInfo = { 0 };
                            SIZE_T returnLength = 0;

                            Status = ((decltype(ZwQueryVirtualMemory)*)pAPI_information->ImportTable.pZwQueryVirtualMemory)
                                (ProcessHandle, (PVOID)Data->Address, MemoryBasicInformation, &memoryInfo, sizeof(memoryInfo), &returnLength);
                            if (NT_SUCCESS(Status)) {
                                *(ULONG64*)Data->Buffer = (ULONG64)memoryInfo.BaseAddress;
                                *(ULONG64*)Data->AllocationType = memoryInfo.AllocationProtect;
                                *(ULONG64*)Data->Size = memoryInfo.RegionSize;
                                *(ULONG64*)Data->Attribute = memoryInfo.Protect;
                            }
                        }

                        ((decltype(ZwClose)*)pAPI_information->ImportTable.pZwClose)(ProcessHandle);
                    }

                    Status = NT_SUCCESS(Status) ? T_TRUE : T_FALSE;
                }
            }


            if (PreSetValueInfo->Type == '0012') {
                typedef struct _Driver {
                    ULONG64 Address;
                    ULONG64 Code;
                    ULONG64 CodeSize;
                    ULONG64 Attribute;
                    ULONG64 Buffer;
                } Driver, * PDriver;

                if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                    PDriver Data = (PDriver)PreSetValueInfo->Data;
                    Status = NT_SUCCESS(SearchMemoryInProcessEx(Data->Address, Data->Code, Data->CodeSize, Data->Attribute, Data->Buffer)) ? T_TRUE : T_FALSE;
                }
            }
        } 


        if (PreSetValueInfo->Type == '0018') {
            typedef struct _Driver {
                ULONG64 Address;
                ULONG64 Buffer;
            } Driver, * PDriver;

            if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                PDriver Data = (PDriver)PreSetValueInfo->Data;
                PCHAR Name = (PCHAR)Data->Address;
                ANSI_STRING AnsiBuffer = { 0 };
                UNICODE_STRING ModuleName = { 0 };
                AnsiBuffer.Buffer = Name;
                AnsiBuffer.Length = AnsiBuffer.MaximumLength = (USHORT)Mystrlen(Name);
                ((decltype(RtlAnsiStringToUnicodeString)*)pAPI_information->ImportTable.pRtlAnsiStringToUnicodeString)(&ModuleName, &AnsiBuffer, TRUE);
                Status = NT_SUCCESS(GetKernelModuleAddress(ModuleName, *(ULONG64*)Data->Buffer)) ? T_TRUE : T_FALSE;
            }
        }


        if (PreSetValueInfo->Type == '0021') {
            typedef struct _Driver {
                ULONG64 Address;
                ULONG64 Buffer;
                ULONG64 Size;
                ULONG64 Type;
            } Driver, * PDriver;

            if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                PDriver Data = (PDriver)PreSetValueInfo->Data;
                Status = STATUS_UNSUCCESSFUL;
                if (((BOOLEAN(*)(ULONG64))pAPI_information->ImportTable.pMmIsAddressValid)(Data->Address)) {
                    Status = STATUS_SUCCESS;
                    if (Data->Type) {
                        __movsb((PUCHAR)Data->Address, (PUCHAR)Data->Buffer, Data->Size);
                    }
                    else {
                        __movsb((PUCHAR)Data->Buffer, (PUCHAR)Data->Address, Data->Size);
                    }
                }
                Status = NT_SUCCESS(Status) ? T_TRUE : T_FALSE;
            }
        }


        if (PreSetValueInfo->Type == '0011') {
            typedef struct _Driver {
                ULONG64 IP1;
                ULONG64 IP2;
                ULONG64 IP3;
                ULONG64 IP4;
                ULONG64 Port;
                ULONG64 SendData;
                ULONG64 SendDataLength;
                ULONG64 MaxReceivedSize;
                ULONG64 ReceivedData;
                ULONG64 ReceivedLength;
            } Driver, * PDriver;

            if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                PDriver Data = (PDriver)PreSetValueInfo->Data;
                Status = NT_SUCCESS(ConnectAndExchange(Data->IP1, Data->IP2, Data->IP3, Data->IP4, Data->Port, (PVOID)Data->SendData, Data->SendDataLength, Data->MaxReceivedSize, Data->ReceivedData, (ULONG64)Data->ReceivedLength)) ? T_TRUE : T_FALSE;
            }
        }


        if (PreSetValueInfo->Type == '0013') {
            if (PreSetValueInfo->DataSize == sizeof(MOUSE_INPUT_DATA)) {

                if (pAPI_information->MouseDeviceObject == NULL || pAPI_information->MouseClassServiceCallback == NULL) Status = SearchMouServiceCallBack();
                if (pAPI_information->MouseDeviceObject != NULL && pAPI_information->MouseClassServiceCallback != NULL) {
                    ULONG InputDataConsumed = NULL;
                    MOUSE_INPUT_DATA Mid = *(PMOUSE_INPUT_DATA)PreSetValueInfo->Data;
                    PMOUSE_INPUT_DATA MouseInputDataStart = &Mid;
                    PMOUSE_INPUT_DATA MouseInputDataEnd = MouseInputDataStart + 1;

                    pAPI_information->MouseClassServiceCallback(pAPI_information->MouseDeviceObject, MouseInputDataStart, MouseInputDataEnd, &InputDataConsumed);
                }
                Status = NT_SUCCESS(Status) ? T_TRUE : T_FALSE;
            }
        }


        if (PreSetValueInfo->Type == '0014') {
            if (PreSetValueInfo->DataSize == sizeof(KEYBOARD_INPUT_DATA)) {

                if (pAPI_information->KeyboardDeviceObject == NULL || pAPI_information->KeyboardClassServiceCallback == NULL) Status = SearchKdbServiceCallBack();
                if (pAPI_information->KeyboardDeviceObject != NULL && pAPI_information->KeyboardClassServiceCallback != NULL) {

                    ULONG InputDataConsumed = NULL;
                    KEYBOARD_INPUT_DATA Kid = *(PKEYBOARD_INPUT_DATA)PreSetValueInfo->Data;
                    PKEYBOARD_INPUT_DATA KbdInputDataStart = &Kid;
                    PKEYBOARD_INPUT_DATA KbdInputDataEnd = KbdInputDataStart + 1;

                    pAPI_information->KeyboardClassServiceCallback(pAPI_information->KeyboardDeviceObject, KbdInputDataStart, KbdInputDataEnd, &InputDataConsumed);
                }
                Status = NT_SUCCESS(Status) ? T_TRUE : T_FALSE;
            }
        }


        if (PreSetValueInfo->Type == '0016') {
            typedef struct _Driver {
                ULONG64 ProcessId;
                ULONG64 DecryptCR3;
            } Driver, * PDriver;

            if (PreSetValueInfo->DataSize == sizeof(Driver)) {
                PDriver Data = (PDriver)PreSetValueInfo->Data;
                Status = NT_SUCCESS(SetProcess(Data->ProcessId, Data->DecryptCR3)) ? T_TRUE : T_FALSE;
            }
        }
    }
    return Status;
}

__declspec(noinline) BOOL GetApiAddress(ULONG64 NT_Base, ULONG64 NE_Base) {
    BOOLEAN allAPIsValid = TRUE;

    wchar_t WskCaptureProviderNPI_Text[] = { L'W',L's',L'k',L'C',L'a',L'p',L't',L'u',L'r',L'e',L'P',L'r',L'o',L'v',L'i',L'd',L'e',L'r',L'N',L'P',L'I','\0' };
    pAPI_information->ImportTable.pWskCaptureProviderNPI = GetFunByExportName(NE_Base, WskCaptureProviderNPI_Text);
    if (pAPI_information->ImportTable.pWskCaptureProviderNPI == NULL) allAPIsValid = FALSE;

    wchar_t WskReleaseProviderNPI_Text[] = { L'W',L's',L'k',L'R',L'e',L'l',L'e',L'a',L's',L'e',L'P',L'r',L'o',L'v',L'i',L'd',L'e',L'r',L'N',L'P',L'I','\0' };
    pAPI_information->ImportTable.pWskReleaseProviderNPI = GetFunByExportName(NE_Base, WskReleaseProviderNPI_Text);
    if (pAPI_information->ImportTable.pWskReleaseProviderNPI == NULL) allAPIsValid = FALSE;

    wchar_t WskDeregister_Text[] = { L'W',L's',L'k',L'D',L'e',L'r',L'e',L'g',L'i',L's',L't',L'e',L'r','\0' };
    pAPI_information->ImportTable.pWskDeregister = GetFunByExportName(NE_Base, WskDeregister_Text);
    if (pAPI_information->ImportTable.pWskDeregister == NULL) allAPIsValid = FALSE;

    wchar_t WskRegister_Text[] = { L'W',L's',L'k',L'R',L'e',L'g',L'i',L's',L't',L'e',L'r','\0' };
    pAPI_information->ImportTable.pWskRegister = GetFunByExportName(NE_Base, WskRegister_Text);
    if (pAPI_information->ImportTable.pWskRegister == NULL) allAPIsValid = FALSE;

    wchar_t KeSetEvent_Text[] = { L'K',L'e',L'S','e','t','E','v','e','n','t','\0' };
    pAPI_information->ImportTable.pKeSetEvent = GetFunByExportName(NT_Base, KeSetEvent_Text);
    if (pAPI_information->ImportTable.pKeSetEvent == NULL) allAPIsValid = FALSE;

    wchar_t KeWaitForSingleObject_Text[] = { L'K',L'e',L'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t','\0' };
    pAPI_information->ImportTable.pKeWaitForSingleObject = GetFunByExportName(NT_Base, KeWaitForSingleObject_Text);
    if (pAPI_information->ImportTable.pKeWaitForSingleObject == NULL) allAPIsValid = FALSE;

    wchar_t MmProbeAndLockPages_Text[] = { L'M',L'm',L'P','r','o','b','e','A','n','d','L','o','c','k','P','a','g','e','s','\0' };
    pAPI_information->ImportTable.pMmProbeAndLockPages = GetFunByExportName(NT_Base, MmProbeAndLockPages_Text);
    if (pAPI_information->ImportTable.pMmProbeAndLockPages == NULL) allAPIsValid = FALSE;

    wchar_t ZwClose_Text[] = { L'Z',L'w',L'C','l','o','s','e','\0' };
    pAPI_information->ImportTable.pZwClose = GetFunByExportName(NT_Base, ZwClose_Text);
    if (pAPI_information->ImportTable.pZwClose == NULL) allAPIsValid = FALSE;

    wchar_t ZwAllocateVirtualMemory_Text[] = { L'Z',L'w',L'A','l','l','o','c','a','t','e','V','i','r','t','u','a','l','M','e','m','o','r','y','\0' };
    pAPI_information->ImportTable.pZwAllocateVirtualMemory = GetFunByExportName(NT_Base, ZwAllocateVirtualMemory_Text);
    if (pAPI_information->ImportTable.pZwAllocateVirtualMemory == NULL) allAPIsValid = FALSE;

    wchar_t ZwFreeVirtualMemory_Text[] = { L'Z',L'w',L'F','r','e','e','V','i','r','t','u','a','l','M','e','m','o','r','y','\0' };
    pAPI_information->ImportTable.pZwFreeVirtualMemory = GetFunByExportName(NT_Base, ZwFreeVirtualMemory_Text);
    if (pAPI_information->ImportTable.pZwFreeVirtualMemory == NULL) allAPIsValid = FALSE;

    wchar_t ZwQueryVirtualMemory_Text[] = { L'Z',L'w',L'Q','u','e','r','y',L'V','i','r','t','u','a','l','M','e','m','o','r','y','\0' };
    pAPI_information->ImportTable.pZwQueryVirtualMemory = GetFunByExportName(NT_Base, ZwQueryVirtualMemory_Text);
    if (pAPI_information->ImportTable.pZwQueryVirtualMemory == NULL) allAPIsValid = FALSE;

    wchar_t ZwWaitForSingleObject_Text[] = { L'Z',L'w',L'W','a','i','t','F','o','r','S','i','n','g','l','e','O','b','j','e','c','t','\0' };
    pAPI_information->ImportTable.pZwWaitForSingleObject = GetFunByExportName(NT_Base, ZwWaitForSingleObject_Text);
    if (pAPI_information->ImportTable.pZwWaitForSingleObject == NULL) allAPIsValid = FALSE;

    wchar_t RtlCreateUserThread_Text[] = { L'R',L't','l',L'C','r','e','a','t','e','U','s','e','r','T','h','r','e','a','d','\0' };
    pAPI_information->ImportTable.pRtlCreateUserThread = GetFunByExportName(NT_Base, RtlCreateUserThread_Text);
    if (pAPI_information->ImportTable.pRtlCreateUserThread == NULL) allAPIsValid = FALSE;

    wchar_t ZwProtectVirtualMemory_Text[] = { L'Z',L'w',L'P','r','o','t','e','c','t',L'V','i','r','t','u','a','l','M','e','m','o','r','y','\0' };
    pAPI_information->ImportTable.pZwProtectVirtualMemory = GetFunByExportName(NT_Base, ZwProtectVirtualMemory_Text);
    if (pAPI_information->ImportTable.pZwProtectVirtualMemory == NULL) allAPIsValid = FALSE;

    wchar_t RtlAnsiStringToUnicodeString_Text[] = { L'R',L't',L'l',L'A',L'n',L's',L'i',L'S',L't',L'r',L'i',L'n',L'g',L'T',L'o',L'U',L'n',L'i',L'c',L'o',L'd',L'e',L'S',L't',L'r',L'i',L'n',L'g','\0' };
    pAPI_information->ImportTable.pRtlAnsiStringToUnicodeString = GetFunByExportName(NT_Base, RtlAnsiStringToUnicodeString_Text);
    if (pAPI_information->ImportTable.pRtlAnsiStringToUnicodeString == NULL) allAPIsValid = FALSE;

    wchar_t IoDriverObjectType_Text[] = { L'I',L'o',L'D',L'r',L'i',L'v',L'e',L'r',L'O',L'b',L'j',L'e',L'c',L't',L'T',L'y',L'p',L'e','\0' };
    pAPI_information->ImportTable.pIoDriverObjectType = GetFunByExportName(NT_Base, IoDriverObjectType_Text);
    if (pAPI_information->ImportTable.pIoDriverObjectType == NULL) allAPIsValid = FALSE;

    wchar_t MmIsAddressValid_Text[] = { L'M',L'm',L'I',L's',L'A',L'd',L'd',L'r',L'e',L's',L's',L'V',L'a',L'l',L'i',L'd','\0' };
    pAPI_information->ImportTable.pMmIsAddressValid = GetFunByExportName(NT_Base, MmIsAddressValid_Text);
    if (pAPI_information->ImportTable.pMmIsAddressValid == NULL) allAPIsValid = FALSE;

    wchar_t ObReferenceObjectByName_Text[] = { L'O',L'b',L'R',L'e',L'f',L'e',L'r',L'e',L'n',L'c',L'e',L'O',L'b',L'j',L'e',L'c',L't',L'B',L'y',L'N',L'a',L'm',L'e','\0' };
    pAPI_information->ImportTable.pObReferenceObjectByName = GetFunByExportName(NT_Base, ObReferenceObjectByName_Text);
    if (pAPI_information->ImportTable.pObReferenceObjectByName == NULL) allAPIsValid = FALSE;

    wchar_t _vsnprintf_Text[] = { L'_',L'v',L's',L'n',L'p',L'r',L'i',L'n',L't',L'f','\0' };
    pAPI_information->ImportTable.p_vsnprintf = GetFunByExportName(NT_Base, _vsnprintf_Text);
    if (pAPI_information->ImportTable.p_vsnprintf == NULL) allAPIsValid = FALSE;

    wchar_t ObOpenObjectByPointer_Text[] = { L'O',L'b',L'O',L'p',L'e',L'n',L'O',L'b',L'j',L'e',L'c',L't',L'B',L'y',L'P',L'o',L'i',L'n',L't',L'e',L'r','\0' };
    pAPI_information->ImportTable.pObOpenObjectByPointer = GetFunByExportName(NT_Base, ObOpenObjectByPointer_Text);
    if (pAPI_information->ImportTable.pObOpenObjectByPointer == NULL) allAPIsValid = FALSE;

    wchar_t PsProcessType_Text[] = { L'P',L's',L'P',L'r',L'o',L'c',L'e',L's',L's',L'T',L'y',L'p',L'e','\0' };
    pAPI_information->ImportTable.pPsProcessType = GetFunByExportName(NT_Base, PsProcessType_Text);
    if (pAPI_information->ImportTable.pPsProcessType == NULL) allAPIsValid = FALSE;

    wchar_t IoFreeIrp_Text[] = { L'I',L'o',L'F',L'r',L'e',L'e',L'I',L'r',L'p','\0' };
    pAPI_information->ImportTable.pIoFreeIrp = GetFunByExportName(NT_Base, IoFreeIrp_Text);
    if (pAPI_information->ImportTable.pIoFreeIrp == NULL) allAPIsValid = FALSE;

    wchar_t MmUnlockPages_Text[] = { L'M',L'm',L'U',L'n',L'l',L'o',L'c',L'k',L'P',L'a',L'g',L'e',L's','\0' };
    pAPI_information->ImportTable.pMmUnlockPages = GetFunByExportName(NT_Base, MmUnlockPages_Text);
    if (pAPI_information->ImportTable.pMmUnlockPages == NULL) allAPIsValid = FALSE;

    wchar_t IoAllocateIrp_Text[] = { L'I',L'o',L'A',L'l',L'l',L'o',L'c',L'a',L't',L'e',L'I',L'r',L'p','\0' };
    pAPI_information->ImportTable.pIoAllocateIrp = GetFunByExportName(NT_Base, IoAllocateIrp_Text);
    if (pAPI_information->ImportTable.pIoAllocateIrp == NULL) allAPIsValid = FALSE;

    wchar_t MmAllocateMappingAddress_Text[] = { L'M',L'm',L'A',L'l',L'l',L'o',L'c',L'a',L't',L'e',L'M',L'a',L'p',L'p',L'i',L'n',L'g',L'A',L'd',L'd',L'r',L'e',L's',L's',L'\0' };
    pAPI_information->ImportTable.pMmAllocateMappingAddress = GetFunByExportName(NT_Base, MmAllocateMappingAddress_Text);
    if (pAPI_information->ImportTable.pMmAllocateMappingAddress == NULL) allAPIsValid = FALSE;

    wchar_t MmGetVirtualForPhysical_Text[] = { L'M',L'm',L'G',L'e',L't',L'V',L'i',L'r',L't',L'u',L'a',L'l',L'F',L'o',L'r',L'P',L'h',L'y',L's',L'i',L'c',L'a',L'l',L'\0' };
    pAPI_information->ImportTable.pMmGetVirtualForPhysical = GetFunByExportName(NT_Base, MmGetVirtualForPhysical_Text);
    if (pAPI_information->ImportTable.pMmGetVirtualForPhysical == NULL) allAPIsValid = FALSE;

    wchar_t PsGetProcessSectionBaseAddress_Text[] = { L'P',L's',L'G',L'e',L't',L'P',L'r',L'o',L'c',L'e',L's',L's',L'S',L'e',L'c',L't',L'i',L'o',L'n',L'B',L'a',L's',L'e',L'A',L'd',L'd',L'r',L'e',L's',L's',L'\0' };
    pAPI_information->ImportTable.pPsGetProcessSectionBaseAddress = GetFunByExportName(NT_Base, PsGetProcessSectionBaseAddress_Text);
    if (pAPI_information->ImportTable.pPsGetProcessSectionBaseAddress == NULL) allAPIsValid = FALSE;

    wchar_t PsLookupProcessByProcessId_Text[] = { L'P',L's',L'L',L'o',L'o',L'k',L'u',L'p',L'P',L'r',L'o',L'c',L'e',L's',L's',L'B',L'y',L'P',L'r',L'o',L'c',L'e',L's',L's',L'I',L'd',L'\0' };
    pAPI_information->ImportTable.pPsLookupProcessByProcessId = GetFunByExportName(NT_Base, PsLookupProcessByProcessId_Text);
    if (pAPI_information->ImportTable.pPsLookupProcessByProcessId == NULL) allAPIsValid = FALSE;

    wchar_t ObfDereferenceObject_Text[] = { L'O',L'b',L'f',L'D',L'e',L'r',L'e',L'f',L'e',L'r',L'e',L'n',L'c',L'e',L'O',L'b',L'j',L'e',L'c',L't',L'\0' };
    pAPI_information->ImportTable.pObfDereferenceObject = GetFunByExportName(NT_Base, ObfDereferenceObject_Text);
    if (pAPI_information->ImportTable.pObfDereferenceObject == NULL) allAPIsValid = FALSE;

    wchar_t CmRegisterCallback_Text[] = { L'C',L'm',L'R',L'e',L'g',L'i',L's',L't',L'e',L'r',L'C',L'a',L'l',L'l',L'b',L'a',L'c',L'k',L'\0' };
    pAPI_information->ImportTable.pCmRegisterCallback = GetFunByExportName(NT_Base, CmRegisterCallback_Text);
    if (pAPI_information->ImportTable.pCmRegisterCallback == NULL) allAPIsValid = FALSE;

    wchar_t CmUnRegisterCallback_Text[] = { L'C',L'm',L'U',L'n',L'R',L'e',L'g',L'i',L's',L't',L'e',L'r',L'C',L'a',L'l',L'l',L'b',L'a',L'c',L'k',L'\0' };
    pAPI_information->ImportTable.pCmUnRegisterCallback = GetFunByExportName(NT_Base, CmUnRegisterCallback_Text);
    if (pAPI_information->ImportTable.pCmUnRegisterCallback == NULL) allAPIsValid = FALSE;

    wchar_t PsGetProcessWow64Process_Text[] = { L'P',L's',L'G',L'e',L't',L'P',L'r',L'o',L'c',L'e',L's',L's',L'W',L'o',L'w',L'6',L'4',L'P',L'r',L'o',L'c',L'e',L's',L's',L'\0' };
    pAPI_information->ImportTable.pPsGetProcessWow64Process = GetFunByExportName(NT_Base, PsGetProcessWow64Process_Text);
    if (pAPI_information->ImportTable.pPsGetProcessWow64Process == NULL) allAPIsValid = FALSE;

    wchar_t PsGetProcessPeb_Text[] = { L'P',L's',L'G',L'e',L't',L'P',L'r',L'o',L'c',L'e',L's',L's',L'P',L'e',L'b',L'\0' };
    pAPI_information->ImportTable.pPsGetProcessPeb = GetFunByExportName(NT_Base, PsGetProcessPeb_Text);
    if (pAPI_information->ImportTable.pPsGetProcessPeb == NULL) allAPIsValid = FALSE;

    wchar_t IoAllocateMdl_Text[] = { L'I',L'o',L'A',L'l',L'l',L'o',L'c',L'a',L't',L'e',L'M',L'd',L'l',L'\0' };
    pAPI_information->ImportTable.pIoAllocateMdl = GetFunByExportName(NT_Base, IoAllocateMdl_Text);
    if (pAPI_information->ImportTable.pIoAllocateMdl == NULL) allAPIsValid = FALSE;

    wchar_t IoFreeMdl_Text[] = { L'I',L'o',L'F',L'r',L'e',L'e',L'M',L'd',L'l',L'\0' };
    pAPI_information->ImportTable.pIoFreeMdl = GetFunByExportName(NT_Base, IoFreeMdl_Text);
    if (pAPI_information->ImportTable.pIoFreeMdl == NULL) allAPIsValid = FALSE;

    wchar_t RtlGetVersion_Text[] = { L'R',L't',L'l',L'G',L'e',L't',L'V',L'e',L'r',L's',L'i',L'o',L'n',L'\0' };
    pAPI_information->ImportTable.pRtlGetVersion = GetFunByExportName(NT_Base, RtlGetVersion_Text);
    if (pAPI_information->ImportTable.pRtlGetVersion == NULL) allAPIsValid = FALSE;

    wchar_t MmGetPhysicalMemoryRanges_Text[] = { L'M',L'm',L'G',L'e',L't',L'P',L'h',L'y',L's',L'i',L'c',L'a',L'l',L'M',L'e',L'm',L'o',L'r',L'y',L'R',L'a',L'n',L'g',L'e',L's',L'\0' };
    pAPI_information->ImportTable.pMmGetPhysicalMemoryRanges = GetFunByExportName(NT_Base, MmGetPhysicalMemoryRanges_Text);
    if (pAPI_information->ImportTable.pMmGetPhysicalMemoryRanges == NULL) allAPIsValid = FALSE;

    wchar_t ExFreePoolWithTag_Text[] = { L'E',L'x',L'F',L'r',L'e',L'e',L'P',L'o',L'o',L'l',L'W',L'i',L't',L'h',L'T',L'a',L'g',L'\0' };
    pAPI_information->ImportTable.pExFreePoolWithTag = GetFunByExportName(NT_Base, ExFreePoolWithTag_Text);
    if (pAPI_information->ImportTable.pExFreePoolWithTag == NULL) allAPIsValid = FALSE;

    wchar_t MmCopyVirtualMemory_Text[] = { L'M',L'm',L'C',L'o',L'p',L'y',L'V',L'i',L'r',L't',L'u',L'a',L'l',L'M',L'e',L'm',L'o',L'r',L'y',L'\0' };
    pAPI_information->ImportTable.pMmCopyVirtualMemory = GetFunByExportName(NT_Base, MmCopyVirtualMemory_Text);
    if (pAPI_information->ImportTable.pMmCopyVirtualMemory == NULL) allAPIsValid = FALSE;

    wchar_t IoGetCurrentProcess_Text[] = { L'I',L'o',L'G',L'e',L't',L'C',L'u',L'r',L'r',L'e',L'n',L't',L'P',L'r',L'o',L'c',L'e',L's',L's',L'\0' };
    pAPI_information->ImportTable.pIoGetCurrentProcess = GetFunByExportName(NT_Base, IoGetCurrentProcess_Text);
    if (pAPI_information->ImportTable.pIoGetCurrentProcess == NULL) allAPIsValid = FALSE;

    wchar_t ZwQueryValueKey_Text[] = { L'Z',L'w',L'Q',L'u',L'e',L'r',L'y',L'V',L'a',L'l',L'u',L'e',L'K',L'e',L'y',L'\0' };
    pAPI_information->ImportTable.pZwQueryValueKey = GetFunByExportName(NT_Base, ZwQueryValueKey_Text);
    if (pAPI_information->ImportTable.pZwQueryValueKey == NULL)allAPIsValid = FALSE;

    wchar_t ZwOpenKey_Text[] = { L'Z',L'w',L'O',L'p',L'e',L'n',L'K',L'e',L'y',L'\0' };
    pAPI_information->ImportTable.pZwOpenKey = GetFunByExportName(NT_Base, ZwOpenKey_Text);
    if (pAPI_information->ImportTable.pZwOpenKey == NULL)allAPIsValid = FALSE;

    wchar_t ObCloseHandle_Text[] = { L'O',L'b',L'C',L'l',L'o',L's',L'e',L'H',L'a',L'n',L'd',L'l',L'e',L'\0' };
    pAPI_information->ImportTable.pObCloseHandle = GetFunByExportName(NT_Base, ObCloseHandle_Text);
    if (pAPI_information->ImportTable.pObCloseHandle == NULL)allAPIsValid = FALSE;

    wchar_t ExAllocatePoolWithTag_Text[] = { L'E',L'x',L'A',L'l',L'l',L'o',L'c',L'a',L't',L'e',L'P',L'o',L'o',L'l',L'W',L'i',L't',L'h',L'T',L'a',L'g',L'\0' };
    pAPI_information->ImportTable.pExAllocatePoolWithTag = GetFunByExportName(NT_Base, ExAllocatePoolWithTag_Text);
    if (pAPI_information->ImportTable.pExAllocatePoolWithTag == NULL)allAPIsValid = FALSE;

    wchar_t MmCopyMemory_Text[] = { L'M',L'm',L'C',L'o',L'p',L'y',L'M',L'e',L'm',L'o',L'r',L'y',L'\0' };
    pAPI_information->ImportTable.pMmCopyMemory = GetFunByExportName(NT_Base, MmCopyMemory_Text);
    if (pAPI_information->ImportTable.pMmCopyMemory == NULL)allAPIsValid = FALSE;

    return allAPIsValid;
}

__declspec(noinline) VOID Init_Tag() {
    pAPI_information->g_TAG.GSDR = GenerateRandomTag();
    pAPI_information->g_TAG.KMHD = GenerateRandomTag();
    pAPI_information->g_TAG.AUTH = GenerateRandomTag();
    pAPI_information->g_TAG.RECV = GenerateRandomTag();
    pAPI_information->g_TAG.RCFI = GenerateRandomTag();
    pAPI_information->g_TAG.DMKT = GenerateRandomTag();
    pAPI_information->g_TAG.TMPT = GenerateRandomTag();
    pAPI_information->g_TAG.REQ1 = GenerateRandomTag();
    pAPI_information->g_TAG.Send = GenerateRandomTag();
    pAPI_information->g_TAG.Code = GenerateRandomTag();
    pAPI_information->g_TAG.SGOC = GenerateRandomTag();
    pAPI_information->g_TAG.NAME = GenerateRandomTag();
}

__declspec(noinline) NTSTATUS InitializeDriver(ULONG64 NT_Base) {
    pAPI_information->DynamicData = (PDYNDATA)((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPoolNx, (sizeof(DYNDATA)), pAPI_information->g_TAG.GSDR);
    if (!pAPI_information->DynamicData) return STATUS_INSUFFICIENT_RESOURCES;
    ZeroMemory_CPU(pAPI_information->DynamicData, sizeof(DYNDATA));

    pAPI_information->DynamicData->KernelBase = (PBYTE)NT_Base;
    if (!pAPI_information->DynamicData->KernelBase) {
        if (pAPI_information->DynamicData) ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(pAPI_information->DynamicData, pAPI_information->g_TAG.GSDR);
        return STATUS_NOT_FOUND;
    }

    pAPI_information->pRegisterNotifyHookBuffer = (PHOOK_NOTIFY_BUFFER)
        ((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPoolNx, (sizeof(HOOK_NOTIFY_BUFFER)), pAPI_information->g_TAG.GSDR);

    if (!pAPI_information->pRegisterNotifyHookBuffer) {
        if (pAPI_information->DynamicData) ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(pAPI_information->DynamicData, pAPI_information->g_TAG.GSDR);
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    ZeroMemory_CPU(pAPI_information->pRegisterNotifyHookBuffer, sizeof(HOOK_NOTIFY_BUFFER));
    return STATUS_SUCCESS;
}

__declspec(noinline) NTSTATUS Init_Base(PDRIVER_OBJECT DriverObject) {
    PLDR_DATA_TABLE_ENTRY NtoskrnlLdr = 0;
    PLDR_DATA_TABLE_ENTRY NEoskrnlLdr = 0;
    ULONG64 NT_Base = 0;
    ULONG64 NE_Base = 0;

    wchar_t text_ntoskrnl_exe[] = { L'n',L't',L'o',L's',L'k',L'r',L'n',L'l',L'.',L'e',L'x',L'e',L'\0' };
    NT_Base = (ULONG64)FindBase(DriverObject, text_ntoskrnl_exe, &NtoskrnlLdr);
    if (!NT_Base) return STATUS_NOT_FOUND;

    wchar_t text_NETI_SYS[] = { L'N', L'E', L'T',L'I',L'O',L'.',L'S', L'Y', L'S', L'\0' };
    NE_Base = (ULONG64)FindBase(DriverObject, text_NETI_SYS, &NEoskrnlLdr);
    if (!NE_Base) return STATUS_NOT_FOUND;

    NtoskrnlLdr->Flags |= 0x20;
    NEoskrnlLdr->Flags |= 0x20;

    wchar_t ExAllocatePool_Text[] = { L'E',L'x',L'A',L'l',L'l',L'o',L'c',L'a',L't',L'e',L'P',L'o',L'o',L'l',L'\0' };
    ULONG64 pExAllocatePool = GetFunByExportName(NT_Base, ExAllocatePool_Text);
    if (pExAllocatePool == NULL) return STATUS_ENTRYPOINT_NOT_FOUND;

    pAPI_information = (PAPI_information)((decltype(ExAllocatePool)*)pExAllocatePool)(NonPagedPoolNx, sizeof(API_information));
    if (pAPI_information == NULL) return STATUS_INSUFFICIENT_RESOURCES;
    pExAllocatePool = 0;
    ZeroMemory_CPU(pAPI_information, sizeof(API_information));

    if (!GetApiAddress(NT_Base, NE_Base)) return STATUS_ENTRYPOINT_NOT_FOUND;
    Init_Tag();

    pAPI_information->g_DriverObject = DriverObject;
    pAPI_information->NtoskrnlLdr = NtoskrnlLdr;

    NTSTATUS Status = InitializeDriver(NT_Base);
    if (!NT_SUCCESS(Status)) return Status;

    NT_Base = 0;
    NE_Base = 0;
}

__forceinline LPBYTE SearchSignForMemory(LPBYTE MemoryBase, DWORD Length, PCHAR Pattern, PCHAR Mask) {
    for (DWORD Index = 0; Index < Length - 2; Index++) {
        LPBYTE pAddress = MemoryBase + Index;
        BOOL match = TRUE;
        for (SIZE_T i = 0; i < 2; i++) {
            if (Mask[i] == 'x' && pAddress[i] != (BYTE)Pattern[i]) {
                match = FALSE;
                break;
            }
        }

        if (match) return pAddress;
    }
    return NULL;
}

__declspec(noinline) NTSTATUS RegisterNotifyInit(BOOLEAN Enable) {
    NTSTATUS Status = STATUS_UNSUCCESSFUL;
    if (pAPI_information->pRegisterNotifyHookBuffer->Enable != Enable) {
        if (pAPI_information->pRegisterNotifyHookBuffer->HookPoint == NULL) {
            LPBYTE ImageBase = pAPI_information->DynamicData->KernelBase;

            if (ImageBase != NULL) {
                PIMAGE_NT_HEADERS Headers = (PIMAGE_NT_HEADERS)(ImageBase + ((PIMAGE_DOS_HEADER)ImageBase)->e_lfanew);
                PIMAGE_SECTION_HEADER Sections = IMAGE_FIRST_SECTION(Headers);

                for (DWORD Index = NULL; Index < Headers->FileHeader.NumberOfSections; ++Index) {
                    PIMAGE_SECTION_HEADER pSection = &Sections[Index];

                    if (!memcmp(pSection->Name, ".text", 5)) {
                        pAPI_information->pRegisterNotifyHookBuffer->HookPoint = SearchSignForMemory(ImageBase + pSection->VirtualAddress, pSection->Misc.VirtualSize, "\xFF\xE1", "xx");
                        break;
                    }
                }
            }
        }

        if (pAPI_information->pRegisterNotifyHookBuffer->HookPoint != NULL) {
            if (Enable == TRUE) {
                Status = ((decltype(CmRegisterCallback)*)pAPI_information->ImportTable.pCmRegisterCallback)
                    ((PEX_CALLBACK_FUNCTION)(pAPI_information->pRegisterNotifyHookBuffer->HookPoint), RegisterNotify, &pAPI_information->pRegisterNotifyHookBuffer->Cookie);
                if (NT_SUCCESS(Status)) pAPI_information->pRegisterNotifyHookBuffer->Enable = TRUE;
            }
            else {
                Status = ((decltype(CmUnRegisterCallback)*)pAPI_information->ImportTable.pCmUnRegisterCallback)(pAPI_information->pRegisterNotifyHookBuffer->Cookie);
                if (NT_SUCCESS(Status)) pAPI_information->pRegisterNotifyHookBuffer->Enable = FALSE;
            }
        }
    }

    if (pAPI_information->pRegisterNotifyHookBuffer->Enable == Enable) Status = STATUS_SUCCESS;

    return Status;
}

__declspec(noinline) VOID CleanupDriver(PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    if (pAPI_information->pRegisterNotifyHookBuffer) {
        RegisterNotifyInit(FALSE);
        if (pAPI_information->pRegisterNotifyHookBuffer) {
            ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(pAPI_information->pRegisterNotifyHookBuffer, pAPI_information->g_TAG.GSDR);
        }
        pAPI_information->pRegisterNotifyHookBuffer = NULL;
    }

    if (pAPI_information->DynamicData) {
        if (pAPI_information->DynamicData) {
            ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(pAPI_information->DynamicData, pAPI_information->g_TAG.GSDR);
        }
        pAPI_information->DynamicData = NULL;
    }

    if (pAPI_information) ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(pAPI_information, 0);
}

__declspec(noinline) NTSTATUS A00_Entry(PDRIVER_OBJECT DriverObject) {
    DriverObject->DriverUnload = CleanupDriver;

    ((PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection)->Flags |= 0x20;

    NTSTATUS Status = Init_Base(DriverObject);
    if (!NT_SUCCESS(Status)) return Status;

    Status = RegisterNotifyInit(TRUE);
    if (!NT_SUCCESS(Status)) {
        CleanupDriver(DriverObject);
        return STATUS_CALLBACK_BYPASS;
    }

    return STATUS_SUCCESS;
}