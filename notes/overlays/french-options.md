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
The two recovered helpers belong to that game's menu code, not Psy-Q or CRT:

| Function | Bytes | Evidence |
|---|---:|---|
| `func_801680AC` | 84 | The drawing routine at `0x80168100` calls it at `0x801681D4`, `0x801681E8`, `0x801682D0` and `0x801682E0`; results become packet vertex Y coordinates. It applies quadratic displacement outside the unchanged interval 60 through 92. |
| `func_801686A4` | 8 | Options initialization conditionally calls this empty hook at `0x801687A4`, passing a sign-extended language byte after comparing texture data. It is not an unconditional compiler startup call. The caller does not consume a return value. |

Neither C helper calls another function or accesses storage. Their arguments
and arithmetic use the existing 32-bit primitive aliases; no speculative
structures, storage declarations or SDK bindings are needed. Function names
remain address-based.

## Exact recovery and ownership

Both helpers use the named `gcc_2_8_1_g0_split` profile: GCC 2.8.1 and
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

Production uses the ordinary overlay extraction, Splat and build pipeline.
Its selected C objects and final function symbols reproduce the same 92
bytes. The four-byte header and unclassified tail have real generated-data
owners; the twelve other recognized functions remain generated assembly.
The resident-only `integrate_verified_match.py` does not accept regional
overlay manifests, so this registration follows the existing regional
overlay layout/inventory/matching-manifest integration path instead.

## Coverage and boundary caveats

The registered code interval is `[0x4, 0x1040)`: fourteen functions,
two matching C functions (92 bytes), and twelve assembly functions
(4,064 bytes). The four-byte header and all 8,128 bytes beginning at
`0x1040` are excluded from C coverage.

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
built, they also inspect its complete bytes and selected input/final C owners.
