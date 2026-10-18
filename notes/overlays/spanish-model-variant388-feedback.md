# Spanish MODEL388 framebuffer-feedback strips

`func_8013CE9C` / `func_8017CE9C` now have independently recovered matching C:
`+1E9C..+2410`, 1,396 bytes, using `gcc_2_8_1_g0_split` (GCC 2.8.1 /
MASPSX 2.81). A fresh comparison against 6,105 accepted regional C entries
examined 36 same-size bodies and found no accepted normalized match.
No French or other regional body was ported.

## Physical loads and retained code

| Model | Record | Stages | Sectors | Command |
|---|---:|---|---|---:|
| 8 | 8 | 7 / 8 | 2388 / 2398 | 554005 |
| 43 | 43 | 9 / 10 | 12068 / 12078 | 554002 |
| 235 | 235 | 9 / 10 | 65060 / 65070 | 554001 |
| 269 | 269 | 9 / 10 | 74444 / 74454 | 554003 |
| 706 | 606 | 9 / 10 | 167456 / 167466 | 554004 |

The exhaustive loader-record scan finds **ten physical loads but eight distinct
20 KiB images**. MODEL43's two images are byte-identical to MODEL269's pair;
their loader commands select different descriptors. Both physical pairs are
registered independently. An image-hash-deduplicated candidate list alone
would miss MODEL43.

Headers are 388 / 538 at load addresses `8013B000` / `8017B000`. Each image has
six complete, contiguous, directly reachable functions at `+4`, `+CD4`,
`+1344`, `+19A4`, `+1E9C`, and `+2410`; code ends at `+2E7C`.
Only `+1E9C` is C. The five other functions and the unchanged raw tail remain
generated assembly/data, not speculative C. This adds ten C instances /
13,960 bytes while retaining fifty ASM instances.

`spanish-model-variant388-instances.csv` pins every physical image hash,
loader record, command, and descriptor fingerprint. The original entry
selects descriptors at `+2F78 + (command % 1000) * 36`.

## Recovered behavior

Entry call `+B6C` passes the original context captured at `+C`, under a
`phase >= 2` gate. Five groups at context `+5C8` have stride `1A8`; each
contains three rows of seventeen `SVECTOR` points, two colors at `+198/+19C`,
and signed progress at `+1A0`. Entry initializes all seventeen points per row
and progress to `-(group_index * 2048 / 5)`. The groups end at `+E10`.

The helper obtains the active framebuffer once. Positive-progress groups use
a sine scale and direction-derived rotation, translating from the signed
origin at `+1824` along the direction at `+182C`. Each group's sixteen strips
have two projection passes:

- Rows zero and two supply framebuffer sampling coordinates. Screen `x0 < 160`
  selects the left texture page; the other branch subtracts 128 from U.
  Active buffer zero uses page X 320 / 448, otherwise 0 / 128.
- Rows zero and one supply the drawn quad. Only this second projection's
  nonnegative depth and flag gate submission; depth narrows to `u16`.

The reused `POLY_GT4` at `+175C..+1790` is initialized after sampling, keeping
its XY fields available for UV assignment. The second projection replaces XY,
not UV. Semitransparency is enabled and raw-texture mode disabled; the first
two vertices use the inner color and the last two use the outer color.

Progress below 2048 advances by signed step `+1864` times 32. At the threshold
it wraps before phase five, otherwise clamps to 2048. The completion flag is
cleared whenever a group needed an update, even if it finishes in that call;
only a subsequent all-complete pass changes phase five to six.

## Exactness and ownership

The initial independent source already matched the body length and all but
two instructions. Declaring the texture-page temporary `u32`, rather than
`u16`, preserves the original zero-extension of the SDK's `u16` return across
`SetPolyGT4`. No SDK declaration, compiler flag, padding, or register override
was changed. Independent compilations match both load slots.

The attempt ledger records both source experiments, the separate slot-one
compilation, and ten terminal whole-image matches. Regressions check 36 target
layouts, original-context reaching definitions, initialization and packet
stores, all six function owners, both data owners, all eighteen call and four
local-jump relocations, and resident SDK/loader/context ownership. The natural
stack frame is 280 bytes.

Run the focused coverage from the repository root:

```sh
MAKEFLAGS=-j4 PYTHONDONTWRITEBYTECODE=1 tools/environments/python/bin/python \
  -m unittest tools.project.tests.test_spanish_model_variant388
```
