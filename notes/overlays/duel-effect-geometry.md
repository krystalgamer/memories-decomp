# French duel-effect geometry and rendering

## Scoped transform/vector C views

Eight shared helpers now have semantic C views in
`geometry_semantic_views.h`. Their exact types remain, and every regional
`duel_effects_symbols.txt` binds each name in place of the former label. Existing SDK includes keep their order; the new
view header follows each caller's complete leading include block.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_SetTransformMatrix` | `func_801513F4` | `0x80151FCC` | `0x8015F488` | `0x801513F4` |
| `DuelEffect_SetScaledLocalScreenMatrix` | `func_801514BC` | `0x80152094` | `0x8015F550` | `0x801514BC` |
| `DuelEffect_CopyMatrixComponents` | `func_801514F8` | `0x801520D0` | `0x8015F58C` | `0x801514F8` |
| `DuelEffect_InitRandomVectorsXYZ` | `func_8014F608` | `0x801501E0` | `0x8015D69C` | `0x8014F608` |
| `DuelEffect_InitRandomVectorsNegatedY` | `func_8014F754` | `0x8015032C` | `0x8015D7E8` | `0x8014F754` |
| `DuelEffect_InitRandomVectorsXY` | `func_8014F89C` | `0x80150474` | `0x8015D930` | `0x8014F89C` |
| `DuelEffect_InitIndexedQuadVertices` | `func_8014F9A0` | `0x80150578` | `0x8015DA34` | `0x8014F9A0` |
| `DuelEffect_InitIndexedQuadToYZero` | `func_8014FA3C` | `0x80150614` | `0x8015DAD0` | `0x8014FA3C` |

Only actual NA addresses enter the eight
`overlay/duel_effects/function` registry rows. Numeric name suffixes are
historical labels, not evidence of the NA address. All forty name cells
across the five configured bank inventories change together; their other
fields, row order, source grouping and compiler profiles remain unchanged.
No German/Italian bank coverage or new native exports are claimed.

### Matrix preparation

`DuelEffect_SetTransformMatrix` first installs the supplied base, transforms
the position into the local matrix's translation and constructs its rotation.
The two conditional operations retain their exact domains:

| `mode` | Compose base (`MulMatrix2`) | Save unscaled components |
|---|---|---|
| 0 | no | no |
| 1 | no | yes |
| 2 | yes | no |
| 3 | yes | yes |
| Other `u16` values | no | no |

Scaling and installation of the result still follow those operations for
every mode. Do not turn the comparisons into unrestricted bit tests, add an
enum/range guard, reorder the SDK calls or remove the unused flag write and
its `PSXLONG *` cast. Actual effect callers supply position, rotation and
scale, and use the optional unscaled snapshot for later layered drawing.

`DuelEffect_SetScaledLocalScreenMatrix` copies to a local matrix before
scaling and installing it; it does not scale the input in place.
`DuelEffect_CopyMatrixComponents` copies nine rotation halfwords and three
translation words, interleaving each row with its translation component.
It leaves the SDK matrix padding untouched. Neither structure assignment,
`memcpy`, zero-initialization nor non-aliasing assumptions replace that loop.

### Vector and indexed-quad initialization

| View | Component expressions | RNG calls per vector |
|---|---|---|
| Random XYZ | Independent scaled signed differences modulo 4096 on each axis | 6 |
| Negated-Y variant | Difference-based X/Z; Y uses the negated scale and one `rand() % 4096` | 5 |
| Random XY | Difference-based X/Y; Z is literal zero | 4 |

Particle/ray callers establish vector-array use without implying a uniform
or normalized distribution, world up/down direction, velocity unit or frame
duration. Preserve the target RNG call order, subtraction/remainder/division
expressions, unsigned-halfword scales/count and signed-halfword stores.
Zero count performs no RNG calls or vector stores. All variants leave
`SVECTOR` padding untouched; none becomes a whole-record initialization.

