# French duel-effect bank

The six previously configured French overlays did not cover all runtime code.
The exact resident `Duel_LoadTerrainPackage` implementation requests WA sectors
`0x1B88 + terrain * 0xF0`. `Duel_LoadPackageStage` uses phase order
`0, 16, 1, 2, 3, 4, 5, 6, 7`: phase 0 returns 16 and the transfer callback
passes `q->result++`. The sizes before phase 7 are
`64 + 5 + 4 + 5 + 32 + 1 + 2 + 32 = 145` sectors.

Phase 7 transfers 44 sectors through `D_800101DC`. The original French
executable contains `0x80146000` at that pointer location. All seven complete
images at WA sectors `7193 + terrain * 240` are byte-identical:

| Property | Value |
|---|---|
| Image length | `0x16000` / 90,112 bytes |
| Load address | `0x80146000` |
| Module word | `0xB` |
| SHA-256 | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| Initial data and tables | `0x0..0x258` |
| Text | `0x258..0x141E4` |
| Trailing data and padding | `0x141E4..0x16000` |

The resident `DuelEffect_UpdateRequests` calls legacy `func_801462B0`,
independently bound to French `0x80146258`. This dispatcher selects many local
effect routines. Its texture-word helper is called at `0x8014F564`; its
`GsIMAGE` field accesses and page/CLUT expressions agree with the existing
local `model_texture_upload.c` and SDK declarations. No reference-project
types, compiler settings, or source bodies were imported.

The initial disassembly identifies 85 contiguous provisional functions,
covering all 81,804 text bytes. The first batch matched **ten functions /
1,380 bytes**:
seven at `0x8014F010..0x8014F490` and three at
`0x8014F524..0x8014F608`. They implement color assignment, randomized vector
initialization, quad setup, integer power, and texture-page/CLUT packing.
Address-based names remain until broader semantic naming review.
That batch left 75 boundaries as provisional generated assembly, not exclusions.
Neither raw data interval is claimed as C-owned storage.

## Experiments and acceptance

| Experiment | Result |
|---|---|
| Texture helper pair under existing `g0_split`, `g8_split`, and `g0` profiles | All 164 bytes match; not accepted until the entire bank was rebuilt. |
| Entire bank from generated assembly, then the helper pair in C | Both complete images match and both linked C symbols have exact sizes and section ownership. |
| Eleven adjacent utilities using array indexing and conditional-value expressions | Four quad functions differ; combined object is 36 bytes too large. |
| Explicit vertex-pointer traversal and sign-factor multiplication | Ten functions match; `func_8014F490` retains eight differing bytes from exchanged `s3`/`s4` allocation. |
| Narrow the remaining quad width to `s16` | Adds four bytes; rejected. |
| Keep 32-bit width with a `register` qualifier | Same eight-byte allocation mismatch; rejected. |
| Retain that 148-byte quad as generated assembly; build two complete C groups | Entire 90,112-byte image matches; all ten functions have exact addresses, sizes, and section-defined ELF ownership. |

An initial broad SDK `stdlib.h` include needed unrelated include search paths;
the existing narrow `rand.h` declaration avoids that dependency. `rand` is
bound to the verified French resident address `0x8008F708` in both the
disassembler symbols and the linker aliases. No aliases override C functions.
Compiler flags and shared resident implementations are unchanged.

The production `french-match-overlays` gate builds this image along with the
original six. `duplicate_sector_offsets` verifies the six additional complete
archive copies before the representative is built, avoiding seven redundant
identical layouts and inventories. Input hashing and duplicate-copy failures
are fatal; existing single-instance manifests retain their previous behavior.

## Remaining runtime scope

The final inventoried French bank helper, `func_8014FABC`, now reuses the
accepted `bolt_vertices.c` body unchanged with `gcc_2_8_1_g0_split`.
Its 836 instruction bytes and 80-byte frame match with the French `csin`
(`0x80086B38`) and `rand` (`0x8008F708`) bindings. Named angles,
calls inside conditional expressions and index-first pointer arithmetic
preserve the original register allocation and repeated division.
Earlier local declaration/profile experiments remained mismatches; the
accepted shared body matched the complete French bank on its first trial.

All **85 inventoried functions / 81,804 instruction bytes** now have actual
matching C owners, preserving the separate 228-byte ritual compiler table.
Production verification covers the complete image and all seven archive copies,
not just the new function's bytes. This closes the bank's inventoried C gap,
not the broader runtime coverage gaps below.

French boot code also exists outside the old six-overlay inventory:
`Main_RunBootSequence` requests WA `0x2503` for `0x25` sectors, and
`Main_LoadBootImageStage` consumes `32 + 1 + 1` sectors before its final
three-sector transfer. Those sectors start at 9509 and load at `0x80168000`
through `D_800101D8`. The complete 6,144-byte module hashes to
`22b33785288bd88f989d56741d6a9aa3621dc5b26d089fa2b5e09f6416454c05`;
resident code calls `0x801680F4` and `0x80168160`. Game versus SDK ownership
requires further evidence before promotion or exclusion.

The overworld `0x1618..0x17D0` fragment overlaps mutable live state, lacks a
prologue, and contains fixed-point code and an epilogue. Absence of external
direct jumps in a module-only scan does not establish unreachability.
MODEL/SU dynamic loads also require an exhaustive entry-point and ownership
audit. Neither resident completion nor this bank's inventory closes #6460.

## Vector and indexed-quad initialization

A second complete source group adds **five functions / 1,204 bytes** at
`0x8014F608..0x8014FABC`, bringing the bank to **15 matching functions /
2,584 bytes**, with 70 provisional functions still in generated assembly.
All ten previous C entries and all 85 original boundaries are preserved.

The first three functions initialize arrays of the existing SDK `SVECTOR`
type. The instruction stride is eight bytes, and writes target the three
halfwords at offsets zero, two, and four; padding is not modified.
Three-dimensional signed random spread, negative-only vertical spread, and
two-dimensional spread preserve the original signed remainder and division
operations. The scale and count arguments are independently constrained by
the observed halfword loads and masks. `rand` uses the already verified
resident binding and existing SDK header, not a local declaration.

The next two functions initialize four vertices from caller-supplied
halfword width and height tables. Both mirror widths across the vertical
axis; one indexes both row heights and the other zeros the lower row.
All Z coordinates are zero, with the original field-store and table-load
ordering retained. The pointers are arguments, not guessed module storage.

