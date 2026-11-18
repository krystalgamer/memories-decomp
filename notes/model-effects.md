# MODEL effect names (`eNNN`)

[`model-effects.csv`](model-effects.csv) maps each MODEL.MRG effect image
that has a confirmed developer name `eNNN` to its header word in every region.
It also lists the image's aliases and the records that load it. These names
replaced the header-number source names (`variant<N>_*`) under
`src/overlays/model_variant/`; see the source naming rule below.

## Evidence: build-script residue

The slack sectors of every MODEL.MRG (NA, JP, EU and the shared
French/German/Italian/Spanish file) contain leftover developer build-script
text. Apart from three lines missing in JP, it is the same text in all four
files. It names each record's variant stages as `bin/eNNN_0.bin` and
`bin/eNNN_1.bin`, the slot 0 and slot 1 builds of effect `eNNN`. Several
command forms are present: `M:\mrgSU\mNNN.mrg` lines, `model.mrg +OFF` lines,
`@REM N` blocks, `OUTFILE=`/`GRP=` tables and `STR_FileName=` groups. `mNNN`
is a model id, not a record. The record is the model id with the gaps 300–349,
650–699 and 720 removed, as in `Model_LoadMonsterMerge`.

Much of the residue is stale. A name counts as **confirmed** only when the
retail bytes support it:

- `residue+fp`: the residue names this stage, and the final image agrees on
  the header word and the first 0x400 bytes, or
- `fp-propagated`: another stage has the same header and the same 0x400-byte
  fingerprint, and that other stage is `residue+fp`.

Names that come only from stale lines, and the `eXXX` placeholders, are not
used. So are names interpolated from the header formula below.

## Header words

Each stage image begins with a header word, an index into a global effect
table. The former `variant<N>_*` source numbers came from these words; some
families used the NA word and others the PAL word. For all 2484 stages per
region, the region offset is exact: **JP = NA − 1** and **PAL = NA + 17**
(EU/FR/DE/IT/ES). Each confirmed `eNNN` has one canonical header word per
slot; the seven second images below add a second one.
Within a group the words are roughly linear, and the slot 1 word is the slot 0
word plus the group size:

| group | slot 0 NA word | slot 1 − slot 0 |
|---|---|---:|
| `e0NN` | ≈ N + 52 | 130 |
| `e5NN` | N − 183 | 150 |
| `e8NN` | N − 183/184 | 67 |
| `e3NN` | N + 451 (449 for e300–e302) | 97 |

Fourteen effects have several confirmed names at the same headers. Their images are
byte-identical copies, for example e001=e019, e036=e590, e820=e838 and
e503=e513=e539. The map uses the lowest name and lists the others in
`aliases`. `e075` is confirmed only in slot 1, by its header 257. Its slot 0
header comes from the slot pair built by the same source, so its basis is
`residue-one-slot`.

The residue writes `e000` for header 655/722, but `e000` is also its usual
placeholder. That image is therefore treated as unnamed (`h655`).

## Second images

Header words are build-list positions. Each group starts linearly and ends in
a tail that also rebuilds older names, so some names exist twice in retail: an
older image at the name's linear slot and the canonical one (`residue+fp`) in
a tail. Seven such linear-slot images are confirmed:

| name | linear NA slot 0/1 | canonical NA slot 0/1 | direct residue lines |
|---|---|---|---|
| e017 | 70/200 | 662/729 | 6, `m140` stages 200/210 |
| e032 | 84/214 | 176/306 | 6, `m130` stages 180/190 |
| e503 | 320/470 | 443/593 | 28, `m110`/`m159` stages 200/210, `m410` stages 180/190 |
| e539 | 356/506 | 443/593 (alias of e503) | 4, `m707` stages 180/190 |
| e584 | 401/551 | 427/577 | 10, `m410` stages 200/210 |
| e338 | 789/886 | 372/522 | 12, `m162`/`m254` stages 200/210, `m283` stages 180/190 |
| e341 | 792/889 | 388/538 | 6, `m79` stages 200/210 |

For each one, direct `mNNN.mrg`/`@REM` lines name exactly the stages that
load the image, the header formula between the two confirmed neighbours gives
the same name, the slot 1 word is slot 0 plus the group size, and no direct
line names another effect (the other records' lines are `eXXX` placeholders
or absent). The two images of a name are different code, not copies. They
are listed as extra `model-effects.csv` rows with basis
`residue-second-image`.

The same brackets also suggest e046 (98), e577 (394), e594 (411), e326 (777)
and e330 (781), but no direct residue line names those stages. They stay
`hNNN`.

## Columns

`e_name`, then the NA, JP and PAL header word for slot 0 and slot 1, then
`aliases`, `records` (MODEL records 0–620 whose stages load the image),
`basis` (`residue`, `residue-aliases`, `residue-one-slot` or
`residue-second-image`) and `evidence` (the number of stage positions, over
all seven builds, confirmed by each method). A `residue-second-image` row
repeats the `e_name` of the canonical row above it; its `evidence` gives the
number of direct residue lines, their model ids, and the confirmed neighbours
that bracket the header formula.

## Source naming rule

- Each former `variant<N>_<role>` source was renamed `e<NNN>_<role>` when the
  image has a confirmed name. Otherwise it was renamed `h<NNN>_<role>`, where
  `NNN` is the image's **slot 0 NA header word**. Header-only images and formula-inferred names
  both use `hNNN`.
- A confirmed second image (see above) keeps its header in the name:
  `e<NNN>_h<HHH>_<role>`, where `HHH` is its slot 0 NA header word, for
  example `e503_h320_entry.c` beside the canonical `e503_entry.c`.
- The `us` number tag (`variant<N>us_*`) and the clash-avoiding `_pal`
  suffix only recorded which region's header numbered the file, so both were
  dropped. `_pal` is kept, or added, only where a PAL-only build would
  otherwise share a name with the NA/JP source of the same image and role
  (`e585_entry.c` and `e585_entry_pal.c`). It goes before any `_slot1`.
- `_usa` wrappers, `_slot1` slot-specific builds and offset-tagged helpers
  (`_sheets_1b9c`) keep their suffix.
- Only e585 and e852 (unnamed, `h668`) use more than one source set in a
  region. Each also links a leftover helper from another family at one
  record. The helper got a record suffix: `variant552_strand.c` became
  `e585_strand_r163.c` (record 163, slot 1 only), and
  `variant447_rings_pal.c` became `h668_rings_r397.c` (record 397). The two
  PAL `sheets` helpers of header 452 became `h452_sheets_1b98.c` and
  `h452_sheets_2b30.c`, named by their image offsets like the NA pair.
- Module and config names (`model_variant_<rec>_…`) and symbols
  (`FrenchModelVariant444_*` …) are unchanged.
