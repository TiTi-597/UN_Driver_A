#include "UN_Driver_A.hpp"

INT main() {
    INITNTAPI();

    printf("\n=== 驱动加载测试 ===\n");
    if (Loadjudgment() != 9) {
        printf("驱动加载失败\n");
        system("pause");
        return 1;
    }
    printf("驱动加载成功\n");

    HWND hwnd = FindWindowW(L"Progman", L"Program Manager");
    if (!hwnd) {
        printf("找不到explorer.exe窗口\n");
        system("pause");
        return 1;
    }

    printf("\n=== 设置进程 ===\n");
    DWORD ProcessId;
    GetWindowThreadProcessId(hwnd, &ProcessId);
    printf("进程explorer.exe PID:%d\n", ProcessId);

    if (SetProcess(ProcessId, TRUE)) {
        printf("设置进程成功\n");
    }
    else {
        printf("设置进程失败\n");
    }

    printf("\n=== 进程信息测试 ===\n");
    ULONG64 Address = GetProcessPEB();
    printf("Peb:%llX\n", Address);

    printf("\n=== 特征码搜素测试 ===\n");
    CHAR Code[] = { 0XC3 };
    std::vector<ULONG64> array;
    size_t count = FeatureCodeSearchEX(0, (CHAR*)Code, sizeof(Code), 64, array);

    size_t displayCount = min(count, static_cast<size_t>(10));

    printf("找到 %zu 个匹配地址，显示前 %zu 个：\n", count, displayCount);
    for (size_t i = 0; i < displayCount; i++) {
        printf("Address[%zu]: 0x%llX\n", i, array[i]);
    }

    printf("\n=== 获取模块基址测试 ===\n");

    ULONG64 explorerAddr = GetMainModuleAddress();
    printf("explorer.exe模块基址:%llX\n", explorerAddr);

    ULONG64 ntdllAddr = GetModuleAddress((CHAR*)"ntdll.dll", 0);
    printf("ntdll.dll模块基址:%llX\n", ntdllAddr);

    ULONG64 user32Addr = GetModuleAddress((CHAR*)"user32.dll", 0);
    printf("user32.dll模块基址:%llX\n", user32Addr);

    ULONG64 KernelAddr = GetKernelModuleAddress((CHAR*)"win32k.sys");
    printf("win32k.sys模块基址:%llX\n", KernelAddr);

    printf("\n=== win32k.sys内核内存读取测试 ===\n");
    BYTE Read_Bytes[20] = { 0 };
    if (ReadKernelMemory(KernelAddr, Read_Bytes, 20)) {
        printf("读全win32k.sys模块基址:{ ");
        for (int i = 0; i < 20; i++) printf("%02X ", Read_Bytes[i]);
        printf("}\n");
    }
    else {
        printf("win32k.sys模块地址读取失败\n");
    }

    printf("\n=== RefreshPage读取测试 ===\n");
    if (ReadMemory(explorerAddr, Read_Bytes, 20)) {
        printf("RefreshPage读取字节集内存:{ ");
        for (int i = 0; i < 20; i++) printf("%02X ", Read_Bytes[i]);
        printf("}\n");
    }
    else {
        printf("RefreshPage读取失败\n");
    }

    printf("\n=== Virtual读取测试 ===\n");
    if (ReadMemory_Virtual(explorerAddr, Read_Bytes, 20)) {
        printf("Virtual读取字节集内存:{ ");
        for (int i = 0; i < 20; i++) printf("%02X ", Read_Bytes[i]);
        printf("}\n");
    }
    else {
        printf("Virtual读取失败\n");
    }

    printf("\n=== 指定内存管理测试 ===\n");
    Address = RequestDesignatedMemory(0xFFFF0000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (Address) {
        printf("申请内存成功:%llX\n", Address);

        printf("写入内存整数(附加)100000\n");
        WriteInt_Virtual(Address, 100000);
        ULONG Int = ReadInt_Virtual(Address);
        printf("读整数型内存(附加):%u\n", Int);

        ULONG64 BaseAddressBuffer = 0;
        ULONG64 AllocationProtectBuffer = 0;
        ULONG64 SizeBuffer = 0;
        INT originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("查询属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        INT newAttr = ProtectMemory(Address, MEM_COMMIT, PAGE_NOACCESS);
        printf("修改属性为PAGE_NOACCESS，原属性:%X\n", newAttr);

        originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("修改后属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        if (FreeMemory(Address, 4096))
            printf("释放内存成功\n");
        else
            printf("释放内存失败\n");
    }
    else {
        printf("申请指定内存失败\n");
    }

    printf("\n=== 普通内存管理测试 ===\n");
    Address = RequestMemory(MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (Address) {
        printf("申请内存成功:%llX\n", Address);

        printf("写入内存整数(附加)100000\n");
        WriteInt_Virtual(Address, 100000);
        ULONG Int = ReadInt_Virtual(Address);
        printf("读整数型内存(附加):%u\n", Int);

        ULONG64 BaseAddressBuffer = 0;
        ULONG64 AllocationProtectBuffer = 0;
        ULONG64 SizeBuffer = 0;
        INT originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("查询属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        INT newAttr = ProtectMemory(Address, MEM_COMMIT, PAGE_NOACCESS);
        printf("修改属性为PAGE_NOACCESS，原属性:%X\n", newAttr);

        originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("修改后属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        if (FreeMemory(Address, 4096))
            printf("释放内存成功\n");
        else
            printf("释放内存失败\n");
    }
    else {
        printf("申请普通内存失败\n");
    }

    printf("\n=== 高地址内存管理测试 ===\n");
    Address = RequestHighMemory(MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (Address) {
        printf("申请高地址内存成功:%llX\n", Address);

        printf("写入内存整数(附加)100000\n");
        WriteInt_Virtual(Address, 100000);
        ULONG Int = ReadInt_Virtual(Address);
        printf("读整数型内存(附加):%u\n", Int);

        ULONG64 BaseAddressBuffer = 0;
        ULONG64 AllocationProtectBuffer = 0;
        ULONG64 SizeBuffer = 0;
        INT originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("查询属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        INT newAttr = ProtectMemory(Address, MEM_COMMIT, PAGE_NOACCESS);
        printf("修改属性为PAGE_NOACCESS，原属性:%X\n", newAttr);

        originalAttr = AttributesQuery(Address, BaseAddressBuffer, AllocationProtectBuffer, SizeBuffer);
        printf("修改后属性:%X\n", originalAttr);
        printf("起始地址:%llX\n", BaseAddressBuffer);
        printf("原始属性:%llX\n", AllocationProtectBuffer);
        printf("内存大小:%llX\n", SizeBuffer);

        if (FreeMemory(Address, 4096))
            printf("释放内存成功\n");
        else
            printf("释放内存失败\n");
    }
    else {
        printf("申请高地址内存失败\n");
    }

    printf("\n=== 鼠标功能测试 ===\n");
    printf("鼠标相对移动测试...\n");
    system("pause");
    if (MouseRelativeMovement(100, 100))
        printf("鼠标相对移动完成\n");
    else
        printf("鼠标相对移动失败\n");

    printf("\n鼠标绝对移动测试...\n");
    system("pause");
    if (MouseAbsoluteMovement(100, 100))
        printf("鼠标绝对移动完成\n");
    else
        printf("鼠标绝对移动失败\n");

    printf("\n所有测试完成\n");
    system("pause");
    return 0;
}