| Experiment | Result |
|---|---|
| Compile the three random-vector initializers with `gcc_2_8_1_g0_split` | First candidate matches all 920 bytes in a complete bank link. |
| Extend the same unit with both indexed-quad initializers | First candidate matches all 1,204 bytes; the complete bank remains byte-identical and all 15 C functions have exact linked ownership. |
| Promote with the owning header and unchanged named profile | Production overlay builds retain the complete image identity and all original module copies. |

No source from a different region or compiler variant was introduced.
`func_8014F490` remains the previously documented register-allocation
mismatch; no partial body, relocation-masked candidate, or new exclusion is
counted as a match.

## Color, quad and matrix helpers

The next batch adds **six functions / 588 bytes** in three complete pairs:

| Unit | Extent | Functions | Text bytes |
|---|---|---:|---:|
| `color_test.c` | `0x8014D378..0x8014D3E8` | 2 | 112 |
| `quad_helpers.c` | `0x8014FE00..0x8014FF40` | 2 | 320 |
| `matrix_helpers.c` | `0x801514BC..0x80151558` | 2 | 156 |

The bank now has **21 matching C functions / 3,172 bytes**, with 64
provisional functions left in assembly. All original boundaries and the
previous 15 C entries remain unchanged.

The color predicates read only three bytes. The quad constructor reuses the
already matched integer-power helper. The swap copies complete eight-byte
SDK `SVECTOR` values, including their padding, through a stack temporary.
The matrix copy uses the existing SDK `MATRIX` layout: nine halfword
coefficients and three word translations at offset 20; it does not copy
the intervening padding. Its wrapper makes a local matrix, scales it with
`ScaleMatrix`, then calls `GsSetLsMatrix`. The resident French linker map
independently establishes those SDK addresses as `0x800875F8` and
`0x80085558`; no guessed declarations or data storage are introduced.

The first candidate for each pair reproduces the full bank with the existing
`gcc_2_8_1_g0_split` profile. Every added function has its exact linked address,
size, function type and executable-section ownership. The original images,
all terrain copies, and unrelated regional implementations remain unchanged.

### Deferred curve experiments

The adjacent 836-byte function at `0x8014FABC` is **not promoted**. Its
observed call at `0x80086B38` binds to SDK `csin`, not `rsin`
(`0x80086628`). Local matched bindings and `libgte.h` provide that declaration.

| Experiment | Precise rejection |
|---|---|
| Sign-factor multiplication inside the coordinate expression | Complete image is eight bytes short; the compiler negates the radius rather than the sine result and changes register allocation. |
| Reverse the sign-factor multiplication operands | Same eight-byte-short output. |
| Explicit radius and sine temporaries with conditional negation | Complete image is 68 bytes short, with different expression scheduling. |
| Conditional expressions containing each sine call | Complete image is 144 bytes too long. |
| Explicit temporaries with `gcc_2_8_1_g0_split_no_cse_follow_jumps` | Same 68-byte-short output. |
| Explicit temporaries with `gcc_2_8_1_g0_split_no_strength_reduce` | Same 68-byte-short output. |
| Single sine call assigned within each coordinate expression | Complete image is 12 bytes too long. |

Sources and trial logs for these rejected experiments remain under
`tmp/`. The accepted quad/swap pair was isolated and verified with the curve
left as generated assembly, then combined with the exact color/matrix pairs.
Neither this curve nor the earlier `0x8014F490` register-allocation case is
counted as matching C.

## French matrix setup and unchanged Spanish drawing reuse

Five additional functions / 1,164 bytes pass complete French bank identity:
the new `matrix_setup.c` at `0x801513F4..0x801514BC` (200 bytes), and the
unchanged Spanish `number_helpers.c` and `primitive_draw.c` groups described
below (964 bytes). All use `gcc_2_8_1_g0_split`; the latter groups retain
their complete source extents, definition order, SDK declarations and
compiler profile without regional conditionals. French now has **26 bank
C functions / 4,336 bytes**, with **59 provisional assembly functions**.

The matrix setup first installs its input matrix, transforms the position
into a local matrix's translation, and computes rotation. Modes two/three
multiply that rotation by the input matrix; modes one/three preserve the
unscaled matrix through the already matched `func_801514F8`. Scaling and
installation follow. The existing French resident bindings independently
identify `RotTrans` (`0x800878F8`), `RotMatrix` (`0x80087CB8`) and
`MulMatrix2` (`0x80087408`); no reference-project types or flags are used.

The neighboring projection wrappers at `0x80151218` and `0x8015131C`
were retained as assembly at this stage. Three scratch candidates preserve image size but
fail exact code generation: `depth + 1 - bias` differs by 12 bytes because
GCC computes `depth - (bias - 1)`; incrementing depth before the bias
branch differs by 17 bytes, including changed register allocation;
`depth - bias + 1` differs by 18 bytes. Isolating the 200-byte matrix
function restores complete bank identity before combining it with the
four independently verified Spanish helpers. Rejected sources and their
full-image comparison logs remain under `tmp/`.

The ordering-table pointer remains preserved bank data, declared through
the existing shared `drawing_helpers.h`; it is not newly allocated or
overridden by an absolute alias. The earlier curve/quad mismatches and
the two Spanish textured-quad mismatches were also retained as assembly
at this stage.

## French projection, sorting and textured-quad integration

Eight further functions / 1,840 bytes form three complete source groups:

| Unit | French extent | Functions | Bytes |
|---|---|---:|---:|
| `projected_wrappers.c` | `0x80151218..0x801513F4` | 2 | 476 |
| `packet_helpers.c` | `0x80152EC4..0x80153200` | 4 | 828 |
| `textured_quads.c` | `0x80156C40..0x80156E58` | 2 | 536 |

The previously rejected projection wrappers match when the depth expression
retains the nested subtraction `depth - (bias - 1)`. This is the fourth
source experiment after the three failures above, using the same
`gcc_2_8_1_g0_split` profile. No compiler changes or scheduling barriers
were needed. The existing matrix setup remains its own unchanged complete
source unit; it is not partially absorbed into the new wrapper group.

Packet types follow callers, not just matching XY offsets. The caller at
`0x80156C40` sets length nine, command `0x2C`, UV fields and texture words
before calling `0x80151218`: that wrapper and its `0x80152F9C` sorter use
SDK `POLY_FT4`. The early tentative `POLY_G4` view produced identical
instructions for the accessed fields but was corrected before promotion.
The caller at `0x801575CC` instead sets length eight, command `0x38` and
per-vertex colors before calling `0x80152EC4`, confirming `POLY_G4` usage.
The shared helper accepts a generic primitive pointer and accesses the
common XY positions rather than introducing a French-only parameter type.
The other wrapper/sorter use `POLY_GT4` with XY offsets 8, 20, 32 and 44.

