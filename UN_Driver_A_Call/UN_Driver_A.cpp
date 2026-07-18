#include "UN_Driver_A.hpp"

typedef LONG(__stdcall* _NtSetValueKey)(HANDLE, PVOID, DWORD, DWORD, PVOID, DWORD);
typedef LONG(__stdcall* _RtlOpenCurrentUser)(ACCESS_MASK, PHANDLE);

_NtSetValueKey pNtSetValueKey = NULL;
_RtlOpenCurrentUser pRtlOpenCurrentUser = NULL;

__declspec(noinline) BOOL __stdcall INITNTAPI() {
    HMODULE ntdllBase = LI_FN(LoadLibraryA)(xorstr_("ntdll.dll"));

    if (!ntdllBase) goto cleanup;

    pNtSetValueKey = (_NtSetValueKey)(LI_FN(GetProcAddress)(ntdllBase, xorstr_("NtSetValueKey")));
    if (!pNtSetValueKey) goto cleanup;

    pRtlOpenCurrentUser = (_RtlOpenCurrentUser)(LI_FN(GetProcAddress)(ntdllBase, xorstr_("RtlOpenCurrentUser")));
    if (!pRtlOpenCurrentUser) goto cleanup;

    return TRUE;

cleanup:
    if (ntdllBase) LI_FN(FreeLibrary)(ntdllBase);

    return FALSE;
}

#define SUCCESS_CODE 0xD0000000

HANDLE DriverHandle = NULL;

LONG WINAPI Call(DWORD Msg, PVOID Data, DWORD DataSize) {
    if (DriverHandle == NULL) pRtlOpenCurrentUser(GENERIC_WRITE, &DriverHandle);
    if (DriverHandle == NULL) return -1;
    LPVOID NTData[3] = { 0 };
    NTData[0] = 0;
    NTData[1] = 0;
    NTData[2] = &NTData;
    LONG Ret = pNtSetValueKey(DriverHandle, &NTData, NULL, Msg, Data, DataSize);
    return Ret;
}

ULONG64 Loadjudgment() {
    ULONG64 data[1] = { 0 };
    ULONG64 Buffer = 0;
    data[0] = (ULONG64)&Buffer;
    Call('0001', &data, sizeof(data));
    return Buffer;
}

