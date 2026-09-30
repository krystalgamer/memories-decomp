# French MODEL headers 465 and 615

4 distinct secondary images reuse the accepted
`src/overlays/model_variant/variant448_*.c` webs, spokes, rings, quad
bodies through 8 three-line canonical renaming wrappers.
The existing `gcc_2_8_1_g0_split` profile uses GCC 2.8.1/MASPSX 2.81.
Shared bodies, headers, G32/PSXLONG annotations and profiles are unchanged
from accepted master `0d8c1df39`. The original spokes/rings/quad addition
at `eed176532` rebuilt its calibration, layouts and canonical links after
the accepted PSXLONG migration invalidated earlier fingerprints.

## Loader and boundaries

Models 108, 573 use these images at stages 9/10.
The matched loader selects ten 2,048-byte sectors from 276-sector compact
records, at record offsets 200/210.
Loads are `0x8013B000`/`0x8017B000`, with entry at `+4`; the controller
supplies context and the initial nonnegative command or update `-1`.
The [instance ledger](french-model-variant465-instances.csv) records
independently checked indices, commands, slices and complete hashes.

The additional function at `0x39FC..0x4354` is **2,392 bytes of retained
assembly**, not entry-reachable and not hidden in raw suffix storage.
Entry-call closure alone would have missed it. Inspection at the known
post-quad boundary and strict full-span walking establish its ownership.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x124C` | 4680 | generated assembly | yes |
| `0x124C..0x166C` | 1056 | generated assembly | yes |
| `0x166C..0x256C` | 3840 | generated assembly | yes |
| `0x256C..0x2AB4` | 1352 | generated assembly | yes |
| `0x2AB4..0x3014` | 1376 | webs C | no |
| `0x3014..0x331C` | 776 | spokes C | no |
| `0x331C..0x3698` | 892 | rings C | no |
| `0x3698..0x39FC` | 868 | quad C | no |
| `0x39FC..0x4354` | 2392 | generated assembly | no |

Strict control-flow walks cover every word of each span with one terminal
return and no unresolved indirect transfer. All 4 C helpers are
**retained code, not direct-entry reachable**; no additional execution path
is claimed. Each four-byte header and 3,244-byte suffix at
`0x4354..0x5000` has a real storage owner. Suffixes remain unclassified,
not proven non-code.

## Accessed layouts

Entry captures `a0 -> s2 -> s6`. **One 296-byte record**, not two
standard sheets, begins at context `+0x11F0`. Its sheet-compatible prefix
has size at `+0x88`, which entry clears and spokes consumes. Two 18-word
arrays at `+0x98` and `+0xE0` are initialized by nested 3-by-2-by-3 loops.
The outer counter starts zero, increments once and repeats only while
nonpositive; both record pointers advance by 296. No `ModelVariantSheet`
array cast or speculative full-record declaration is introduced.

Six 144-byte rings follow at `+0x1318`, four 144-byte spoke records at
`+0x1678`, and one 144-byte quad at `+0x18B8`. Adjacent extents agree exactly.
The generic local `sizeof(ModelVariantSheet) == 152` layout constant is
not claimed as this family's record stride.

77 entry anchors check pointer capture/saves/reloads,
initialized counters, increments, bounds, strides and accessed fields.
Seventy-one freshly target-compiled local C/SDK constants check primitive
widths, ring/spoke/quad and sheet-prefix views, `SVECTOR`, `VECTOR`,
`MATRIX`, `GsCOORDINATE2`, `GsGLINE`, `POLY_G4` and `POLY_GT4`.
Unrelated generic type sizes do not establish runtime record layouts.
These are accessed-view observations, not whole-context allocation proof.

## Retained narrow webs

The unchanged accepted `variant448_webs.c` body also reproduces
`0x2AB4..0x3014` in all four complete images. Entry initializes three
416-byte records at context `0x6F8..0xBD8`: two 4-by-6 `SVECTOR` grids
at record offsets `0`/`0xC0`, color at `0x180`, and scale at `0x194`.
Pointer saves/reloads, 8-byte element and 48-byte row advances, four/six
loop bounds, 416-byte record advances and the three-record bound establish
the local layout independently of the accepted US declarations.

The helper reads timing at context `0x11F0 + 0x88 = 0x1278`, the accessed
prefix of the single 296-byte record described above, not an array of
152-byte sheets. It uses `GsGLINE` at `0x1A84`, shrinks phase-zero scale
by `step * 0xE0`, and sorts only when projected depth is positive.
No direct entry-call path to this retained helper is claimed.

Forty-three fresh retail anchors and 27 target-compiled C/SDK constants
verify these views. Real archive command `631000` selects descriptor zero
in the 52-byte-stride table at module `0x4450`; the entry's shift/add
sequence independently establishes that stride. The direct context access
minimum is `0x1B50`, separated from the measured model, auxiliary and
overlay loads. This is not whole-allocation capacity or global-isolation
proof. Eight helper callees and three resident caller owners were checked
against actual resident ELF bytes. The existing `0x80089928` binding is
renamed to the independently established `ratan2`, retaining all 37 addresses.

## Exactness and preservation

The [attempt ledger](french-model-variant465-attempts.csv) records
8 terminal wrapper matches. Candidate rebasing was
only a locator; actual links reproduce all 4 complete unmasked images.
Production verification checks selected object owners and final ELF bytes:
16 C owners / 15,648 C bytes,
20 assembly owners / 53,280 assembly bytes,
8 raw owners, 37 independently verified fresh-resident
callees, and 27 freshly recompiled web-layout constants. The original
71-constant spokes/rings/quad evidence remains preserved.

Five inherited regressions check archive slices/commands/hashes, canonical
sources/fingerprints, all spans/local calls, entry anchors and raw extents.
Two additional regressions cover the preserved binding address and
family-specific descriptors/context separation.
The combined 431/440/465 batch preserves all 158 accepted French
registrations, reproduces 168 complete images and the clean
French resident, and adds 28 C instances / 23,656 instruction bytes.
Configured totals are 714/1231 C instances
and 641,460 C instruction bytes. The accepted #6668 images are preserved;
no unmerged PR is stacked. Untranslated functions, unclassified tails and unregistered
runtime areas remain; these totals do not establish exhaustive coverage.
General report snapshots remain separate.

The webs addition is independent of that earlier census batch and starts
from accepted master `0d8c1df39`. It adds four C instances / 5,504 bytes,
preserves all 198 French image registrations and every other owner, and
reproduces the complete French resident. Configured totals are now
842/1,377 C instances and 792,204 C instruction bytes. Shared bodies,
headers and compiler profiles remain unchanged; no unmerged PR is stacked.
