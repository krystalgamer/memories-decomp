# Spanish PAL options helpers

The independently checksum-verified Spanish WA archive contains five identical
12,288-byte options images at sectors 10170, 10211, 10252, 10293 and 10334.
Their SHA-256 is
`3083d6f5fcbd5d695e2466a4a52f9bdb5f1c54193334b9b3c89ff2507c8b4cd2`.
One module and four duplicate offsets are registered, not five modules.
All fourteen function control-flow graphs were checked in every physical copy.

Nine unchanged accepted local sources in `src/overlays/pal_options/` reproduce
1,040 instruction bytes using the named `gcc_2_8_1_g0_split` profile
(GCC 2.8.1 / MASPSX 2.81). Spanish bindings were independently recovered from
the Spanish resident inventory and linker metadata. No reference types,
compiler flags, production declarations or resident registrations were changed.
The [attempt ledger](spanish-options-attempts.csv) records the nine successful
source/profile experiments, using individual source SHA-256 fingerprints.
The first six bodies were also rechecked after accepted additive declarations;
that reconciliation did not introduce a new body or profile.

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
| `func_801686A4` | 8 | Empty language hook called conditionally by initialization at `0x801687A4`; no consumed return value. |
| `func_80168D34` | 52 | Changed-language request, called by update at `0x80168ED0`; stores the resident byte and starts the accepted boot/text loader. |
| `func_80168D68` | 180 | Language-image transfer called at `0x80168F28`; waits, stores VRAM, synchronizes, replaces the first staging byte, loads VRAM and synchronizes again. |
| `func_80168E1C` | 340 | Live menu update: low-nibble dispatch, bit-0x80 setup latch, input callback, fades, asynchronous-transfer gates, cursor update and signed completion result. |
| `func_80168F70` | 192 | Signed-halfword step/clamp. Distances above 32767 clamp immediately; ordinary distances advance by four or clamp. No direct options caller found; angular semantics are not asserted. |
| `func_80169030` | 16 | Signed language-byte accessor. No direct caller found in the checked resident/options images. |

The negative caller observations include aligned address words for the color
initializer and accessor. They do not rule out indirect or other-module
references. Contiguous game code and game-specific storage establish ownership;
they do not establish runtime reachability for these retained helpers.

## Real storage and function owners

The overlay's signed halfwords are at `0x80169050` and `0x80169072`.
Signed selection bytes occupy `0x80169070`, `0x80169134` and `0x80169140`;
the update state at `0x801691FC` is unsigned. Four-byte `G32` pointers at
`0x80169078` and `0x8016913C` refer to canonical `DisplayObjectConfig` and
`DisplayObject` views. These eight views total sixteen bytes. All are backed
by sized generated-data symbols, never absolute aliases masquerading as owners.

The resident layout is **not** the French raw-object layout:

| Selected Spanish raw object | Resident views |
|---|---|
| `spanish_raw_800101d8` | Four-byte options load pointer at `0x800101D8`. |
| `spanish_raw_8009c398` | Language byte `0x8009C44B`; four-byte transfer flags `0x8009C460`, secondary status `0x8009C484` and live buffer pointer `0x8009C4B0`. |
| `image_after_viewport` | Inline `RECT[2]` at `0x8009C838`, canonical fade state at `0x800EB248`, color bytes at `0x801BF98C` and transfer-buffer view at `0x801DC000`. |

`D_800E9D70` is a sixteen-byte array of two rectangles, not a pointer variable.
`D_8009B118_IS_POINTER_IN_DATA` selects the existing four-byte buffer-pointer
declaration. Its zero initial value is not the runtime destination:
`File_SetPositionTable` passes `gLibrary_aCardArtRecord` to
`File_InitTransferState`, which stores that pointer. The language-image helper
uses 1,536 bytes for the 48-by-16 halfword rectangle, from `(0x290, 0)` to
`(0x290, 192)`. No new array capacity or whole-game lifetime isolation is claimed.
Canonical `FadeTransitionState` is 40 bytes; its flags are one byte at offset
six. The active mask is `0x80`; the transfer-blocking mask is `0x02000030`.

Resident C dependencies include `DisplayObject_UpdateResourceVariant` at
`0x80040748`, boot helpers at `0x80043D7C` / `0x80043DC8`, fade helpers at
`0x800156F8` / `0x80015820`, and `SD_BGMFadeOut` at `0x80040258`.
The boot helpers belong to `src/game/spanish/main_run_boot_sequence.c`,
not the French wrapper. SDK `DrawSync`, `LoadImage` and `StoreImage` at
`0x8007FC64`, `0x8007FF10` and `0x8007FF70` are actual functions in the
selected `generated/spanish_80073c4c` assembly object. None is promoted as
game C. Complete resident bodies, selected objects and final sections were
checked for all 22 resident callees used by the options image.

## Validation and remaining coverage

Each scratch image links nine real compiler-C owners (1,040 bytes), five
real generated-assembly owners (3,116 bytes), and seventeen sized raw owners
(8,132 bytes). All 12,288 bytes match each of the five independently extracted
retail copies. Target-compiled probes verify 58 canonical layout/type constants
in 232 read-only bytes; host behavior tests are not alternative matching builds.

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

The code inventory covers `[0x4, 0x1040)`. Five routines remain assembly.
The four-byte header and entire 8,128-byte suffix stay out of C coverage;
only sixteen suffix bytes have the scalar/pointer contracts above. Other
suffix bytes remain unclassified, not declared non-code. The generated raw
owners can share input objects; seventeen symbols do not imply seventeen
separate objects.

Regional regression tests reuse the existing options ownership machinery
while independently pinning the Spanish selection, bindings, raw owners and
attempt history. Missing legal inputs skip before they are opened.
Registration uses the existing regional overlay pipeline because the
resident-only `integrate_verified_match.py` does not accept overlay manifests.
All 250 prior Spanish modules remain unchanged; this adds nine unique C
functions and fourteen inventoried functions, not five times those counts.
