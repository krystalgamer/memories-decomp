# Spanish MODEL headers 442 and 592

Four distinct Spanish images for models259/630 reuse the unchanged accepted
`variant425_{webs,bands,spokes,rings,quad}.c` bodies through ten existing
French442 wrappers. A separately refined ribbon body is selected directly
for Spanish slot 0 and included by the slot-1 wrapper. Twenty-four
compiler-owned C instances cover 27,696
instruction bytes using named `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX
2.81. Previously accepted implementations, local/SDK declarations and
profiles are unchanged; French source provenance is not assumed Spanish
byte identity.

## Loader and boundaries

Models259/630 map to compact records259/580. Stages7/8 select ten-sector
slices180/190 within the 276-sector records, loaded at
`0x8013B000`/`0x8017B000`. The matched resident controller calls offset4
with context and decoded initial command, then update command-1.
The [instance ledger](spanish-model-variant442-instances.csv) records
actual Spanish archive slices, hashes, headers and requests608000.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1174` | 4464 | generated assembly | yes |
| `1174..1560` | 1004 | generated assembly | yes |
| `1560..1F2C` | 2508 | generated assembly | yes |
| `1F2C..2924` | 2552 | generated assembly | yes |
| `2924..2CE8` | 964 | webs C | yes |
| `2CE8..3334` | 1612 | ribbons C | no |
| `3334..3A40` | 1804 | bands C | no |
| `3A40..3D50` | 784 | spokes C | no |
| `3D50..40CC` | 892 | rings C | no |
| `40CC..4430` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all ten spans,
including retained ribbons at `+0x2CE8`. Only webs is an entry-call
reachable C helper; the other five remain retained code without a
demonstrated entry execution path.

Sixteen assembly instances / 42,112 bytes remain untranslated. Real
input/final storage owners preserve every four-byte header and 3,024-byte
suffix, covering all 81,920 image bytes. The 12,096 suffix bytes remain
unclassified, not established non-code.

## Independently verified views

Entry captures `a0 -> s3 -> s6` and initializes three 512-byte webs at
context0 through `+0x600`. Each has two five-by-six `SVECTOR` grids
at0 and `+0xF0`, 48-byte rows, color at `+0x1E0` and scale at `+0x1F4`.
These are not family435's 608-byte six-by-six records. Entry calls the
webs helper at module `+0xFF0`, passing `a0 = s3` in its delay slot.

One 456-byte band at `+0x1CA0` is followed by one 152-byte sheet at
`+0x1E68`, six 144-byte rings at `+0x1F00`, four 144-byte spokes at
`+0x2260` and one 144-byte quad at `+0x24A0`. The band is not the
492-byte padded form used by family435. Its point rows, projected
coordinates, four-byte color entries and depths have independently
verified offsets and endpoints. Spokes read the sheet size at
`0x1E68 + 0x88 = 0x1EF0`.

Eight 108-byte `ModelVariantRibbonShort` records occupy
`+0x1940..+0x1CA0`. Their points, projected coordinates, angles, widths,
depths and halfword offsets have verified field and array endpoints.
The ribbon `POLY_G3` packet occupies `+0x2530..+0x254C`, immediately
before the quad packet.

The line packet is at `+0x266C`, band `POLY_GT4` at `+0x2570` and
quad `POLY_G4` at `+0x254C`. Web translation reads
`+0x26A8/+0x26AC/+0x26B0`, whereas band/ring/quad translation reads
`+0x2694/+0x2698/+0x269C`. These distinct views are not conflated.
Step and phase accesses include `+0x2700/+0x2748`.

Seventy-two Spanish instruction anchors per image verify entry/helper
capture, record initialization and strides, loop bounds, descriptor
arithmetic and the webs call. The 147 independently target-compiled
constants verify all seven canonical record types, accessed fields,
grid/array endpoints, SDK vectors/matrices, stored coordinate pointers,
packet fields and four-byte pointer/integer widths. Actual storage is a
588-byte `.rodata` section with three arrays at0/292/448; GCC's size-zero
NOTYPE labels are checked against the real section extent and all values.

Every actual request608000 selects command0. The 56-byte descriptor is
at module `+0x452C + (command % 1000) * 56`; entry stores its pointer
at context `+0x2714`. All selected windows fit their preserved suffix
owners. Direct entry accesses establish the minimum context view
`+0x2760` (10,080 bytes).

Fresh exact Spanish resident proofs verify 36 real callee input/final
owners, three matching initializer/controller/loader owners, and both
context-pointer owners at `0x80010024/28`. Those pointers belong to input
`.data` in `spanish_raw_80010000.o`, despite the mixed executable output
`.main`. Contexts `0x80136000/0x80176000` and their minimum views do
not overlap the selected MODEL, primary or secondary loads. This is not
allocation-capacity or whole-game lifetime-isolation evidence.

## Exactness and scope

The [attempt ledger](spanish-model-variant442-attempts.csv) records twelve
terminal wrapper matches. Full unmasked links, actual compiler/assembly/raw
owners, sized functions, dependency fingerprints, layouts and resident
owners are checked independently.

The ribbon calibration used accepted `variant425_ribbons.c` with local
declarations, then recovered the first-edge indexing from Spanish
instructions. Both slots were checked against both model images:

| Experiment | Compiled / target bytes | Differing words per image |
|---|---:|---:|
| Accepted fixed-index first edge | 1600 / 1612 | 221 |
| Indexed first edge, `sa[k + 1]` | 1612 / 1612 | 0 |
| Promoted body with existing resident binding alias | 1612 / 1612 | 0 |

The indexed form reproduces the separate screen pointer at stack `+0xF8`
and the `0x128` frame without pinned registers or inline assembly.
The initial calibration link exposed a missing `RotTransPers` name;
existing Spanish bindings establish its address `0x80087868`. The
promoted body uses the existing `func_french_80087868` symbol, preserving
all 36 bindings and their independently verified resident owners.

Regional regressions reuse the French source/boundary fixture, whose
descriptor test now selects the configured region's archive rather than
always reading France. Spanish-specific checks cover all fallback bindings,
actual descriptor arithmetic and the observed minimum context.
Previously accepted module records are preserved; progress snapshots
remain separate. Unknown game code, further Spanish runtime discovery and
the expanded seven-release campaign remain open.
