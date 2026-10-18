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
| `func_80168100` | 736 | Draws textured strips in two directions through the accepted easing helper, a measured five-by-three byte table, canonical `POLY_FT4` and SDK `GsSortPoly`. No direct resident/options caller was found; runtime reachability is not claimed. |
| `func_801683E0` | 708 | Draws a four-by-eight grid through two measured five-by-nine tables when the signed language byte equals the object's unsigned selector; otherwise sorts one fast sprite. Reuses unchanged accepted Spanish C. No direct resident/options caller was found; runtime reachability is not claimed. |
| `func_801686A4` | 8 | Options initialization conditionally calls this empty hook at `0x801687A4`, passing a sign-extended language byte after comparing texture data. It is not an unconditional compiler startup call. The caller does not consume a return value. |
| `func_801686AC` | 928 | Resident menu runner calls it at `0x8002D8C0`. It saves and compares two VRAM rectangles, selects the language, allocates/configures five canonical display objects on the zero-mode path, initializes update state and chooses background music. |
| `func_80168A4C` | 412 | Live update calls it at `0x80168E6C`. It handles output-type and language selection, updates canonical resource-view fields, plays feedback sounds, and requests the exit transition. |
| `func_80168BE8` | 332 | Updates two measured five-by-nine tables of signed heights and packed grayscale colors using a phase word and the canonical SDK cosine routine. No direct resident/options caller was found; runtime reachability is not claimed. |
| `func_80168D34` | 52 | Update calls it at `0x80168ED0`. It stores a changed language byte and requests the corresponding boot/text package asynchronously through the accepted resident loader. |
| `func_80168D68` | 180 | Update calls it at `0x80168F28` after fade and file-transfer completion. It preserves a VRAM rectangle in the established transfer buffer, replaces its first byte with the selected language, and uploads it to the second canonical rectangle. |
| `func_80168E1C` | 340 | Live update entry called by the resident menu runner. It dispatches input, coordinates fades and asynchronous language loading, updates cursor layout, and returns the signed language byte on completion or -1 while active. |
| `func_80168F70` | 192 | Signed-halfword step using two real overlay storage locations. It chooses a direction with a 32767 threshold and moves by four with a clamp. No direct options caller was found; angular semantics and runtime reachability are not asserted. |
| `func_80169030` | 16 | Signed accessor for the same overlay byte that initialization, input handling and update use as the language selection. No direct caller was found; reachability is not claimed. |
| `func_80169D78` | 452 | Unoptimized counterpart of the signed-halfword step, reproduced by the same C body as `func_80168F70`. Its current and target views are at `0x80169F88` and `0x80169FAA`. No direct caller was found; original producer and runtime reachability remain unproven. |
| `func_80169F3C` | 56 | Unoptimized counterpart of the signed-byte accessor, reproduced by the existing accessor body with its measured byte reference at `0x8016A078`. No direct caller was found; original producer and runtime reachability remain unproven. |

The original easing and empty-hook helpers call no functions or access storage. Their arguments
and arithmetic use the existing 32-bit primitive aliases; no speculative
structures, storage declarations or SDK bindings are needed. Function names
remain address-based.

## Exact recovery and ownership

The fourteen prefix routines use the named `gcc_2_8_1_g0_split` profile;
the two suffix leaves use the existing `gcc_2_8_1_o0_g0_split` profile.
Both use GCC 2.8.1 and MASPSX 2.81. The easing function needs the three piecewise assignments to a
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

The strip renderer has four materially distinct source/header experiments.
The first failed to compile because the canonical `libgpu.h`/`libgs.h`
prerequisites from `libgte.h` were missing; its ledger instruction counts are
blank, not zero. Correct includes produced 736 bytes with twenty differing
words, and an unchanged accepted-base replay confirmed that result. Keeping
the narrowed phase in the meaningful retained start scalar reduced this to
four scheduling differences. Initializing packet colors before phase
arithmetic resolved those four words without forced registers or extra stores.
Its digest entries use the same frozen source/header scheme. All five complete
images were independently linked with this C object, nine then-accepted C
objects and four real assembly function objects; integration also preserves
all eleven C objects accepted before the renderer's promotion.

The initializer matched on its second experiment. The first body was
900 bytes with 212 differing common-position words: its counted scan and
rectangle-pointer lifetime shortened the transfer prefix, while downstream
object setup already matched after the shift. Taking the transfer-pointer
snapshot before state stores, retaining the second rectangle while scoping
the first rectangle separately, and using a bounded equality scan restore
all 928 bytes. The all-equal path reloads the global transfer pointer before
reading its first byte. Both ledger rows hash the sorted frozen source/header
digest lines. The exact object and all twelve accepted C objects were linked
with the remaining mesh assembly definition in five complete images;
integration preserves those twelve previous objects, including relocations.