The sorters apply the two halfword offsets at `D_8015B7F8` and submit
packets through the existing resident and SDK declarations. Their distinct
priority behavior is preserved: the Gouraud sorter adds one, the opaque
flat-textured path does not, and the opaque Gouraud-textured path adds one.
The default matrix initializer keeps the original coefficient-store order,
unit diagonal, zero X/Y translation and Z translation 300 without touching
padding. `D_8015B7F8` and the halfword priority at `D_8015B800` remain
original raw module data, with declarations only and no absolute override.
The packet group and its existing header are reused unchanged from the
accepted Spanish integration. This replaces the independent scratch
implementation and avoids conflicting declarations for the SDK vector
view of `D_8015B7F8` and the generic packet argument. Its complete French
image and linked ownership were reverified after the substitution.

The accepted Spanish textured pair is reused unchanged, including its
existing `POLY_FT4` callee declaration and SDK `addVector` expression.
Its original [independent research](duel-effect-textured-quads.md) remains
applicable; the new French proof additionally links its projected callee
from C. All 90,112 bytes and every C function's exact linked ownership
match. French now has **34 bank C functions / 6,176 bytes**, with all 26
prior entries and all 85 boundaries preserved; **51 functions remain
assembly**. The earlier `0x8014F490` and `0x8014FABC` mismatches remain
unpromoted, and this does not resolve the outstanding runtime-coverage
questions.

## French color, layered drawing and projected strips

Twelve more functions reuse four accepted Spanish source groups unchanged:

| Source group | French range | Functions | Bytes |
|---|---|---:|---:|
| `color_transition.c` | `0x80153F28..0x80154084` | 3 | 348 |
| `screen_draw.c` | `0x801556F4..0x801558F4` | 3 | 512 |
| `layered_drawing.c` | `0x801558F4..0x80155F94` | 3 | 1696 |
| `drawing_tail.c` | `0x80155F94..0x80156448` | 3 | 1204 |

The first independent trial linked the nine color/fullscreen/layered functions
and reproduced the entire French bank. A second trial against accepted master
`bea02675c` added the three texture/strip routines, again matching all 90,112
bytes and every compiled-object and final-ELF function extent. Neither trial
changed the shared sources, headers, named compiler profile or declaration
order. The original Spanish expression/type evidence remains applicable in
[the layered drawing notes](duel-effect-layered-drawing.md) and
[the texture/strip notes](duel-effect-drawing-tail.md); regional image equality
was not used as a substitute for the independent French build.

The only new external binding is `D_8009B300 = 0x8009C688`, independently
confirmed by the French resident linker map. The color header already reuses
the resident declaration from `src/game/sorted_entry.h`. This word remains
resident-owned; no overlay storage, guessed allocation or overriding alias
for module data is introduced. Existing packet globals and texture-prefix
data retain their generated storage.

All 34 earlier entries and all 85 boundaries are preserved. French now has
**46 bank C functions / 9,936 bytes**, leaving **39 assembly boundaries**.
The seven configured images contain **170 matching C instances / 65,788
bytes**. These counts do not resolve the separate boot, MODEL/SU or overworld
coverage questions.

## French rectangle helper allocation resolution

`func_8014F490` now matches its complete 148 bytes in `rect_vertices.c`,
without changing the accepted utility or texture-word source groups.
It fills four `SVECTOR` records: alternating negative/positive X extent,
negative Y for the first two vertices and positive Y for the last two,
and zero Z. The eight-byte stride and halfword stores reuse the existing
SDK vector declaration; padding remains untouched. The width is used as a
32-bit value and height is sign-extended from a halfword.

The follow-up experiments used the current shared declarations and existing
`gcc_2_8_1_g0_split` and `gcc_2_8_1_g0_split_no_cse_follow_jumps` profiles.
Both profiles gave the same failed results:

| Source experiment | Exact mismatch |
|---|---|
| Direct `-height : height`, with original/reversed product or value temporary | 156 rather than 148 bytes; hoisted positive/negative height values require another saved register. |
| Direct height selection with an explicit vertex pointer | 164 bytes. |
| Direct height selection with a local width value | 156 bytes. |
| Sign-factor multiplication, original/reversed product or value temporary | 148 bytes, but six differing words at offsets `0x04`, `0x08`, `0x0C`, `0x10`, `0x3C`, `0x48` exchange width/pointer allocation. |
| Sign-factor multiplication with an explicit vertex pointer | 156 bytes. |
| Sign-factor multiplication with `s32 horizontal = width` | Exact with `gcc_2_8_1_g0_split`; no profile change or register pin. |

The local width value is therefore retained deliberately. A private complete
French bank link then verified the new definition alongside all 46 earlier
C functions. Production verification checks all seven images and linked
ownership again; the bank retains its original SHA-256 and all 85 boundaries.
French reaches **47 bank C functions / 10,084 bytes**, with **38 boundaries
still assembly**, and **171 configured C instances / 65,936 bytes**.
The curve at `0x8014FABC` remains unmatched.

## Regional presence and Spanish integration

Direct archive inspection confirms this runtime bank in all seven available
retail versions, not only French. Every release has seven byte-identical
terrain copies within its own archive. The table gives WA archive sector
locations for terrain `t = 0..6`; each sector is 2,048 bytes.

| Release | Disc ID | First sector formula | Sectors | Load address | Image SHA-256 |
|---|---|---|---:|---|---|
| English PAL | `SLES-03947` | `7193 + 240*t` | 44 | `0x80146000` | `e0863650755d5de5d4abfdcfaead643e5b2b2463b0019c790df881e1dadb6075` |
| French | `SLES-03948` | `7193 + 240*t` | 44 | `0x80146000` | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| German | `SLES-03949` | `7193 + 240*t` | 44 | `0x80146000` | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| Italian | `SLES-03950` | `7193 + 240*t` | 44 | `0x80146000` | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| Spanish | `SLES-03951` | `7193 + 240*t` | 44 | `0x80146000` | `a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3` |
| North American | `SLUS-01411` | `5970 + 235*t` | 44 | `0x80146000` | `baa203b937dc6bdf91b1826c5832f0f32e11ae5fe9d05193a4361bc08158b9e0` |
| Japanese | `SLPM-86398` | `5953 + 239*t` | 48 | `0x80154000` | `17784f0d718e5218e8770eb9bc98d566060a133f9cdcc59133dc295823080561` |

