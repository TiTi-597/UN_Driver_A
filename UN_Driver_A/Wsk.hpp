#define SOCKET_ERROR -1
#define HTON_SHORT(n) (((((unsigned short)(n) & 0xFFu)) << 8) | (((unsigned short)(n) & 0xFF00u) >> 8))

enum WSK_STATE { DEINITIALIZED, DEINITIALIZING, INITIALIZING, INITIALIZED };

__forceinline static NTSTATUS NTAPI CompletionRoutine(__in PDEVICE_OBJECT DeviceObject, __in PIRP Irp, __in PKEVENT CompletionEvent) {
    ASSERT(CompletionEvent);
    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(DeviceObject);

    ((decltype(KeSetEvent)*)pAPI_information->ImportTable.pKeSetEvent)(CompletionEvent, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

__forceinline static NTSTATUS InitWskData(__out PIRP* pIrp, __out PKEVENT CompletionEvent) {
    ASSERT(pIrp);
    ASSERT(CompletionEvent);

    *pIrp = ((decltype(IoAllocateIrp)*)pAPI_information->ImportTable.pIoAllocateIrp)(1, FALSE);
    if (!*pIrp) return STATUS_INSUFFICIENT_RESOURCES;

    MyKeInitializeEvent(CompletionEvent, SynchronizationEvent, FALSE);
    IoSetCompletionRoutine(*pIrp, (PIO_COMPLETION_ROUTINE)CompletionRoutine, CompletionEvent, TRUE, TRUE, TRUE);
    return STATUS_SUCCESS;
}

__forceinline static NTSTATUS InitWskBuffer(__in PVOID Buffer, __in ULONG BufferSize, __out PWSK_BUF WskBuffer) {
    NTSTATUS Status = STATUS_SUCCESS;

    ASSERT(Buffer);
    ASSERT(BufferSize);
    ASSERT(WskBuffer);

    WskBuffer->Offset = 0;
    WskBuffer->Length = BufferSize;

    WskBuffer->Mdl = ((decltype(IoAllocateMdl)*)pAPI_information->ImportTable.pIoAllocateMdl)(Buffer, BufferSize, FALSE, FALSE, NULL);
    if (!WskBuffer->Mdl) return STATUS_INSUFFICIENT_RESOURCES;

    ((decltype(MmProbeAndLockPages)*)pAPI_information->ImportTable.pMmProbeAndLockPages)(WskBuffer->Mdl, KernelMode, IoWriteAccess);
    return Status;
}

__forceinline static VOID FreeWskBuffer(__in PWSK_BUF WskBuffer) {
    ASSERT(WskBuffer);

    ((decltype(MmUnlockPages)*)pAPI_information->ImportTable.pMmUnlockPages)(WskBuffer->Mdl);
    ((decltype(IoFreeMdl)*)pAPI_information->ImportTable.pIoFreeMdl)(WskBuffer->Mdl);
}

__declspec(noinline) NTSTATUS WSKStartup() {
    WSK_CLIENT_NPI WskClient = { 0 };
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (_InterlockedCompareExchange(&pAPI_information->g_SocketsState, INITIALIZING, DEINITIALIZED) != DEINITIALIZED) return STATUS_ALREADY_REGISTERED;

    WskClient.ClientContext = NULL;
    WskClient.Dispatch = &pAPI_information->g_WskDispatch;

    Status = ((decltype(WskRegister)*)pAPI_information->ImportTable.pWskRegister)(&WskClient, &pAPI_information->g_WskRegistration);
    if (!NT_SUCCESS(Status)) {
        _InterlockedExchange(&pAPI_information->g_SocketsState, DEINITIALIZED);
        return Status;
    }

    Status = ((decltype(WskCaptureProviderNPI)*)pAPI_information->ImportTable.pWskCaptureProviderNPI)(&pAPI_information->g_WskRegistration, WSK_NO_WAIT, &pAPI_information->g_WskProvider);
    if (!NT_SUCCESS(Status)) {
        ((decltype(WskDeregister)*)pAPI_information->ImportTable.pWskDeregister)(&pAPI_information->g_WskRegistration);
        _InterlockedExchange(&pAPI_information->g_SocketsState, DEINITIALIZED);
        return Status;
    }

    _InterlockedExchange(&pAPI_information->g_SocketsState, INITIALIZED);
    return STATUS_SUCCESS;
}

__forceinline VOID WSKCleanup() {
    if (_InterlockedCompareExchange(&pAPI_information->g_SocketsState, INITIALIZED, DEINITIALIZING) != INITIALIZED) return;

    ((decltype(WskReleaseProviderNPI)*)pAPI_information->ImportTable.pWskReleaseProviderNPI)(&pAPI_information->g_WskRegistration);
    ((decltype(WskDeregister)*)pAPI_information->ImportTable.pWskDeregister)(&pAPI_information->g_WskRegistration);
    _InterlockedExchange(&pAPI_information->g_SocketsState, DEINITIALIZED);
}

__declspec(noinline) PWSK_SOCKET NTAPI CreateSocket(__in ADDRESS_FAMILY AddressFamily, __in USHORT SocketType, __in ULONG Protocol, __in ULONG Flags) {
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    PWSK_SOCKET WskSocket = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED) return NULL;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) return NULL;

    Status = pAPI_information->g_WskProvider.Dispatch->WskSocket(pAPI_information->g_WskProvider.Client, AddressFamily, SocketType, Protocol, Flags, NULL, NULL, NULL, NULL, NULL, Irp);
    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    WskSocket = NT_SUCCESS(Status) ? (PWSK_SOCKET)Irp->IoStatus.Information : NULL;
    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);

    return (PWSK_SOCKET)WskSocket;
}

