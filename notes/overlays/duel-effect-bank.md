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
| `sorting_helpers.c` | `0x80152EC4..0x80153200` | 4 | 828 |
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
per-vertex colors before calling `0x80152EC4`, confirming `POLY_G4`.
The other wrapper/sorter use `POLY_GT4` with XY offsets 8, 20, 32 and 44.

The sorters apply the two halfword offsets at `D_8015B7F8` and submit
packets through the existing resident and SDK declarations. Their distinct
priority behavior is preserved: the Gouraud sorter adds one, the opaque
flat-textured path does not, and the opaque Gouraud-textured path adds one.
The default matrix initializer keeps the original coefficient-store order,
unit diagonal, zero X/Y translation and Z translation 300 without touching
padding. `D_8015B7F8` and the halfword priority at `D_8015B800` remain
original raw module data, with declarations only and no absolute override.

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
