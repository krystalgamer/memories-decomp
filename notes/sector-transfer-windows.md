# Sector-transfer destination windows

The transfer descriptor's two address words and the auxiliary-model upload
sector form one producer/consumer contract. Their names distinguish incoming
RAM destinations from the separate ready-source cursor, VRAM coordinates,
SPU destination, and direct-transfer destination. No transfer implementation
or allocation is added.

| Original view | Canonical view | Preserved representation |
| --- | --- | --- |
| `FileTransferDescriptor.value_08` | `sector_destination` | `u32` at `+0x08` |
| `FileTransferDescriptor.value_0C` | `alternate_sector_destination` | `u32` at `+0x0C` |
| `D_801DD800` | `gModel_abAuxiliaryPaletteSector` | incomplete `u8[]` native-address view |

`FileTransferDescriptor` stays `0x48` bytes. The address words remain integers:
they are not converted to pointers, a new array, an immutable buffer base,
or an end/limit pair. All casts, stores, integer arithmetic, and offset
expressions retain their original form.

## The incoming-sector destination depends on the mode

`File_TransferReadyCallback` establishes the roles:

- In mode 1, `CdGetSector` or the existing software-copy loop writes one sector
  to `sector_destination`. The descriptor's current destination then advances
  by `FILE_SECTOR_SIZE`. The existing `0x200000` skip path does neither the copy
  nor that advance. Source-cursor, total-byte, phase, and completion work
  retain their independent behavior.
- In mode 2, the image path selects either address word using
  `buffer_index & 1`, receives a sector into that RAM buffer, and uploads from
  it with `LoadImage2`. The integer-sum selector still adds the descriptor base
  before the member-derived offset and scaled index. It is not rewritten as
  a C array access, even though the two words are adjacent.
- In mode 3, the first address is RAM staging for a full or partial sector
  passed to `SpuWrite`. It does not become a linear RAM cursor there.
  The SPU destination advances through the existing `field_30.word`, not
  through either renamed address word.

The words describe RAM destinations for incoming data. Uploading the freshly
filled buffer subsequently reads from it; that does not turn the second word
into a source cursor or an end pointer. The ready-source cursor remains a
separate global.

`File_InitTransferDescriptor` preserves the raw negative-address bits supplied
through its existing parameter and the alternative image-buffer setup.
Its direct-destination flag path remains separate. Phase callbacks continue
to store the same address into both words in non-image modes, including
stores whose second value is not consumed by that mode.

## Reloads and callbacks are part of the contract

The software-copy loop intentionally reloads the destination member while
copying. A destination that aliases the descriptor can change that word
during the copy. No cached local destination replaces those loads.

The callback retains every global descriptor reload, post-incremented phase
index, source-cursor advance, byte-count update, wait loop, and buffer-index
increment. A phase callback may change the active descriptor or counters;
the field names do not introduce a stable-descriptor assumption.

The mixed `field_30` union remains unchanged: its halfwords serve sector
counters or VRAM coordinates in some paths, while its whole word serves
other address/packed-value roles. It is not absorbed into this destination
pair or assigned a uniform interpretation.

## Auxiliary-model palette-sector view

`Model_LoadAuxiliaryPackageStage` phase 2 programs one sector into
`D_801DD800` through both descriptor words. Phase 3 uploads a `0x100`-by-1
rectangle at `MODEL_TEXTURE_PALETTE_RECT_X`, y `0xF1`, from that label.
It then programs the next transfer using the same label minus
`FILE_SECTOR_SIZE`.

That producer and palette-upload consumer support
`gModel_abAuxiliaryPaletteSector`. They do not establish an exclusive
allocation, a universal extent, or the format of the sector's unused
remainder. The declaration remains incomplete. In particular, the following
subtract-one-sector expression is not folded into
`gFile_abPackageStagingBuffer`, even where the resulting numeric address
coincides.

