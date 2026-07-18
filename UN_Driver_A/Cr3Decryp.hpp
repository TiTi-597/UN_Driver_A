typedef struct _MI_ACTIVE_PFN
{
	union
	{
		struct
		{
			struct
			{
				unsigned __int64 Tradable : 1; /* bit position: 0 */
				unsigned __int64 NonPagedBuddy : 43; /* bit position: 1 */
			};
		} Leaf;
		struct
		{
			struct
			{
				/* 0x0000 */ unsigned __int64 Tradable : 1; /* bit position: 0 */
				/* 0x0000 */ unsigned __int64 WsleAge : 3; /* bit position: 1 */
				/* 0x0000 */ unsigned __int64 OldestWsleLeafEntries : 10; /* bit position: 4 */
				/* 0x0000 */ unsigned __int64 OldestWsleLeafAge : 3; /* bit position: 14 */
				/* 0x0000 */ unsigned __int64 NonPagedBuddy : 43; /* bit position: 17 */
			}; /* bitfield */
		} /* size: 0x0008 */ PageTable;
		/* 0x0000 */ unsigned __int64 EntireActiveField;
	}; /* size: 0x0008 */
} MI_ACTIVE_PFN, * PMI_ACTIVE_PFN; /* size: 0x0008 */

typedef struct _MMPTE_HARDWARE
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 Dirty1 : 1; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 Owner : 1; /* bit position: 2 */
		/* 0x0000 */ unsigned __int64 WriteThrough : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned __int64 CacheDisable : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Accessed : 1; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Dirty : 1; /* bit position: 6 */
		/* 0x0000 */ unsigned __int64 LargePage : 1; /* bit position: 7 */
		/* 0x0000 */ unsigned __int64 Global : 1; /* bit position: 8 */
		/* 0x0000 */ unsigned __int64 CopyOnWrite : 1; /* bit position: 9 */
		/* 0x0000 */ unsigned __int64 Unused : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Write : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 PageFrameNumber : 40; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 ReservedForSoftware : 4; /* bit position: 52 */
		/* 0x0000 */ unsigned __int64 WsleAge : 4; /* bit position: 56 */
		/* 0x0000 */ unsigned __int64 WsleProtection : 3; /* bit position: 60 */
		/* 0x0000 */ unsigned __int64 NoExecute : 1; /* bit position: 63 */
	}; /* bitfield */
} MMPTE_HARDWARE, * PMMPTE_HARDWARE; /* size: 0x0008 */

typedef struct _MMPTE_PROTOTYPE
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 DemandFillProto : 1; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 HiberVerifyConverted : 1; /* bit position: 2 */
		/* 0x0000 */ unsigned __int64 ReadOnly : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Combined : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 Unused1 : 4; /* bit position: 12 */
		/* 0x0000 */ __int64 ProtoAddress : 48; /* bit position: 16 */
	}; /* bitfield */
} MMPTE_PROTOTYPE, * PMMPTE_PROTOTYPE; /* size: 0x0008 */

typedef struct _MMPTE_SOFTWARE
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 PageFileReserved : 1; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 PageFileAllocated : 1; /* bit position: 2 */
		/* 0x0000 */ unsigned __int64 ColdPage : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Transition : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 PageFileLow : 4; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 UsedPageTableEntries : 10; /* bit position: 16 */
		/* 0x0000 */ unsigned __int64 ShadowStack : 1; /* bit position: 26 */
		/* 0x0000 */ unsigned __int64 Unused : 5; /* bit position: 27 */
		/* 0x0000 */ unsigned __int64 PageFileHigh : 32; /* bit position: 32 */
	}; /* bitfield */
} MMPTE_SOFTWARE, * PMMPTE_SOFTWARE; /* size: 0x0008 */

typedef struct _MMPTE_TIMESTAMP
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 MustBeZero : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 Unused : 3; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Transition : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 PageFileLow : 4; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 Reserved : 16; /* bit position: 16 */
		/* 0x0000 */ unsigned __int64 GlobalTimeStamp : 32; /* bit position: 32 */
	}; /* bitfield */
} MMPTE_TIMESTAMP, * PMMPTE_TIMESTAMP; /* size: 0x0008 */

typedef struct _MMPTE_TRANSITION
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 Write : 1; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 Spare : 1; /* bit position: 2 */
		/* 0x0000 */ unsigned __int64 IoTracker : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Transition : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 PageFrameNumber : 40; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 Unused : 12; /* bit position: 52 */
	}; /* bitfield */
} MMPTE_TRANSITION, * PMMPTE_TRANSITION; /* size: 0x0008 */