BOOL ReadMemory(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 0;
    return Call('0002', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL WriteMemory(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 1;
    return Call('0002', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL WriteMemory_X(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 2;
    return Call('0002', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL ReadMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 0;
    return Call('0004', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL WriteMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 1;
    return Call('0004', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL ReadKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 0;
    return Call('0021', &data, sizeof(data)) == SUCCESS_CODE;
}

BOOL WriteKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    ULONG64 data[4] = { 0 };
    data[0] = Address;
    data[1] = (ULONG64)Buffer;
    data[2] = Size;
    data[3] = 1;
    return Call('0021', &data, sizeof(data)) == SUCCESS_CODE;
}

ULONG64 GetProcessPEB() {
    ULONG64 data[1] = { 0 };
    ULONG64 buffer = 0;
    data[0] = (ULONG64)&buffer;
    Call('0006', &data, sizeof(data));
    return buffer;
}

ULONG64 GetMainModuleAddress() {
    ULONG64 data[1] = { 0 };
    ULONG64 buffer = 0;
    data[0] = (ULONG64)&buffer;
    Call('0005', &data, sizeof(data));
    return buffer;
}

ULONG64 GetModuleAddress(CHAR* ModuleName, ULONG64 Type) {
    ULONG64 data[3] = { 0 };
    ULONG64 buffer = 0;
    data[0] = (ULONG64)ModuleName;
    data[1] = (ULONG64)&buffer;
    data[2] = Type;
    Call('0007', &data, sizeof(data));
    return buffer;
}

ULONG64 GetKernelModuleAddress(CHAR* ModuleName) {
    ULONG64 data[2] = { 0 };
    ULONG64 buffer = 0;
    data[0] = (ULONG64)ModuleName;
    data[1] = (ULONG64)&buffer;
    Call('0018', &data, sizeof(data));
    return buffer;
}

BOOL CreateRemoteThread(ULONG64 StartAddress, ULONG64 ParameterAddress) {
    ULONG64 data[6] = { 0 };
    data[0] = StartAddress;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    data[4] = ParameterAddress;
    data[5] = 0;
    return (Call('0008', &data, sizeof(data)) == SUCCESS_CODE);
}

ULONG64 RequestMemory(ULONG64 Size, ULONG64 Attribute) {
    ULONG64 data[6] = { 0 };
    ULONG64 buffer = 0;
    data[0] = 0;
    data[1] = Size;
    data[2] = (ULONG64)&buffer;
    data[3] = MEM_RESERVE | MEM_COMMIT;
    data[4] = Attribute;
    data[5] = 1;
    Call('0008', &data, sizeof(data));
    return buffer;
}

ULONG64 RequestHighMemory(ULONG64 Size, ULONG64 Attribute) {
    ULONG64 data[6] = { 0 };
    ULONG64 buffer = 0;
    data[0] = 0;
    data[1] = Size;
    data[2] = (ULONG64)&buffer;
    data[3] = MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN;
    data[4] = Attribute;
    data[5] = 1;
    Call('0008', &data, sizeof(data));
    return buffer;
}

ULONG64 RequestDesignatedMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute) {
    ULONG64 data[6] = { 0 };
    ULONG64 buffer = 0;
    data[0] = Address;
    data[1] = Size;
    data[2] = (ULONG64)&buffer;
    data[3] = MEM_RESERVE | MEM_COMMIT;
    data[4] = Attribute;
    data[5] = 1;
    Call('0008', &data, sizeof(data));
    return buffer;
}

BOOL FreeMemory(ULONG64 Address, ULONG64 Size) {
    ULONG64 data[6] = { 0 };
    data[0] = Address;
    data[1] = Size;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 2;
    return (Call('0008', &data, sizeof(data)) == SUCCESS_CODE);
}

ULONG64 ProtectMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute) {
    ULONG64 data[6] = { 0 };
    ULONG64 buffer = 0;
    data[0] = Address;
    data[1] = Size;
    data[2] = (ULONG64)&buffer;
    data[3] = 0;
    data[4] = Attribute;
    data[5] = 3;
    Call('0008', &data, sizeof(data));
    return buffer;
}

ULONG64 AttributesQuery(ULONG64 Address, ULONG64 &BaseAddress, ULONG64 &AllocationProtect, ULONG64 &Size) {
    ULONG64 data[6] = { 0 };
    ULONG64 SizeBuffer = 0;
    ULONG64 BaseAddressBuffer = 0;
    ULONG64 AllocationProtectBuffer = 0;
    ULONG64 ProtectBuffer = 0;
    data[0] = Address;
    data[1] = (ULONG64)&SizeBuffer;
    data[2] = (ULONG64)&BaseAddressBuffer;
    data[3] = (ULONG64)&AllocationProtectBuffer;
    data[4] = (ULONG64)&ProtectBuffer;
    data[5] = 4;
    Call('0008', &data, sizeof(data));
    BaseAddress = BaseAddressBuffer;
    AllocationProtect = AllocationProtectBuffer;
    Size = SizeBuffer;
    return ProtectBuffer;
}

BOOL KernelPacketSending(ULONG64 IP1, ULONG64 IP2, ULONG64 IP3, ULONG64 IP4, ULONG64 Port, CHAR* SendData, ULONG64 SendDataLength, ULONG64 MaxReceivedSize, PCHAR* ReceivedData, ULONG64* ReceivedLength) {
    ULONG64 data[10] = { 0 };
    BYTE DATA[4096] = { 0 };
    BYTE size[8] = { 0 };
    data[0] = IP1;
    data[1] = IP2;
    data[2] = IP3;
    data[3] = IP4;
    data[4] = Port;
    data[5] = (ULONG64)SendData;
    data[6] = SendDataLength;
    data[7] = MaxReceivedSize;
    data[8] = (ULONG64)&DATA;
    data[9] = (ULONG64)&size;

    BOOL result = Call('0011', &data, sizeof(data)) == SUCCESS_CODE;

    *ReceivedData = (PCHAR)DATA;
    if (ReceivedLength != NULL)  *ReceivedLength = *(ULONG64*)size;
    return result;
}

ULONG64 FeatureCodeSearch(ULONG64 Address, PCHAR Code, ULONG64 CodeSize, ULONG64 Attribute) {
    ULONG64 data[5] = { 0 };
    ULONG64 buffer = 0;
    data[0] = Address;
    data[1] = (ULONG64)Code;
    data[2] = CodeSize;
    data[3] = Attribute;
    data[4] = (ULONG64)&buffer;

    Call('0012', &data, sizeof(data));
    return buffer;
}

INT FeatureCodeSearchEX(ULONG64 Address, PCHAR Code, ULONG64 CodeSize, ULONG64 Attribute, std::vector<ULONG64>& AddressArray) {
    ULONG64 CodeAddress = 0;
    AddressArray.clear();
    while (TRUE) {
        CodeAddress = FeatureCodeSearch(Address, Code, CodeSize, Attribute);
        if (CodeAddress == 0) break;
        AddressArray.push_back(CodeAddress);
        Address = CodeAddress;
    }
    return AddressArray.size();
}

BOOL MouseCallback(MOUSE_INPUT_DATA MOUSE_INPUT_DATA) {
    return (Call('0013', &MOUSE_INPUT_DATA, sizeof(MOUSE_INPUT_DATA)) == SUCCESS_CODE);
}

BOOL MouseRelativeMovement(INT x, INT y) {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.Flags = 0;
    InputData.LastX = x;
    InputData.LastY = y;
    return MouseCallback(InputData);
}

BOOL MouseAbsoluteMovement(INT x, INT y) {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.Flags = 1;
    InputData.LastX = x * 65535 / LI_FN(GetSystemMetrics)(SM_CXSCREEN);
    InputData.LastY = y * 65535 / LI_FN(GetSystemMetrics)(SM_CYSCREEN);
    return MouseCallback(InputData);
}

BOOL LeftMouseButtonPressed() {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.ButtonFlags = 1;
    return MouseCallback(InputData);
}

BOOL LeftMouseButtonReleased() {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.ButtonFlags = 2;
    return MouseCallback(InputData);
}

BOOL RightMouseButtonPressed() {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.ButtonFlags = 4;
    return MouseCallback(InputData);
}

BOOL RightMouseButtonReleased() {
    MOUSE_INPUT_DATA InputData = { 0 };
    InputData.ButtonFlags = 8;
    return MouseCallback(InputData);
}

BOOL KeyboardCallback(KEYBOARD_INPUT_DATA KEYBOARD_INPUT_DATA) {
    return (Call('0014', &KEYBOARD_INPUT_DATA, sizeof(KEYBOARD_INPUT_DATA)) == SUCCESS_CODE);
}

BOOL KeyPressed(INT KeyCode) {
    KEYBOARD_INPUT_DATA InputData = { 0 };
    InputData.MakeCode = LI_FN(MapVirtualKeyA)(KeyCode, NULL);
    InputData.Flags = 0;
    return KeyboardCallback(InputData);
}

BOOL KeyRelease(INT KeyCode) {
    KEYBOARD_INPUT_DATA InputData = { 0 };
    InputData.MakeCode = LI_FN(MapVirtualKeyA)(KeyCode, NULL);
    InputData.Flags = 1;
    return KeyboardCallback(InputData);
}

BOOL SetProcess(ULONG64 ProcessId, BOOL DecryptCR3) {
    ULONG64 data[2] = { 0 };
    data[0] = ProcessId;
    data[1] = DecryptCR3 ? 1 : 0;
    return  Call('0016', &data, sizeof(data)) == SUCCESS_CODE;
}

BYTE ReadByte(ULONG64 Address){
    BYTE buf = '\0';
    ReadMemory(Address, (PBYTE)&buf, sizeof(BYTE));
    return buf;
}

SHORT ReadShort(ULONG64 Address){
    SHORT buf = 0;
    ReadMemory(Address, (PBYTE)&buf, sizeof(SHORT));
    return buf;
}

ULONG ReadInt(ULONG64 Address){
    INT buf = 0;
    ReadMemory(Address, (PBYTE)&buf, sizeof(INT));
    return buf;
}

INT64 ReadInt64(ULONG64 Address){
    INT64 buf = 0;
    ReadMemory(Address, (PBYTE)&buf, sizeof(INT64));
    return buf;
}

FLOAT ReadFloat(ULONG64 Address){
    FLOAT buf = 0.0f;
    ReadMemory(Address, (PBYTE)&buf, sizeof(FLOAT));
    return buf;
}

BOOL ReadBytes(ULONG64 Address, PBYTE Buffer, ULONG Size){
    return ReadMemory(Address, Buffer, Size);
}

BOOL WriteByte(ULONG64 Address, BYTE Buffer){
    return WriteMemory(Address, (PBYTE)&Buffer, sizeof(BYTE));
}

BOOL WriteShort(ULONG64 Address, SHORT Buffer){
    return WriteMemory(Address, (PBYTE)&Buffer, sizeof(SHORT));
}

BOOL WriteInt(ULONG64 Address, INT Buffer){
    return WriteMemory(Address, (PBYTE)&Buffer, sizeof(INT));
}

BOOL WriteInt64(ULONG64 Address, INT64 Buffer){
    return WriteMemory(Address, (PBYTE)&Buffer, sizeof(INT64));
}

BOOL WriteFloat(ULONG64 Address, FLOAT Buffer){
    return WriteMemory(Address, (PBYTE)&Buffer, sizeof(FLOAT));
}

BOOL WriteBytes(ULONG64 Address, PBYTE Buffer, ULONG Size){
    return WriteMemory(Address, Buffer, Size);
}

BOOL WriteByte_X(ULONG64 Address, BYTE Buffer) {
    return WriteMemory_X(Address, (PBYTE)&Buffer, sizeof(BYTE));
}

BOOL WriteShort_X(ULONG64 Address, SHORT Buffer) {
    return WriteMemory_X(Address, (PBYTE)&Buffer, sizeof(SHORT));
}

BOOL WriteInt_X(ULONG64 Address, INT Buffer) {
    return WriteMemory_X(Address, (PBYTE)&Buffer, sizeof(INT));
}

BOOL WriteInt64_X(ULONG64 Address, INT64 Buffer) {
    return WriteMemory_X(Address, (PBYTE)&Buffer, sizeof(INT64));
}

BOOL WriteFloat_X(ULONG64 Address, FLOAT Buffer) {
    return WriteMemory_X(Address, (PBYTE)&Buffer, sizeof(FLOAT));
}

BOOL WriteBytes_X(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    return WriteMemory_X(Address, Buffer, Size);
}

BYTE ReadByte_Virtual(ULONG64 Address) {
    BYTE buf = '\0';
    ReadMemory_Virtual(Address, (PBYTE)&buf, sizeof(BYTE));
    return buf;
}

SHORT ReadShort_Virtual(ULONG64 Address) {
    SHORT buf = 0;
    ReadMemory_Virtual(Address, (PBYTE)&buf, sizeof(SHORT));
    return buf;
}

ULONG ReadInt_Virtual(ULONG64 Address) {
    INT buf = 0;
    ReadMemory_Virtual(Address, (PBYTE)&buf, sizeof(INT));
    return buf;
}

INT64 ReadInt64_Virtual(ULONG64 Address) {
    INT64 buf = 0;
    ReadMemory_Virtual(Address, (PBYTE)&buf, sizeof(INT64));
    return buf;
}

FLOAT ReadFloat_Virtual(ULONG64 Address) {
    FLOAT buf = 0.0f;
    ReadMemory_Virtual(Address, (PBYTE)&buf, sizeof(FLOAT));
    return buf;
}

BOOL ReadBytes_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    return ReadMemory_Virtual(Address, Buffer, Size);
}

BOOL WriteByte_Virtual(ULONG64 Address, BYTE Buffer) {
    return WriteMemory_Virtual(Address, (PBYTE)&Buffer, sizeof(BYTE));
}

BOOL WriteShort_Virtual(ULONG64 Address, SHORT Buffer) {
    return WriteMemory_Virtual(Address, (PBYTE)&Buffer, sizeof(SHORT));
}

BOOL WriteInt_Virtual(ULONG64 Address, INT Buffer) {
    return WriteMemory_Virtual(Address, (PBYTE)&Buffer, sizeof(INT));
}

BOOL WriteInt64_Virtual(ULONG64 Address, INT64 Buffer) {
    return WriteMemory_Virtual(Address, (PBYTE)&Buffer, sizeof(INT64));
}

BOOL WriteFloat_Virtual(ULONG64 Address, FLOAT Buffer) {
    return WriteMemory_Virtual(Address, (PBYTE)&Buffer, sizeof(FLOAT));
}

BOOL WriteBytes_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    return WriteMemory_Virtual(Address, Buffer, Size);
}

BYTE ReadKernelByte(ULONG64 Address) {
    BYTE buf = '\0';
    ReadKernelMemory(Address, (PBYTE)&buf, sizeof(BYTE));
    return buf;
}

SHORT ReadKernelShort(ULONG64 Address) {
    SHORT buf = 0;
    ReadKernelMemory(Address, (PBYTE)&buf, sizeof(SHORT));
    return buf;
}

ULONG ReadKernelInt(ULONG64 Address) {
    INT buf = 0;
    ReadKernelMemory(Address, (PBYTE)&buf, sizeof(INT));
    return buf;
}

INT64 ReadKernelInt64(ULONG64 Address) {
    INT64 buf = 0;
    ReadKernelMemory(Address, (PBYTE)&buf, sizeof(INT64));
    return buf;
}

FLOAT ReadKernelFloat(ULONG64 Address) {
    FLOAT buf = 0.0f;
    ReadKernelMemory(Address, (PBYTE)&buf, sizeof(FLOAT));
    return buf;
}

BOOL ReadKernelBytes(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    return ReadKernelMemory(Address, Buffer, Size);
}

BOOL WriteKernelByte(ULONG64 Address, BYTE Buffer) {
    return WriteKernelMemory(Address, (PBYTE)&Buffer, sizeof(BYTE));
}

BOOL WriteKernelShort(ULONG64 Address, SHORT Buffer) {
    return WriteKernelMemory(Address, (PBYTE)&Buffer, sizeof(SHORT));
}

BOOL WriteKernelInt(ULONG64 Address, INT Buffer) {
    return WriteKernelMemory(Address, (PBYTE)&Buffer, sizeof(INT));
}

BOOL WriteKernelInt64(ULONG64 Address, INT64 Buffer) {
    return WriteKernelMemory(Address, (PBYTE)&Buffer, sizeof(INT64));
}

BOOL WriteKernelFloat(ULONG64 Address, FLOAT Buffer) {
    return WriteKernelMemory(Address, (PBYTE)&Buffer, sizeof(FLOAT));
}

BOOL WriteKernelBytes(ULONG64 Address, PBYTE Buffer, ULONG Size) {
    return WriteKernelMemory(Address, Buffer, Size);
}