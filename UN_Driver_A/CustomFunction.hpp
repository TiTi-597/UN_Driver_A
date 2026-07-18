__declspec(noinline) VOID GetTscFast(PLARGE_INTEGER TscValue) {
	NT_ASSERT(TscValue != NULL);
	TscValue->QuadPart = __rdtsc();
}

__declspec(noinline) ULONG GenerateRandomTag() {
	LARGE_INTEGER tscValue;
	GetTscFast(&tscValue);
	ULONG seed = tscValue.LowPart ^ (tscValue.HighPart << 16) ^ (ULONG)&tscValue;
	ULONG tag = 0;
	for (int i = 0; i < 4; i++) {
		seed ^= seed << 13;
		seed ^= seed >> 17;
		seed ^= seed << 5;
		char random_char = 'A' + (seed % 26);
		tag |= ((ULONG)random_char) << (i * 8);
		seed = seed * 214013 + 2531011;
	}
	return tag;
}

__forceinline size_t Mystrlen(const char* str) {
	if (str == NULL) {
		return 0;
	}
	const char* ptr = str;
	while (*ptr) ptr++;
	return ptr - str;
}

__forceinline VOID ZeroMemory_CPU(PVOID dest, SIZE_T size) {
	if (size >= 16) {
		__m128i zero = _mm_setzero_si128();
		SIZE_T alignedSize = size & ~0xF;
		SIZE_T i = 0;

		for (; i < alignedSize; i += 16) {
			_mm_store_si128((__m128i*)((PUCHAR)dest + i), zero);
		}
		for (; i < size; i++) {
			*((PUCHAR)dest + i) = 0;
		}
	}
	else {
		for (SIZE_T i = 0; i < size; i++) {
			*((PUCHAR)dest + i) = 0;
		}
	}
}

__forceinline VOID MyRtlInitUnicodeString(PUNICODE_STRING DestinationString, PCWSTR SourceString) {
	DestinationString->Length = 0;
	DestinationString->MaximumLength = 0;
	DestinationString->Buffer = (PWSTR)SourceString;

	if (SourceString) {
		USHORT length = 0;
		while (SourceString[length] != L'\0' && length < 65535) {
			length++;
		}
		DestinationString->Length = length * sizeof(WCHAR);
		DestinationString->MaximumLength = (length + 1) * sizeof(WCHAR);
	}
}

__declspec(noinline) SIZE_T MyRtlCompareMemory(_In_ const VOID* Source1, _In_ const VOID* Source2, _In_ SIZE_T Length) {
    const UCHAR* s1 = (const UCHAR*)Source1;
    const UCHAR* s2 = (const UCHAR*)Source2;
    SIZE_T i = 0;

    if (Source1 == Source2) return Length;
    if (!Source1 || !Source2) return 0;
    if (Length == 0) return 0;

    if (Length < 16) {
        for (i = 0; i < Length; ++i) {
            if (s1[i] != s2[i]) return i;
        }
        return Length;
    }

    if ((((ULONG_PTR)s1 & 0x7) == 0) && (((ULONG_PTR)s2 & 0x7) == 0)) {
        const ULONGLONG* ll1 = (const ULONGLONG*)s1;
        const ULONGLONG* ll2 = (const ULONGLONG*)s2;
        SIZE_T qwordCount = Length / 8;

        for (i = 0; i < qwordCount; ++i) {
            if (ll1[i] != ll2[i]) {
                SIZE_T bytePos = i * 8;
                const UCHAR* b1 = (const UCHAR*)&ll1[i];
                const UCHAR* b2 = (const UCHAR*)&ll2[i];

                for (SIZE_T j = 0; j < 8; ++j) {
                    if (b1[j] != b2[j]) return bytePos + j;
                }
            }
        }

        s1 += qwordCount * 8;
        s2 += qwordCount * 8;
        SIZE_T remaining = Length % 8;

        for (i = 0; i < remaining; ++i) {
            if (s1[i] != s2[i]) return (qwordCount * 8) + i;
        }

        return Length;
    }

    if ((((ULONG_PTR)s1 & 0x3) == 0) && (((ULONG_PTR)s2 & 0x3) == 0)) {
        const ULONG* l1 = (const ULONG*)s1;
        const ULONG* l2 = (const ULONG*)s2;
        SIZE_T dwordCount = Length / 4;

        for (i = 0; i < dwordCount; ++i) {
            if (l1[i] != l2[i]) {
                SIZE_T bytePos = i * 4;
                const UCHAR* b1 = (const UCHAR*)&l1[i];
                const UCHAR* b2 = (const UCHAR*)&l2[i];

                for (SIZE_T j = 0; j < 4; ++j) {
                    if (b1[j] != b2[j]) return bytePos + j;
                }
            }
        }

        s1 += dwordCount * 4;
        s2 += dwordCount * 4;
        SIZE_T remaining = Length % 4;

        for (i = 0; i < remaining; ++i) {
            if (s1[i] != s2[i]) return (dwordCount * 4) + i;
        }

        return Length;
    }

    for (i = 0; i < Length; ++i) {
        if (s1[i] != s2[i]) return i;
    }

    return Length;
}

