# French PAL options helpers

The French options package contains a previously unregistered runtime image.
Its six code sectors begin at WA sectors 10170, 10211, 10252, 10293 and
10334. All five complete 12,288-byte images are identical:
`3083d6f5fcbd5d695e2466a4a52f9bdb5f1c54193334b9b3c89ff2507c8b4cd2`.
The manifest registers one representative and four exact duplicate offsets,
not five independently configured modules.

## Loader and game ownership

The accepted regional `File_RequestOptionsPackage` requests
`0x2797 + language * 0x29` sectors with length `0x29`. Its staged loader
consumes the preceding image/data blocks before copying the final six
sectors to the pointer stored at `D_800101D8`. The independently
checksum-verified French executable stores `0x80168000` there.
See `src/game/european/options_package_stages.c` and its shared
`src/game/frontend_package_stages.c` body. The French resident inventory
places these staged/request functions at `0x8003C76C` and `0x8003C90C`.

The accepted menu runner at French `0x8002D89C` uses the shared European
wrapper in `src/game/european/main_run_options_menu.c`: it calls options
initialization at `0x801686AC` and update at `0x80168E1C`.
The recovered helpers belong to that game's menu code, not Psy-Q or CRT:

| Function | Bytes | Evidence |
|---|---:|---|
| `func_80168004` | 68 | Same game color-slot setup as the prefix of North American `Options_InitTextDisplay`, without its text-box calls. It uses the existing `gText_abColorSlots` declaration. No direct caller was found; reachability is not claimed. |
| `func_80168048` | 100 | Initialization calls it at `0x80168A04`, and update at `0x80168F3C`. It positions a cursor through canonical `DisplayObject` halfwords and updates another object's resource variant. |
| `func_801680AC` | 84 | The drawing routine at `0x80168100` calls it at `0x801681D4`, `0x801681E8`, `0x801682D0` and `0x801682E0`; results become packet vertex Y coordinates. It applies quadratic displacement outside the unchanged interval 60 through 92. |
| `func_801686A4` | 8 | Options initialization conditionally calls this empty hook at `0x801687A4`, passing a sign-extended language byte after comparing texture data. It is not an unconditional compiler startup call. The caller does not consume a return value. |
| `func_80168A4C` | 412 | Live update calls it at `0x80168E6C`. It handles output-type and language selection, updates canonical resource-view fields, plays feedback sounds, and requests the exit transition. |
| `func_80168BE8` | 332 | Updates two measured five-by-nine tables of signed heights and packed grayscale colors using a phase word and the canonical SDK cosine routine. No direct resident/options caller was found; runtime reachability is not claimed. |
| `func_80168D34` | 52 | Update calls it at `0x80168ED0`. It stores a changed language byte and requests the corresponding boot/text package asynchronously through the accepted resident loader. |
| `func_80168D68` | 180 | Update calls it at `0x80168F28` after fade and file-transfer completion. It preserves a VRAM rectangle in the established transfer buffer, replaces its first byte with the selected language, and uploads it to the second canonical rectangle. |
| `func_80168E1C` | 340 | Live update entry called by the resident menu runner. It dispatches input, coordinates fades and asynchronous language loading, updates cursor layout, and returns the signed language byte on completion or -1 while active. |
| `func_80168F70` | 192 | Signed-halfword step using two real overlay storage locations. It chooses a direction with a 32767 threshold and moves by four with a clamp. No direct options caller was found; angular semantics and runtime reachability are not asserted. |
| `func_80169030` | 16 | Signed accessor for the same overlay byte that initialization, input handling and update use as the language selection. No direct caller was found; reachability is not claimed. |

The original easing and empty-hook helpers call no functions or access storage. Their arguments
and arithmetic use the existing 32-bit primitive aliases; no speculative
structures, storage declarations or SDK bindings are needed. Function names
remain address-based.

## Exact recovery and ownership

All eleven routines use the named `gcc_2_8_1_g0_split` profile: GCC 2.8.1 and
MASPSX 2.81. The easing function needs the three piecewise assignments to a
shared result scalar and in-place squaring/shifting of the distance.
Early returns reverse the final branch layout; a single return of the
modified argument shortens the function to 80 bytes.
The [attempt ledger](french-options-attempts.csv) retains every distinct
source/profile experiment, including failed ones.

Before integration, each of the five archive slices was independently linked
with both compiled C objects and actual raw-region owners for the remaining
bytes. Every complete image matched, and the selected input objects and
final ELF symbols were checked for executable, section-defined function
ownership and exact extents. Raw fixture ownership is not a claim that the
remaining bytes are data or matching C.

The four additional helpers matched their first independently recovered
source shapes. The color probe initially stopped after compilation and
ownership checks because an assumed direct caller did not exist. Its exact
object was preserved, not recompiled as a new experiment; the reachability
caveat is retained in the inventory and regressions.