typedef struct _MMPTE_SUBSECTION
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 Unused0 : 3; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 ColdPage : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 Unused1 : 3; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 ExecutePrivilege : 1; /* bit position: 15 */
		/* 0x0000 */ __int64 SubsectionAddress : 48; /* bit position: 16 */
	}; /* bitfield */
} MMPTE_SUBSECTION, * PMMPTE_SUBSECTION; /* size: 0x0008 */

typedef struct _MMPTE_LIST
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned __int64 Valid : 1; /* bit position: 0 */
		/* 0x0000 */ unsigned __int64 OneEntry : 1; /* bit position: 1 */
		/* 0x0000 */ unsigned __int64 filler0 : 2; /* bit position: 2 */
		/* 0x0000 */ unsigned __int64 SwizzleBit : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned __int64 Protection : 5; /* bit position: 5 */
		/* 0x0000 */ unsigned __int64 Prototype : 1; /* bit position: 10 */
		/* 0x0000 */ unsigned __int64 Transition : 1; /* bit position: 11 */
		/* 0x0000 */ unsigned __int64 filler1 : 16; /* bit position: 12 */
		/* 0x0000 */ unsigned __int64 NextEntry : 36; /* bit position: 28 */
	}; /* bitfield */
} MMPTE_LIST, * PMMPTE_LIST; /* size: 0x0008 */

typedef struct _MMPTE
{
	union
	{
		union
		{
			/* 0x0000 */ unsigned __int64 Long;
			/* 0x0000 */ volatile unsigned __int64 VolatileLong;
			/* 0x0000 */ struct _MMPTE_HARDWARE Hard;
			/* 0x0000 */ struct _MMPTE_PROTOTYPE Proto;
			/* 0x0000 */ struct _MMPTE_SOFTWARE Soft;
			/* 0x0000 */ struct _MMPTE_TIMESTAMP TimeStamp;
			/* 0x0000 */ struct _MMPTE_TRANSITION Trans;
			/* 0x0000 */ struct _MMPTE_SUBSECTION Subsect;
			/* 0x0000 */ struct _MMPTE_LIST List;
		}; /* size: 0x0008 */
	} /* size: 0x0008 */ u;
} MMPTE, * PMMPTE; /* size: 0x0008 */

typedef struct _MIPFNBLINK
{
	union
	{
		struct /* bitfield */
		{
			/* 0x0000 */ unsigned __int64 Blink : 40; /* bit position: 0 */
			/* 0x0000 */ unsigned __int64 NodeBlinkLow : 19; /* bit position: 40 */
			/* 0x0000 */ unsigned __int64 TbFlushStamp : 3; /* bit position: 59 */
			/* 0x0000 */ unsigned __int64 PageBlinkDeleteBit : 1; /* bit position: 62 */
			/* 0x0000 */ unsigned __int64 PageBlinkLockBit : 1; /* bit position: 63 */
		}; /* bitfield */
		struct /* bitfield */
		{
			/* 0x0000 */ unsigned __int64 ShareCount : 62; /* bit position: 0 */
			/* 0x0000 */ unsigned __int64 PageShareCountDeleteBit : 1; /* bit position: 62 */
			/* 0x0000 */ unsigned __int64 PageShareCountLockBit : 1; /* bit position: 63 */
		}; /* bitfield */
		/* 0x0000 */ unsigned __int64 EntireField;
		/* 0x0000 */ volatile __int64 Lock;
		struct /* bitfield */
		{
			/* 0x0000 */ unsigned __int64 LockNotUsed : 62; /* bit position: 0 */
			/* 0x0000 */ unsigned __int64 DeleteBit : 1; /* bit position: 62 */
			/* 0x0000 */ unsigned __int64 LockBit : 1; /* bit position: 63 */
		}; /* bitfield */
	}; /* size: 0x0008 */
} MIPFNBLINK, * PMIPFNBLINK; /* size: 0x0008 */

typedef struct _MMPFNENTRY1
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned char PageLocation : 3; /* bit position: 0 */
		/* 0x0000 */ unsigned char WriteInProgress : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned char Modified : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned char ReadInProgress : 1; /* bit position: 5 */
		/* 0x0000 */ unsigned char CacheAttribute : 2; /* bit position: 6 */
	}; /* bitfield */
} MMPFNENTRY1, * PMMPFNENTRY1; /* size: 0x0001 */

typedef struct _MMPFNENTRY3
{
	struct /* bitfield */
	{
		/* 0x0000 */ unsigned char Priority : 3; /* bit position: 0 */
		/* 0x0000 */ unsigned char OnProtectedStandby : 1; /* bit position: 3 */
		/* 0x0000 */ unsigned char InPageError : 1; /* bit position: 4 */
		/* 0x0000 */ unsigned char SystemChargedPage : 1; /* bit position: 5 */
		/* 0x0000 */ unsigned char RemovalRequested : 1; /* bit position: 6 */
		/* 0x0000 */ unsigned char ParityError : 1; /* bit position: 7 */
	}; /* bitfield */
} MMPFNENTRY3, * PMMPFNENTRY3; /* size: 0x0001 */