__declspec(noinline) LONG MyRtlCompareUnicodeString(_In_ PCUNICODE_STRING String1, _In_ PCUNICODE_STRING String2, _In_ BOOLEAN CaseInsensitive) {
    PCWSTR str1 = NULL;
    PCWSTR str2 = NULL;
    SIZE_T length1 = 0;
    SIZE_T length2 = 0;
    SIZE_T i = 0;

    PAGED_CODE();

    if (!String1 || !String2) {
        if (!String1 && !String2) return 0;
        return (String1) ? 1 : -1;
    }

    if (String1->Length == 0 && String2->Length == 0) return 0;
    if (String1->Length == 0) return -1;
    if (String2->Length == 0) return 1;

    if (!String1->Buffer || !String2->Buffer) {
        if (!String1->Buffer && !String2->Buffer) return 0;
        return (String1->Buffer) ? 1 : -1;
    }

    str1 = String1->Buffer;
    str2 = String2->Buffer;
    length1 = String1->Length / sizeof(WCHAR);
    length2 = String2->Length / sizeof(WCHAR);
    SIZE_T minLength = (length1 < length2) ? length1 : length2;

    if (!CaseInsensitive) {
        SIZE_T bytesEqual = MyRtlCompareMemory(str1, str2, minLength * sizeof(WCHAR));

        if (bytesEqual < minLength * sizeof(WCHAR)) {
            SIZE_T diffPos = bytesEqual / sizeof(WCHAR);
            WCHAR c1 = str1[diffPos];
            WCHAR c2 = str2[diffPos];
            return (c1 < c2) ? -1 : 1;
        }
    }
    else {
        for (i = 0; i < minLength; ++i) {
            WCHAR c1 = str1[i];
            WCHAR c2 = str2[i];

            if (c1 == c2) continue;

            WCHAR upper1 = c1;
            WCHAR upper2 = c2;

            if (L'a' <= c1 && c1 <= L'z') upper1 = c1 - 0x20;
            if (L'a' <= c2 && c2 <= L'z') upper2 = c2 - 0x20;

            if (upper1 != upper2) return (upper1 < upper2) ? -1 : 1;
        }
    }

    if (length1 != length2) return (length1 < length2) ? -1 : 1;

    return 0;
}

__forceinline VOID MyKeInitializeEvent(PRKEVENT Event, EVENT_TYPE Type, BOOLEAN State) {
    ZeroMemory_CPU(Event, sizeof(KEVENT));

    Event->Header.Type = 1;
    if (Type == NotificationEvent) Event->Header.Type = 1;
    else if (Type == SynchronizationEvent) Event->Header.Type = 0;

    Event->Header.SignalState = State ? 1 : 0;
    Event->Header.WaitListHead.Flink = &Event->Header.WaitListHead;
    Event->Header.WaitListHead.Blink = &Event->Header.WaitListHead;
}

__forceinline VOID Xor(UCHAR* data, size_t data_len, const UCHAR* key, size_t key_len) {
    for (size_t i = 0; i < data_len; i++) {
        data[i] ^= key[i % key_len];
    }
}