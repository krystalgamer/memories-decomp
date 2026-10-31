# Low-level memory-card work controller

`MemCardWork_UpdateOperation` uses the request/event API, not the separate LIBMCRD modal
driver. Its read/write completion cases remain separate structured bodies:
manually merging their tails changes GCC's later cross-jump choices. The
signed sector conversion retains its explicit negative adjustment before
the arithmetic shift. No register pin or executable assembly is introduced.

## Workspace and dispatch bounds

`mem_card_work.h` describes a `0x1C` root and `0x268`-byte slots. The root's
block byte is at `+0x10`, remaining count at `+0x12`, offset at `+0x14`,
chunk at `+0x16`, and text index at `+0x1A`. Slot cursor, free-block count
and directory-entry index are at `+0`, `+0xC` and `+0xD`.

The existing `bss_image_after_viewport` backing covers `gMemCardWork_Area` at
`0x800EF6D0`. A bounded two-slot view ends at `0x800EFBBC`, four bytes
before the independently used directory buffer at `0x800EFBC0`. This is a
capacity view of existing storage, not a claim that the runtime selects both
slots. The only observed explicit resident GP store covering `gMemCardWork_bSlotIndex`
is the byte store of zero at `0x8003D31C`, in `MemCardWork_InitializeOperation`. In particular,
the selector's `u8` type does not justify 256 slots; arbitrary selectors
remain outside the measured contract. The root's block byte and text index
retain their existing interior identities, `gMemCardWork_bRootBlocks` and
`D_800EF6EA`.

The initialized `gDialog_apfnTransitionSteps` view at `0x80090F88` has five
words, ending before `D_80090F9C`: `Dialog_UpdateMessageTransition`,
`Dialog_UpdateOpenTransition`, `Dialog_UpdateCloseTransition`,
`Dialog_UpdateSpotlightContraction`, and `Dialog_UpdateMessageTransition`.
It is the `+0x20` interior of `gDisplayEffect_apfnSteps`, not separate
callback storage. Its two-argument prototype is the
controller's measured calling view. Existing implementations take the root
as `MenuRecord *` and ignore the second argument; their prototypes and
bodies are not rewritten. A low-nibble dispatch is not proof of sixteen
allocated table entries.
The [dialog-transition-family.md](dialog-transition-family.md) contract
records each callback's distinct progress, completion and lifetime behavior.

## Directory and inherited return ABI

The directory header's absolute arm preserves the controller's word loads
of the count, free-block count and `struct DIRENTRY *`. Other consumers keep
their existing declarations. The free count is intentionally read through
its low byte here. `DIRENTRY` is the actual SDK type, with size `40` and
sector-head field at `+0x20`; the controller does not substitute a private
directory-record approximation.

`MemCard_FindLoadedEntry` is an unusual existing binary ABI. Its matching C
definition is **void**, calls `MemCard_FindEntry`, and returns without
overwriting the index in MIPS `$v0`. The controller consumes that index.
`MEM_CARD_FIND_LOADED_ENTRY_RESULT_VIEW` explicitly selects the returning
caller declaration; the definition retains the default void declaration.
This is not a normal ISO C return guarantee. Native behavior tests must
model the measured returning ABI with a returning stub, not call the void
implementation and assume the host leaves its return register intact.
The unchanged implementation was separately reprobed: all 40 bytes match.

## Partial operation initialization and separate state bytes

`MemCardWork_InitializeOperation` (`0x8003D300`) keeps exactly six stores
in their original order: `gMemCardWork_bRootBlocks[0] = 1`,
`gMemCardWork_OperationState = 0`, `gDialog_bCallbackState = 0`,
`gMemCardWork_wFlags = 0x8000`, `gMemCardWork_bSlotIndex = 0`, then
`gMemCardWork_OperationMode = value`. It does not allocate or clear the
whole `gMemCardWork_Area`, submit a request, initialize SDK events or
establish a new active caller. The independently spelled root-block byte
`gMemCardWork_bRootBlocks` stays an unsized array, not a new typed-member assignment.

The operation state remains `u8`: its low nibble selects the existing
legacy controller case and bit `0x80` is that case's first-entry latch.
Whole-byte state assignments retain their latch-clearing effect. The
mode remains a complete raw `u8`: only bit zero selects the controller's
header-read/header-write paths and the combined dialog opener's
`0xD0/0xD1` channel. All other incoming bits are still stored.

Neither byte is `D_8009B3C1`. That shared byte retains its transition
dispatch nibble, transition progress flags and modal load/save/trade
first-entry uses without a new universal semantic name or type. Likewise
`tent_MemCardDialogStepState` and `gMemCard_wDialogFlags` remain the
separate LIBMCRD modal runtime's state and flags, not work-operation state.

## Request completion and UI work retain their original precedence

The work controller's initial signed status, optional debug output and
request polling stay in their existing order. Its result cases still
handle directory completion and independent read/write chunk bodies.
Both chunk bodies narrow the remaining count through the original signed
halfword view, advance the selected slot cursor and preserve unsigned
halfword offset wrap. They are not merged into a helper or shared tail.