typedef struct _MI_PFN_ULONG5
{
	union
	{
		/* 0x0000 */ unsigned long EntireField;
		struct
		{
			struct /* bitfield */
			{
				/* 0x0000 */ unsigned long NodeBlinkHigh : 21; /* bit position: 0 */
				/* 0x0000 */ unsigned long NodeFlinkMiddle : 11; /* bit position: 21 */
			}; /* bitfield */
		} /* size: 0x0004 */ StandbyList;
		struct
		{
			/* 0x0000 */ unsigned char ModifiedListBucketIndex : 4; /* bit position: 0 */
		} /* size: 0x0001 */ MappedPageList;
		struct
		{
			struct /* bitfield */
			{
				/* 0x0000 */ unsigned char AnchorLargePageSize : 2; /* bit position: 0 */
				/* 0x0000 */ unsigned char Spare1 : 6; /* bit position: 2 */
			}; /* bitfield */
			/* 0x0001 */ unsigned char ViewCount;
			/* 0x0002 */ unsigned short Spare2;
		} /* size: 0x0004 */ Active;
	}; /* size: 0x0004 */
} MI_PFN_ULONG5, * PMI_PFN_ULONG5; /* size: 0x0004 */

typedef union _virt_addr_t
{
	void* value;
	struct
	{
		uintptr_t offset : 12;
		uintptr_t pt_index : 9;
		uintptr_t pd_index : 9;
		uintptr_t pdpt_index : 9;
		uintptr_t pml4_index : 9;
		uintptr_t reserved : 16;
	};
} virt_addr_t, * pvirt_addr_t;

typedef struct _MMPFN
{
	union
	{
		/* 0x0000 */ struct _LIST_ENTRY ListEntry;
		/* 0x0000 */ struct _RTL_BALANCED_NODE TreeNode;
		struct
		{
			union
			{
				union
				{
					/* 0x0000 */ struct _SINGLE_LIST_ENTRY NextSlistPfn;
					/* 0x0000 */ void* Next;
					struct /* bitfield */
					{
						/* 0x0000 */ unsigned __int64 Flink : 40; /* bit position: 0 */
						/* 0x0000 */ unsigned __int64 NodeFlinkLow : 24; /* bit position: 40 */
					}; /* bitfield */
					/* 0x0000 */ struct _MI_ACTIVE_PFN Active;
				}; /* size: 0x0008 */
			} /* size: 0x0008 */ u1;
			union
			{
				/* 0x0008 */ struct _MMPTE* PteAddress;
				/* 0x0008 */ unsigned __int64 PteLong;
			}; /* size: 0x0008 */
			/* 0x0010 */ struct _MMPTE OriginalPte;
		}; /* size: 0x0018 */
	}; /* size: 0x0018 */
	/* 0x0018 */ struct _MIPFNBLINK u2;
	union
	{
		union
		{
			struct
			{
				/* 0x0020 */ unsigned short ReferenceCount;
				/* 0x0022 */ struct _MMPFNENTRY1 e1;
				/* 0x0023 */ struct _MMPFNENTRY3 e3;
			}; /* size: 0x0004 */
			struct
			{
				/* 0x0020 */ unsigned short ReferenceCount;
			} /* size: 0x0002 */ e2;
			struct
			{
				/* 0x0020 */ unsigned long EntireField;
			} /* size: 0x0004 */ e4;
		}; /* size: 0x0004 */
	} /* size: 0x0004 */ u3;
	/* 0x0024 */ struct _MI_PFN_ULONG5 u5;
	union
	{
		union
		{
			struct
			{
				unsigned __int64 PteFrame : 40;
				unsigned __int64 ResidentPage : 1;
				unsigned __int64 Unused1 : 1;
				unsigned __int64 Unused2 : 1;
				unsigned __int64 Partition : 10;
				unsigned __int64 FileOnly : 1;
				unsigned __int64 PfnExists : 1;
				unsigned __int64 NodeFlinkHigh : 5;
				unsigned __int64 PageIdentity : 3;
				unsigned __int64 PrototypePte : 1;
			};
			unsigned __int64 EntireField;
		};
	} u4;
} MMPFN, * PMMPFN;

__forceinline NTSTATUS read_physical(uintptr_t address, PVOID buffer, size_t size, size_t* bytes) {
    MM_COPY_ADDRESS target_address = { 0 };
    target_address.PhysicalAddress.QuadPart = address;
    return (((decltype(MmCopyMemory)*)pAPI_information->ImportTable.pMmCopyMemory))(buffer, target_address, size, MM_COPY_MEMORY_PHYSICAL, bytes);
}

