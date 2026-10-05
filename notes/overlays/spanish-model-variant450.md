# Spanish MODEL headers 450 and 600

Model 174, record 174, stages 9/10 contains independently compiled
900-byte line helpers at image offset `2D88`, together with the separately
documented [1,480-byte quad helpers](spanish-model-variant450-quads.md) at
`27C0`. The accepted French 2,492-byte primary-ribbon and 1,848-byte
three-band sources are selected directly at `DD8` and `1794`, together with
the accepted 2,100-byte coil renderer at `310C`; the complete
French images are byte-identical to the corresponding Spanish images.
Both complete 20 KiB Spanish images
match retail under the named `gcc_2_8_1_g0_split` profile (GCC 2.8.1 /
MASPSX 2.81). No external-reference body, instruction patch, register pin or opaque
instruction array is used as C.

The [instance inventory](spanish-model-variant450-instances.csv) records
sectors 48224/48234, headers 450/600 and command 616000. A census of the
four documented ten-sector secondary windows in each of 621 complete
MODEL records found only these instances. This is not exhaustive evidence
for every possible runtime loading path.

## Complete ownership and unresolved code

| Image interval | Bytes per image | Owner |
| --- | ---: | --- |
| `0..4` | 4 | Raw header |
| `4..DD8` | 3540 | Entry assembly |
| `DD8..1794` | 2492 | Shared primary-ribbon helper C |
| `1794..1ECC` | 1848 | Shared three-band helper C |
| `1ECC..27C0` | 2292 | Assembly |
| `27C0..2D88` | 1480 | Quad helper C |
| `2D88..310C` | 900 | Line helper C |
| `310C..3940` | 2100 | Shared coil renderer C |
| `3940..5000` | 5824 | Unclassified raw suffix |

The [historical ribbon investigation](spanish-model-variant450-ribbons.md)
records the last helper's seven-release counterparts, initializer-backed
layout and bounded scalar behavior. The subsequent accepted French coil
implementation now reproduces both Spanish spans through real compiled C.
There is still no demonstrated direct entry-call path to this retained helper.

All seven function spans have closed, fully reached CFGs. The two images
contribute ten C instances / 17,640 instruction bytes and four generated
assembly instances / 11,664 instruction bytes. All 11,648 suffix bytes remain
unclassified and in scope; they are not declared harmless data or excluded
game code. Raw header/suffix owners are disjoint from the code owners.

The initial private whole-image proof preserved the non-helper bytes in raw
prefix/suffix objects. Production now selects two generated assembly
functions per image, with five sized compiler functions, header and suffix.
The symbol-only slot-one wrappers are compiled separately; neither slot is
accepted solely because relocation-masked instructions resemble the other.

## Line-helper behavior and declarations

Six 104-byte groups contain nine eight-byte points and RGB at offset 72.
The primary views begin at `440`, stride 536, with threshold at zero and
translation at `28/2C/30`. Fade views begin at `36C0`, stride 160, with
scale at zero, fading at `10`, and brightness at `14`. The shared 20-byte
GsGLINE is at `4240`.

Each group projects eight spokes. The first and third projection inputs
are the group center; their screen-output pointers both address line `x0`.
The second input is the selected spoke, with output at `x1`. The first
endpoint uses the group RGB and the second is black. Submission requires
nonnegative depth and projection flag plus primary threshold at least 1024.
The queue API narrows depth to sixteen bits.

Flag bit zero at `42A8` adds signed scale / 4, truncating toward zero.
Fading multiplies RGB by signed brightness / 1024 before byte narrowing.
Each fading group, not each frame, advances `42F4` by `42B4 * 4`.
Rotation uses its low halfword; scale is four times the fade scale plus
the optional term. All four leading `ratan2` calls remain, even though
their return values are overwritten.

Thirty-three target-compiled constants establish the canonical SDK layouts
and local group, primary, fade and partial-context offsets. The `42F8`-byte
view describes this helper's accesses, not an original struct identity,
allocation size or the complete entry's working set. Unknown gaps remain
byte spans, and uncertain declarations remain local to this family.

## Compiler recovery

The [attempt ledger](spanish-model-variant450-attempts.csv) retains twenty
material line-helper experiments, both private complete-image matches, both
line production matches, and six terminal shared-source selections. Nineteen
line experiments mismatch; the twentieth is exact.
Positional word counts compare complete relocated spans, including size
differences, and are not semantic-similarity scores.

Actual GCC RTL identified the extra primary translation induction responsible
for the first oversized candidate. A precomputed primary base permits the
complete pointer induction rather than a spilled integer offset. The first
CSE pass also reverses adjacent root/cursor copies and changes `REG_EQUIV`
to `REG_EQUAL`; the local typed view and early index initialization preserve
the required capture and allocation.

Finally, independent full-width primary/fade indices produce separate
strength-reduction classes. Their arithmetic disappears completely, while
their generated pointers enter the correct second/third `ratan2` delay slots.
A shared index leaves precisely those two instructions exchanged. The short
six-group counter and eight-spoke counter retain the observed narrowing.
Do not collapse these source forms without rechecking the complete images.

The coil integration keeps the accepted body, local layout header, named
profile and symbol-only slot wrapper unchanged. Both selected compiler
objects and linked definitions have the exact 2,100-byte executable extent.
All external call relocations agree with retail and real Spanish resident
function starts. Headers and complete suffixes retain non-executable raw
owners. The shared target-compiled coil-view checks run for the Spanish
integration as well; these measured views do not prove allocation bounds
or remove the documented context overlap.

## Caller, resident and packet evidence

A fresh Spanish resident reproduces retail. All ten helper imports resolve
to real sized function definitions in selected resident input objects.
The actual slot initializer with a null HMD establishes the secondary context,
and the complete controller update dispatch passes it with argument -1.
Both actual parent prefixes preserve that pointer in `s3/s6`; the direct
helper call at image `C34` passes `s3` in its delay slot. Static inspection
finds no `s3` destination between the update prefix and this call.
That intervening geometry slice is not executed by this proof and assumes
the callees preserve `s3` according to their ABI.

The actual retail `GsSortGLine` and its complete queue helper execute without
hooks in the CPU fixture. Three submissions copy separate 24-byte GPU commands,
advance the real work-buffer cursor, and link the narrowed-depth OT buckets.
Later source overwrites leave all earlier packets unchanged. A sign-bit
attribute skips submission. The cursor and viewport words have verified
containing raw resident owners; buffer capacity, OT extent and viewport
values are supplied fixture state, not allocation evidence.

**Lifetime limit:** the measured view extends through context `+42F8`,
overlapping the other loader window beginning at context `+4000` by 760
bytes. Exact matching preserves those retail accesses; it does not prove
phase isolation or establish that this overlap is safe for arbitrary state.
No full CD, GTE geometry, GPU consumption, animation, allocator or complete
noninterference claim is made. Other revisions may contain differently
compiled semantic counterparts; the earlier no-exact/shape-donor result
is not proof that this behavior is region-exclusive.