These offsets follow the accepted resident loader implementations, not a
search for similar-looking payloads. The North American package starts at
`0x16C6 + 0xEB*t`, with 140 sectors consumed before stage 7. The Japanese
wrapper selects `0x16B5 + 0xEF*t` and 48 stage-7 sectors, again after 140.
The European path consumes an additional five-sector language upload before
stage 7, yielding the 145-sector prefix described above. The initial
`D_800101DC` pointer in each checked executable confirms its load destination.
The Japanese image is 98,304 bytes; the other images are 90,112 bytes.
Distinct hashes mean cross-region C portability still needs independent
complete-image matching; presence alone does not establish decompilation.

Spanish initially registered all seven copies and the full 85-boundary
inventory, reusing all three accepted shared C units and the named
`gcc_2_8_1_g0_split` profile unchanged: **15 C functions / 2,584 bytes**.
All seven configured Spanish module images match, and every one of the 15
bank definitions has its exact linked address, size, function type and
executable-section ownership. The Spanish resident `rand` bytes agree with
the French binding at `0x8008F708`. That initial integration retained all 70
unmatched bank boundaries as assembly without reclassifying raw data or
unknown functions as C.

The Italian and German bank copies and boot images were independently
checked as well. Their identical payloads support future reuse but do not
stand in for regional build registration or C-ownership verification.

## Spanish digit and primitive helpers

Four more functions / 964 bytes form two complete shared C groups:

| Group | Functions | Bytes |
|---|---|---:|
| `number_helpers.c` | `0x80156AD4`, `0x80156B40` | 364 |
| `primitive_draw.c` | `0x80156E58`, `0x80156FA4` | 600 |

The digit counter repeatedly divides a signed halfword by ten while its
absolute value is nonzero; zero therefore returns zero, not one. Its
`__builtin_abs` spelling follows existing resident prior art and preserves
the inline absolute-value instructions. The digit writer retains its
original signed-halfword view of the input for the count, unsigned-halfword
arithmetic for the remaining value, signed-halfword power results, repeated
count calls, and `count - (i + 1)` expression association. This is not a
new generalized integer-formatting implementation.

The triangle helper constructs two fixed endpoints and projects each
eight-byte input `SVECTOR` with `RotAverage3`. Its depth bias is applied
before the signed shift and unsigned-halfword packet priority. The quad
helper preserves `RotAverage4`'s flag rejection and the distinct opaque
`GsSortPoly` and flagged resident packet-submission paths. Both use the
existing SDK polygon/vector types and resident-owned declarations.
`D_8015B7F4` is a declared `GsOT *` read from preserved module data, not a
new C definition or an absolute alias overriding that storage.

| Experiment | Result |
|---|---|
| Six-function trial using conditional absolute value and direct stack-polygon accesses | All six differ; sizes are 124/248/268/260/328/256 bytes versus 108/256/272/264/332/268. |
| Explicit packet pointers and `count - (i + 1)` | Digit writer and flat quad match; external `abs` still emits a call. Triangle has seven differing words; textured quads retain extra live registers. |
| Semantic `__builtin_abs`, separate endpoint stores and combined textured-quad stack record | Counter matches. Triangle has three differing hoisted instructions; textured quads have 14 differing words despite correct sizes. |
| Move depth-bias subtraction into the projection-result expression | Triangle matches. Per-vertex pointer forms do not solve the textured-quad induction/register differences. |
| Keep the two textured quads as assembly and compile only the four exact functions in complete groups | Private and production full-bank links match all 90,112 bytes with exact C object and ELF ownership. |

This four-function integration brought Spanish to **19 matching bank
functions / 3,548 bytes** and **66 provisional assembly functions**.
All 15 earlier entries, all 85 boundaries,
all seven terrain copies and the original six configured module hashes are
preserved. `0x80156C40` and `0x80156D50` were retained as assembly at that
stage alongside the earlier deferred `0x8014F490`; no mismatching source was
promoted. The new groups use
the unchanged `gcc_2_8_1_g0_split` profile and do not alter existing shared
units. French adoption is independently verified above; shared image
identity alone does not replace the remaining regional build gates.
The [subsequent textured-quad pair](duel-effect-textured-quads.md) resolves
the two neighboring functions using the existing SDK macro, bringing Spanish
to 21 C functions / 4,084 bytes with 64 still in assembly.

## Spanish reuse of color, quad and matrix helpers

The six helpers accepted for French in `e6740a96b` are reused unchanged in
Spanish as three complete groups: `color_test.c` (112 bytes),
`quad_helpers.c` (320 bytes) and `matrix_helpers.c` (156 bytes).
The existing `gcc_2_8_1_g0_split` profile and shared header remain unchanged.
Spanish resident metadata independently supplies `ScaleMatrix = 0x800875F8`
and `GsSetLsMatrix = 0x80085558`; no module-owned function is replaced by an
absolute alias.

The independent six-function proof preserves all 19 previously accepted
Spanish entries and all 85 provisional boundaries, reaching **25 C functions /
4,136 bytes**, with **60 boundaries remaining assembly**. It does not depend
on the separate textured-quad integration. The allocation-sensitive
`0x8014F490` and curve `0x8014FABC` remain assembly.

Acceptance requires the complete 90,112-byte Spanish bank, every terrain
copy and all seven configured module images to match, plus exact C symbol
addresses, sizes, function types and executable sections in both compiled
objects and the linked image. Identical regional archive slices alone are
not the acceptance criterion. Boot, MODEL/SU and the opaque overworld
fragment remain separate runtime-coverage work.
Combining this reuse with the four newly recovered
[packet/matrix helpers](duel-effect-packet-helpers.md) gives **29 C functions /
4,964 bytes**, leaving **56 boundaries in assembly**. Both independently
verified sets are published together; neither relies on an unmerged PR.

The final batch also adds the six
[color/fullscreen helpers](duel-effect-color-helpers.md) and preserves the
accepted textured-quad pair. Its complete production bank contains **37 C
functions / 6,360 bytes**, with all **48 remaining boundaries** still
generated assembly. All 21 previously accepted Spanish entries are unchanged.

## North American integration

`config/slus_01411/overlays.json` registers the North American bank as
`duel_effects`: the image at WA sector 5970, with the six other terrain copies
(`5970 + 235*t`) checked byte-identical as duplicates. The module word is
`0x18`. Tables and strings occupy `0x0..0x2B0`, text `0x2B0..0x14270` (the word
after the final `jr $ra` and its delay slot), and raw data follows to `0x16000`.

The three shared C units apply unchanged. Under relocation masking, their
French ranges and the North American ones are equal instruction for
instruction, at a constant offset of `0xBD8`:

