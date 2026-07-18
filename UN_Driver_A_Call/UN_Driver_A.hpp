#include <Windows.h>
#include <filesystem>
#include "importer.hpp"
#include "xorstr.hpp"

typedef struct _MOUSE_INPUT_DATA {
    USHORT UnitId;
    USHORT Flags;
    union {
        ULONG Buttons;
        struct {
            USHORT  ButtonFlags;
            USHORT  ButtonData;
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

BOOL INITNTAPI();
ULONG64 Loadjudgment();

BOOL ReadMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory_X(ULONG64 Address, PBYTE Buffer, ULONG Size);

BOOL ReadMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);

BOOL ReadKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);

ULONG64 GetProcessPEB();
ULONG64 GetMainModuleAddress();
ULONG64 GetModuleAddress(CHAR* ModuleName, ULONG64 Type = 0);
ULONG64 GetKernelModuleAddress(CHAR* ModuleName);

BOOL SetProcess(ULONG64 ProcessId, BOOL DecryptCR3);

ULONG64 FeatureCodeSearch(ULONG64 Address, PCHAR Code, ULONG64 CodeSize, ULONG64 Attribute);
INT FeatureCodeSearchEX(ULONG64 Address, PCHAR Code, ULONG64 CodeSize, ULONG64 Attribute, std::vector<ULONG64>& AddressArray);

BOOL CreateRemoteThread(ULONG64 StartAddress, ULONG64 ParameterAddress);
ULONG64 RequestMemory(ULONG64 Size, ULONG64 Attribute);
ULONG64 RequestHighMemory(ULONG64 Size, ULONG64 Attribute);
ULONG64 RequestDesignatedMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute);
BOOL FreeMemory(ULONG64 Address, ULONG64 Size);
ULONG64 ProtectMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute);
ULONG64 AttributesQuery(ULONG64 Address, ULONG64& BaseAddress, ULONG64& AllocationProtect, ULONG64& Size);

BOOL KeyboardCallback(KEYBOARD_INPUT_DATA KEYBOARD_INPUT_DATA);
BOOL KeyPressed(INT KeyCode);
BOOL KeyRelease(INT KeyCode);
BOOL MouseCallback(MOUSE_INPUT_DATA MOUSE_INPUT_DATA);
BOOL MouseRelativeMovement(INT x, INT y);
BOOL MouseAbsoluteMovement(INT x, INT y);
BOOL LeftMouseButtonPressed();
BOOL LeftMouseButtonReleased();
BOOL RightMouseButtonPressed();
BOOL RightMouseButtonReleased();

BOOL KernelPacketSending(ULONG64 IP1, ULONG64 IP2, ULONG64 IP3, ULONG64 IP4, ULONG64 Port, CHAR* SendData, ULONG64 SendDataLength, ULONG64 MaxReceivedSize, PCHAR* ReceivedData, ULONG64* ReceivedLength);

BYTE ReadByte(ULONG64 Address);
SHORT ReadShort(ULONG64 Address);
ULONG ReadInt(ULONG64 Address);
INT64 ReadInt64(ULONG64 Address);
FLOAT ReadFloat(ULONG64 Address);
BOOL ReadBytes(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteByte(ULONG64 adAddressdress, BYTE Buffer);
BOOL WriteShort(ULONG64 Address, SHORT Buffer);
BOOL WriteInt(ULONG64 Address, INT Buffer);
BOOL WriteInt64(ULONG64 Address, INT64 Buffer);
BOOL WriteFloat(ULONG64 Address, FLOAT Buffer);
BOOL WriteBytes(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteByte_X(ULONG64 Address, BYTE Buffer);
BOOL WriteShort_X(ULONG64 Address, SHORT Buffer);
BOOL WriteInt_X(ULONG64 Address, INT Buffer);
BOOL WriteInt64_X(ULONG64 Address, INT64 Buffer);
BOOL WriteFloat_X(ULONG64 Address, FLOAT Buffer);
BOOL WriteBytes_X(ULONG64 Address, PBYTE Buffer, ULONG Size);

BYTE ReadByte_Virtual(ULONG64 Address);
SHORT ReadShort_Virtual(ULONG64 Address);
ULONG ReadInt_Virtual(ULONG64 Address);
INT64 ReadInt64_Virtual(ULONG64 Address);
FLOAT ReadFloat_Virtual(ULONG64 Address);
BOOL ReadBytes_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteByte_Virtual(ULONG64 Address, BYTE Buffer);
BOOL WriteShort_Virtual(ULONG64 Address, SHORT Buffer);
BOOL WriteInt_Virtual(ULONG64 Address, INT Buffer);
BOOL WriteInt64_Virtual(ULONG64 Address, INT64 Buffer);
BOOL WriteFloat_Virtual(ULONG64 Address, FLOAT Buffer);
BOOL WriteBytes_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);

BYTE ReadKernelByte(ULONG64 Address);
SHORT ReadKernelShort(ULONG64 Address);
ULONG ReadKernelInt(ULONG64 Address);
INT64 ReadKernelInt64(ULONG64 Address);
FLOAT ReadKernelFloat(ULONG64 Address);
BOOL ReadKernelBytes(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteKernelByte(ULONG64 Address, BYTE Buffer);
BOOL WriteKernelShort(ULONG64 Address, SHORT Buffer);
BOOL WriteKernelInt(ULONG64 Address, INT Buffer);
BOOL WriteKernelInt64(ULONG64 Address, INT64 Buffer);
BOOL WriteKernelFloat(ULONG64 Address, FLOAT Buffer);
BOOL WriteKernelBytes(ULONG64 Address, PBYTE Buffer, ULONG Size);