The two quad helpers use `u16` arguments as indices into existing signed
halfword width/height tables, not as pixel extents. Both write four XY-plane
vertices with zero Z; the second uses literal zero for both lower Y
coordinates instead of another height-table lookup. Preserve signed table
values, negation/narrowing, access/store order, unchecked indices and
untouched padding. No ground-plane or allocation meaning is inferred.

### Conservation boundary

The matrix and five-function vector objects remain complete and in original
definition order. All actual bank callers use the canonical views without
changing their arguments, casts or control flow. Every pre-existing header,
test, fixture, historical record, source selector and negative control
remains unchanged. No new C/source-string/native harness is added.

Fresh accepted-parent compiler/CPP/native comparisons account for eight
canonical and eight native compatibility declarations, one semantic include
per affected C file and eight identifier substitutions. All native symbols,
compiler/MASPSX output, ordinary/fallback assembly and the whole NA
executable/ELF must remain identical. Full five-bank and seven-copy
acceptance uses the existing PR gates, including the targeted Japanese/UK
bank steps, not a local cloud-test run.

## Ring, rectangle and record C views

The same semantic header also owns these eight related names. SDK types and
all pre-existing tests remain unchanged.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_InitCircle32Vertices` | `func_8014EA7C` | `0x8014F654` | `0x8015CB10` | `0x8014EA7C` |
| `DuelEffect_InitHalfStepXYRingVertices` | `func_8014EB1C` | `0x8014F6F4` | `0x8015CBB0` | `0x8014EB1C` |
| `DuelEffect_InitHalfStepXZRingVertices` | `func_8014EC8C` | `0x8014F864` | `0x8015CD20` | `0x8014EC8C` |
| `DuelEffect_InitXZRingVertices` | `func_8014EE0C` | `0x8014F9E4` | `0x8015CEA0` | `0x8014EE0C` |
| `DuelEffect_InitRandomVectorsUnscaled` | `func_8014EF2C` | `0x8014FB04` | `0x8015CFC0` | `0x8014EF2C` |
| `DuelEffect_InitRectVertices` | `func_8014F490` | `0x80150068` | `0x8015D524` | `0x8014F490` |
| `DuelEffect_InitQuadrantQuadVertices` | `func_8014FE00` | `0x801509D8` | `0x8015DE94` | `0x8014FE00` |
| `DuelEffect_SwapVectorRecords` | `func_8014FED4` | `0x80150AAC` | `0x8015DF68` | `0x8014FED4` |

The fixed circle writes exactly 32 XY points at repeated `i * 128` angles
and zero Z. The variable XY ring first scales its unsigned-halfword extents
with the original `csin(512) << 1` expressions, then uses the integer
half-step formula. Its four-point callers use that same ring helper to
construct quads; no special four-point implementation replaces it.

The half-step XZ ring keeps both regional implementations. The NA/JP/UK/ES
`polygon_vertices.c` body computes `level - offset` in its loop; the French
`height_ring.c` body preserves its separate nonzero-count guard, signed
count promotion and guarded height-difference setup. Both take full-word
vertical arguments. Their sine-based extent scaling remains before the
count test, so zero count still performs both SDK sine calls but no
count-dependent division or vertex store. Do not unify the bodies, narrow
the arguments, move the promotion or turn them into early returns.

`DuelEffect_InitXZRingVertices` instead uses unadjusted angular steps and
negates its signed-halfword height for Y. Its actual definition carries the
canonical name (formerly `func_8014EE0C`) with its original typed signature;
no wrapper or duplicate implementation stands in for it.

The unscaled random helper writes signed `rand() - rand()` remainders
modulo 4096 to XYZ, with six target RNG calls per vector. Count zero
performs no RNG or vector stores. It is not replaced by a call to a scaled
initializer, normalized sampling or a claimed angle/velocity convention.

The rectangle helper keeps its full `s32` width/local and signed-halfword
height, original integer-power sign calls, corner order and zero Z. The
quadrant helper constructs four corners against the coordinate axes and
origin; its actual drawing caller supplies indices 0 through 3. Its `u16`
index behavior outside that range, exponent narrowing, comparisons and
power calls are still retained rather than masked, bounded or table-driven.

All of these constructors write components without changing the SDK
`SVECTOR` pad member. **The swap helper deliberately does the opposite:**
its temporary structure assignments exchange complete records, including
that pad member. Keep self/alias behavior and full-record transfers; an
XYZ-only swap or new alias guard would change the contract.

Forty name-only inventory cells and eight scoped NA registry rows describe
these views across the five configured banks. Every grouped object,
profile, extent and definition order remains. Exact CPP conservation
accounts for the sixteen new declarations and for the already accepted
sixteen declarations only where an added include makes them newly visible.
Declarations already present in a baseline context are retained, not
filtered away. All native outputs, symbols, NA executable/whole ELF and
existing guards must match; existing five-bank/seven-copy CI remains the
acceptance gate. No new C/native/source-string harness is added.

## Original matching groups

Ten exact functions / 3,604 bytes are integrated as six complete source groups
with the existing `gcc_2_8_1_g0_split` profile:

| Source | French range | Functions | Bytes |
|---|---|---:|---:|
| `cross_lines.c` | `0x8014E35C..0x8014E3EC` | 1 | 144 |
| `ring_vertices.c` | `0x8014EA7C..0x8014EC8C` | 2 | 528 |
| `radial_random_vectors.c` | `0x8014EE0C..0x8014F010` | 2 | 516 |
| `gradient_strip.c` | `0x80156448..0x801566D4` | 1 | 652 |
| `gradient_lines.c` | `0x801570B0..0x801573A8` | 1 | 760 |
| `display_quads.c` | `0x801573A8..0x80157794` | 3 | 1004 |

The last three groups and their headers are reused unchanged from accepted
Spanish work. See the existing [gradient-strip](duel-effect-gradient-strip.md),
[gradient-line](duel-effect-gradient-lines.md) and
[display-quad](duel-effect-display-quads.md) evidence. In particular, the
display endpoints use the already recovered SDK `DVECTOR` view rather than
inventing an eight-byte stride from accesses to X/Y alone.

## New geometry helpers

The first routine submits two white `GsLINE` diagonals across the PAL
320-by-256 viewport, retaining attribute `0x50000000`, priority zero and
the second line's explicit coordinate stores. The existing ordering-table
pointer remains raw module data; no C storage definition or alias replaces it.

The fixed circle writes 32 `SVECTOR` records at angular steps of 128.
The variable ring first scales its unsigned-halfword extents using twice
`csin(512)`, then uses half-step angular placement. The radial helper uses
unadjusted angular steps, writes negative height to Y and sine-scaled depth
to Z. The random-vector helper writes three independent signed differences
of `rand()` results modulo 4096. Count-zero paths perform no vector stores
or count-dependent division; the variable ring still scales its extents
before testing the count. Vector padding remains untouched.

All types and declarations come from the existing utility/drawing and Psy-Q
headers. New public declarations remain under those existing shared owners.
No inline assembly, register pins, source-local externs, alternate compiler,
new flags or guessed data allocation is used.

## Bounded source experiments

The initial five-function geometry candidate also included the intervening
height ring at `0x8014EC8C`. It was never promoted as a whole translation unit.

| Experiment | Result |
|---|---|
| Pointer traversal, explicit angle local, sine multiplied by two, inline height subtraction | Sizes 176/368/392/288/244 versus 160/368/384/288/228. Only the radial helper matches; the variable ring differs at offsets `0x40` and `0x68` because GCC doubles the width operand instead of the sine result. |
| Indexed fixed-circle/random stores and explicit sine shift | Variable ring, radial and random helpers match. The 160-byte circle still differs in eight words due to the angle local; the height ring is 392 bytes. |
| Repeat the circle's angle expression; precompute height difference | Circle also matches. The height ring reaches 384 bytes but retains 15 differing words. |
| Guard height setup and use a do-while loop | Four functions still match; the height ring retains 12 differing words from count/output allocation and setup scheduling. |
| Add an explicit output-pointer alias | Same 12-word height-ring mismatch. |
| Use halfword height parameters | Height ring retains 14 differing words. |
| Use a separate signed height-difference local | Same four exact functions; height ring retains 14 differing words. |

The accepted four functions were then compiled as two complete contiguous
groups around the preserved 384-byte assembly function. The cross-line helper
matched its first candidate. A complete private bank link verified those five
functions, followed by a second independent link incorporating the five
accepted Spanish rendering routines. No function-sized or masked comparison
was used as the final acceptance gate.

## SDK bindings and exact-image proof

`GsSortLine = 0x80083F38`, `ccos = 0x800868A8` and `csin = 0x80086B38`
are already named in the French resident linker map. The two additional
bindings were independently checked using hash-verified French and Spanish
executables: all 44 bytes of `RotTransPers` at `0x80087868` and all 264 bytes
of `GsSortGLine` at `0x800840B8` are identical. Accepted Spanish bindings and
SDK header declarations supply their names; both complete French extents
remain classified as resident `sdk_asm`, not new game C.

The complete 90,112-byte French bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Every promoted function has its exact compiled-object extent and final
address, size, function type and executable-section ownership. Production
acceptance additionally checks every configured French image and all seven
terrain copies.

At independent accepted-master cutoff `9badfb1e0`, all 34 earlier entries
remain intact: the batch reaches **44/85 bank C functions / 9,780 bytes**,
with **41 assembly boundaries** and **168 configured C instances / 65,632
bytes**. The unmerged color/rectangle batch is not included in that cutoff.
Boot, MODEL/SU, overworld-tail and remaining bank ownership are still separate
work; these configured totals are not a claim of exhaustive runtime coverage.

Before publication, the now-accepted color/rectangle batch and later regional
additions were integrated additively through `19f4630ee`. All 47 accepted
French entries are preserved, reaching **57/85 bank C functions / 13,688
bytes**, **28 assembly boundaries**, and **181 configured C instances /
69,540 bytes**. The new contribution remains these ten functions / 3,604 bytes.

## Height-ring follow-up

After #6552 was accepted, `func_8014EC8C` (`8014EC8C..8014EE0C`,
384 bytes) was recovered as a separate complete `height_ring.c` unit. Neither
neighboring two-function source group is split or partially replaced.

| Experiment | Result |
|---|---|
| Promote `count` to a signed local before the nonzero guard; update `bottom` in place | 380 bytes, four bytes short; the count setup/lifetime differs from retail. |
| Assign that signed local inside `if (count != 0)`, before the height subtraction | All 384 bytes match, including the previously differing saved registers and setup order. |

The accepted function takes unsigned halfword width, depth and count, full
signed-word top and bottom, and an SDK `SVECTOR` output pointer. The original
entry preserves full-word vertical arguments; the earlier halfword-height
experiment is not promoted. Output is cosine-scaled X, `top - bottom` Y and
sine-scaled Z. Padding remains untouched.

Zero count still performs both extent-scaling sine calls, but performs no
count-dependent division or vector stores. Keep the signed count promotion
inside the guard: moving it earlier changes instruction bytes.

The private 58-function bank first proved the height ring independently.
A second complete-bank proof combined it with the unchanged accepted Spanish
dispatcher, reaching **59/85 C functions / 15,360 bytes**, with **26 assembly
boundaries** and **183 configured C instances / 71,212 bytes**. Both complete
objects and linked owners are exact; all 57 previously accepted entries are
unchanged. The randomized curve at `8014FABC` remains assembly.
