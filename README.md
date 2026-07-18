# UN_Driver

Windows 内核驱动程序开发框架，用于安全研究与教育目的。

## 项目简介

本项目提供了一个完整的 Windows 内核驱动程序开发框架，包含内核驱动层和用户态调用层。主要用于学习和研究 Windows 内核编程、内存管理机制以及驱动开发技术。

### 核心特性

- 通过注册表回调实现隐蔽通信，无设备对象和 IRP 处理
- 支持 Invlpg 页表映射和 MmCopyVirtualMemory 两种内存读写方式
- 内置 CR3 解密支持，可访问进程真实页表
- 完整的内存管理、特征码搜索、输入模拟等功能

## 功能模块

### 1. 进程内存操作

| 功能 | 说明 |
|------|------|
| 进程附加 | 通过 PID 附加目标进程，支持 CR3 解密 |
| 内存读取 | Invlpg 方式（绕过内存保护）和 Virtual 方式 |
| 内存写入 | 支持普通写入和绕过保护写入 |
| 内核内存 | 直接读写内核地址空间 |

### 2. 内存管理

| 功能 | 说明 |
|------|------|
| 内存申请 | 普通申请、高地址申请、指定地址申请 |
| 内存释放 | 释放指定内存区域 |
| 属性修改 | 修改内存保护属性 |
| 属性查询 | 查询内存区域信息 |

### 3. 模块与进程信息

| 功能 | 说明 |
|------|------|
| 主模块基址 | 获取目标进程主模块基址 |
| 模块枚举 | 根据名称获取指定模块基址 |
| 内核模块 | 获取内核模块基址 |
| PEB 解析 | 获取进程 PEB 地址 |

### 4. 特征码搜索

| 功能 | 说明 |
|------|------|
| 单结果搜索 | 返回首个匹配地址 |
| 多结果搜索 | 返回所有匹配地址数组 |

### 5. 输入模拟

| 功能 | 说明 |
|------|------|
| 鼠标移动 | 相对移动、绝对移动 |
| 鼠标按键 | 左键/右键 按下、释放 |
| 键盘输入 | 按键按下、释放 |

### 6. 网络通信

| 功能 | 说明 |
|------|------|
| 内核 Socket | WSK 内核网络通信 |

## 项目结构

```
UN_Driver/
├── UN_Driver_A/                  # 内核驱动程序
│   ├── Main.cpp                  # 驱动入口与注册表回调
│   ├── Driver.hpp                # 驱动结构定义
│   ├── Memory_Tools.hpp          # Invlpg 内存读写
│   ├── Virtual.hpp               # 虚拟内存管理
│   ├── GetMoudle.hpp             # 模块枚举
│   ├── FeatureCode.hpp           # 特征码搜索
│   ├── KeyboardMouse.hpp         # 键盘鼠标模拟
│   ├── Wsk.hpp                   # WSK 网络通信
│   ├── Cr3Decryp.hpp             # CR3 解密
│   ├── CustomFunction.hpp        # 自定义功能
│   └── kli.hpp                   # 内核库接口
│
├── UN_Driver_A_Call/             # 用户态调用程序
│   ├── Main.cpp                  # 测试示例
│   ├── UN_Driver_A.cpp           # 用户态接口实现
│   ├── UN_Driver_A.hpp           # 用户态接口定义
│   ├── importer.hpp              # 函数导入
│   └── xorstr.hpp                # 字符串加密
│
├── UN_Driver.sln                 # Visual Studio 解决方案
├── LICENSE                       # MIT 许可证
└── README.md                     # 项目说明
```

## 编译要求

| 组件 | 版本要求 |
|------|----------|
| Visual Studio | 2019 或更高版本 |
| Windows SDK | 10 |
| Windows Driver Kit (WDK) | 10 |
| 目标平台 | x64 |

## 编译步骤

1. 安装 Visual Studio 2019+ 和 WDK 10
2. 打开 `UN_Driver.sln`
3. 选择 `Release|x64` 配置
4. 构建解决方案
5. 输出文件位于 `x64/Release/` 目录

## 用户态 API 参考

### 驱动管理

```cpp
// 初始化 NT API
BOOL INITNTAPI();

// 检查驱动加载状态（返回 9 表示成功）
ULONG64 Loadjudgment();
```

### 进程操作

```cpp
// 附加目标进程（DecryptCR3: 是否解密 CR3）
BOOL SetProcess(ULONG64 ProcessId, BOOL DecryptCR3);

// 获取进程 PEB
ULONG64 GetProcessPEB();
```

### 内存读写 - Invlpg 方式

```cpp
// 字节集读写
BOOL ReadMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory_X(ULONG64 Address, PBYTE Buffer, ULONG Size);  // 绕过保护

// 类型读写
BYTE   ReadByte(ULONG64 Address);
SHORT  ReadShort(ULONG64 Address);
ULONG  ReadInt(ULONG64 Address);
INT64  ReadInt64(ULONG64 Address);
FLOAT  ReadFloat(ULONG64 Address);

BOOL   WriteByte(ULONG64 Address, BYTE Buffer);
BOOL   WriteShort(ULONG64 Address, SHORT Buffer);
BOOL   WriteInt(ULONG64 Address, INT Buffer);
BOOL   WriteInt64(ULONG64 Address, INT64 Buffer);
BOOL   WriteFloat(ULONG64 Address, FLOAT Buffer);
BOOL   WriteBytes(ULONG64 Address, PBYTE Buffer, ULONG Size);
```

### 内存读写 - Virtual 方式