__forceinline PVOID split_memory(PVOID SearchBase, SIZE_T SearchSize, const void* Pattern, SIZE_T PatternSize) {
    const UCHAR* searchBase = static_cast<const UCHAR*>(SearchBase);
    const UCHAR* pattern = static_cast<const UCHAR*>(Pattern);

    for (SIZE_T i = 0; i <= SearchSize - PatternSize; ++i) {
        SIZE_T j = 0;
        for (; j < PatternSize; ++j) {
            if (searchBase[i + j] != pattern[j]) break;
        }

        if (j == PatternSize) return const_cast<UCHAR*>(&searchBase[i]);
    }

    return nullptr;
}

__declspec(noinline) static auto init_mmpfn_database() {
    struct MmPfnDatabaseSearchPattern {
        const UCHAR* bytes;
        SIZE_T bytes_size;
        bool hard_coded;
    };

    MmPfnDatabaseSearchPattern patterns;

    static const UCHAR kPatternWin10x64[] = {
        0x48, 0x8B, 0xC1,
        0x48, 0xC1, 0xE8, 0x0C,
        0x48, 0x8D, 0x14, 0x40,
        0x48, 0x03, 0xD2,
        0x48, 0xB8,
    };

    patterns.bytes = kPatternWin10x64;
    patterns.bytes_size = sizeof(kPatternWin10x64);
    patterns.hard_coded = true;

    const auto p_MmGetVirtualForPhysical = reinterpret_cast<UCHAR*>(((pAPI_information->ImportTable.pMmGetVirtualForPhysical)));

    if (!p_MmGetVirtualForPhysical) return STATUS_PROCEDURE_NOT_FOUND;

    auto found = reinterpret_cast<UCHAR*>(split_memory(p_MmGetVirtualForPhysical, 0x20, patterns.bytes, patterns.bytes_size));
    if (!found) return STATUS_UNSUCCESSFUL;

    found += patterns.bytes_size;

    if (patterns.hard_coded) {
        pAPI_information->CR3.MmPfnDataBase = *reinterpret_cast<void**>(found);
    }
    else {
        const auto mmpfn_address = *reinterpret_cast<ULONG_PTR*>(found);
        pAPI_information->CR3.MmPfnDataBase = *reinterpret_cast<void**>(mmpfn_address);
    }

    pAPI_information->CR3.MmPfnDataBase = PAGE_ALIGN(pAPI_information->CR3.MmPfnDataBase);

    return STATUS_SUCCESS;
}

__declspec(noinline) uintptr_t dirbase_from_base_address(void* base) {
    if (!NT_SUCCESS(init_mmpfn_database())) return 0;

    virt_addr_t virt_base{};
    virt_base.value = base;

    size_t read{};
    auto ranges = ((decltype(MmGetPhysicalMemoryRanges)*)pAPI_information->ImportTable.pMmGetPhysicalMemoryRanges)();

    for (int i = 0;; i++) {
        auto elem = &ranges[i];

        if (!elem->BaseAddress.QuadPart || !elem->NumberOfBytes.QuadPart)
            break;

        uintptr_t current_phys_address = elem->BaseAddress.QuadPart;

        for (int j = 0; j < (elem->NumberOfBytes.QuadPart / 0x1000); j++, current_phys_address += 0x1000) {
            _MMPFN* pnfinfo = (_MMPFN*)((uintptr_t)pAPI_information->CR3.MmPfnDataBase + (current_phys_address >> 12) * sizeof(_MMPFN));

            if (pnfinfo->u4.PteFrame == (current_phys_address >> 12)) {
                MMPTE pml4e{};
                if (!NT_SUCCESS(read_physical(current_phys_address + 8 * virt_base.pml4_index, &pml4e, 8, &read))) continue;
                if (!pml4e.u.Hard.Valid) continue;

                MMPTE pdpte{};
                if (!NT_SUCCESS(read_physical((pml4e.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pdpt_index, &pdpte, 8, &read))) continue;
                if (!pdpte.u.Hard.Valid) continue;

                MMPTE pde{};
                if (!NT_SUCCESS(read_physical((pdpte.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pd_index, &pde, 8, &read))) continue;
                if (!pde.u.Hard.Valid) continue;

                MMPTE pte{};
                if (!NT_SUCCESS(read_physical((pde.u.Hard.PageFrameNumber << 12) + 8 * virt_base.pt_index, &pte, 8, &read))) continue;
                if (!pte.u.Hard.Valid) continue;

                return current_phys_address;
            }
        }
    }

    return 0;
}