| Unit | French range | North American range |
|---|---|---|
| `utility_helpers.c` | `0x9010..0x9490` | `0x9BE8..0xA068` |
| (quad, assembly) | `0x9490..0x9524` | `0xA068..0xA0FC` |
| `texture_words.c` | `0x9524..0x9608` | `0xA0FC..0xA1E0` |
| `vector_init.c` | `0x9608..0x9ABC` | `0xA1E0..0xA694` |

The C keeps its French address-based names, bound to the North American
addresses in `duel_effects_symbols.txt`. `rand` binds to the resident
`0x8008E590`. That gives **15 C functions / 2,584 bytes** of 85 boundaries;
the other 70 remain generated assembly. `make match-overlays` rebuilds the
complete 90,112-byte image to the hash above.

## Japanese integration

`config/slpm_86398/overlays.json` registers the Japanese bank as
`japanese_duel_effects`: 48 sectors from WA sector 5953, loaded at
`0x80154000`, with the six other terrain copies (`5953 + 239*t`) checked
byte-identical as duplicates. Its module word is `0x18`. Its text has the
same file bounds as the North American one, `0x2B0..0x14270`; raw data runs
from there to `0x18000`.

The three shared C units match unchanged, each `0x94` further into the image
than in French (`utility_helpers.c` at `0x90A4`, `texture_words.c` at `0x95B8`,
`vector_init.c` at `0x969C`, and the quad kept as assembly at `0x9524`). The
C keeps its French address-based names, bound to the Japanese addresses in
`duel_effects_symbols.txt`. `rand` binds to the resident `0x8008E3B0`, the
target of the bank's 111 generated calls. That gives **15 C functions / 2,584
bytes** of 85 boundaries; the other 70 remain generated assembly.
`make japanese-match-overlays` rebuilds the complete 98,304-byte image to the
hash above, and `make japanese-inventory` now refreshes its inventory too.

## European integration

`config/sles_03947/overlays.json` registers the English PAL bank as
`european_duel_effects`, at the same WA sectors as French (`7193 + 240*t`,
with six duplicates checked byte-identical). Its image differs from the
French one in 445 words, but its layout is the French layout exactly:
- text `0x258..0x141E4`;
- the same 85 boundaries;
- the three shared C units at the same offsets.

So the layout and the matching manifest are the French ones. The only binding
that changes is `rand`, which goes to the European resident `0x8008F504`, the
target of the bank's 111 generated calls. That gives **15 C functions / 2,584
bytes**; the other 70 boundaries remain generated assembly.
`make european-match-overlays` rebuilds the complete image to the hash above.

## European shared helpers

Since its registration, seven more accepted units also apply to the European
bank:
- `color_test.c`, `quad_helpers.c`, `matrix_setup.c` and `matrix_helpers.c`
  from French;
- `number_helpers.c`, `textured_quads.c` and `primitive_draw.c` from Spanish.

The European layout is the French one, so each unit sits at its French image
offset and is masked-identical there. Their resident calls bind to the
European targets of the bank's own `jal`s, each of which agrees with
`config/sles_03947/symbols.txt`:
- `GsSetLsMatrix` `0x80085354`;
- `RotTrans` `0x800876F4`;
- `RotMatrix` `0x80087AB4`;
- `MulMatrix2` `0x80087204`;
- `ScaleMatrix` `0x800873F4`;
- `RotAverage3` `0x800877D4`;
- `RotAverage4` `0x80087834`;
- `GsSortPoly` `0x800840A4`;
- `func_8005B260` `0x8004D3B4`.

The bank-internal `func_80151218`, `D_8015B748` and `D_8015B7F4` keep their
French addresses. The European bank now has **28 C functions / 4,872 bytes**
of 85; 57 remain generated assembly.

## Japanese shared helpers

The seven units accepted since the Japanese registration also apply to the
Japanese bank. Each occurs exactly once under relocation masking, and each
keeps one address shift for all its functions:
- `color_test.c` at image `0x73D0`;
- `quad_helpers.c` at `0x9E94`;
- `matrix_setup.c` at `0xB488`;
- `matrix_helpers.c` at `0xB550`;
- `number_helpers.c`, `textured_quads.c` and `primitive_draw.c` at
  `0x10B64..0x11140`.

Their resident calls bind to the Japanese addresses the bank's own `jal`s use,
each of which agrees with `config/slpm_86398/symbols.txt`:
- `ScaleMatrix` `0x80086260`;
- `GsSetLsMatrix` `0x800841C0`;
- `RotTrans` `0x80086560`;
- `RotMatrix` `0x80086920`;
- `MulMatrix2` `0x80086070`;
- `RotAverage3` `0x80086640`;
- `RotAverage4` `0x800866A0`;
- `GsSortPoly` `0x80082F10`;
- `func_8005B260` `0x8004CFF8`.

The shared C names three bank-internal symbols by their French addresses.
They are bound to the Japanese addresses decoded from the same `jal` and
`%hi`/`%lo` instructions:
- `func_80151218` -> `0x8015F2AC`;
- `D_8015B748` -> `0x801697E0`;
- `D_8015B7F4` -> `0x8016988C`.

The Japanese bank now has **28 C functions / 4,872 bytes** of 85; 57 remain
generated assembly.

## Japanese shared drawing and sorting units

After #6538 and #6540 reconciled the shared drawing and packet declarations,
five more accepted units apply to the Japanese bank:
- `projected_wrappers.c` at image `0xB2AC`;
- `packet_helpers.c` at `0xCF58`;
- `color_transition.c` at `0xDFBC`;
- `layered_drawing.c` at `0xF984`;
- `drawing_tail.c` at `0x10024`.

Each is the unique relocation-masked occurrence, with one shift per unit.
`screen_draw.c` is left out: its `setXY4`
coordinates are PAL 256-line literals, and the Japanese instructions load 240
at the same positions. That makes a masked match but a byte mismatch.

`D_8015B7F8` and `D_8015B800` bind to the Japanese `0x801697D0` and
`0x801697C8`, decoded from the same `%hi`/`%lo` pairs. The resident
`D_8009B300` binds to `0x8009B1F0`, which is `gJapanese_D_8009B300` in
`config/slpm_86398/symbols.txt`. The Japanese bank now has **43 C functions /
9,424 bytes** of 85; 42 remain generated assembly.

## European shared drawing and sorting units

After #6538 and #6540, six more accepted units apply to the European bank at
their French image offsets:
- `projected_wrappers.c` `0xB218`;
- `packet_helpers.c` `0xCEC4`;
- `color_transition.c` `0xDF28`;
- `screen_draw.c` `0xF6F4`;
- `layered_drawing.c` `0xF8F4`;
- `drawing_tail.c` `0xFF94`.

