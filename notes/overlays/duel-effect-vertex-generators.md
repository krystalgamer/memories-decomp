# Spanish duel-bank line and vertex generators

These three complete functions use `gcc_2_8_1_g0_split`, the established
GCC 2.8.1 / MASPSX 2.81 pipeline, and independently recovered SDK layouts.

| Address | Bytes | Source | Observed behavior |
| --- | ---: | --- | --- |
| `8014E35C` | 144 | `cross_lines.c` | Two white `GsLINE` diagonals from the top corners, attribute `0x50000000`, priority zero |
| `8014EA7C` | 160 | `ring_vertices.c` | 32 XY circle vertices, angle step 128, zero Z |
| `8014EF2C` | 228 | `radial_random_vectors.c` | Three signed random differences modulo 4096 per vertex |

`func_8014EA7C` and `func_8014EF2C` were first recovered as the single-function
sources `circle_vertices.c` and `random_vectors.c`. Every bank now registers
them from the whole `ring_vertices.c` and `radial_random_vectors.c` units,
which also hold `func_8014EB1C` and `func_8014EE0C`.

The line routine reuses one stack packet. The second call keeps the existing
attribute/RGB values, reverses the X endpoints, and preserves the common
256-pixel endpoint Y. Both calls use the real module ordering-table pointer.

The circle generator uses canonical `ccos`/`csin`, not `rcos`/`rsin`: the
resident bindings are respectively `800868A8` and `80086B38`.
`GsSortLine` is at `80083F38`. All three bindings already exist in the
authoritative Spanish resident linker metadata, and their inventory rows
remain SDK assembly. This change does not rename or count resident SDK code.

Both vector routines write only X/Y/Z halfwords of the eight-byte SDK
`SVECTOR`; padding is untouched. Circle scaling uses signed division by
4096, including negative-result correction before shifting. Random vectors
preserve six calls per iteration and C's signed remainder, rather than
masking negative values. Zero random-vector count performs no calls.

## Matching evidence and deliberately excluded candidates

All three bodies matched their full instruction extents on the first source
experiment. An independent linker proof then compiled them as three separate
translation units, preserving their noncontiguous locations and every
accepted function. The entire 90,112-byte bank reproduced SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Each new function has its exact object extent and real executable-section
ownership in the linked ELF.

The independent baseline is accepted `9badfb1e0`: all 51 existing entries
remain unchanged. The addition is three functions / 532 bytes, reaching
54/85 bank functions / 12,884 bytes and 178/209 configured overlay instances /
68,736 bytes. The other 31 bank boundaries remain explicit assembly.
Boot and other dynamic runtime coverage are still separate, unresolved work.

Nearby `8014EB1C`, `8014EC8C`, `8014EE0C` and `8014F490` were investigated
alongside these bodies but are not promoted. Experiments included SDK vector
expressions, independent cursors, per-iteration views, loop-update order,
halfword parameter/product variants, coordinate temporaries and the named
no-follow-jumps profile. The best polygon candidate still differs in three
pointer-base instructions; the best quad candidate still swaps two saved
registers. Correct size or semantic equivalence alone is not acceptance.
Sources and complete per-attempt diagnostics remain private under `tmp/`.
