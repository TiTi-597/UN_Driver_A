#define T_TRUE     0xD0000000
#define T_FALSE    0xD0000001

typedef struct _MEMORY_TAG {
	ULONG GSDR;
	ULONG KMHD;
	ULONG AUTH;
	ULONG RECV;
	ULONG RCFI;
	ULONG DMKT;
	ULONG TMPT;
	ULONG REQ1;
	ULONG Send;
	ULONG Code;
	ULONG SGOC;
	ULONG NAME;
} MEMORY_TAG, * PMEMORY_TAG;

typedef struct _HOOK_NOTIFY_BUFFER {
	ULONG Enable;
	PVOID HookPoint;
	UCHAR NewBytes[13];
	UCHAR OldBytes[13];
	PVOID NotifyHandle;
	LARGE_INTEGER Cookie;
} HOOK_NOTIFY_BUFFER, * PHOOK_NOTIFY_BUFFER;
typedef struct _DYNDATA {
	ULONG UserVerify;
	ULONG Key;
	PBYTE KernelBase;
} DYNDATA, * PDYNDATA;

typedef struct _PEB_LDR_DATA32 {
	ULONG Length;
	UCHAR Initialized;
	ULONG SsHandle;
	LIST_ENTRY32 InLoadOrderModuleList;
	LIST_ENTRY32 InMemoryOrderModuleList;
	LIST_ENTRY32 InInitializationOrderModuleList;
	ULONG EntryInProgress;
	UCHAR ShutdownInProgress;
	ULONG ShutdownThreadId;
}PEB_LDR_DATA32, * PPEB_LDR_DATA32;
typedef struct _PEB_LDR_DATA64 {
	ULONG Length;
	UCHAR Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
	UCHAR ShutdownInProgress;
	PVOID ShutdownThreadId;
}PEB_LDR_DATA64, * PPEB_LDR_DATA64;
typedef struct _PEB32 {
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR Spare;
	ULONG Mutant;
	ULONG ImageBaseAddress;
	ULONG Ldr;
} PEB32, * PPEB32;
typedef struct _PEB64 {
	UCHAR InheritedAddressSpace;
	UCHAR ReadImageFileExecOptions;
	UCHAR BeingDebugged;
	UCHAR Spare;
	UCHAR Padding0[4];
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PEB_LDR_DATA64* Ldr;
}PEB64, * PPEB64;
typedef struct _LDR_DATA_TABLE_ENTRY32 {
	LIST_ENTRY32 InLoadOrderLinks;
	LIST_ENTRY32 InMemoryOrderLinks;
	LIST_ENTRY32 InInitializationOrderLinks;
	ULONG DllBase;
	ULONG EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING32 FullDllName;
	UNICODE_STRING32 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY32 HashLinks;
	ULONG SectionPointer;
	ULONG CheckSum;
	ULONG TimeDateStamp;
	ULONG LoadedImports;
	ULONG EntryPointActivationContext;
	ULONG PatchInformation;
	LIST_ENTRY32 ForwarderLinks;
	LIST_ENTRY32 ServiceTagLinks;
	LIST_ENTRY32 StaticLinks;
	ULONG ContextInformation;
	ULONG OriginalBase;
	LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;
typedef struct _LDR_DATA_TABLE_ENTRY64 {
	LIST_ENTRY64 InLoadOrderLinks;
	LIST_ENTRY64 InMemoryOrderLinks;
	LIST_ENTRY64 InInitializationOrderLinks;
	ULONG64 DllBase;
	ULONG64 EntryPoint;
	ULONG64 SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY64 HashLinks;
	ULONG64 SectionPointer;
	ULONG64 CheckSum;
	ULONG64 TimeDateStamp;
	ULONG64 LoadedImports;
	ULONG64 EntryPointActivationContext;
	ULONG64 PatchInformation;
	LIST_ENTRY64 ForwarderLinks;
	LIST_ENTRY64 ServiceTagLinks;
	LIST_ENTRY64 StaticLinks;
	ULONG64 ContextInformation;
	ULONG64 OriginalBase;
	LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	short LoadCount;
	short TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PVOID* EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _MOUSE_INPUT_DATA {
	USHORT UnitId;
	USHORT Flags;
	union {
		ULONG Buttons;
		struct {
			USHORT ButtonFlags;
			USHORT ButtonData;
		};
	};
	ULONG RawButtons;
	LONG LastX;
	LONG LastY;
	ULONG ExtraInformation;
} MOUSE_INPUT_DATA, * PMOUSE_INPUT_DATA;
typedef struct _KEYBOARD_INPUT_DATA {
	USHORT UnitId;
	USHORT MakeCode;
	USHORT Flags;
	USHORT Reserved;
	ULONG ExtraInformation;
} KEYBOARD_INPUT_DATA, * PKEYBOARD_INPUT_DATA;
typedef VOID(*_MouseClassServiceCallback)(PDEVICE_OBJECT, PMOUSE_INPUT_DATA, PMOUSE_INPUT_DATA, PULONG);
typedef VOID(*_KeyboardClassServiceCallback)(PDEVICE_OBJECT, PKEYBOARD_INPUT_DATA, PKEYBOARD_INPUT_DATA, PULONG);

typedef struct _DriverControl {
	ULONG64 Address;
	ULONG64 Buffer;
	ULONG64 Size;
	ULONG64 Type;
} DriverControl, * PDriverControl;

typedef struct _API_information {
	struct {
		ULONG64 pKeSetEvent;
		ULONG64 pKeWaitForSingleObject;
		ULONG64 pZwWaitForSingleObject;

		ULONG64 pMmProbeAndLockPages;
		ULONG64 pMmUnlockPages;
		ULONG64 pExAllocatePoolWithTag;
		ULONG64 pExFreePoolWithTag;
		ULONG64 pMmAllocateMappingAddress;
		ULONG64 pMmGetVirtualForPhysical;
		ULONG64 pMmGetPhysicalMemoryRanges;
		ULONG64 pZwAllocateVirtualMemory;
		ULONG64 pZwFreeVirtualMemory;
		ULONG64 pZwQueryVirtualMemory;
		ULONG64 pZwProtectVirtualMemory;
		ULONG64 pMmCopyVirtualMemory;
		ULONG64 pMmIsAddressValid;
		ULONG64 pIoAllocateMdl;
		ULONG64 pIoFreeMdl;
		ULONG64 pMmCopyMemory;

		ULONG64 pRtlCreateUserThread;
		ULONG64 pIoGetCurrentProcess;
		ULONG64 pPsLookupProcessByProcessId;
		ULONG64 pObfDereferenceObject;
		ULONG64 pPsGetProcessWow64Process;
		ULONG64 pPsGetProcessPeb;
		ULONG64 pPsGetProcessSectionBaseAddress;
		ULONG64 pPsProcessType;

		ULONG64 pObReferenceObjectByName;
		ULONG64 pObOpenObjectByPointer;
		ULONG64 pObCloseHandle;

		ULONG64 pIoDriverObjectType;
		ULONG64 pIoFreeIrp;
		ULONG64 pIoAllocateIrp;

		ULONG64 pRtlAnsiStringToUnicodeString;
		ULONG64 pRtlGetVersion;
		ULONG64 p_vsnprintf;

		ULONG64 pCmRegisterCallback;
		ULONG64 pCmUnRegisterCallback;

		ULONG64 pWskCaptureProviderNPI;
		ULONG64 pWskReleaseProviderNPI;
		ULONG64 pWskDeregister;
		ULONG64 pWskRegister;

		ULONG64 pZwClose;
		ULONG64 pZwQueryValueKey;
		ULONG64 pZwOpenKey;
	} ImportTable;

	struct {	
		ULONG64 CurrentCr3;
		void* MmPfnDataBase;
	} CR3;

	struct {
		PEPROCESS Process;
	}gProcess;

	struct {
		ULONG64 PteBase;
		ULONG64 Ptes;
		ULONG64 MapMemory;
		CHAR threadUseMap[256];
		ULONG64 threadDataBuffer[256];
		ULONG64 threadDataBufferSize[256];
	} Invlpg;

	PDEVICE_OBJECT MouseDeviceObject;
	PDEVICE_OBJECT KeyboardDeviceObject;
	_MouseClassServiceCallback MouseClassServiceCallback;
	_KeyboardClassServiceCallback KeyboardClassServiceCallback;

	WSK_REGISTRATION g_WskRegistration;
	WSK_PROVIDER_NPI g_WskProvider;
	WSK_CLIENT_DISPATCH g_WskDispatch;
	LONG g_SocketsState;

	PDRIVER_OBJECT g_DriverObject;
	PHOOK_NOTIFY_BUFFER pRegisterNotifyHookBuffer;
	PDYNDATA DynamicData;
	PLDR_DATA_TABLE_ENTRY NtoskrnlLdr;
	MEMORY_TAG g_TAG;
}API_information, * PAPI_information;

PAPI_information pAPI_information = NULL;

EXTERN_C PPEB64 PsGetProcessPeb(__in PEPROCESS Process);
EXTERN_C PPEB32 PsGetProcessWow64Process(__in PEPROCESS Process);
NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(__in PEPROCESS Process);
EXTERN_C NTKERNELAPI NTSTATUS RtlCreateUserThread(HANDLE ProcessHandle, PSECURITY_DESCRIPTOR SecurityDescriptor, BOOLEAN CreateSuspended, ULONG StackZeroBits, SIZE_T StackReserve, SIZE_T StackCommit, PVOID StartAddress, PVOID StartParameter, PHANDLE ThreadHandle, PVOID ClientID);
EXTERN_C NTKERNELAPI NTSTATUS ZwProtectVirtualMemory(HANDLE ProcessHandle, PVOID* BaseAddress, SIZE_T* NumberOfBytesToProtect, ULONG NewAccessProtection, PULONG OldAccessProtection);
EXTERN_C NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS FromProcess, PVOID FromAddress, PEPROCESS ToProcess, PVOID ToAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T NumberOfBytesCopied);
EXTERN_C POBJECT_TYPE* IoDriverObjectType;
EXTERN_C NTSTATUS ObReferenceObjectByName(PUNICODE_STRING, ULONG, PACCESS_STATE, ACCESS_MASK, POBJECT_TYPE, KPROCESSOR_MODE, PVOID, PDRIVER_OBJECT*);