The language-image routine matched on its third source experiment: writing
the second rectangle's X field before introducing a scoped destination
pointer gives the measured Y/width/height store bases. The live dispatcher
matched directly. The signed step matched on its fourth experiment using
assigned current/target values in the condition and distinct scoped
next/limit lifetimes, without forced registers or artificial stores.
These eight ledger rows hash the sorted frozen source/header digest lines
as `name:sha256\n`. The three new C objects were also linked together with
the six prior C and five actual generated-assembly function objects into
each complete archive image. Every function had a real section-defined
owner, including the then-assembly input-handler callee.

The input handler matched on its second experiment: a meaningful selection
scalar restores the initial lifetime, and the existing
`GINPUT_PAD1_REPEAT_IS_VOLATILE` declaration arm preserves the two measured
halfword reads at `0x80168AF8` and `0x80168B0C`. The ordinary scalar had
commoned the latter read. This does not introduce a new shared input type.
The table update matched on its sixth experiment. Its height store from
the assigned shade value preserves both lifetimes across signed division;
separate assignments duplicate a shift, and the reverse assignment nesting
adds a register move and a delay-slot nop. The additional eight ledger rows
use the same sorted frozen source/header digest-line scheme. Both objects
were linked together with the nine prior C and three actual assembly
definitions in all five complete images.

Production uses the ordinary overlay extraction, Splat and build pipeline.
Its selected C objects and final function symbols reproduce 1,784 bytes.
The four-byte header and remaining tail regions have real generated-data
owners; the three other recognized functions remain generated assembly.
The resident-only `integrate_verified_match.py` does not accept regional
overlay manifests, so this registration follows the existing regional
overlay layout/inventory/matching-manifest integration path instead.

## Storage and resident contracts

The following overlay storage is backed by real generated-data definitions,
not absolute linker aliases:

| Address | Bytes | Contract and evidence |
|---|---:|---|
| `0x80169050` | 2 | Signed current halfword read, stepped, clamped and written by `func_80168F70`; no more specific semantics asserted. |
| `0x80169070` | 1 | Signed output-selection byte; initialized at `0x8016892C` and changed by input handling at `0x80168AA4`. |
| `0x80169072` | 2 | Signed target halfword read and compared by `func_80168F70`. |
| `0x80169074` | 4 | `G32` canonical resource-view pointer, acquired through `DisplayObject_AcquireSlot` and stored at `0x801688A4`. |
| `0x80169078` | 4 | `G32` display-object resource view pointer, stored at `0x80168998` after object acquisition. |
| `0x80169080` | 180 | Five rows of nine signed 32-bit heights. The table loop writes exactly this extent, ending before the selection byte at `0x80169134`. |
| `0x80169134` | 1 | Signed selection byte passed by update to the existing cursor-layout routine. |
| `0x80169138` | 4 | Second `G32` canonical resource-view pointer, acquired and stored at `0x80168928`; input updates its established selector bytes at `0x68` and `0x69`. |
| `0x8016913C` | 4 | `G32` canonical `DisplayObject` pointer, stored at `0x801689FC` after object acquisition; layout writes the established halfwords at offsets `0x30` and `0x32`. |
| `0x80169140` | 1 | Signed language-selection byte, initialized at `0x80168724` and changed by input handling at `0x80168B5C`. |
| `0x80169144` | 4 | Phase word advanced by 0x100 before filling the tables. |
| `0x80169148` | 180 | Five rows of nine packed grayscale words, ending immediately before update state at `0x801691FC`. |
| `0x801691FC` | 1 | Unsigned update-state byte; low nibble selects the state and bit 0x80 records one-time transition setup. |

The external bindings reference separately verified resident owners.
`D_8009C02B` is the existing unsigned byte declaration in
`src/game/duel_effect_resource_setup.h`, relocated to French `0x8009C44B`.
`gText_abColorSlots` retains its incomplete byte-array declaration in
`src/game/text_constants.h` at `0x801BF98C`; no array bound is invented.
Both lie inside the actual selected `resident_tail_8cb98` raw input object,
whose section-defined start is `0x8009C398`. The complete input region and
its final ELF bytes match the retail executable. The absolute convenience
symbols themselves are not treated as defining objects.

The boot package's three text-bank destinations are independently measured
from the French executable: `0x801B0000` for 30 sectors, `0x801C0000` for 32,
and `0x801D5800` for seven. None overlaps the color-slot address.
The resident calls are `DisplayObject_UpdateResourceVariant` at `0x80040748`
(40 bytes) and `func_80043BC8` at `0x80043DC8` (116 bytes). Their accepted
shared declarations, selected compiled C objects, final section-defined
functions and complete retail bodies are verified. The loader's declaration
is exposed through its existing `main_run_boot_sequence.h` owner.

