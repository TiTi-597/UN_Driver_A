__declspec(noinline) NTSTATUS VirtualMemory(PDriverControl pDriver) {
	ULONG64 Copied = 0;
	if (pDriver->Type == 0) {
		return ((decltype(MmCopyVirtualMemory)*)pAPI_information->ImportTable.pMmCopyVirtualMemory)
			(pAPI_information->gProcess.Process, (PVOID)pDriver->Address, ((decltype(IoGetCurrentProcess)*)pAPI_information->ImportTable.pIoGetCurrentProcess)(), (PVOID)pDriver->Buffer, pDriver->Size, KernelMode, &Copied);
	}
	else {
		return ((decltype(MmCopyVirtualMemory)*)pAPI_information->ImportTable.pMmCopyVirtualMemory)
			(((decltype(IoGetCurrentProcess)*)pAPI_information->ImportTable.pIoGetCurrentProcess)(), (PVOID)pDriver->Buffer, pAPI_information->gProcess.Process, (PVOID)pDriver->Address, pDriver->Size, KernelMode, &Copied);
	}
}