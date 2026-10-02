# Spanish PAL options helpers

The independently checksum-verified Spanish WA archive contains five identical
12,288-byte options images at sectors 10170, 10211, 10252, 10293 and 10334.
Their SHA-256 is
`3083d6f5fcbd5d695e2466a4a52f9bdb5f1c54193334b9b3c89ff2507c8b4cd2`.
One module and four duplicate offsets are registered, not five modules.
All fourteen function control-flow graphs were checked in every physical copy.

Fourteen shared local sources in `src/overlays/pal_options/` reproduce all 4,156 inventoried instruction
bytes using the named `gcc_2_8_1_g0_split` profile
(GCC 2.8.1 / MASPSX 2.81). Spanish bindings were independently recovered from
the Spanish resident inventory and linker metadata. No reference types,
compiler flags, canonical types or resident registrations were imported or changed.
The renderer header adds only the recovered function's prototype.
The [attempt ledger](spanish-options-attempts.csv) records the fourteen original
successful source/profile experiments and the shared-step replay, using
individual source SHA-256 fingerprints.
The first six bodies were also rechecked after accepted additive declarations;
that reconciliation did not introduce a new body or profile.
The input and wave-table follow-up adds 744 unique instruction bytes to the
previous nine helpers; it adds no module or physical-image multiplicity.
The subsequent textured-strip renderer and initializer add another 1,664
unique instruction bytes while preserving those eleven selections.
The final grid renderer adds 708 unique instruction bytes, preserving all
thirteen previous selections and the historical attempt-ledger prefix.

The subsequent French suffix investigation recovers one shared signed-step
body that also reproduces this accepted optimized 192-byte helper exactly.
The historical Spanish attempt rows remain unchanged; an additional successful
replay records the revised source fingerprint after complete Spanish-image and
real-owner verification. Spanish registrations, compiler profiles, instruction
coverage and raw suffix ownership remain unchanged. The French unoptimized
wrappers are not registered as Spanish C.

## Loader and direct callers

Spanish `func_8003C56C` at `0x8003C76C` and `func_8003C70C` at `0x8003C90C`
use `src/game/european/options_package_stages.c`. The request starts at
`0x2797 + language * 0x29` and spans 41 sectors. The four phases consume
32, one, two and six sectors. The last phase copies to the pointer stored
at `0x800101D8`, whose retail value is `0x80168000`. Thus each independently
extracted code slice starts 35 sectors into its language package.
The Spanish menu runner at `0x8002D89C` directly calls initialization
(`0x801686AC`) and update (`0x80168E1C`).

| Function | Bytes | Observed contract |
|---|---:|---|
| `func_80168004` | 68 | Game text-color initialization using the canonical incomplete byte array. No direct caller found in the checked resident/options images. |
| `func_80168048` | 100 | Cursor layout using canonical object halfwords and resource pointers; called by initialization at `0x80168A04` and update at `0x80168F3C`. |
| `func_801680AC` | 84 | Quadratic displacement outside the unchanged 60..92 interval; four drawing calls at `0x801681D4`, `0x801681E8`, `0x801682D0` and `0x801682E0`. |
| `func_80168100` | 736 | Textured strips using a scratchpad `POLY_FT4`, five palette rows and signed-halfword phase; no direct caller or aligned address reference found in the checked resident/options images or five resource chunks. |
| `func_801683E0` | 708 | Signed language/unsigned object-selector dispatch to a 4-by-8 textured grid or one fast sprite; canonical scratchpad descriptors and two 5-by-9 word tables; runtime reachability remains unproven. |
| `func_801686A4` | 8 | Empty language hook called conditionally by initialization at `0x801687A4`; no consumed return value. |
| `func_801686AC` | 928 | Initialization called by the resident menu runner at `0x8002D8C0`; two VRAM captures, bounded comparison, full-width mode dispatch, five canonical object acquisitions and mode-specific music. |
| `func_80168A4C` | 412 | Input handler called by update at `0x80168E6C`; output-mode changes, signed language selection, three resource pointers and confirm/cancel completion. |
| `func_80168BE8` | 332 | Two 5-by-9 wave tables with signed phase, cosine displacement and packed grayscale; no direct caller found in the options image, so runtime reachability remains unproven. |
| `func_80168D34` | 52 | Changed-language request, called by update at `0x80168ED0`; stores the resident byte and starts the accepted boot/text loader. |
| `func_80168D68` | 180 | Language-image transfer called at `0x80168F28`; waits, stores VRAM, synchronizes, replaces the first staging byte, loads VRAM and synchronizes again. |
| `func_80168E1C` | 340 | Live menu update: low-nibble dispatch, bit-0x80 setup latch, input callback, fades, asynchronous-transfer gates, cursor update and signed completion result. |
| `func_80168F70` | 192 | Signed-halfword step/clamp. Distances above 32767 clamp immediately; ordinary distances advance by four or clamp. No direct options caller found; angular semantics are not asserted. |
| `func_80169030` | 16 | Signed language-byte accessor. No direct caller found in the checked resident/options images. |

