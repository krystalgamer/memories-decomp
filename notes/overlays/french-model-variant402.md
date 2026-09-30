# French MODEL headers 402 and 552

Four stage-7/8 images for models 6 and 551 contain an independently
recovered 1,180-byte rings helper. The body, its local accessed-view header
and a slot-1 symbol wrapper use the named `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. No reference-project types or compiler claims
were imported.

Both canonical symbols reproduce all four complete scratch and production
images. All 194 preceding module records are preserved, and the full gate
reproduces all 198 configured French images.

## Loader and image boundaries

The compact records are 6 and 501. Stages 7/8 select ten 2,048-byte
sectors at `record * 276 + 180/190`, loading at
`0x8013B000/0x8017B000`. Entry is at `+4`. All selected commands are
568,000, passing initialization argument zero. The
[instance ledger](french-model-variant402-instances.csv) records each
independently verified slice, actual header word and complete hash.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0xA5C` | 2,648 | generated assembly | yes |
| `0xA5C..0x1048` | 1,516 | generated assembly | no |
| `0x1048..0x169C` | 1,620 | generated assembly | no |
| `0x169C..0x1B38` | 1,180 | rings C | no |
| `0x1B38..0x2060` | 1,320 | generated assembly | yes |

Strict control-flow walks cover all twenty spans, with one terminal return
per span. Entry calls only the last helper at `+0x1B38`, not `+0xB38`.
The three intervening functions are retained code, including rings.
**No entry-call execution path is claimed for the C helper.**
Each four-byte header and 12,192-byte suffix has a real sized raw owner.
The suffix at `0x2060..0x5000` remains unclassified, not established
wholly data or non-code.

## Independent rings recovery

The entry captures `a0 -> s2 -> s8`, derives `context + 0x58`, and
initializes two records with stride 144. Four point rows at offsets
0/32/64/96 contain four `SVECTOR` values each. Entry clears the scale
word at 128 and following measured words; both records end at `+0x178`.
The helper independently captures the context and repeats twice with the
same 144-byte stride and four quads per record.

This is **not** the accepted 152-byte header-337 ring with scale at 136.
The new view keeps the measured 144-byte record, scale at 128 and unknown
trailing bytes, rather than importing a similar but incompatible type.
Thirty-one entry/helper instruction anchors and 25 freshly target-compiled
constants establish these layouts and SDK argument offsets.

The helper's partial context view includes the quad at `+0x6DC`,
transform at `+0x814` with translation at `+0x828`, target at `+0x834`,
velocity at `+0x83C`, frame at `+0x868`, unsigned elapsed at `+0x86C`,
step at `+0x874`, configuration at `+0x87C`, selected part at `+0x890`,
signed interpolation halfword at `+0x8A6` and phase at `+0x8AC`.
The view ends at `+0x8B0`; entry accesses further fields through
`+0x8C4`. Neither extent is an allocation-capacity assertion.

The selected descriptor is a 20-byte record at module `+0x215C`, indexed
by initialization argument. All four archive commands select record zero,
whose part count is one and unsigned duration is 92. The descriptor
remains inside the single preserved suffix, without duplicate storage.

The first ring uses the transform translation. The second adds velocity
scaled by the signed interpolation divided by 1,024. Three quad vertices
use RGB `(0,64,192)` and the fourth uses `(192,192,192)`. Drawing requires
`0 < depth < 2048`; the projection flag is not tested. Phase-dependent
scale changes run only for the final selected part. Unsigned duration
arithmetic and the distinct first/second-ring phase transitions are
preserved exactly.

## Experiments and ownership

The [attempt ledger](french-model-variant402-attempts.csv) records the first
candidate, its refinement and two terminal canonical-source records.
The first candidate already had the target's 1,180-byte code size and
272-byte stack frame, but eleven aligned instructions differed around
the depth check. GCC folded a combined range expression into subtraction
and an unsigned comparison. Nested `if` statements retain the target's
two branches and produce byte-identical text. Do not simplify that nesting
without repeating exact acceptance.

Actual source/header and slot-wrapper links reproduce all 81,920 image
bytes. Each image has one sized C owner, four assembly owners and two
raw owners. All 35 resident callees across the module and all ten C
call relocations were checked against a fresh exact French resident.
Three resident initializer/controller/loader owners have sized matching C
symbols and retail-identical bytes.

The initializer passes fixed `0x80136000/0x80176000` contexts through
slot `field_DEC`; the controller supplies the selected command or `-1`
to secondary entry. The selected model/primary/secondary load ranges do
not overlap the measured minimum view. Complete allocation capacity,
all primary-context writes and whole-game lifetime isolation are not
inferred. Layout compatibility alone does not prove execution of retained
code.

The addition is four configured images and four matching C instances /
4,720 instruction bytes. Configured French totals become 198 images,
798/1,377 C instances and 731,308 C instruction bytes. Four assembly
functions per image and the unclassified suffixes remain; these counts
are not exhaustive runtime coverage. General report snapshots remain
separate from this matching change.

Final acceptance includes the clean exact French resident gate, checks of
all four new C owners, sixteen assembly owners and eight raw owners, all
35 resident callees and three caller owners, and fresh compilation of the
25 layout constants. All 108 French MODEL variant and 47 progress/global
usage regressions pass, together with metadata, basic-type and
G32/PSXLONG policy checks.
