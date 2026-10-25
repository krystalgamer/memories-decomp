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

## Transfer-mode byte, not a completion flag

The former `FileTransferDescriptor.done` member at `+0x46` is now
`transfer_mode`. This is a high-confidence consumer-derived name, not an
original symbol: `File_TransferReadyCallback` switches on the byte to select
the RAM-sector path (1), alternating-buffer VRAM upload (2), or RAM-staged
SPU upload (3). Those branches remain active while the same descriptor
contains outstanding bytes. Completion instead follows `total_bytes`,
`phase_remaining`, transfer-state flags and the existing callback handoffs;
the byte is not normalized to a completed/not-completed Boolean.

The wider transfer lifecycle uses the same byte without the ready callback
interpreting every value. Seek-only setup stores zero, but the seek path
is selected by the existing transfer-state flag, not by zero alone.
Sector-range setup stores four and sets the range flag; activation tests
four before setting its step flag. Cancellation service stores five and
clears `substate`; the non-range stepper handles that mode by ending ready
callbacks and submitting Pause, then finishes after its existing completion
callback sets `substate` to one. These relationships do not define a new
enum, make mode zero an idle/completion state, or bypass the separate range
and cancellation flags.

Every direct typed production access is renamed, including the independent
Japanese card-data transfer and the package, card-preview, script-image,
Library, boot, display-effect and model-slot callbacks. Their stage/mode
arguments, callback-result increments and the descriptor's byte selector
remain distinct. No callback stage number is changed to a transfer-mode
name, and all numeric assignments, local `one` values, switch cases and
unknown-mode behavior remain unchanged.

The representation is still a plain `u8` at `+0x46`, followed by `substate`
at `+0x47`, in the same `0x48`-byte descriptor. The existing size and offset
assertions retain their values; only the affected member/assertion
identifiers change. The `FileTransferDescriptorWords` copy, byte-offset
constant, loose callback ABI, pointer annotations, aliases and compiler
profiles are untouched. Unrelated model/effect records' `done` members
remain unchanged.

The two existing executable controls that name this typed member receive
only `descriptor.done` to `descriptor.transfer_mode` substitutions. Their
complete remaining sources, scenarios, callbacks, assertions, mutation
paths, values and offsets stay exact. No test, fixture, generated-C,
source-string harness or local cloud-test execution is added.

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

## Ready-sector options and skip producers

Three high-confidence option names now expose existing ready-callback
decisions without changing the request/state word or any numeric bit:

| Option | Bit | Exact consuming branch |
| --- | --- | --- |
| `FILE_TRANSFER_FLAG_SKIP_RAM_WRITE` | `0x200000` | Mode 1 bypasses both incoming-sector copy and destination advancement |
| `FILE_TRANSFER_FLAG_IMAGE_ADVANCE_X` | `0x20000` | Mode 2 advances its stored X coordinate by `0x40` instead of taking the Y-first path |
| `FILE_TRANSFER_FLAG_MEMORY_SOURCE` | `0x40000000` | Modes 1/2/3 take their existing software-copy paths instead of `CdGetSector` |

The skip option is also named at all twelve direct setters: ten existing
auxiliary-model package skip branches and the common and independent
Japanese card-data gap branches. Skipping still consumes the original
sector budget and callback phases. In mode 1, the separate ready-source
cursor still advances even when no copy or destination advance occurs.
There is no new cancellation, pause, byte-count adjustment or completion
meaning in this option.

The image option retains the exact fixed X increment, Y increment, wrap
test and alternate-buffer selection. It is not used to replace a phase
size: the equal-valued `0x20000` in card-list image setup remains a byte
count, and unrelated model/GPU fields are untouched.

Memory-source selection does not initialize that cursor, establish its
allocation, or by itself bypass disc-command scheduling. Mode 1 retains
its unconditional source-cursor advance; image mode advances it only on
the software path, and SPU mode retains its partial-sector byte advance.
The software loop's destination-member reloads, descriptor aliases,
callback/global reloads and all ordering remain exact.

Only seventeen proven production literals are replaced across four C
units. The three definitions retain the original unsuffixed integer
literals. Mixed phase clear masks, `0x10000` setup stores, request arguments,
signed tests, all other bits and every existing executable control remain
byte-for-byte untouched. No exhaustive flag set, new type, new expression,
guard, harness, testcase, source-string control or local cloud-test
execution is introduced.
