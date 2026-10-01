# French MODEL headers 439 and 589

The additive band/web batch below retains the original sheets and curtains.
The initial registration evidence is historical; the new production gate
must separately prove all four C owners in every image.

## Initial sheets and curtains registration

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

## Additive bands and retained webs

The accepted `variant422_bands.c` has the correct 1,912-byte size and
296-byte frame, but differs at four next-column loads, function offsets
`0x520/0x52C/0x550/0x55C`, in both French slots. Using `band->sc[j+1]`
and `band->sb[j+1]` rather than shifted `col->sc[1]/sb[1]` reproduces
all fourteen complete images. Two `VERSION_FRENCH` guards isolate these
four expressions; the original US expressions remain unchanged.
Canonical wrappers rename `func_8013C70C` to `func_8013C704/8017C704`.

The unchanged `variant422_webs.c` separately reproduces all fourteen
complete images: 1,304 bytes and a 288-byte frame per function. Its
wrappers rename `func_8013D36C` to `func_8013D360/8017D360`.
This helper is retained code with **no direct entry-call path**.
Entry initializes its records, but initialization does not establish
that the renderer executes. Bands are entry-called at module `+0x10C8`,
with original context in the `+0x10CC` delay slot, when phase is positive.

The first three 416-byte narrow webs occupy context `0..0x4E0`.
Their near/far four-by-six vector grids start at `0/0xC0`, with
48-byte rows; color, scale and done are at `0x180/0x194/0x198`.
Entry initializes scale to `0x1000 + index * 0x1000 / 3` and clears done.
One 456-byte band follows at `0x4E0..0x6A8`, immediately before the two
previously accepted sheets. Its three nine-point rows start at
`0/0x48/0x90`, screen rows at `0xD8/0xFC/0x120`, colors at
`0x144/0x168`, and depth at `0x1A4`.

Band radius is signed halfword `+0x1964 / 256`, or that halfword times
24/4096 according to frame `+0x1938` low bit. Its path fraction is
`+0x1968`; the `POLY_GT4` packet is at `+0x17A0`. Nine projections feed
eight adjacent pairs, each producing two quads. Negative depth is clamped
before sorting its low 16 bits. The nine projection flags are stack
`PSXLONG[1][9]` at `+0xC8`, not band storage; projection `p` is at `+0xF0`.
Phase one grows the path fraction to `0x400`, then enters phase two.
The later timed radius shrink enters phase three.

Web rendering preserves all four `ratan2` calls. Phase zero fades above
`0x800` toward `0x1000`; phase one uses black at scale `0x1000`; later
phases clamp nonpositive render scale and fade above `0x1800` toward
`0x2000`. Translation uses words `+0x18F8/18FC/1900` before phase two,
otherwise halfwords `+0x1904/1906/1908`. The shared line packet is at
`+0x18D0`; duplicated near/far points and screen destinations are passed
to `RotTransPers4`. Projection outputs are stack `+0xD0/+0xD4`.
Only **strictly positive depth** gates sorting; the projection flag is
unused here, unlike the French341 web helper. Endpoint colors reverse
with phase, retaining packet `r0/r1` offsets 12/15.

Web phase zero derives scale from frame time `+0x193C` and descriptor
`+0x20`; phase one resets each scale to `-(index * 0x2000 / 3)`.
Later growth uses step `+0x1944 << 8`. At phase four the final completed
record can advance phase to five; otherwise scale wraps by `0x2000`.
The original constant done comparison is preserved.

The selected descriptor timings at `+0x20/24/28/2C` are:

| Command | Growth start/end | Shrink start/end |
|---|---|---|
| `605000` | 100/112 | 200/240 |
| `605001` | 56/68 | 240/300 |
| `605002` | 56/64 | 120/140 |
| `605003` | 56/64 | 320/360 |
| `605004` | 180/192 | 360/420 |
| `605005` | 110/120 | 360/420 |

Every divisor and timing difference used by these helpers is nonzero
for the selected records. Stage 9/10 commands come from record metadata
`+0x114`, not the stage 7/8 word at `+0x110`. Forty-eight freshly
target-compiled layout constants and 202 independently checked retail
anchors verify the views in every image. The twelve band and eight web
callees comprise fourteen distinct addresses; all 36 family bindings
and three resident caller/loader C owners are checked. The `0x1990`
minimum context extent remains a minimum, not capacity.

The original four terminal ledger rows are retained byte-for-byte.
Two original band mismatches and two indexed exact experiments precede
four canonical matches. No header, compiler profile, loader manifest or
resident binding changes are needed. The intended production ownership
is 56 C owners /77,224 bytes, preserving 28 prior owners /32,200 bytes
and adding 28 owners /45,024 bytes. Twenty-eight assembly owners /82,432
bytes and 28 header/suffix raw owners /127,064 bytes remain.

Independent production acceptance reproduces all 263 US images and the
clean US resident, then all 252 French images and the clean French resident.
Actual linked ELF and defining-object checks establish the complete
56-C/28-assembly/28-raw ownership above and retain all fourteen US422
band C owners /26,768 bytes. The fresh US executable and ELF were saved
before French output replacement. All 204 French, 112 Spanish, sixteen
progress and five US-toolchain regressions pass without skips, together
with metadata, basic-type, external-attempt and G32 policy checks.
Independent-cutoff configured totals are 1,074/1,581 C instances and
1,145,180 instruction bytes; they are not exhaustive runtime coverage.

The independent verified checkpoint was ordinarily merged with accepted
French341 webs (`154f085e`), never with their pending branch. The sole
conflict was aggregate progress; both additions are retained for
1,078/1,581 C instances /1,149,388 bytes. Reconciled acceptance passed
all 252 complete French images and the clean resident, plus 204 French,
112 Spanish and 21 progress/toolchain regressions and repository policy.
Fresh ELF/defining-object checks preserve all 56 Family439 C owners,
all fourteen US422 band owners and all sixteen accepted Family341
owners /14,832 bytes. Fifty of the original 52 authored files remain
byte-identical; only this note and the progress fixture changed.
