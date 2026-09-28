# French duel-effect geometry and rendering

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
