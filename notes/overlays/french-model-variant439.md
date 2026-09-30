# French MODEL headers 439 and 589

Fourteen distinct images reuse unchanged accepted `variant422_sheets.c`
and `variant422_curtains.c` through four canonical renaming wrappers,
using the named GCC 2.8.1/MASPSX 2.81 profile `gcc_2_8_1_g0_split`.
Models 185, 391, 436, 504 and 594 select stages 7/8; models 367 and 395
select stages 9/10. The [instance ledger](french-model-variant439-instances.csv)
records the actual compact indices, commands, slices and hashes.

## Loader and complete ownership

Each image occupies ten 2,048-byte sectors in a 276-sector compact MODEL
record. Stages 7/8 use record offsets 180/190; stages 9/10 use 200/210.
Loads are `0x8013B000`/`0x8017B000`, entry `+4`. The matched loader and
controller supply context and command/update arguments.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `0x4..0x1230` | 4652 | generated assembly | yes |
| `0x1230..0x1704` | 1236 | generated assembly | no |
| `0x1704..0x1E7C` | 1912 | generated assembly | yes |
| `0x1E7C..0x2360` | 1252 | sheets C | yes |
| `0x2360..0x2878` | 1304 | generated assembly | no |
| `0x2878..0x2C90` | 1048 | curtains C | yes |

Strict walks cover all instructions and delay slots in each complete
span, with one terminal return and no unresolved indirect transfer.
The four-byte header and 9,072-byte suffix at `0x2C90..0x5000` have
real storage owners. The suffix remains unclassified, not proven non-code.

## Four initialized curtains, three drawn

Entry captures `a0 -> s3 -> s8`. Two 152-byte sheets at
`0x6A8..0x7D8` have four corner arrays at `0/0x20/0x40/0x60`,
colors at `0x80/0x84` and size at `0x88`, independently checked through
saved-pointer reloads, field writes, counters and advances.

Entry initializes **four** 280-byte curtain records at
context `0x1300..0x1760`. Each has two rows of seventeen eight-byte
`SVECTOR`s at record offsets `0/0x88`, scale at `0x110`, and wrap count
at `0x114`. The pointer is saved at stack `0x9C`; the inner bound is
17, the outer bound is 4, and both record walkers advance by `0x118`.
The fourth record receives a distinct zero-scale initialization.

The matched helper draws only the **first three** records
(`0x1300..0x1648`), producing sixteen `POLY_GT4` strips per curtain
at context `0x183C`. The fourth initialized record is not silently
discarded from the layout, nor claimed to be drawn by this helper.
The helper's sixteen-strip/three-record bounds and 280-byte advances
independently confirm this distinction.

Commands `605000..605005` select 48-byte descriptors at module
`0x2D8C + (command % 1000) * 48`, stored in context `0x194C`.
The entry shift/add sequence confirms the stride. The direct
context-access minimum `0x1990` is separated from the measured
model, auxiliary and overlay loads; it is not full-allocation capacity
or proof of global isolation.

Forty-six retail instruction anchors and the batch's 50 target-compiled
C/SDK constants verify the accessed views. All 36 resident callee
addresses and loader/controller owners are checked against actual
resident ELF bytes. The existing French `ratan2` address remains
`0x80089928`.

## Exactness and preservation

The [attempt ledger](french-model-variant439-attempts.csv) records four
terminal canonical-wrapper matches after both helpers are linked together
in every complete unmasked image. Production ownership comprises
28 C owners / 32,200 C bytes,
56 assembly owners / 127,456 assembly bytes, and 28 raw owners.

An initial scratch comparison selected `0x1230..0x1704`, the wrong
1,236-byte function; that rejected comparison remains local evidence,
not a source/compiler failure or a matching result. The independently
walked 1,048-byte span at `0x2878` matches all fourteen images.

This is part of the independent [Family415](french-model-variant415.md)
batch at accepted `e4f38060e`: 24 new images, 48 new C instances and
57,960 C bytes, preserving all 198 prior French registrations.
Shared sources, headers and compiler profiles remain unchanged.
Seven inherited family regressions preserve the independently recovered
boundaries, views, four-versus-three curtain counts and loader metadata.
Untranslated functions and unclassified suffixes remain; configured
coverage is not an exhaustive runtime-completion claim.