__declspec(noinline) NTSTATUS NTAPI Bind(__in PWSK_SOCKET WskSocket, __in PSOCKADDR LocalAddress) {
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED || !WskSocket || !LocalAddress) return STATUS_INVALID_PARAMETER;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) return Status;

    Status = ((PWSK_PROVIDER_CONNECTION_DISPATCH)WskSocket->Dispatch)->WskBind(WskSocket, LocalAddress, 0, Irp);
    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);
    return Status;
}

__declspec(noinline) NTSTATUS NTAPI Connect(__in PWSK_SOCKET WskSocket, __in PSOCKADDR RemoteAddress) {
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED || !WskSocket || !RemoteAddress) return STATUS_INVALID_PARAMETER;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) return Status;

    Status = ((PWSK_PROVIDER_CONNECTION_DISPATCH)WskSocket->Dispatch)->WskConnect(WskSocket, RemoteAddress, 0, Irp);
    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);
    return Status;
}

__declspec(noinline) NTSTATUS NTAPI CloseSocket(__in PWSK_SOCKET WskSocket) {
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED || !WskSocket) return STATUS_INVALID_PARAMETER;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) return Status;

    Status = ((PWSK_PROVIDER_BASIC_DISPATCH)WskSocket->Dispatch)->WskCloseSocket(WskSocket, Irp);
    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);
    return Status;
}

__declspec(noinline) LONG NTAPI Send(__in PWSK_SOCKET WskSocket, __in PVOID Buffer, __in ULONG BufferSize, __in ULONG Flags){
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    WSK_BUF WskBuffer = { 0 };
    LONG BytesSent = SOCKET_ERROR;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED || !WskSocket || !Buffer || !BufferSize) return SOCKET_ERROR;

    Status = InitWskBuffer(Buffer, BufferSize, &WskBuffer);
    if (!NT_SUCCESS(Status)) return SOCKET_ERROR;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) {
        FreeWskBuffer(&WskBuffer);
        return SOCKET_ERROR;
    }

    Status = ((PWSK_PROVIDER_CONNECTION_DISPATCH)WskSocket->Dispatch)->WskSend(WskSocket, &WskBuffer, Flags, Irp);

    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    BytesSent = NT_SUCCESS(Status) ? (LONG)Irp->IoStatus.Information : SOCKET_ERROR;

    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);
    FreeWskBuffer(&WskBuffer);

    return BytesSent;
}

__declspec(noinline) LONG NTAPI Receive(__in PWSK_SOCKET WskSocket, __out PVOID Buffer, __in ULONG BufferSize, __in ULONG Flags) {
    KEVENT CompletionEvent = { 0 };
    PIRP Irp = NULL;
    WSK_BUF WskBuffer = { 0 };
    LONG BytesReceived = SOCKET_ERROR;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    if (pAPI_information->g_SocketsState != INITIALIZED || !WskSocket || !Buffer || !BufferSize) return SOCKET_ERROR;

    ZeroMemory_CPU(Buffer, BufferSize);
    Status = InitWskBuffer(Buffer, BufferSize, &WskBuffer);
    if (!NT_SUCCESS(Status)) return SOCKET_ERROR;

    Status = InitWskData(&Irp, &CompletionEvent);
    if (!NT_SUCCESS(Status)) {
        FreeWskBuffer(&WskBuffer);
        return SOCKET_ERROR;
    }

    Status = ((PWSK_PROVIDER_CONNECTION_DISPATCH)WskSocket->Dispatch)->WskReceive(WskSocket, &WskBuffer, Flags, Irp);
    if (Status == STATUS_PENDING) {
        ((decltype(KeWaitForSingleObject)*)pAPI_information->ImportTable.pKeWaitForSingleObject)(&CompletionEvent, Executive, KernelMode, FALSE, NULL);
        Status = Irp->IoStatus.Status;
    }

    BytesReceived = NT_SUCCESS(Status) ? (LONG)Irp->IoStatus.Information : SOCKET_ERROR;

    ((decltype(IoFreeIrp)*)pAPI_information->ImportTable.pIoFreeIrp)(Irp);
    FreeWskBuffer(&WskBuffer);

    return BytesReceived;
}

