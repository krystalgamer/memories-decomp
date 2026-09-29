# Spanish offset-height polygon

`func_8014EC8C` (`8014EC8C..8014EE0C`, 384 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. It creates an X/Z polygon with a constant Y
coordinate of `level - offset`, scaling its unsigned width/depth through
`csin(512)`. Angles retain signed division by the unsigned-halfword count
after integer promotion, followed by an unsigned half-step.

The accepted French `ring_vertices.c` provided useful evidence for the
expression structure and advancing vector cursor. The decisive difference
was the height temporary's lifetime: calculate it after the angle but before
the trigonometric calls. GCC then hoists the invariant subtraction into the
exact loop-entry position and produces the original saved-register allocation.
No forced registers, inline assembly or special compiler flags are used.

All seven new bounded probes are recorded under `tmp/polygon-probe/`:

| Attempt | Change | Bytes | Differing words |
| --- | --- | ---: | ---: |
| 1 | Advancing cursor and accepted half-step expression | 392 | 58 |
| 2 | Explicit height calculation before the loop | 384 | 15 |
| 3 | Guard the pre-loop calculation with nonzero count | 384 | 23 |
| 4 | Signed-halfword coordinate arguments | 392 | 73 |
| 5 | Unsigned-halfword coordinate arguments | 392 | 73 |
| 6 | Signed-word arguments; height local before angle | 384 | 14 |
| 7 | Height local after angle and before calls | 384 | 0 |

The signed-word arguments are retained; narrowing them was not justified by
the target. SDK vector padding is untouched. Zero count still performs the
initial width/depth calculations but skips division and all vertex writes.

The independent complete 90,112-byte bank link reproduces SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
That private preflight also verified two accepted French polygon helpers,
but those source regroupings are deliberately not promoted in this batch.
Only the new complete 384-byte routine is integrated; all previously accepted
Spanish manifest entries remain unchanged. Production image, metadata and
exact object/final-ELF function ownership are checked separately.
