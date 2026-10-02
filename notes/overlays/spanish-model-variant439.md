# Spanish MODEL headers 439 and 589

Fourteen distinct Spanish images independently reuse five unchanged accepted
local C bodies: entry, bands, sheets, webs and curtains. All use the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 with MASPSX 2.81. Models 185, 391,
436, 504 and 594 select stages 7/8; models 367 and 395 select stages 9/10.
The [instance ledger](spanish-model-variant439-instances.csv) records actual
compact records, sectors, commands and independent complete-image hashes.
The [attempt ledger](spanish-model-variant439-attempts.csv) records all
70 terminal physical function matches, arising from ten exact source/profile
experiments (five roles in both slots). No source or compiler change is needed.

## Complete image ownership

| Offset range | Bytes per image | Actual selected owner | Direct-entry reachable |
| --- | ---: | --- | --- |
| `0..4` | 4 | Raw header | Not code |
| `4..1230` | 4652 | Entry C | Yes |
| `1230..1704` | 1236 | Generated assembly | No |
| `1704..1E7C` | 1912 | Bands C | Yes |
| `1E7C..2360` | 1252 | Sheets C | Yes |
| `2360..2878` | 1304 | Webs C | No |
| `2878..2C90` | 1048 | Curtains C | Yes |
| `2C90..5000` | 9072 | Unclassified raw suffix | Not established |

All 286,720 image bytes match without masking. Actual input objects, defining
symbols, executable/data sections and final bytes establish 70 compiler-C
owners /142,352 instruction bytes, fourteen genuine assembly owners /17,304
bytes, and 28 header/suffix storage owners /127,064 bytes. Every one of the
309 fallback instruction words per image is generated disassembly, not an
opaque instruction array. The suffix is not claimed to be non-code.

Strict walks cover all six complete function spans and their delay slots.
The entry directly calls sheets, curtains and bands, but not the retained
web renderer or the assembly helper at `+0x1230`. Initializing web records
does not establish that their renderer runs.

## Independent Spanish loader and ownership evidence

Each image occupies ten 2,048-byte sectors in a 276-sector compact MODEL
record. Stages 7/8 occupy record sectors 180/190; stages 9/10 occupy 200/210.
Loads are `0x8013B000` and `0x8017B000`, entry `+4`.

The loader copies slot `field_DFE` to transfer `position` and the slot index
to `callback_data`. The transfer callback selects both actual alternate
routes. Record sector 275 at `+0x110/+0x114` supplies slot commands at
`+0xD08/+0xD0C`; the dispatcher passes the selected command modulo 1000 to
secondary entry `+4` for initialization, and -1 for updates.

All 36 imports are independently recovered from accepted Spanish metadata
and checked against the actual resident inventory and retail bodies.
A fresh matching Spanish resident establishes their real defining objects,
four loader/dispatcher C owners and eight pointer-storage owners. Input
pointer storage is data even when the final resident section combines
code and data. Both slot context roots are checked against the measured
model, auxiliary and overlay load ranges.

## Context, geometry and packets

Fresh target compilation verifies 150 layout constants /600 read-only bytes.
The local entry view accesses a minimum `0x1990` bytes; this is **not**
allocation capacity or proof of whole-game lifetime isolation.

Three 416-byte webs occupy `0..4E0`, one 456-byte band `4E0..6A8`,
and two 152-byte sheets `6A8..7D8`. Six rings, four spokes, one fan and
three screen-ring records precede four 280-byte curtains at `1300..1760`.
The entry initializes all four curtains, including seventeen points per
row. The helper draws only the first three (`1300..1648`), sixteen strips
each. The fourth initialized record is neither discarded nor claimed drawn.

Entry captures original context `a0 -> s3 -> s8`. Initialization reuses s3
and its stack `+0x94` cursor, so a delay-slot-aware reaching-definition walk
independently proves the original s3 definition reaches all three update
calls at `1094/10B0/10C8`, with argument moves in their delay slots.
Fifty complete preserved-register write sets per image check all five C
functions. Frames are 248/296/256/288/272 bytes for entry/bands/sheets/webs/
curtains; the entry's incoming command home is stack `+252`.

Stable 52-byte `POLY_GT4` packets are at context `17A0`, `17D4` and `183C`
for bands, sheets and curtains. Their projected screen pairs are at packet
`8/20/32/44`, with RGB groups at `4/16/28/40`. The web's 20-byte `GsGLINE`
is at `18D0`, with colors at `12/15`. Fresh compiled layouts, 105 family
instruction anchors per image and additional runtime assertions check
these views and the independent initialization/drawing bounds.

## Gates, descriptors and timing

Bands clamp negative depth and clear projection flags before sorting the
low sixteen depth bits. Sheets scale depth by 8/10 and require nonnegative
depth and flag. Curtains require positive scale and nonnegative depth and
flag. Retained webs sort only strictly positive depth and do not read the
saved projection flag. These differences are preserved, not normalized.

Commands `605000..605005` select 48-byte descriptors at
image `+0x2D8C + (command % 1000) * 48`, saved in context `+0x194C`.
Actual command-selected values are checked in every Spanish slice:

| Command | Sheet start | Band growth start/end | Shrink start/end |
| --- | ---: | --- | --- |
| 605000 | 60 | 100/112 | 200/240 |
| 605001 | 0 | 56/68 | 240/300 |
| 605002 | 10 | 56/64 | 120/140 |
| 605003 | 10 | 56/64 | 320/360 |
| 605004 | 40 | 180/192 | 360/420 |
| 605005 | 60 | 110/120 | 360/420 |

Every selected timing divisor is nonzero. Entry uses an unsigned comparison
for sheet start versus frame, then phase gates for curtains and bands.
Its 84 call sites and the helpers' 16/10/11/9 call sites are checked against
the real local and resident owners.

## Scope and reproduction

Preserve all 236 previous Spanish registrations. This batch adds fourteen
images and 70 C instances, for configured totals of 250 images,
1,306/1,574 functions in C and 1,725,348 C instruction bytes. These are
configured inventories, not exhaustive runtime coverage or all-region
completion. No C/header/profile/French-registration/workflow or shared
progress-report changes are part of this batch.

From the repository root with the legally supplied inputs:

```sh
MAKEFLAGS=-j4 make spanish-match spanish-match-overlays
tools/environments/python/bin/python -m unittest \
  tools.project.tests.test_spanish_model_variant439 \
  tools.project.tests.test_model_texture_transfer
MAKEFLAGS=-j4 make match
tools/environments/python/bin/python -m unittest discover -s tools/project/tests
```