The negative caller observations include aligned address words for the color
initializer, textured-strip renderer, grid renderer and accessor. They do not rule out indirect or other-module
references. Contiguous game code and game-specific storage establish ownership;
they do not establish runtime reachability for these retained helpers.

## Real storage and function owners

The overlay's signed halfwords are at `0x80169050` and `0x80169072`.
Signed selection bytes occupy `0x80169070`, `0x80169134` and `0x80169140`;
the update state at `0x801691FC` is unsigned. Four-byte `G32` pointers at
`0x80169074`, `0x80169078`, `0x80169138` and `0x8016913C` refer to canonical
`DisplayObjectConfig` and `DisplayObject` views. A signed phase word at
`0x80169144` and two 180-byte arrays at `0x80169080` / `0x80169148` complete
thirteen views totaling 388 bytes. The arrays have five rows of nine four-byte
elements, with a 36-byte row stride. All are backed
by sized generated-data symbols, never absolute aliases masquerading as owners.
The renderer/initializer additionally establish the fifteen-byte palette at
`0x80169040` and unsigned mode byte at `0x80169052`: fifteen measured views
totaling 404 suffix bytes. Their adjacent padding remains separately owned raw
data, not invented fields.

The resident layout is **not** the French raw-object layout:

| Selected Spanish raw object | Resident views |
|---|---|
| `spanish_raw_800101d8` | Four-byte options load pointer at `0x800101D8`. |
| `spanish_raw_8009c398` | Language byte `0x8009C44B`; four-byte transfer flags `0x8009C460`, secondary status `0x8009C484` and live buffer pointer `0x8009C4B0`. |
| `spanish_raw_800918dc` | The SDK cosine helper's 1,025-entry signed-halfword quarter-wave table at `0x80095C38`; 2,050 bytes, not compiler-C coverage. |
| `image_after_viewport` | Input halfwords at `0x8009C728` / `0x8009C72C`, output-mode byte at `0x8009C784`, inline `RECT[2]` at `0x8009C838`, canonical fade state at `0x800EB248`, object pool at `0x800F1210`, resource-bank transfer view at `0x801AF000`, color bytes at `0x801BF98C` and transfer-buffer view at `0x801DC000`. |

The three input resource pointers are acquired on the initializer's normal
construction path: calls to `DisplayObject_FindFreeGeneralSlot` at `0x80040350`
and `DisplayObject_AcquireSlot` at `0x800403D0` precede pointer stores at
`0x801688A4`, `0x80168928` and `0x80168998`. Their source is the canonical
96-record pool with a 112-byte stride; its allocatable tail begins after
sixteen records at `0x800F1910`. Byte selectors at offsets `0x68` / `0x69`
agree between the full record and the 106-byte narrow configuration view.
This is not a claim of allocation-failure safety.

Input preserves the ordered right-then-left output toggles, right precedence
for simultaneous language-repeat directions, signed-byte selection behavior,
and both measured volatile repeat-halfword loads. The wave helper increments
phase by `0x100`, calls SDK `rcos` forty times per invocation, divides signed
products toward zero, scales negative displacements by sixteen and
nonnegative ones by thirty-two, clamps intensities to 0..255, and writes
packed 24-bit grayscale. These are bounded helper contracts, not evidence
that the wave helper runs in the active options path.

The renderer uses the canonical forty-byte `POLY_FT4`, not a `POLY_GT4`.
Its packet occupies hardware scratchpad `[0x1F800344, 0x1F80036C)`, below
`0x1F800400`; this is hardware ownership, not an ELF-defined data symbol.
The packet has length nine and command `0x2C`, unsigned-halfword ordering
priority, CLUT at offset fourteen and texture page at offset twenty-two.
Forward and backward strips use signed remainder modulo 160, 32-unit steps,
the accepted easing helper and opposite five-row palette wrapping.
No renderer pointer appears in the five 4,096-byte resource chunks loaded
from package sectors 33..34, immediately before the six-sector code image.
This additional negative evidence does not establish an active drawing path.