The shared display-transition dispatch sets local status one before its
existing two-argument callback call. The later `status >= 0` return
retains precedence over message pumping and operation-state advancement.
The message path retains the raw word read over text flags, the regional
completion mask and original channel stride; only the existing equality
clears work flag `0x400`. No new completion interpretation is added.

The operation switch keeps every first-entry latch, assignment, missing
state/default behavior and fallthrough. State zero can initialize the SDK
and its events, then fall through to state one's directory request in the
same invocation. Later directory handling retains the signed lookup
result, free-block byte, encoded directory-head arithmetic and the
mode-bit branch into read or write work.

The write-header completion path still creates its message and sets the
message-wait flag before falling through into state eight's checksum and
sector-write setup in that same call. There is no newly inserted
message wait. The checksum loop retains the fixed-address frame view,
five existing interior byte stores, XOR over bytes `0..0x7E` and final
checksum store at `0x7F`. Its sector conversion keeps the explicit
negative adjustment before shifting, rather than substituting an unsigned
or floor-division interpretation.

Create-file cases retain the free-block comparison and existing terminal
debug behavior. Request results and unchecked directory/slot selectors
are not given new bounds, fallbacks, guards or status returns. This is
the legacy request/event controller, not a replacement for
`MemCardDialog_Update` or its load/save/trade state machines.

## CPU-addressed scratch and checksum backing

The read, directory-frame and write staging addresses remain respectively
`0x80200000`, `0x80210000`, and `0x80300000` (body at `0x80300200`).
The constants preserve those literal request arguments. No equivalence with
other RAM aliases or assumptions about extra physical RAM are introduced.

`MEM_CARD_WORK_FRAME_BYTES` is a bounded 128-byte view at the literal
`0x80210000` address. Bytes `D_8021007A` through `D_8021007E` and
`gMemCardWork_bFrameChecksum` (`+0x7F`) are existing
absolute linker identities inside that same range. They are **not** six
independent allocations. The controller modifies `+0x7A..+0x7E`, reads
`+0..+0x7E`, and writes the XOR to `+0x7F`. Native witnesses place one
128-byte allocation at that address and link the six names to its interior
bytes, with an adjacent guard outside the frame.

Reading the checksum through the named array instead of the literal-address
view generated an extra `addiu` for its zero low half, increasing text by
four bytes and shifting four table entries. The bounded physical-address
view preserves the literal materialization and is exact. The normalized
final source has 2,128 exact text bytes and 92 exact private-table bytes at
`0x800103B0`. Normalization runs before the probe, using the intended final
`src/game` include path.

## Native names and naming-only acceptance

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Initialize operation | `0x8003D300` | `0x8003C8C4` | `0x8003D2CC` | `0x8003D4CC` |
| Update operation | `0x8003DC1C` | `0x8003D1C8` | `0x8003DBE0` | `0x8003DDE0` |
| Raw mode | `0x8009B3C7` | `0x8009B2B7` | `0x8009C336` | `0x8009C756` |
| Operation state | `0x8009B3CF` | `0x8009B2BF` | `0x8009C33A` | `0x8009C75A` |
| Bounded work area | `0x800EF6D0` | `0x800ECDA8` | `0x800F08B0` | `0x800F0CD0` |

Only native bindings at those addresses receive the new names. The UK
`Checkerboard_DrawBackground` at the coincident `0x8003D300` address,
unrelated French NA-number aliases and all other metadata remain.
Japanese controller/area macros retire in favor of canonical native
bindings, but its other ABI, channel and SDK aliases remain. The European
controller out-parameter is bound directly as
`gMemCardWork_nRequestResult`, distinct from the modal runtime's
`gMemCardDialog_SyncCommand` word.

Every complete compiler/MASPSX output is compared after reversing only
these five names, without filtering instructions, directives, relocations
or filenames. Entire changed C files likewise conserve every byte except
the five identities and two obsolete Japanese aliases. All seven native
inventories preserve every other field and row; profiles, manifests,
source/header paths, grouping, layouts and initialized tables stay intact.
The complete accepted semantic ledger gains five evidence/caveat rows.

Three existing memory-card tests change only identifiers, including their
controller extraction marker, ABI/type probes, setup and negative-control
references. All diagnostics, assertions, fixtures, mutation behavior and
controls remain after reversing those names. No test or synthetic/generated-C
or source-string harness is added, and cloud tests are not run locally.
Clean full-executable matching remains mandatory.

The original work-operation naming proof precedes accepted #7641
integration. Its complete dialog callback identities, interior table ABI,
regional bindings, documentation and identifier-only test/trace updates
are preserved. The combined-tree proof explicitly aligns only those five
accepted names in the saved baseline before reversing the five owned work
names; every complete artifact remains. Accepted model-camera evidence and
both header moves were already part of the baseline. The integrated tree
receives a fresh clean retail gate; no cloud tests are run locally.