The grid matches the unchanged accepted [Spanish implementation](spanish-options.md).
Its twenty-one previous experiments and replays remain nonmatching in the ledger.
Their fingerprints retain the sorted frozen `mesh.c`/`mesh.h` digest-line scheme;
word counts now include absent or extra words rather than only the common length.
The terminal row hashes sorted repository-relative digest lines for `grid.c`,
`renderers.h`, `helpers.h`, `sprite_primitive.h` and `display_object.h`.
Narrowing the bottom-row displacement before adding baseline Y, and advancing X
before the column counter, preserve the accepted source's exact scheduling.
No source, header, compiler profile or canonical type was changed for French.
Both complete regional images and all ten physical copies were independently
linked and compared, with twenty-eight real C function owners, forty-six sized
raw symbols and all twenty-seven previous C objects preserved.

Production uses the ordinary overlay extraction, Splat and build pipeline.
Its sixteen selected C objects and final function symbols reproduce 4,664 bytes.
The four-byte header and remaining tail regions have real generated-data
owners; no inventoried options function remains generated assembly.
The resident-only `integrate_verified_match.py` does not accept regional
overlay manifests, so this registration follows the existing regional
overlay layout/inventory/matching-manifest integration path instead.

## Unoptimized suffix leaves

The suffix contains a second options-shaped instruction sequence. The two
complete leaf boundaries at `0x80169D78..0x80169F3C` and
`0x80169F3C..0x80169F74` reproduce the accepted game step and accessor under
an existing unoptimized profile. The step's recovered body also reproduces
the accepted optimized 192-byte prefix helper without changing its instructions.
This shared-source result, the signed storage accesses, and the surrounding
options-shaped state/table operations establish game-code ownership, not a
Psy-Q/CRT signature. They do not identify the original producer or establish
that the suffix is reached by the active menu.

The former accepted step body produced 452 bytes but 92 differing words at
the suffix boundary. Direct reconstruction reproduces all 452 bytes, including
the real difference store at `0x80169DB8` to frame offset eight. No subsequent
instruction reads that slot. The shared source therefore retains the observed
otherwise-unused local; optimization removes it from the unchanged prefix
instructions. This is not a forced register, volatile store or fake dependency.
The two wrappers rename only measured function and scalar addresses before
including the shared bodies and existing header declarations. No new types,
source-local extern declarations, compiler profiles or resident aliases are added.

The unchanged accessor matches all 56 bytes with both existing O0 split and
no-split profiles; production selects the split profile. The ledger retains
both calibrations, the rejected step shape, the direct reconstruction, and
the accepted-base shared-source replays. New source-set fingerprints hash sorted
repository-relative `path:sha256\n` lines for the relevant body, wrapper,
`helpers.h` and `src/types.h`; historical probe rows use their frozen probe
path and the header/body revision from their recorded calibration base.

An aligned literal/J/JAL/conditional-branch encoding scan of the verified French
resident and all 253 configured French images found no incoming references to
either leaf entry. This does not cover arbitrary computed or register-indirect
targets and is not a dead-code exclusion. Two sampled external calls in the
surrounding suffix, `0x80065E08` and `0x80066054`, land inside other inventoried
functions in all seven verified resident versions. They are not legitimate
callee identities and receive no guessed bindings.

The existing fifteen-byte palette reappears at `0x80169F74`; other surrounding
state/table references are displaced by `0xF38` from their prefix counterparts.
An older unoptimized image remainder is a possibility, not established
provenance. All other bytes remain opaque raw owners, including the image end
that resembles packed data. Only 508 instruction bytes and three measured scalar
views totaling five bytes are newly classified; 7,211 suffix bytes remain
unclassified. Inventoried function completion is not exhaustive image completion.

## Storage and resident contracts

The following overlay storage is backed by real generated-data definitions,
not absolute linker aliases:

| Address | Bytes | Contract and evidence |
|---|---:|---|
| `0x80169040` | 15 | Five three-byte rows supplying U, V and palette-X offset. Both strip loops wrap indices within 0 through 4 and access row offsets 0, 1 and 2. The following byte remains separately unclassified. |
| `0x80169050` | 2 | Signed current halfword read, stepped, clamped and written by `func_80168F70`; no more specific semantics asserted. |
| `0x80169052` | 1 | Initialization stores the low byte of its mode argument at `0x801686F0`. The following 29 bytes remain separately unclassified; no wider field is inferred. |
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
| `0x80169F88` | 2 | Signed current halfword loaded and stored by the unoptimized step. No angular meaning or active runtime state is inferred. |
| `0x80169FAA` | 2 | Signed target halfword compared by the unoptimized step. |
| `0x8016A078` | 1 | Signed byte loaded by the unoptimized accessor and referenced by the surrounding options-shaped sequence. Active language selection is not asserted for this suffix view. |

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