The grid renderer always calls `SetPolyGT4`, including before the fallback.
It copies the canonical object fields into a 36-byte `SpritePrim` at
`[0x1F800320, 0x1F800344)`, layout-compatible with `GsSPRITE`. Its 52-byte
`POLY_GT4` occupies `[0x1F800344, 0x1F800378)`: length twelve, command `0x3C`,
no descriptor overlap, and both remain below `0x1F800400`.
Signed `D_80169140` is compared with unsigned object byte `0x6A`.
Equality emits 32 packets with zero priority; inequality submits one fast
sprite with zero priority. The grid reads all four corners from the two
5-by-9 word tables, reaching offset 176..179 but no farther.
Horizontal coordinate/UV steps are six and vertical steps eight, with
halfword coordinates and byte UV wrapping. The first color is stored as
three bytes, preserving the packet command; the other three colors use
whole-word stores. The sprite tail and untouched packet fields stay unchanged.

Twenty-four source/profile experiments were preserved privately. Narrowing
bottom-row displacement before baseline addition restores the target's
instruction shape. Advancing `x` before `column` in the loop increment clause
resolves the remaining saved-register allocation. The result is ordinary
compiler-generated C, not register-pinned code or rewritten instructions.
No direct J/JAL or aligned grid-function pointer was found in the checked
resident/options images or five resource chunks. Indirect/other-module
references remain possible; no unused-code exclusion is asserted.

`D_800E9D70` is a sixteen-byte array of two rectangles, not a pointer variable.
`D_8009B118_IS_POINTER_IN_DATA` selects the existing four-byte buffer-pointer
declaration. Its zero initial value is not the runtime destination:
`File_SetPositionTable` passes `gLibrary_aCardArtRecord` to
`File_InitTransferState`, which stores that pointer. The language-image helper
uses 1,536 bytes for the 48-by-16 halfword rectangle, from `(0x290, 0)` to
`(0x290, 192)`. No new array capacity or whole-game lifetime isolation is claimed.
Initialization captures those two rectangles at buffer offsets `0x2000` and
zero, synchronizing after each. Its measured staging footprint is
`0x2600` bytes, and comparison covers offsets `0x60` through `0x59F`
inclusive. Equal compared bytes select buffer byte zero; a mismatch preserves
the resident language, narrowed from unsigned byte to the signed overlay byte.
Mode is stored as a byte but dispatched as the original signed 32-bit argument:
256 follows the nonzero path. That path calls the empty hook, selects state two
and plays `0x7370`. Zero mode acquires/configures five objects, clamps negative
signed output modes to zero, establishes the resource/cursor pointers, resets
phase and plays `0x7350`. `D_801AF000` remains an incomplete byte bank; the
loader's 4,096-byte transfer is a measured view, not a general capacity claim.
Canonical `FadeTransitionState` is 40 bytes; its flags are one byte at offset
six. The active mask is `0x80`; the transfer-blocking mask is `0x02000030`.

Resident C dependencies include `DisplayObject_UpdateResourceVariant` at
`0x80040748`, boot helpers at `0x80043D7C` / `0x80043DC8`, fade helpers at
`0x800156F8` / `0x80015820`, and `SD_BGMFadeOut` at `0x80040258`.
Input additionally uses `DisplayObject_SetResourceVariant` at `0x80040734`,
`SD_SEPlayFull` at `0x80040204` and `SD_SetOutputType` at `0x80047430`.
The boot helpers belong to `src/game/spanish/main_run_boot_sequence.c`,
not the French wrapper. SDK `DrawSync`, `LoadImage` and `StoreImage` at
`0x8007FC64`, `0x8007FF10` and `0x8007FF70` are actual functions in the
selected `generated/spanish_80073c4c` assembly object. SDK `rcos` at
`0x800866F8` is likewise a real 160-byte function in that object; its lookup
table has a separate raw-data owner. `GsSortPoly` is the actual 452-byte SDK
function at `0x800842A8`, not a similarly named alternative. None is promoted as
game C. Complete resident bodies, selected objects and final sections were
checked for all 22 resident callees used by the options image.
The renderer/initializer follow-up separately verifies its nine actual callees,
including the canonical allocation/configuration/depth helpers and `SD_BGMPlay`.
The grid separately verifies real SDK owners for `SetPolyGT4` at `0x80082EE8`
(20 bytes), `GsSortPoly` at `0x800842A8` (452 bytes), and `GsSortFastSprite`
at `0x80084978` (380 bytes), including selected input objects and final
executable sections rather than just absolute symbol bindings.

## Validation and remaining coverage

