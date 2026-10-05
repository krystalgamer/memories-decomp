# Spanish MODEL headers 439 and 589

Fourteen distinct Spanish images select six C functions: five unchanged
accepted bodies (entry, bands, sheets, webs and curtains), plus an independently
recovered retained screen-ring helper. The latter was unmatched in every
configured region; it is not a port of an accepted French implementation.
All use the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 with MASPSX 2.81. Models 185, 391,
436, 504 and 594 select stages 7/8; models 367 and 395 select stages 9/10.
The [instance ledger](spanish-model-variant439-instances.csv) records actual
compact records, sectors, commands and independent complete-image hashes.
The [attempt ledger](spanish-model-variant439-attempts.csv) preserves the
70 original terminal physical matches and adds fifteen rejected ring controls
and fourteen terminal ring matches. The first exact ring candidate was
experiment sixteen; its second-slot wrapper independently matched afterward.
Existing sources, headers and compiler profiles remain unchanged.

## Complete image ownership

| Offset range | Bytes per image | Actual selected owner | Direct-entry reachable |
| --- | ---: | --- | --- |
| `0..4` | 4 | Raw header | Not code |
| `4..1230` | 4652 | Entry C | Yes |
| `1230..1704` | 1236 | Retained screen-ring C | No |
| `1704..1E7C` | 1912 | Bands C | Yes |
| `1E7C..2360` | 1252 | Sheets C | Yes |
| `2360..2878` | 1304 | Webs C | No |
| `2878..2C90` | 1048 | Curtains C | Yes |
| `2C90..5000` | 9072 | Unclassified raw suffix | Not established |

All 286,720 image bytes match without masking. Actual input objects, defining
symbols, executable/data sections and final bytes establish 84 compiler-C
owners /159,656 instruction bytes and 28 header/suffix storage owners
/127,064 bytes. The new ring selections replace 17,304 assembly bytes;
no instruction arrays or assembly bodies are presented as C.
The suffix is not claimed to be non-code.

Strict walks cover all six complete function spans and their delay slots.
The entry directly calls sheets, curtains and bands, but not the retained
web renderer or the screen-ring helper at `+0x1230`. Initializing their
records does not establish that these renderers run.

## Independently recovered screen rings

The 1,236-byte helper processes three 424-byte records at context
`E08..1300`. Each contains three seventeen-element `SVECTOR` rows at
record `0/88/110`, colors at `198/19C`, signed scale at `1A0` and count
at `1A4`. It reuses the entry's independently measured local declarations;
25 fresh target-compiled constants check these views, the packet and the
context fields.

Positive scale enables drawing. Zero rotation and translation from context
`18F8/18FC/1900` form the coordinate matrix. The helper obtains the local
screen matrix, reads and replaces its rotation, applies uniform scale,
and installs it. Seventeen points use angle increments of 256, X/Y cosine
and sine, and zero Z. Their radius is
`scale * 192 / 4096 + 128`, preserving signed division.
Keeping the `+128` at the vertex expressions, rather than pre-adding it
to the local radius, recovers the original scheduling exactly.

Sixteen quads reuse the 52-byte `POLY_GT4` at `1808..183C`.
The first projection uses rows A/C to establish texture coordinates.
Screen X below 160 selects texture-page X 320 or 0 depending on the
active buffer; otherwise X 448 or 128 is selected and 128 is subtracted
from each U coordinate. The second projection uses rows A/B for geometry.
Inner/outer colors fill the two vertex pairs. Only strictly positive
depth sorts, using its low sixteen bits; the saved projection flag is
not read. All eighteen call sites and fourteen distinct resident callees
are independently checked. The canonical `GsGetActiveBuff` import replaces
the old local alias at the unchanged resident address `0x800852A8`.

Scale below 4096 advances by context step `1944` times 96. Upon reaching
4096, phase `197C` at least three clamps it; earlier phases subtract 4096
once and increment the record count. This is not an unbounded wrap loop.
These context views do not establish allocation capacity or a direct
entry-call path.

Rejected controls retain exact mismatch counts in the ledger. They include
alias-dependent reloads, an explicitly rejected wrong translation-field
probe, named scheduling/CSE controls, byte-identical expression controls
and a same-size candidate with 24 differing setup words. The final source
uses no padding, register forcing, self-stores or identical branch bodies.

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
Fifty original preserved-register write sets per image check the five
previous C functions. Frames are 248/296/256/288/272 bytes for
entry/bands/sheets/webs/curtains; the new ring helper has a 272-byte frame.
The entry's incoming command home is stack `+252`.

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
Its 84 call sites and the helpers' 18/16/10/11/9 call sites are checked
against the real local and resident owners. Production regressions inspect
every selected C input and final definition, local and external call
relocations, all raw extents, and the actual selected resident inputs and
linked definitions for all 36 distinct family imports.

## Scope and reproduction

The initial family registration added fourteen images and 70 C instances
while preserving the then-existing 236 images. The independent ring
recovery adds fourteen C instances /17,304 instruction bytes to those same
images, preserving all accepted coverage on base `7d62be1f5`.
Its cumulative fixtures require 279 configured images, 1,581/1,787
functions in C and 2,087,928 C instruction bytes. These are configured
inventories, not exhaustive runtime coverage or all-region completion.
No French registrations, shared headers/profiles, workflows or
progress reports are changed.

From the repository root with the legally supplied inputs:

```sh
MAKEFLAGS=-j4 make spanish-match spanish-match-overlays
tools/environments/python/bin/python -m unittest \
  tools.project.tests.test_spanish_model_variant439 \
  tools.project.tests.test_model_texture_transfer
MAKEFLAGS=-j4 make match
tools/environments/python/bin/python -m unittest discover -s tools/project/tests
```
