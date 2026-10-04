# French MODEL335 sheets and screen rings

Four complete `MODEL.MRG` images for models 44 and 558, stages 7 and 8,
contain the same slot-relative five-function group. Header words are 335
and 485. The selected command is 501000, so the controller initializes
the secondary handler with argument zero and subsequently calls it with
`-1`. The image instances and independent hashes are recorded in
[the instance table](french-model-variant335-instances.csv).

The sheet helper at module offset `0x1534..0x1BC4` and ring helper at
`0x1BC4..0x20D8` are matching C: 1,680 and 1,300 instruction bytes per
image, respectively, using `gcc_2_8_1_g0_split`
(GCC 2.8.1/MASPSX 2.81). Slot 1 only renames each function. No compiler
change, forced register, assembly, artificial dependency, or source-local
external declaration is used. No shared source for another region changes.

## Ownership and remaining work

All five spans have closed, contiguous control-flow graphs:

| Offset | Bytes | Ownership |
| --- | ---: | --- |
| `0x4` | 2,964 | Entry, generated assembly |
| `0xB98` | 2,460 | Entry-called helper, generated assembly |
| `0x1534` | 1,680 | Entry-called sheets, C |
| `0x1BC4` | 1,300 | Entry-called screen rings, C |
| `0x20D8` | 2,068 | Entry-called helper, generated assembly |

The entry's calls at `0x9F8` and `0xA1C`, with the incoming context passed
in their delay slots, establish the sheet and ring helpers' callers.
All 34 distinct external call targets across the five functions are actual French resident function
starts. The four-byte module header and the complete `0x28EC..0x5000`
suffix retain separate raw owners. The suffix remains unclassified; it
is not asserted to contain only data or excluded from further research.

This registration contains 20 inventoried functions, of which eight are C,
and 11,920 C instruction bytes. The sheet recovery adds 6,720 C bytes
without adding images or function spans. It does not establish exhaustive French
overlay coverage. A complete archive reconciliation against the preceding
270 registered variant instances found 2,214 unregistered instances
(2,098 distinct images), with no exact registered entry-span duplicates.
These four images are a bounded subset of that remaining work.

## Independently recovered views

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

[The attempt ledger](french-model-variant335-attempts.csv) preserves four
paired sheet experiments and four paired ring experiments, followed by
their respective complete-production verification records.

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