Each complete image links fourteen real compiler-C owners (4,156 bytes), no
generated-assembly instruction owner, and twenty-three sized raw owners
(8,132 bytes). All 12,288 bytes match each of the five independently extracted
retail copies. Target-compiled probes verify 58 canonical layout/type constants
in 232 read-only bytes; host behavior tests are not alternative matching builds.
The input/wave follow-up independently compiles another 46 layout/type
constants in 184 read-only bytes and checks fresh resident input objects for
the cosine table, object pool and pointer-producing callees.
The renderer/initializer probe compiles 86 layout/type constants in 344
read-only bytes, independently checking the packet, canonical objects,
rectangle array, measured staging/resource views and call signatures.
The grid target probe verifies another 77 layout/type constants in 308
read-only bytes, including both scratchpad descriptors and all table extents.

An ILP32 host oracle exercised 3,073 easing positions, 768 cursor cases,
1,792 language requests, every signed language-byte representation,
1,048,576 signed-step pairs, 256 language-image transfers and 12,288 update
state/gate combinations, including callback-driven completion. Actual source
mutations removing the signed-distance quirk, the transfer-blocking mask or
GPU synchronization were compiled and rejected. A separate canonical loader
oracle checked five language requests and 1,536 phase/initial-byte-pattern
cases, including unchanged bytes and out-of-range phases. Host-only include
ordering accommodates the pre-existing graphics-buffer header cycle; historical
unspecified-argument callback semantics require a pre-C23 host dialect.
No production headers were altered to accommodate the host compiler.

The follow-up ILP32 oracle checks 163,840 input combinations, including all
signed-byte selection representations, simultaneous directions, confirm
buttons, exact call order and untouched configuration bytes. It also checks
4,105 wave states (all 4,096 phase residues plus nine signed-wrap boundaries),
184,725 table cells, forty calls per invocation and adjacent storage guards.
Cosine values come from the checksum-verified Spanish SDK table. Six compiled
source mutations are rejected: reversed direction precedence, wrong confirm
sound, wrong selector field, arithmetic shift replacing signed division,
missing final column and wrong phase increment. Host-only signed-wrap flags
model the measured target additions; host tests do not establish reachability.

The renderer oracle executes the unchanged renderer and easing sources for
262,144 cases: all 65,536 signed-halfword phases with four palette patterns,
including the actual Spanish palette and UV/CLUT wrap boundaries. It checks
all forty packet bytes, final packet state, unsigned priorities, SDK call
order/count/arguments and guards around the privately mapped scratchpad
address. Eight compiled source mutations are rejected.

The initializer oracle executes the unchanged initializer and cursor sources
for 172,032 cases: 22,528 full-width-mode/comparison-boundary cases, 131,072
signed language/output combinations and 18,432 pointer-reload cases.
It checks both VRAM/sync sequences, inline rectangles, staging extents and
guards, five allocations and configuration arguments, flags, pointer stores,
cursor coordinates, state, phase, music and otherwise untouched object bytes.
Twelve compiled source mutations are rejected. The first host fixture's
incorrect signed resident-byte declaration was rejected by the canonical
header; only that fixture was corrected. Host stubs are not proof of GPU
execution, allocation-failure safety or whole-menu runtime behavior.

The grid ILP32 oracle executes the unchanged recovered body for 196,608 cases:
all 65,536 signed-language/unsigned-selector pairs, plus every halfword
coordinate value under two table/byte patterns. It checks 4,198,400 complete
grid packets, the fallback, unconditional initialization, SDK call order and
arguments, coordinate/UV wrapping, table corners and extreme words, untouched
descriptor bytes, scratchpad guards and unchanged inputs. Fourteen compiled
source mutations are rejected. Host signed-wrap flags model measured target
arithmetic. Bottom-halfword narrowing affects target instruction shape but
not the stored low halfword, so exact target comparison, not this oracle,
enforces that distinction. SDK mocks do not prove GPU execution or reachability.

The code inventory covers `[0x4, 0x1040)` and all fourteen functions select C.
The four-byte header and entire 8,128-byte suffix stay out of C coverage;
404 suffix bytes have the scalar/pointer/table contracts above. Other
suffix bytes remain unclassified, not declared non-code. The generated raw
owners can share input objects; twenty-three symbols do not imply twenty-three
separate objects.

Regional regression tests reuse the existing options ownership machinery
while independently pinning the Spanish selection, bindings, raw owners and
attempt history. Missing legal inputs skip before they are opened.
Registration uses the existing regional overlay pipeline because the
resident-only `integrate_verified_match.py` does not accept overlay manifests.
The original registration preserved all 250 prior Spanish modules and added
fourteen inventoried functions. This follow-up preserves all 251 module
records and all thirteen prior C selections while adding one unique C function,
not five times that count. Configured options coverage does not establish
whole-release completion or classify the remaining suffix.
