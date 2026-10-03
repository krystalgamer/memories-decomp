# French MODEL headers 410 and 560

Two independently identified model 54 images, stages 7/8, retain one exact
ring helper each. The [instance ledger](french-model-variant410-instances.csv)
records their sectors, load addresses through slot identity, and complete
image hashes. They occupy ten sectors each at MODEL.MRG sectors 15084/15094,
loaded at `0x8013B000`/`0x8017B000`.

## Ownership and limits

| Offset range | Bytes per image | Owner |
|---|---:|---|
| `0..4` | 4 | raw header |
| `4..AAC` | 2728 | entry assembly |
| `AAC..F58` | 1196 | first helper assembly |
| `F58..18B8` | 2400 | second helper assembly |
| `18B8..1D04` | 1100 | rings C |
| `1D04..5000` | 13052 | unclassified raw suffix |

All four measured function spans in each image have fully reached, closed
control-flow graphs and one return. The entry calls the first two helpers,
but no direct call in the four measured spans reaches the ring helper.
Entry initialization establishes its accessed records; it does not prove
that this retained renderer executes. The suffix is not classified as
padding, data-only, unreachable, or non-code.

The two new C owners contribute 2,200 instruction bytes. Six other function
owners remain generated assembly, totaling 12,648 bytes; four raw
header/suffix owners preserve 26,112 bytes. These twelve owners account for
both complete 20,480-byte images, not exhaustive runtime coverage.

## Independently measured views

Two 152-byte records start at context `0x1EF8`. Each has four rows of four
eight-byte `SVECTOR`s, at `0/0x20/0x40/0x60`; colors at `0x80/0x84`;
and signed scale at `0x88`. The last twelve bytes remain opaque to this
helper. Entry and helper independently advance records by `0x98`.
Entry initializes the inner color to `(255,255,255)` and outer color to
`(0,64,255)`; the fourth projected vector row is at the local origin.

The renderer uses the `POLY_GT4` at `0x20C4`. Entry establishes a packet
pointer at `0x2090`, advances it by 52 bytes, and calls `SetPolyGT4` at
module offsets `0x238/0x23C/0x240`. This initializer evidence independently
supports the packet type, rather than merely relying on compatible fields.

Ring zero uses three word coordinates at `0x2184`; ring one uses signed
halfwords at `0x2190`. Frame parity is at `0x21C4`, unsigned elapsed time at
`0x21C8`, step at `0x21D0`, a `G32` configuration pointer at `0x21D8`, and
phase at `0x21F0`. The unsigned duration divisor is configuration `+0xC`.
Entry also compares elapsed time and duration unsigned.

The dedicated header's `0x21F4` extent is a partial accessed view, not
allocation capacity. No descriptor count or valid command range is inferred.
SDK types and declarations come from the existing project headers.

## Preserved behavior

On odd frames, signed scale divided by eight supplies a pulse; even frames
use zero. Zero rotation and the selected origin feed the original matrix
sequence: `RotMatrix`, `GsGetLs`, `GsSetLsMatrix`, `ReadRotMatrix`, another
`RotMatrix`, `ScaleMatrix`, then `SetRotMatrix`.

Four projections per record use the four vector rows. The first three
vertices receive color `+0x84`, and the fourth receives color `+0x80`.
Sorting requires strictly positive depth below 2048. The projection flag
is not used as an additional visibility condition.

For ring zero, phase zero computes scale as unsigned
`(elapsed << 12) / duration`, clamps at 4096, and advances phase to one.
Phase three shrinks it by `step * 32`, clamping at zero without advancing
phase. For ring one, phase two grows scale by `step * 512`, clamps at
16384, and advances phase to three. Its separate phase-three check can
then run in the same call, shrinking by `step * 96`; reaching zero sets
phase four. These are separate checks, not mutually exclusive branches.

## Matching evidence

The accepted French/Spanish MODEL402 renderer supplied structural prior
art. MODEL410 independently requires different record layout, dynamic
colors, direct halfword destination coordinates, and phase transitions;
its dedicated source leaves the accepted implementation unchanged.

The first reconstruction matches both complete 1,100-byte functions with
272-byte frames under the existing `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. The
[append-only attempt ledger](french-model-variant410-attempts.csv) records
both calibrations and promoted source fingerprints. Slot one changes
only the function symbol.

Independent scratch validation checks 34 target-compiled layout constants,
all eight closed function spans, packet-initializer anchors in both slots,
and 34 actual resident input-function owners against complete retail
bodies. Both complete image relinks match without masked comparisons,
using the ring C objects and explicitly owned raw fallback spans.

Clean production acceptance reproduces the complete French resident and all
291 configured French overlays. Both new production images independently
relink with maps to identical production ELFs. The twelve selected input
owners have the exact extents listed above; both C object texts equal their
frozen candidates, and all 34 resident owners are reverified after the clean
build. The 289 prior overlay registrations remain unchanged.

All 60 focused regressions pass without skips, including six dedicated
MODEL410 tests and both-slot packet/color initializer anchors. Repository
metadata validation passes. French configured totals become 1,587 C
instances out of 1,867 inventoried functions, with 2,124,588 C instruction
bytes. These counts do not establish exhaustive coverage; French #6460
stays open.
