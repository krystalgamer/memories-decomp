# Spanish SU intro runtime

The accepted European `src/game/european/model_intro_controller.c` wrapper
selects SU sector `0x6E7` (1767). The shared controller requests sixteen
2048-byte sectors at `0x80180000`, calls reset at `0x80180420`, starts cue zero
at `0x80180004`, and subsequently polls `0x8018019C`. This is a separate load
from the main-menu module at the same address.

The complete 32,768-byte module starts with header word `0x34` and has SHA-256
`f5fa6a720f1a584e229f63e8e8e6bc9c6a9bdb3ad37ec279de38df6ba3380ce1`.
The SU archive SHA-256 is
`4785e6cecf44792ed311c7274f22c21a93967fdc5ab8d155fb9692131f30b3e5`.
Existing Spanish SU input staging covers this module; no additional private
input or upload is introduced.

## Recovered C extent

`src/overlays/spanish_model_intro/runtime.c` is one contiguous translation unit,
compiled with the named `gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).

| Address | Bytes | Observed operation |
|---|---:|---|
| `0x80180004` | 408 | Start primary text cue and optional secondary channel |
| `0x8018019C` | 644 | Poll four channel states and update grid brightness |
| `0x80180420` | 128 | Reset active fields and install display callback |
| `0x801804A0` | 16 | Read completion byte |
| `0x801804B0` | 288 | Draw scrolling horizontal and vertical line grid |

These five functions cover exactly **1,484 instruction bytes**, ending at
`0x801805D0`. Reset stores the last function's actual linked address into
`DisplayObject.field_4C`; it is a callback, not a resident import. Address-based
names remain in use. No inline assembly, copied instruction words or padded
substitute function is used.

## Local data and resident contracts

| Symbol | Bytes | Recovered view |
|---|---:|---|
| `D_801805D0` | 128 | 32 four-byte text/secondary/clear/flags cue records |
| `D_80180650` | 1 | Completion flag |
| `D_80180654` | 24 | Four six-byte delay/phase/unused/cue/active records |
| `D_8018066C` | 4 | Display-object pointer |
| `D_80180670` | 4 | Unsigned frame counter |

Each symbol has actual generated `.data` input storage. None is defined in the
external linker bindings. These are **preserved data, not C-owned data**;
three padding bytes after the completion flag and the four-byte module header
are preserved separately from the five recovered functions.

The cue and state types stay local. Existing headers supply `DuelEffectChannel`,
`DisplayObject`, `LINE_F2`, `GsOT`, text-box and packet APIs. The owning
`text_box_lifecycle.h` now declares the existing `TextBox_DestroyIndex(s32)`.
The canonical channel-array symbol `D_800EB0F8` binds to Spanish `0x800F0850`,
and the canonical signed word `D_8009B0D8` binds to `0x8009C43C`; its explicit
`u16` view preserves the observed halfword load. Display brightness uses only
the low byte of the existing `DisplayObject.field_0C` word.

`DuelEffect_HasActiveEntry` binds to `0x800372E0`, named `func_800370E0` by the
Spanish resident inventory. This is the same alias used by accepted European
wrappers, not a new guessed signature. All other resident bindings are recorded
in `config/sles_03951/overlays/model_intro_linker_symbols.txt`.

## Preserved behavior and matching experiments

Start marks a primary state active before handling cue text 255. Reset clears
only active fields, not entire states. Poll uses the signed halfword delay and
low-halfword frame step; completion sets phase 16. Secondary fading checks
channel flags `0x8000`. The original early sentinel, unused state byte and
channel state transitions have not been normalized.

The grid uses chained RGB/coordinate assignments, unsigned remainders,
a separate signed right shift for the vertical offset, and post-tested loops
whose counters advance before packet submission. Remainders guarantee that
both loops run at least once. The source preserves original store order.

The first complete candidate had sizes 408/636/128/16/336, with respectively
2/158/1/0/72 differing words. Correcting increment/store order and grid loop
shape reduced this to 0/137/1/0/7. Reusing the brightness variable as the outer
index and splitting cue-index evaluation restored the 644-byte poll, leaving
18 differences there and none elsewhere. Separating the next-cue temporary
from the secondary-loop counter left three allocation differences. Direct
cue-array addressing removed those last differences. The reset discrepancy
was only the callback address displaced by the earlier short poll.

The terminal scratch candidate fingerprint was
`349fd07deed9a790420b14c7a723b1329e039beb3da7ff330db6fcbb1d5b36fb`.
Promotion changes include paths and moves the text-box prototype to its owner;
the production module must still pass `make spanish-match-overlays`.

## Unresolved tail and completion scope

Offset `0x674..0x8000` is **31,116 bytes of unclassified preserved storage**.
Its separate `opaque_tail` Splat segment is a storage mechanism, not evidence
that every byte is non-code. No C function count, SDK exclusion or exhaustive
module-discovery claim is inferred from that tail or from the full-image hash.
Other MODEL/SU loads, the boot ownership audit and opaque overworld fragments
remain part of the larger runtime campaign.

Validation uses the actual linked function addresses/sizes and selected
compiler object, real generated-data owners, target structure layouts,
resident imports, complete Spanish production images and clean resident
matching. Metadata regressions pin the loader slice, contiguous group,
preserved tail and distinct module accounting.

## Independent French registration

The accepted French resident manifest also selects the European controller
for all four controller functions. Its sector `0x6E7`, sixteen-sector read,
load address and reset/start/poll callbacks identify the French intro slice
independently of the Spanish module name. Reading that slice from
`game/france/DATA/SU.MRG` verifies the complete archive and 32,768-byte module
hashes above, including header word `0x34`.

The first unchanged shared-source trial reproduced the entire French slice.
All five functions have their exact linked addresses and sizes and occupy
one real 1,484-byte executable C input section without gaps or assembly
substitutes. The callback address is supplied by paired HI16/LO16 relocations
to the actual `func_801804B0` C definition. No French source copy, wrapper,
type changes or compiler-profile changes are needed.

All twelve imports were checked against French resident ownership, not merely
copied from the Spanish aliases. The ten callable bindings correspond to
matching French resident functions; `DuelEffect_HasActiveEntry` uses the
established `func_800370E0` alias at `0x800372E0`. The two data bindings agree
with French `link_symbols.ld`: channel array `D_800EB0F8` at `0x800F0850`
and frame-step word `D_8009B0D8` at `0x8009C43C`. Complete-image equality
also checks their actual encoded uses.

Each of the five local data symbols in the table above has one real,
non-executable generated-data input owner with its exact extent and retail
bytes, and a section-defined final symbol. Ownership inspection enumerates
the actual linker inputs, not stale objects left in a build directory.
Twenty-three target-GCC constants verify cue/channel sizes and field offsets,
display brightness and callback offsets, channel flags/style offsets, and
the complete `LINE_F2` packet layout used here.

French registration adds five functions and 1,484 C bytes without altering
the accepted duel bank or other images. Against accepted `8cd92d502`, this is
210 matching function instances / 126,264 C bytes across eight configured
images. The bank remains 81/85 functions on this independent branch.
The 31,116-byte opaque intro tail remains explicitly unclassified; neither
5/5 inventoried intro functions nor the whole-module hash resolves it.
Boot ownership, other MODEL/SU loads and overworld fragments remain open.

The additive refresh onto accepted `2bd5202b8` retains all 83 accepted French
bank rows, including effects 16/20 and the number renderer. Only the five
intro registrations are added: 212 matching instances / 130,088 C bytes
across eight configured images. Ritual and curve remain bank assembly
fallbacks on this independent branch; no pending ritual PR is stacked.
