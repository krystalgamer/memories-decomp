# French MODEL headers 402 and 552

Four stage-7/8 images for models 6 and 551 contain an independently
recovered 1,620-byte ribbons helper, 1,180-byte rings helper and 1,320-byte
band helper. Their bodies,
local accessed-view headers and slot-1 wrappers use the named `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. No reference-project types or compiler claims
were imported.

Canonical symbols reproduce all four complete scratch and production
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
| `0x1048..0x169C` | 1,620 | ribbons C | no |
| `0x169C..0x1B38` | 1,180 | rings C | no |
| `0x1B38..0x2060` | 1,320 | bands C | yes |

Strict control-flow walks cover all twenty spans, with one terminal return
per span. Entry calls only the last helper at `+0x1B38`, not `+0xB38`.
The three intervening functions are retained code, including rings.
**No entry-call execution path is claimed for ribbons or rings.**
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

The initial rings addition was four configured images and four matching C
instances / 4,720 instruction bytes. Configured French totals became 198
images, 798/1,377 C instances and 731,308 C instruction bytes. Four assembly
functions per image and the unclassified suffixes remained; these counts
are not exhaustive runtime coverage. General report snapshots remain
separate from this matching change.

Initial rings acceptance included the clean exact French resident gate, checks of
all four new C owners, sixteen assembly owners and eight raw owners, all
35 resident callees and three caller owners, and fresh compilation of the
25 layout constants. All 108 French MODEL variant and 47 progress/global
usage regressions pass, together with metadata, basic-type and
G32/PSXLONG policy checks.

## Entry-called band follow-up

The independently recovered `func_8013CB38` compiles to all 1,320 target
bytes with its 304-byte stack frame. The slot wrapper renames it to
`func_8017CB38`. Actual canonical links reproduce all four complete images
while preserving the accepted rings C and replacing only the final
assembly function. Bands is the sole local helper called by entry, at
module `+0x8E0` with the captured context in `a0`; unlike rings, its
entry-call path is proven. Final production validation reproduces all 198
configured French images and the clean French resident. The family has eight
section-defined C owners, twelve assembly owners and eight raw owners;
the old band assembly object is absent from selected link inputs. All
35 module callees and three caller/loader owners were checked again,
along with ten band and twenty-five ring layout constants. All 109 French
MODEL variant and 47 progress/global-usage regressions pass, together with
metadata, basic-type and G32/PSXLONG policy checks.

Entry initializes two 280-byte records at context `+0x438..+0x668`.
Each has seventeen inner `SVECTOR` points at 0, seventeen outer points at
`+0x88`, scale at `+0x110`, and a cycle counter at `+0x114`.
The seventeen-point bound, two-record bound, 280-byte stride, initialization
stores and call pointer are verified in all images by twenty additional
anchors. Ten new target-compiled constants check this record and its SDK
types. This is not the similar 288-byte header-432 band with inline colors.

The helper derives a bearing from the velocity, preserves a second unused
`ratan2` call, and orients both bands using the measured slot direction.
Frame parity changes the outer radius between 512/576 and depth between
192/256. It generates seventeen points per edge and draws sixteen quads
per band, fading six context color bytes above scale 2,048. The signed
halfword fade calculation, scale limit 4,096, cycle increment and final
phase transition are preserved without normalizing them to another effect.

Six source/compiler experiments are retained in the attempt ledger. The
first missed loop induction and depth-product caching. Reusing the draw
depth for the product added spills; a separate scalar restored allocation.
Grouping outer-vector stores with the existing `setVector` macro restored
the target address formation. Separating the initial bearing from the
loop angle then fixed the final four differing register words. All
experiments use the same named compiler profile; no padding or register
binding was introduced.

Fresh checks cover all 35 module callees, the three resident caller/loader
owners and thirteen C call relocations per slot. `rcos`, `rsin` and
`ratan2` replace address-based binding names using identities already
established in accepted French family-435 bindings. All 35 destination
addresses remain unchanged; no duplicate binding addresses or new SDK
declarations are introduced. Existing legal commands, descriptors and
context/load separation were checked again. The `+0x8C4` minimum view
remains a lower bound, not an allocation or global lifetime assertion.

The follow-up adds four C instances and 5,280 instruction bytes, without
adding image registrations or function inventory rows. The family now
has eight C owners, twelve remaining assembly owners and eight raw owners.
Configured French totals become 198 images, 802/1,377 C instances and
736,588 C instruction bytes. Rings remains retained-only, and every
suffix remains unclassified; these counts are not exhaustive coverage.

## Retained ribbon follow-up

The helper at `+0x1048..+0x169C` reproduces all 1,620 instruction bytes
and its 296-byte frame in both load slots. Four canonical complete-image
links preserve both accepted rings and bands, establishing twelve C,
eight assembly and eight raw owners. This independent follow-up starts
from accepted master `64aac47fb`, including the accepted Family445 webs.
It does not stack the separately proposed strip helper at `+0xA5C`.

The helper traverses eight **88-byte ribbon records** at context
`+0x178..+0x438`, between the two measured rings and the band array.
Local fields are two `SVECTOR` points at 0, packed screen coordinates at
16, angles at 24, sideways points at 32, their packed screens at 48,
widths at 56, depths at 64 and halfword offsets at 72/76. Bytes 80..88
remain opaque. This differs from the shared 116-byte and 108-byte ribbon
records, so a local accessed-view header reuses established SDK types
without changing those shared declarations.

Geometry and drawing require positive phase at `+0x8AC`. Each ribbon
constructs two points, but its projection and draw loops run once. The
retained alternative projection branch is preserved as emitted; it does
not establish a second executed iteration. Scale comes from `+0x80` of
the **first 144-byte ring at `+0x58`**, not a 152-byte sheet or descriptor.
Translation uses signed interpolation at `+0x8A6`. The `POLY_G3` packet
at `+0x668` has gray outer vertices and a `(0,64,255)` tip. Sorting
requires `0 < depth < 2048`, with no separate projection-flag condition.

After the phase-gated section, selected part at `+0x890` plus one is
compared with the unsigned halfword at `+0xC` of the selected 20-byte
descriptor. The angle halfword at `+0x8A8` advances by step times 55 on
equality, including when phase is nonpositive. All four archive commands
select descriptor zero, whose comparison value is one; this is not a
claim about the whole animation's duration.

Three materially distinct experiments are recorded. The first already
had the exact size and frame, but differed in six instructions. Nested
depth conditions recover two signed branch instructions that GCC folds
for a combined range expression. Separating the tilt call result from
its `0x400` bias around the first-ring pointer assignment recovers the
remaining four scheduled instructions. Indexed first-segment screen
expressions retain the required derived-pointer spill. No compiler,
padding, inline assembly or register-binding workaround is used.

Forty-two freshly target-compiled constants and 84 focused retail anchors
verify the local/SDK layouts, loop bounds, strides, fields, descriptor
selection and arithmetic. All 35 resident binding addresses, eleven
helper callees, eighteen call relocations per slot and three resident
caller/loader owners are checked independently. Only the existing
`0x80087868` binding is renamed to `RotTransPers`. No direct entry-call
path to ribbons is demonstrated; `+0x8C4` remains only a direct context
minimum, not capacity or lifetime isolation.

The addition is four C instances / 6,480 bytes, without new image
registrations or inventory rows. Configured totals at this fixed cutoff
are 222 images, 906/1,521 C instances and 873,204 C bytes. All 222 complete
French images and the clean French resident match unmasked retail bytes.
Fresh production objects verify twelve C owners / 16,480 bytes, eight
assembly owners / 16,656 bytes and eight raw owners / 48,784 bytes, with
all prior C selections preserved. The 35 bindings, three resident caller
owners and 42 newly compiled layout constants pass again. All 132 French
variant, 64 Spanish variant and 16 progress regressions pass without
skips, together with metadata, basic-types, external-attempts and G32
gates. Shared declarations, compiler profiles and other regional code
remain unchanged. The entry, strip and unclassified suffixes remain
untranslated here; no exhaustive regional coverage is claimed.
