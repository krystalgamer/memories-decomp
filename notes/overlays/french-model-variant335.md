# French MODEL335 ribbons, sheets and screen rings

Four complete `MODEL.MRG` images for models 44 and 558, stages 7 and 8,
contain the same slot-relative five-function group. Header words are 335
and 485. The selected command is 501000, so the controller initializes
the secondary handler with argument zero and subsequently calls it with
`-1`. The image instances and independent hashes are recorded in
[the instance table](french-model-variant335-instances.csv).

The ribbon helper at `0xB98..0x1534`, sheet helper at `0x1534..0x1BC4`
and ring helper at `0x1BC4..0x20D8` are matching C: 2,460, 1,680 and
1,300 instruction bytes per image, respectively, using `gcc_2_8_1_g0_split`
(GCC 2.8.1/MASPSX 2.81). Slot 1 only renames each function. No compiler
change, forced register, assembly, artificial dependency, or source-local
external declaration is used. No shared source for another region changes.

## Ownership and remaining work

All five spans have closed, contiguous control-flow graphs:

| Offset | Bytes | Ownership |
| --- | ---: | --- |
| `0x4` | 2,964 | Entry, generated assembly |
| `0xB98` | 2,460 | Entry-called ribbons, C |
| `0x1534` | 1,680 | Entry-called sheets, C |
| `0x1BC4` | 1,300 | Entry-called screen rings, C |
| `0x20D8` | 2,068 | Entry-called helper, generated assembly |

The entry's calls at `0x9F0`, `0x9F8` and `0xA1C`, with the incoming context
passed in their delay slots, establish the ribbon, sheet and ring callers.
All 34 distinct external call targets across the five functions are actual French resident function
starts. The four-byte module header and the complete `0x28EC..0x5000`
suffix retain separate raw owners. The suffix remains unclassified; it
is not asserted to contain only data or excluded from further research.

This registration contains 20 inventoried functions, of which twelve are C,
and 21,760 C instruction bytes. The ribbon recovery adds 9,840 C bytes
while preserving the accepted sheets and rings, without adding images or
function spans. It does not establish exhaustive French
overlay coverage. A complete archive reconciliation against the preceding
270 registered variant instances found 2,214 unregistered instances
(2,098 distinct images), with no exact registered entry-span duplicates.
These four images are a bounded subset of that remaining work.

## Independently recovered views

Three `0x394`-byte ribbon records fill context `0..0xABC`, exactly before
the sheet array. Each has 17-point rows at `0x0/0x110`, packed screen rows
at `0x88/0x198`, angle and width arrays at `0xCC/0x1DC`, colour at `0x220`,
depth and projection flags at `0x2C8/0x30C`, and signed-halfword offset
columns at `0x350/0x372`. Bytes `0x224..0x2C8` remain unnamed. The native
screen union exposes the same four bytes as a `DVECTOR` or `PSXLONG`.
The shared terminal halfword-column view addresses the measured fields
within the same record; it does not introduce another record or storage.
Two alternating 40-byte `POLY_FT4` packets fill `0x2D18..0x2D68`.

The existing resident binding at `0x80087868` is named `RotTransPers`
consistently in this family's linker and symbol lists. Its 44-byte SDK
body loads the two vector words, executes RTPS, writes SXY2, IR0 and FLAG
through arguments two through four, and returns SZ3 shifted right by two.
This directly verifies the local SDK declaration and is an alias correction,
not a resident inventory or SDK ownership change.

The entry initializes four `0x98`-byte sheet records at context `+0xABC`,
ending exactly at the ring array at `+0xD1C`. The helper projects four
quads per record from four rows of four eight-byte points. Rows start at
`+0x0/+0x20/+0x40/+0x60`; outer and inner colours are at `+0x80/+0x84`,
and signed scale is at `+0x88`. These independently measured offsets
agree with the existing `ModelVariantSheet` declaration. The sheet
`POLY_GT4` at `+0x2CB0` ends where the ring packet begins at `+0x2CE4`.
Target-compiler checks cover both record layouts and the projection and
transform types; no reference-project type or compiler claim is used.

The entry initializes three ring records at context `+0xD1C`, advancing
by `0x1A8` and generating 17 points in each of three rows. The ring helper
uses the same stride and bounds. Its local record contains:

| Field | Offset | Bytes |
| --- | ---: | ---: |
| Point row A | `0x0` | 136 |
| Point row B | `0x88` | 136 |
| Point row C | `0x110` | 136 |
| Inner colour | `0x198` | 4 |
| Outer colour | `0x19C` | 4 |
| Scale | `0x1A0` | 4 |
| Cycle count | `0x1A4` | 4 |

The fourth colour bytes are not read by this helper. The local record
view does not create additional storage or claim an allocation capacity.
Target-compiler layout constants verify the 424-byte stride, all seven
field offsets, the eight-byte `SVECTOR`, and the 52-byte `POLY_GT4`.
The quad is at `+0x2CE4`; signed translation halfwords are at
`+0x2E18/+0x2E1A/+0x2E1C`; step and phase words are at `+0x2E58`
and `+0x2EA0`. These helper accesses fit below context `+0x2EA4`.
The resident pointer table places secondary contexts at `0x80136000`
and `0x80176000`; these spans do not overlap their loaded model,
primary-handler, or secondary-handler images. This is a bounded helper
access proof, not a global allocation or lifetime proof.

## Reconstruction evidence

[The attempt ledger](french-model-variant335-attempts.csv) preserves thirteen
paired ribbon experiments, four paired sheet experiments and four paired
ring experiments, with their respective complete-production records.

The ribbon recovery progresses from 2,444 bytes/393 differing words to
2,460 bytes with the exact 304-byte frame. Genuine sheet-pointer declaration
order recovers spills; a promoted geometry index recovers point arithmetic.
The terminal offset-column view and native signed screen access recover
projection operations. A previous-screen iterator advances by the measured
record stride, rather than being recomputed each iteration. Initializing
the point index, projection-output reference and screen iterator in their
observed order recovers the final two setup instructions. The ledger retains
the intervening regressions, including the 312-byte-frame candidate.

The ribbons descend from the moving origin at `0x2DE8` to its X/Z copy
with Y zero at `0x2DF8`. Three angular phases and two sinusoidal components
shape each 17-point path; the copy is displaced along the recovered yaw
by a width derived from signed halfword `0x2E92`. The helper preserves the
second `ratan2` call whose return value is unused. Terminal projection uses
points 15/16; other segments use consecutive points. Nonnegative depth and
flags gate polygon sorting. Phase four uses unsigned timing interpolation
to grow signed count `0x2E8C` to 16 and advance to phase five. Phase six
derives width from the first sheet's signed scale divided by four, clamped
at zero. Wave counters at `0x2E98/0x2E9C` advance by step times 1150/200.

The first sheet candidate had the exact 1,680-byte size and 272-byte frame
but differed in 12 setup instructions. Splitting the bearing calculation
into a separate statement did not change the output. Reusing the radial
angle variable for the initial rotation bias corrected the setup sequence
but extended its lifetime, producing 95 register-colouring differences.
A distinct, genuinely used one-use rotation-bias scalar recovered every
instruction in both slots.

The first three sheets orbit the word-vector origin at `+0x2DE8`, using
half the signed radius at `+0x2E78`; the fourth uses the translation
halfwords at `+0x2E18`. The angle combines the bearing from
`+0x2E34/+0x2E3C`, rotation at `+0x2E74`, 3072, and one third-turn per
sheet. Rendering in phase zero is restricted to sheet zero; later phases
render all sheets, subject to nonnegative depth and projection flags.
The phase transitions retain the measured unsigned frame interpolation
through the timing-table pointer at `+0x2E60`. The fourth sheet grows to
16384 in phase five and later shrinks without setting phase seven; the
first three shrink from 4096 and set phase seven on completion. These
differences are preserved rather than generalized across the sheets.

The first ring candidate had the right size and frame but
34 differing words. Correcting the scale-update branch scope and the
measured spill ordering reduced this to 23. Moving the radius expression
after the transform fields added 24 bytes and regressed to 298 differing
words. Keeping `scale / 8` before transform setup and adding 512 in the
two point-coordinate expressions recovered every instruction in both
slots. The compiler hoists this genuine loop-invariant addition.

The retail branch skips rendering when the cycle count is nonpositive,
but still advances scale and checks completion. Its completion scalar
starts at one and is assigned one again in the terminal branch; the C
preserves that observed behaviour without introducing volatility.

Complete production images, sized linked C ownership, every retained
assembly/raw span, and the French resident executable are the acceptance
gates. Tests also cover loader sectors and commands, independent image
hashes, all five CFGs, caller arguments, local record layout, external
callee starts, and the slot wrappers.