__forceinline ULONG ChangeUintToIp(ULONG a, ULONG b, ULONG c, ULONG d) {
    if (a > 255 || b > 255 || c > 255 || d > 255) return INADDR_ANY;

    ULONG address = 0;
    address |= d << 24;
    address |= c << 16;
    address |= b << 8;
    address |= a;

    return address;
}

__forceinline NTSTATUS SafeSendData(PWSK_SOCKET socket, PVOID UserBuffer, ULONG BufferSize){
    PVOID kernelBuffer = ((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPoolNx, BufferSize, pAPI_information->g_TAG.Send);
    if (!kernelBuffer) return STATUS_INSUFFICIENT_RESOURCES;

    __movsb((PUCHAR)kernelBuffer, (PUCHAR)UserBuffer, BufferSize);
    LONG bytesSent = Send(socket, kernelBuffer, BufferSize, 0);

    ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(kernelBuffer, pAPI_information->g_TAG.Send);

    return bytesSent;
}

__declspec(noinline) NTSTATUS ConnectAndExchange(ULONG64 IP1, ULONG64 IP2, ULONG64 IP3, ULONG64 IP4, ULONG64 Port, PVOID SendData, ULONG64 SendDataLength, ULONG64 MaxReceivedSize, ULONG64 ReceivedData, ULONG64 ReceivedLength) {
    PWSK_SOCKET socket = NULL;
    NTSTATUS status = STATUS_SUCCESS;
    PVOID kernelReceiveBuffer = NULL;
    LONG bytesReceived = 0;
    ULONG actualBytesReceived = 0;

    if (!SendData || SendDataLength == 0) return STATUS_INVALID_PARAMETER;
    if (!ReceivedData) return STATUS_INVALID_PARAMETER;
    if (!ReceivedLength) return STATUS_INVALID_PARAMETER;
    if (MaxReceivedSize == 0) return STATUS_INVALID_PARAMETER;

    if (IP1 > 255 || IP2 > 255 || IP3 > 255 || IP4 > 255) return STATUS_INVALID_PARAMETER;
    if (Port > 65535) return STATUS_INVALID_PARAMETER;

    status = WSKStartup();
    if (!NT_SUCCESS(status)) return status;

    do {
        socket = CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, WSK_FLAG_CONNECTION_SOCKET);
        if (!socket) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        SOCKADDR_IN localAddress = { 0 };
        localAddress.sin_family = AF_INET;
        localAddress.sin_addr.s_addr = INADDR_ANY;

        status = Bind(socket, (PSOCKADDR)&localAddress);

        SOCKADDR_IN remoteAddress = { 0 };
        remoteAddress.sin_family = AF_INET;
        remoteAddress.sin_addr.s_addr = ChangeUintToIp(IP1, IP2, IP3, IP4);
        remoteAddress.sin_port = HTON_SHORT(Port);

        status = Connect(socket, (PSOCKADDR)&remoteAddress);
        if (!NT_SUCCESS(status)) break;

        LONG bytesSent = SafeSendData(socket, SendData, SendDataLength);
        if (bytesSent == SOCKET_ERROR || bytesSent != (LONG)SendDataLength) {
            status = STATUS_UNSUCCESSFUL;
            break;
        }

        kernelReceiveBuffer = ((decltype(ExAllocatePoolWithTag)*)pAPI_information->ImportTable.pExAllocatePoolWithTag)(NonPagedPoolNx, MaxReceivedSize, pAPI_information->g_TAG.RECV);
        if (!kernelReceiveBuffer) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        ZeroMemory_CPU(kernelReceiveBuffer, MaxReceivedSize);
        bytesReceived = Receive(socket, kernelReceiveBuffer, MaxReceivedSize, 0);

        if (bytesReceived == SOCKET_ERROR || bytesReceived <= 0) {
            status = STATUS_UNSUCCESSFUL;
            break;
        }

        ULONG copySize = min((ULONG)bytesReceived, MaxReceivedSize);
        __movsb((PUCHAR)ReceivedData, (PUCHAR)kernelReceiveBuffer, copySize);

        actualBytesReceived = copySize;
        __movsb((PUCHAR)ReceivedLength, (PUCHAR)&actualBytesReceived, sizeof(ULONG));

        if (copySize < (ULONG)bytesReceived) status = STATUS_BUFFER_OVERFLOW;

    } while (FALSE);

    if (socket) CloseSocket(socket);
    if (kernelReceiveBuffer) {
        __stosb((PUCHAR)kernelReceiveBuffer, 0, MaxReceivedSize);
        ((decltype(ExFreePoolWithTag)*)pAPI_information->ImportTable.pExFreePoolWithTag)(kernelReceiveBuffer, pAPI_information->g_TAG.RECV);
    }

    WSKCleanup();
    return status;
}