The existing declaration owner, `file_transfer_steps.h`, retains its legacy
`extern u8 D_801DD800[]` and adds the canonical declaration using
`__asm__("D_801DD800")`. This keeps the actual native import unchanged and
works with the existing strict-C99 native consumers. The old extern also
keeps the provider visible to the existing tracked-alias policy mechanism.

All seven resident linker inputs export the canonical name as an alias to
that same native provider. The accepted NA provider is an absolute symbol
at `0x801DD800`; it is not turned into a C data definition or assigned an
invented ELF size. No storage, numeric production pin, compiler flag,
overlay import dependency, or instruction is added.

The previously unclassified `D_800F5694` is now independently tied to slot 2's
load-step member in all seven images. Its canonical retained-native view is
`gModel_bAuxiliaryLoadStep`; see [the lifecycle contract](model-slot-lifecycle.md).
The new evidence is aggregate/alias identity and its readers, not the phase-5 store alone.

## Existing native consumers and conservation

The CD-ready and preview-upload native witnesses use the production
descriptor type. Only their corresponding two member identifiers change:
their complete remaining source, numeric offsets, byte-layout observations,
mutation paths, negative controls, oracle conditions, and compiler flags
remain identical. The model-texture-transfer witness's legacy sector
definition remains untouched because the canonical declaration preserves
its native identity.

No new generated-C, source-string, or synthetic harness is introduced.
Cloud witnesses and regional/overlay image suites remain PR validation,
not local execution. Target compiler qualification includes actual registered
include/profile closures, independent Japanese code, overlay consumers,
and profiles with complete compiler, filtered, and MASPSX output files.

The existing first-include guards, callback ABI, pointer annotations,
record widths, source-group boundaries, region wrappers, and named compiler
profiles remain authoritative. Naming these fields does not normalize
flags, invalid-mode behavior, callbacks, signed inputs, buffer counts,
overlap behavior, or completion/failure policy.

## Callback phase cursor and initial request argument

The former `FileTransferDescriptor.result` word at `+0x40` is now
`phase_index`. Its high-confidence role comes from all four callback
dispatch sites: ready-sector completion, the shared VRAM/SPU tail,
initial active-transfer setup and `File_StepTransferPhase`. Each supplies
the current word as the callback's phase argument while post-incrementing
the stored word. This is an input cursor for the next callback, not its
return value or a completion/error status.

The request constructors propagate the existing signed
`initial_phase_index` argument into this unchanged `u32` member. The three
primary/secondary request definitions and their owning declarations now
name that argument and the callback argument without changing parameter
order, widths, calling convention or the loose callback typedef.

The cursor is not necessarily monotonic or zero-based. The card-data
callback, including its independent Japanese implementation, writes one
or two to select the next data/skip step. The PAL duel-package callback
writes `0x10` to visit its extra language stage, then writes one through
the existing signed word view to resume the ordinary path. The sound
phase callback can write two before falling through to its reduced
payload leg. All such redirects remain in the same order.

Unsigned post-increment wraps at the original word boundary, including
`0xFFFFFFFF` to zero. The callback sees the old phase value, may observe the
already-incremented stored cursor, and may replace it with any word.
The callbacks' signed phase arguments retain their original bit
conversions. No validation, signedness change, cached stable-descriptor
assumption, fixed stage count, automatic reset or completion policy is added.
Every descriptor/global reload, partial-sector path and callback-side
descriptor replacement remains intact.

The representation stays a plain `u32` at `+0x40`, before the unchanged
buffer-index halfword at `+0x44` and transfer-mode byte at `+0x46`.
Descriptor size, offsets, word-copy representation, stage/mode arguments,
transfer-state flags and all other members are unchanged. In particular,
`file_bytes` is not renamed or assigned an unproven source-cursor role,
and unrelated AI/ritual/SDK `result` identifiers remain untouched.

The existing CD-ready executable control changes only its six typed member
identifiers. Its complete remaining source, wrap/redirect observations,
callback replacement scenarios, assertions, numeric values, mutations and
generator remain exact. No new case, harness or local cloud-test execution
is introduced. The original field-relative identity, evidence and caveats
are recorded here rather than inventing a linker address for a member.