The language-image update reuses `graphics_frame.h`'s two-element `RECT`
array (`D_800E9D70`, French `0x8009C838`, 16 bytes) and `unmatched.h`'s
existing `D_8009B118_IS_POINTER_IN_DATA` arm: a four-byte `u8 *G32`
transfer-buffer pointer at `0x8009C4B0`. Its initial retail value is zero,
not the runtime buffer address. Accepted `File_SetPositionTable`
(`0x80013600`, 256 bytes) passes `gLibrary_aCardArtRecord` at `0x801DC000`
to `File_InitTransferState` (`0x800137B4`, 92 bytes), which stores that
address. The required 48-by-16 halfword transfer extent is 1,536 bytes,
within the independently verified resident raw owner; no array bound is
invented. The rectangle pair and pointer storage share that same real owner.

The existing font setup `func_80043B7C` is accepted C at French `0x80043D7C`
(76 bytes), declared through `main_run_boot_sequence.h`. Adding its
prototype preserves all twenty previously configured resident/overlay
source-profile consumers as complete ELF objects, including relocations.
The SDK dependencies are **DrawSync** at `0x8007FC64` (104 bytes),
**LoadImage** at `0x8007FF10` (96 bytes), and **StoreImage** at
`0x8007FF70` (96 bytes), not their `*Image2` variants. Their actual
section-defined functions live in the selected `text_73c4c` assembly object;
SDK aliases alone are not ownership evidence and none is promoted as game C.

Update reuses canonical `gFade_State.flags`, file-transfer masks/status words,
and sound declarations. `gFade_State` at `0x800EB248` (40 bytes),
`D_8009B0F4` at `0x8009C460` (four bytes), and `D_8009B134` at
`0x8009C484` (four bytes) are within the measured raw owner.
Its accepted resident C callees are `Fade_StartIn` at `0x800156F8`
(64 bytes), `Fade_StartOut` at `0x80015820` (64 bytes), and
`SD_BGMFadeOut` at `0x80040258` (36 bytes). Regressions verify the
selected input objects and complete final bodies, not masked instructions.

Input retains canonical `input.h`, `sound.h` and `DisplayObjectConfig`
contracts. The two halfword input owners at `0x8009C72C` and `0x8009C728`,
and signed output-type byte at `0x8009C784`, lie within the verified resident
raw region. Accepted C callees are `SD_SetOutputType` at `0x80047430`
(104 bytes), `SD_SEPlayFull` at `0x80040204` (40 bytes), and
`DisplayObject_SetResourceVariant` at `0x80040734` (20 bytes).
Pointer provenance is checked through `DisplayObject_FindFreeGeneralSlot`
at `0x80040350` (64 bytes) and `DisplayObject_AcquireSlot` at `0x800403D0`
(352 bytes), including the returned-pointer moves and actual overlay stores.
No initial zero pointer value is used to infer an allocated object's address.

The cosine callee is `rcos`, not `rsin`: alias `0x800866F8` references the
actual 160-byte `func_800866F8` definition in the selected `text_73c4c`
SDK assembly object. Its input/final function owners and full retail body
are verified. The two arrays' dimensions follow the five-row, nine-column
loops, four-byte stores and 0x24-byte row strides; their real data definitions
do not overlap adjacent state. The table update has no demonstrated direct
caller or aligned address-word reference in the verified resident/options
images. Its menu-table behavior and contiguous boundaries support game
ownership without claiming execution or excluding indirect/other-module use.

## Coverage and boundary caveats

The registered code interval is `[0x4, 0x1040)`: fourteen functions,
eleven matching C functions (1,784 bytes), and three assembly functions
(2,372 bytes). The four-byte header and all 8,128 bytes beginning at
`0x1040` are excluded from C coverage. 388 bytes of that suffix now have
the measured scalar/pointer/array contracts above; the other 7,740 bytes remain
unclassified.

Neither direct jumps/calls nor aligned address words targeting the color
initializer or language accessor were found in the complete verified options
and resident images. Their game ownership follows their contiguous function
boundaries and established game-only state/behavior, not a demonstrated
runtime call chain. They may be retained unused routines; indirect or other
module references have not been ruled out.

The census is discovery evidence, not the function inventory. Its starts at
`0x801680E8`, `0x801680F4`, `0x8016866C`, `0x80168D3C`, `0x80168FCC` and
`0x80169024` lie inside the registered functions and are not separately
promoted. In particular, the branches and returns of the easing function
occupy the complete `[0xAC, 0x100)` interval. The unclassified suffix contains
additional instruction-shaped material and references to current function
interiors; neither their execution nor their ownership is established by
those patterns. This partial registration does not assert exhaustive options
or French runtime coverage.

Reproduce from the repository root with legal French inputs:

```sh
MAKEFLAGS=-j4 make french-match-overlays
tools/environments/python/bin/python -m unittest discover \
  -s tools/project/tests -p test_french_options.py
```

The regression tests check configured coverage, duplicate slices, loader
storage, helper call sites and contracts. When the production image has been
built and optional `pyelftools` is available, they also inspect its complete
bytes and selected input/final C owners. Missing retail/build inputs or that
optional ELF dependency produce explicit skips; metadata tests still run.