```cpp
BOOL ReadMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteMemory_Virtual(ULONG64 Address, PBYTE Buffer, ULONG Size);

// 类型读写
BYTE   ReadByte_Virtual(ULONG64 Address);
SHORT  ReadShort_Virtual(ULONG64 Address);
ULONG  ReadInt_Virtual(ULONG64 Address);
// ... 同上
```

### 内核内存读写

```cpp
BOOL ReadKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);
BOOL WriteKernelMemory(ULONG64 Address, PBYTE Buffer, ULONG Size);

BYTE   ReadKernelByte(ULONG64 Address);
ULONG  ReadKernelInt(ULONG64 Address);
// ... 同上
```

### 内存管理

```cpp
// 申请内存
ULONG64 RequestMemory(ULONG64 Size, ULONG64 Attribute);                    // 普通申请
ULONG64 RequestHighMemory(ULONG64 Size, ULONG64 Attribute);                // 高地址申请
ULONG64 RequestDesignatedMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute);  // 指定地址

// 释放内存
BOOL FreeMemory(ULONG64 Address, ULONG64 Size);

// 修改保护属性
ULONG64 ProtectMemory(ULONG64 Address, ULONG64 Size, ULONG64 Attribute);

// 查询内存信息
ULONG64 AttributesQuery(ULONG64 Address, ULONG64& BaseAddress,
                        ULONG64& AllocationProtect, ULONG64& Size);
```

### 模块操作

```cpp
// 获取主模块基址
ULONG64 GetMainModuleAddress();

// 获取指定模块基址（Type: 0=用户模块）
ULONG64 GetModuleAddress(CHAR* ModuleName, ULONG64 Type = 0);

// 获取内核模块基址
ULONG64 GetKernelModuleAddress(CHAR* ModuleName);
```

### 特征码搜索

```cpp
// 单结果搜索
ULONG64 FeatureCodeSearch(ULONG64 Address, PCHAR Code,
                          ULONG64 CodeSize, ULONG64 Attribute);

// 多结果搜索（返回数量）
INT FeatureCodeSearchEX(ULONG64 Address, PCHAR Code,
                        ULONG64 CodeSize, ULONG64 Attribute,
                        std::vector<ULONG64>& AddressArray);
```

### 输入模拟

```cpp
// 鼠标操作
BOOL MouseRelativeMovement(INT x, INT y);     // 相对移动
BOOL MouseAbsoluteMovement(INT x, INT y);     // 绝对移动
BOOL LeftMouseButtonPressed();                 // 左键按下
BOOL LeftMouseButtonReleased();                // 左键释放
BOOL RightMouseButtonPressed();                // 右键按下
BOOL RightMouseButtonReleased();               // 右键释放

// 键盘操作
BOOL KeyPressed(INT KeyCode);                  // 按键按下
BOOL KeyRelease(INT KeyCode);                  // 按键释放
```

### 网络通信

```cpp
// 内核 Socket 发送接收
BOOL KernelPacketSending(ULONG64 IP1, ULONG64 IP2, ULONG64 IP3, ULONG64 IP4,
                         ULONG64 Port, CHAR* SendData, ULONG64 SendDataLength,
                         ULONG64 MaxReceivedSize, PCHAR* ReceivedData,
                         ULONG64* ReceivedLength);
```

## 使用示例

```cpp
#include "UN_Driver_A.hpp"

int main() {
    // 初始化驱动
    INITNTAPI();
    if (Loadjudgment() != 9) {
        printf("驱动加载失败\n");
        return 1;
    }

    // 附加进程
    DWORD pid = 1234;
    if (SetProcess(pid, TRUE)) {
        printf("附加进程成功\n");
    }

    // 获取模块基址
    ULONG64 baseAddr = GetMainModuleAddress();
    ULONG64 ntdllAddr = GetModuleAddress("ntdll.dll", 0);

    // 内存读写
    BYTE buffer[100];
    ReadMemory(baseAddr, buffer, sizeof(buffer));
    WriteInt(baseAddr + 0x100, 12345);

    // 特征码搜索
    CHAR code[] = { 0x48, 0x89, 0x5C };
    std::vector<ULONG64> results;
    int count = FeatureCodeSearchEX(0, code, sizeof(code), 64, results);

    // 鼠标移动
    MouseAbsoluteMovement(500, 500);

    return 0;
}
```

## 技术实现

### 通信机制

驱动通过 `CmRegisterCallback` 注册注册表回调，用户态通过 `RegSetValue` 触发回调实现通信。通信数据通过 Type 字段区分功能码。

### 内存读写方式

1. **Invlpg 方式**：通过修改 PTE 并触发 TLB 刷新实现物理内存映射，可绕过内存保护
2. **Virtual 方式**：使用 `MmCopyVirtualMemory` 进行内核态内存复制
3. **CR3 解密**：通过 MMPFN 数据库解析进程真实页表基址

## 安全声明

本项目仅供安全研究与教育目的使用：

- **学习用途**：帮助开发者理解 Windows 内核编程和安全机制
- **禁止滥用**：严禁用于任何非法活动或侵害他人权益的行为
- **免责声明**：使用本项目所产生的一切后果由使用者自行承担

## 许可证

本项目基于 [MIT License](LICENSE) 开源。

## 参考资料

- [Windows Driver Kit Documentation](https://docs.microsoft.com/en-us/windows-hardware/drivers/)
- [Windows Internals Book](https://docs.microsoft.com/en-us/sysinternals/resources/windows-internals)
- [ReactOS Source Code](https://github.com/reactos/reactos)