Initialization also saves a rectangle at buffer offset `0x2000`, so its two
transfers require an enclosing extent of `0x2600` bytes, through
`0x801DE600` exclusive. That complete range lies inside the verified raw
owner; this is not an exclusive buffer-lifetime claim or an invented common
array bound. The comparison covers offsets `[0x60, 0x5A0)` in each saved
rectangle, within their `0x600`-byte extents.

The resource argument is the canonical incomplete byte bank `D_801AF000`
from `display_asset_banks.h`, not a display-object allocation. Its address
and starting byte have the same real raw owner. Passing the address does
not establish the bank's complete extent. Initializer calls reuse the
accepted `DisplayObject_FindFreeGeneralSlot`/`DisplayObject_AcquireSlot`
contracts and existing `G32` object/resource-view pointers. Additional
accepted C owners are `DisplayObject_ConfigureSpriteAtPosition` at
`0x80040800` (68 bytes), `DisplayObject_ConfigureSpriteAtPositionWithResource`
at `0x80042BD8` (68 bytes), `DisplayObject_SetDepthOffset` at `0x80042C1C`
(44 bytes), and `SD_BGMPlay` at `0x8004022C` (44 bytes). Their selected
input objects and full final retail bodies are independently verified.

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

The strip renderer uses the actual 452-byte `func_800842A8` definition
behind `GsSortPoly` at `0x800842A8` in the selected SDK object, not a
similarly named sprite or fast-primitive routine. Its argument contracts
are canonical `POLY_FT4`, `GsOT` and `DisplayObject.field_14`; no SDK code
is promoted. The packet is the established forty-byte GPU primitive at
hardware scratchpad address `0x1F800344`, wholly inside the SDK-documented
`0x1F800000` through `0x1F800400` extent. Compiler-measured canonical field
offsets agree with the stores, including length nine at offset 3 and
code 0x2C at offset 7. Scratchpad ownership is hardware, not an invented
ELF symbol or an absolute alias presented as a defining object.
The separate fifteen-byte table and existing signed phase halfword have
real generated-data owners. The renderer reuses the accepted easing C
function and keeps the no-direct-reference caveat: no direct jump/call
or aligned address word was found in the verified resident/options images.
Indirect, constructed and other-module references remain possible.

The grid uses the same `GsSortPoly` owner, plus actual SDK definitions
`func_80082EE8` (`SetPolyGT4`, 20 bytes) and `func_80084978`
(`GsSortFastSprite`, 380 bytes). Both regions' selected SDK inputs and complete
final retail bodies were independently verified; no SDK function is promoted.
`SetPolyGT4` is called before either branch. Both sort paths pass priority zero.
The canonical 36-byte `SpritePrim`/`GsSPRITE` view occupies
`[0x1F800320, 0x1F800344)` and the 52-byte `POLY_GT4` occupies
`[0x1F800344, 0x1F800378)`, inside hardware scratchpad. Independently
target-compiled layout constants confirm the canonical offsets, including
`DisplayObject.attribute` at 4 rather than the sprite's attribute at 0.
The table accesses reach row 4, column 8, bytes 176 through 179 of each
180-byte owner. Initial RGB uses byte stores preserving the packet command;
the remaining vertex colors use full-word stores. X/UV advance by six and
Y by eight. The signed language byte and unsigned object selector at `0x6A`
retain their distinct contracts.

Neither complete regional options image nor any of their ten preceding
4,096-byte resource chunks contains a direct jump/call or aligned address
word for the grid renderer. The verified residents likewise provide no
direct or aligned-word reference. These negative searches do not rule out
indirect, constructed or other-module references.

## Coverage and boundary caveats

The registered code intervals are `[0x4, 0x1040)` and `[0x1D78, 0x1F74)`:
sixteen matching C functions (4,664 bytes), with no inventoried assembly
functions. The four-byte header and 7,620 remaining suffix bytes are excluded
from C coverage. Of the latter, 409 bytes have measured scalar/pointer/array
contracts; the other 7,211 bytes remain unclassified. The three selected raw
input objects expose thirty sized symbols, not thirty independent objects.

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