Being PAL, the European bank takes `screen_draw.c` with its 256-line
coordinates unchanged. The resident `D_8009B300` binds to
`0x8009C268`, as in `config/sles_03947/symbols.txt`. The European bank now has
**46 C functions / 9,936 bytes** of 85; 39 remain generated assembly.

## Japanese and European gradient and display quads

The two units #6544 accepted for Spanish also apply to both banks. Each is
the unique relocation-masked occurrence:

| Unit | Japanese image | European image | Functions |
|---|---|---|---:|
| `gradient_strip.c` | `0x104D8` | `0x10448` | 1 |
| `display_quads.c` | `0x11438` | `0x113A8` | 3 |

They need no new resident or bank-internal bindings. The Japanese bank now has
**47 C functions / 11,080 bytes** (38 generated). The European bank has
**50 C functions / 11,592 bytes** (35 generated).

## Japanese and European rect vertices and gradient lines

`rect_vertices.c` (French, the former `quad_unmatched` boundary) and
`gradient_lines.c` (Spanish, #6548) apply to both banks:

| Unit | Japanese image | European image | Functions |
|---|---|---|---:|
| `rect_vertices.c` | `0x9524` | `0x9490` | 1 |
| `gradient_lines.c` | `0x11140` | `0x110B0` | 1 |

`gradient_lines.c` calls `RotTransPers` and `GsSortGLine`, which bind to the
targets of the bank's own `jal`s:
- Japanese: `RotTransPers` `0x800864D0`, `GsSortGLine` `0x80082D20`;
- European: `RotTransPers` `0x80087664`, `GsSortGLine` `0x80083EB4`.

Both pairs keep the Spanish spacing: European = Spanish − `0x204` and
Japanese = European − `0x1194`, the same offsets as the other bank bindings.
The Japanese bank now has **49 C functions / 11,988 bytes** (36 generated),
and the European bank **52 C functions / 12,500 bytes** (33 generated).

## Japanese and European circle, random and crossed-line vertices

The three units #6550 accepted for Spanish were checked against both banks:

| Unit | Japanese image | European image | Functions |
|---|---|---|---:|
| `circle_vertices.c` | `0x8B10` | `0x8A7C` | 1 |
| `random_vectors.c` | `0x8FC0` | `0x8F2C` | 1 |
| `cross_lines.c` | none | `0x835C` | 1 |

`cross_lines.c` has no relocation-masked occurrence in the Japanese bank, so
that boundary stays generated there. The new resident calls bind to each
bank's own `jal` targets, each named in its `symbols.txt`:
- Japanese: `ccos` `0x80085510`, `csin` `0x800857A0`;
- European: `ccos` `0x800866A4`, `csin` `0x80086934`, `GsSortLine`
  `0x80083D34`.

The Japanese bank now has **51 C functions** and the European bank
**55 C functions**.

## North American shared units

The accepted shared units were run through the same unique relocation-masked
check against the North American bank. Seventeen apply, with one shift per
unit. Several units sit in a different order here than in French: for
example, `color_test.c` is at image `0x1B70` and `drawing_tail.c` at
`0x21DC`. Three stay out:
- `layered_drawing.c` and `cross_lines.c` have no masked occurrence in this
  bank.
- `screen_draw.c` uses PAL 256-line literals, as in the Japanese bank.

The shared C names functions by their French addresses. One such name,
`func_8015616C` (bound to `0x801483B4`), coincides with the address of a
different North American function. The generated assembly at `0x8015616C`
is therefore named `func_801558F4`, its French counterpart, which has the
same size (`0x2CC`) and is masked-identical.

The bank-internal data names are bound to the North American addresses
decoded from the same `%hi`/`%lo` pairs:
- `D_8015B748` -> `0x8015B7E0`;
- `D_8015B7F4` -> `0x8015B88C`;
- `D_8015B7F8` -> `0x8015B7D0`;
- `D_8015B800` -> `0x8015B7C8`.

The resident `D_8009B300` keeps its own address. The North American bank now
has **48 C functions / 10,680 bytes** of 85; 37 remain generated assembly.

## North American effect routines

Three of the eight effect routines claimed on #6258 now have C in the North
American bank, all with `gcc_2_8_1_g0_split`:

| French name | North American address | Size | Unit |
|---|---|---|---|
| `func_80146258` (dispatcher) | `0x801462B0` | `0x508` | `dispatch.c` (accepted in #6555) |
| `func_801481A8` (effect id 14) | `0x8014A06C` | `0x9FC` | `gather_effect.c` |
| `func_80149F90` (effect id 3) | `0x8014AA68` | `0x954` | `tile_effect.c` |

`gather_effect.c` and `tile_effect.c` are new. They were matched against
the North American image and use only the existing shared headers
(`dispatch.h`, `color_helpers.h`, `layered_drawing.h`, `textured_quads.h`,
`utility_helpers.h`, `drawing_helpers.h`). Their call sites needed two
declaration changes to already-accepted units, and neither changes a single
instruction of those units' `gcc_2_8_1_g0_split` assembly:
- `func_8014E35C` in `cross_lines.c` takes the `s32 mode` the routines pass
  in `$a0`;
- `func_80155D90` in `layered_drawing.c` takes `height` as `u16`, since the
  caller loads it with `lhu`.

The routine callees without C are bound to their North American addresses
under their French names. Resident calls (`SetGeomOffset`, `PushMatrix`,
`PopMatrix`, `memset`, `Model_GetFrameStep`, `Model_SetFrameStepOverride`)
bind to the targets of the bank's own `jal`s.

Three more routines are parked as candidates in `notes/overlays/candidates/`,
each with its residue and levers recorded:
- `trap_effect.c` (`func_80147B18`) is one instruction short;
- `shower_effect.c` (`func_8014C8FC`) is exact length, register allocation
  only;
- `vortex_effect.c` (`func_80148BA4`) is 20 instructions short.

`effect_routines.h` there holds the call-site prototypes they were measured
with, and is not built. The North American bank now has **51 C functions /
16,912 bytes** of 85; 34 remain generated assembly.

## Japanese and European dispatcher and effect routines

The accepted dispatcher and effect units now also cover the Japanese and
European banks. Each is the unique relocation-masked occurrence:

| Unit | Japanese image | European image |
|---|---|---|
| `dispatch.c` | `0x2B0` | `0x258` |
| `polygon_vertices.c` | `0x8D20` | `0x8C8C` |
| `effect_19.c` | `0xDB70` | `0xDADC` |
| `gather_effect.c` | `0x2200` | not registered |
| `tile_effect.c` | `0x3FE8` | not registered |

`gather_effect.c` and `tile_effect.c` match the NTSC Japanese bank byte for
byte. In the PAL European bank they differ in nine screen-coordinate
immediates, for example `0x62` against `0x6A`, like `screen_draw.c` in the
other direction. They stay generated there until a region height macro
exists.

The dispatcher reads two header-area data words, `D_80146024` and
`D_801461C8`, bound to the Japanese `0x80154024` and `0x80154220`. The
European resident `D_8009B261` and `D_8009B264` bind to `0x8009C1E0` and
`0x8009C1DC`, as in `config/sles_03947/symbols.txt`. The Japanese bank now has
**56 C functions / 20,092 bytes** (29 generated), and the European bank **58 C
functions / 15,804 bytes** (27 generated).

## North American polygon vertices and effect 19

The accepted `polygon_vertices.c` and `effect_19.c` occur exactly once in the
North American bank, at image `0x9864` and `0xE6B4` (`0xBD8` past French):
- `effect_19.c` reads the header-area `D_801461C8`, bound to `0x80146210`;
- it calls `func_801556F4`, bound to `0x8014D3D4`;
- the resident `D_8009B261` and `D_8009B264` keep their own addresses.

The North American bank now has **53 C functions / 18,396 bytes** of 85; 32
remain generated assembly.

## European effect 18

The accepted `effect_18.c` (#6561) occurs once in the European bank at its
French image offset `0xE084`, and matches byte for byte there.

In the North American and Japanese banks it is masked-identical but differs in
one immediate: `li s6, 0x6A` against the NTSC `0x62`. That is the same PAL/NTSC
coordinate as `gather_effect.c`, so it stays generated there until a region
macro exists (proposed on #6258).

The European bank now has **59 C functions / 17,344 bytes**; 26 remain
generated assembly.

## Japanese effect 0 and European PAL gather/tile

`effect_0.c` (#6564) occurs once in both the Japanese and the European bank,
at image `0xE71C` and `0xE688`, and matches byte for byte in both.

#6564 also gave `gather_effect.c` and `tile_effect.c` PAL variants through
`src/overlays/european/duel_effects/`, and those wrappers now match the
European bank at the French offsets `0x21A8` and `0x3F90`. The Japanese bank
keeps the shared NTSC sources registered in #6562.

The Japanese bank now has **57 C functions / 21,284 bytes** (28 generated),
and the European bank **62 C functions / 23,480 bytes** (23 generated).

## European effect 6

`effect_6.c` (#6566) occurs once in the European bank at its French image
offset `0xEB30` and matches byte for byte. It has no relocation-masked
occurrence in the Japanese bank. The European bank now has **63 C functions /
26,492 bytes**; 22 remain generated assembly.

## North American effects 0, 1, 2, 4, 8, 9, 10, 12, 15 and 21, and the vertex generators

Ten more accepted units occur exactly once in the North American bank:
- `effect_21.c` at image `0x8FC4`;
- `effect_2.c` at `0xDDD8`;
- `effect_8.c` at `0x39DC`;
- `effect_12.c` at `0xB9D8`;
- `effect_0.c` at `0xF104`;
- `effect_15.c` at `0xAB18`;
- `effect_1.c` at `0x10438`;
- `effect_9.c` at `0x12048`;
- `effect_10.c` at `0x75D4`;
- `effect_4.c` at `0x1274C`.

`effect_8.c` (#6572) is `func_80147B18`, the trap routine parked above. Its
Spanish source matches the North American bank unchanged, so the
`trap_effect.c` candidate is removed.

`effect_10.c` (#6590) is `func_8014C8FC`, the shower routine. The
`shower_effect.c` candidate stopped at seven register-only rows, and the
accepted source matches the North American bank unchanged, so that candidate
is removed as well.

`effect_5.c` also occurs once, but it differs in one immediate: the rising
particles start at height `-98` in the North American bank and `-106` in the
PAL bank. It needs the `VERSION_EUROPE` split that `gather_effect.c` uses,
which also moves the French and Spanish registrations to a European wrapper,
so it is left for a separate change.

One French data name, `D_80146014` (bound here to `0x8014606C`), coincides
with the address of a different North American header word. That word is
named `gNorthAmerican_D_80146014`, following the regional `gEuropean_` and
`gJapanese_` prefixes.

The vortex candidate carries its latest measured residue: it is blocked by
`func_8015405C`, whose single shared declaration cannot be both the `u8` its
definition needs and the `u16` this call site passes.

`func_8014EA7C` and `func_8014EF2C` were registered here from
`circle_vertices.c` and `random_vectors.c`, which define them a second time.
The French and Spanish banks register the same two functions from
`ring_vertices.c` and `radial_random_vectors.c`, alongside their neighbours
`func_8014EB1C` and `func_8014EE0C`. Both whole units occur once in the North
American bank (image `0x9654` and `0x99E4`) and match it, so they replace the
single-function sources here and add the two neighbours.

The North American bank now has **65 C functions / 36,116 bytes** of 85;
20 remain generated assembly.

## Japanese and European effects 1, 2, 4, 5, 8, 9, 10, 12, 15 and 21

These accepted units occur exactly once in the bank and match byte for byte:

| Unit | Japanese image | European image |
|---|---|---|
| `effect_21.c` | `0x8480` | `0x83EC` |
| `effect_2.c` | `0xD294` | `0xD200` |
| `effect_8.c` | `0x1B70` | `0x1B18` |
| `effect_12.c` | `0xAE94` | `0xAE00` |
| `effect_10.c` | `0x6954` | `0x68FC` |
| `effect_15.c` | `0x9FD4` | `0x9F40` |
| `effect_1.c` | `0x11824` | `0x11794` |
| `effect_9.c` | `0x13434` | `0x133A8` |
| `effect_4.c` | `0x13B38` | `0x13AAC` |
| `effect_5.c` | -- | `0x11E10` |

`effect_5.c` is European only. The Japanese copy at image `0x11EA0` differs in
one immediate: its rising particles start at height `-98`, as in the North
American bank, where the PAL source has `-106`.

The Japanese bank now has **66 C functions / 37,156 bytes** (19 generated).
The European bank has **73 C functions / 44,868 bytes** (12 generated).

## NTSC arms of six shared units, four new ports, the vertex generators and contour quads

Six accepted units were recovered from the PAL banks, and their North
American and Japanese copies differ only in vertical screen constants. Each
PAL value relates to its NTSC one as 256 to 240 lines:

| Unit | NTSC (default) | PAL (`VERSION_EUROPE`) |
|---|---|---|
| `screen_draw.c` | fullscreen height 240 | 256 |
| `cross_lines.c` | line end Y 240, plus the error texts below | 256 |
| `effect_5.c` | rising start height -98 | -106 |
| `effect_6.c` | start height `y * 98`, target spread `* 12` | `* 106`, `* 13` |
| `effect_7.c` | trail spread `y * 12`, target height `* 98` | `* 13`, `* 106` |
| `effect_18.c` | ring height 98 | 106 |

These follow the `gather_effect.c` pattern: the shared source takes the NTSC
value by default, and `src/overlays/european/duel_effects/<unit>.c` defines
`VERSION_EUROPE` (and `GRAPHICS_DEFAULT_HEIGHT` 256 for the two screen-size
units) before including it. The French, Spanish and European banks now
register the wrappers, and the North American and Japanese banks register the
shared sources. The multiplier changes in effects 6 and 7 and the extra block
in `cross_lines.c` alter the instruction shape, so those three units have no
relocation-masked occurrence and were registered by hand, with their data
addresses taken from the aligned instructions.

The NTSC `func_8014E35C` also reports an effect error after drawing the two
lines: for a non-zero mode it prints `"Error:In Effect ...\n"`, and for mode 1
it adds `"Invalid ID. Expected ID is more small. Exit Effect Sequence.\n"`.
The PAL banks carry neither the call nor the texts. The texts sit in the bank
header as `gNorthAmerican_D_80146014` and `gNorthAmerican_D_8014602C`, which
are North American `0x80146014`/`0x8014602C` and Japanese
`0x80154158`/`0x80154170`.

`effect_13.c` (#6582), `effect_23.c` (#6583), `effect_11.c` (#6593) and
`effect_17.c` (#6596) match the North American, Japanese and European banks
unchanged. `effect_17.c` is the North American `func_80148BA4` at
`0x80158E84`. `effect_11.c` is
the North American `func_80146760` at `0x801467B8`. `effect_7.c` now matches
the European bank as well.

The Japanese and European banks register `func_8014EA7C` and `func_8014EF2C`
from `ring_vertices.c` and `radial_random_vectors.c`, as North America already
does. That adds `func_8014EB1C` and `func_8014EE0C`. With no remaining users,
`circle_vertices.c` and `random_vectors.c`, which defined the two functions a
second time, are removed.

`layered_drawing.c` held `func_801558F4`, `func_80155BC0` and
`func_80155D90`. The French, Spanish, European and Japanese banks keep them
contiguous, but the North American bank places `func_801558F4` at
`0x8015616C`, before `func_80157794`, and the other two at `0x80147E08`,
directly before the `drawing_tail.c` group. `func_801558F4` therefore moves
unchanged into `contour_quads.c`. Every bank now registers two adjacent units
where it had one, and the North American bank gains all three functions.

| Bank | C functions | Bytes | Generated |
|---|---|---|---:|
| North American | 65 → **80** | **65,024** | 5 |
| Japanese | 66 → **80** | **65,024** | 5 |
| European | 73 → **80** | **64,972** | 5 |

## North American effects 16/20 and the projected-number renderer

Two more accepted Spanish units occur exactly once in the North American bank
and match it unchanged:

| Unit | North American address | Image |
|---|---|---|
| `effect_16.c` (#6602) | `func_80151558` at `0x80152130` | `0xC130` |
| `number_renderer.c` (#6599) | `func_801566D4` at `0x8014891C` | `0x291C` |

Their data names are bound to the North American addresses decoded from the
same instructions. `D_80146198` is at `0x801461E0`, `D_8015AEF4` at
`0x8015AFC0` and `D_8015B3C0` at `0x8015A5A0`.

The North American bank now has **82 C functions / 68,848 bytes** of 85.
Three remain generated assembly: `func_8014FABC`, `func_8014A8E4` (effect 22)
and `func_8014D3E8`.

## North American effect 24

`effect_24.c` (#6584, the Exodia burst) occurs exactly once in the North
American bank, at image `0x8050` (`func_8014D3E8` at `0x8014E050`), and matches
it unchanged. Its header vector `D_80146148` is bound to `0x80146190`, and the
resident `DisplayObject_CopyWorkSlots` to `0x8002CB50`, both decoded from the
same instructions.

The North American bank now has **83 C functions / 72,804 bytes** of 85. The
two still generated are `func_8014FABC` and `func_8014A8E4` (effect 22).

## North American bolt vertices

`func_8014FABC` (image `0xA694`, `0x80150694`, `0x344`) builds the jagged vertex
column that effects 13 and 17 draw. It is now `bolt_vertices.c`, used only by
the North American bank; the other regions keep their generated assembly for
it. Three spellings carry the match:

- The angle for `vx` is named (`n = 2048 / count * i;`).
- `csin` is called inside the condition of each multiply's ternary,
  `a * ((s = csin(n), x < 0) ? (s = -s) : s)`. gcc 2.8's `preexpand_calls`
  stops at a `COND_EXPR`, so the multiplicand is sign-extended into a
  callee-saved register before the call, as in retail. The ternary also ends
  the CSE path, so the second angle is recomputed.
- The `vy`/`vz` element is addressed through one pointer written index-first,
  `(SVECTOR *)(i * sizeof(SVECTOR) + (s32)vertices)`, to get retail's
  `addu s1,v1,t0`.

The North American bank now has **84 C functions / 73,640 bytes** of 85. The
one still generated is `func_8014A8E4` (effect 22).

Effect 22 needs no North American source of its own: the ritual effect matched
from the Spanish bank (`effect_22.c`) compiles to the North American bytes at
`0x8014B3BC` unchanged, with its compiler-owned jump table at `0x8014609C`
(module offset `0x9C`) and the data after it at `0x80146180`. The bank is now
**85 C functions / 81,856 bytes** of 85, all of it C.

The Japanese bank's last five are the same shared units at the Japanese
addresses: effect 22 (`0x8015893C`, with its jump table at module offset
`0x54` and the data after it at `0x138`, as in the Spanish bank), effect 24
(`0x8015B440`), `bolt_vertices` (`0x8015DB50`), effect 16 (`0x8015F5EC`) and
`number_renderer` (`0x80164764`). Each compiles to the Japanese bytes
unchanged, so the Japanese bank is also **85 C functions / 81,856 bytes** of 85.
The European (English) bank's same five, at the Spanish addresses
(`0x8014A8E4`, `0x8014D3E8`, `0x8014FABC`, `0x80151558`, `0x801566D4`, with
effect 22's jump table at `0x54`), also compile unchanged: **85 C functions /
81,804 bytes** of 85.
