# Duel-effect texture tiles and projected strips

The contiguous Spanish bank interval `0x80155F94..0x80156448` is owned by
`src/overlays/duel_effects/drawing_tail.c`, in definition order, with the
unchanged `gcc_2_8_1_g0_split` profile (GCC 2.8.1 / MASPSX 2.81).

| Function | Bytes | Recovered behavior |
|---|---:|---|
| `func_80155F94` | 208 | Build a size-64 FT4 with half-intensity input color and a fixed texture tile. |
| `func_80156064` | 264 | Select one of four texture tiles using two signed `rand() % 2` results, then submit the supplied vertices and depth. |
| `func_8015616C` | 732 | Project and draw both sides of a 32-segment strip between three vertex arrays. |

The first two routines matched their complete instruction ranges on the first
candidate. For the third, three distinct source experiments were retained in
the local attempt ledger:

1. Incrementing `depth` in place produced the correct 732 bytes but four
   register-choice mismatches at relative offsets `0x188`, `0x1A0`,
   `0x24C` and `0x264`: the increment overwrote `$a2` rather than producing
   the target's temporary `$v0`.
2. Combining `depth + 1 - mode` in the nonzero-mode branch still produced
   732 bytes and four mismatches at those offsets. GCC rewrote the operation
   as `depth - (mode - 1)`, changing both instruction operands.
3. A separate `adjusted = depth + 1` before the mode branch, followed by
   `depth = adjusted - mode`, reproduced all four instructions exactly.

Both depth and projection flag must be nonnegative. Mode zero uses the
existing GT4 packet helper. Nonzero mode clamps the adjusted depth to zero,
shifts it by two, truncates the priority to `u16`, and passes the signed mode
to the resident submitter as flags. The two projection blocks and signed
modulo operations are retained rather than simplified.

The texture-prefix view exposes the observed page/CLUT pairs at offsets
`0x04/0x06`, `0x08/0x0A` and `0x0C/0x0E`. Together with the independently
observed `0x00/0x02` pair, these occupy the first sixteen bytes; the remaining
`0x18` unknown bytes preserve the accepted `0x28..0x2E` fields. This is a view
of generated retail data, not a new C-owned definition or linker alias.
Canonical packet, texture, SDK random and projection declarations are reused.

A private full-bank link first verified all three routines alongside the
accepted 21-function baseline, then again alongside all 37 accepted functions:
40 C functions / 7,564 bytes, preserving every accepted entry. The complete
90,112-byte Spanish bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production acceptance additionally checks all terrain copies, every compiled
object extent, final function address/size/executable ownership, actual local
callees and generated texture/ordering-table data. The other 45 provisional
boundaries remain assembly; no unreviewed runtime coverage is claimed.
