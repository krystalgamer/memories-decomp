# French MODEL474 entry

The loader entries `func_8013B004` / `func_8017B004`, at offset `0x4` in
MODEL121 stages 7/8 and MODEL431 stages 9/10, now use pure C with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81). Each entry preserves
all 3,636 instruction bytes. The slot-1 wrapper only renames the entry, its two
helpers and the configuration table. The byte-identical Spanish copies of the
same four images use the same sources.

## Measured context

| Offset | Initialized family |
|---|---|
| `0x0000` | Two `0x118` funnels, inner/outer rows of seventeen points |
| `0x0230` | One `0x4AC` ribbon |
| `0x06DC` | A 9x17 point mesh followed by nine colour rows |
| `0x0BC8` | Twelve `0x7C` spirals |
| `0x1198` | Two `0x98` sheets |
| `0x12C8` | Two `0x334` streamers |

Packet templates span `0x1964..0x1B2C`: a glow `POLY_GT4`, two bar
`POLY_GT4`s, a `POLY_G3`, a `POLY_G4`, two textured `POLY_GT4`s and two flat
`POLY_FT4`s. The part matrix is at `0x1B3C`, the opposite-slot target at
`0x1B5C`, the direction at `0x1B64`, screen delta at `0x1B74`, view delta at
`0x1B78`, the two view angles at `0x1B88`, then frame counters, the fade word,
the configuration row pointer and the part. The phase word is at `0x1C1C`,
the camera vector at `0x1C28`, the tint at `0x1C30`, the slot at `0x1C34` and
the command at `0x1C36`.

Configuration rows are `0x34` bytes at `D_8013E980 + 0x1C`: the part byte at
`+0x10`, the trigger at `+0x18` and the fade window at `+0x20`/`+0x24`. The
five texture images sit immediately before the table. The old
`D_8013E8BC` symbol is split at `0x8013E980` so the source can address the
table directly.

## Source shape

The setup keeps the third texture's packed word in a short-lived `packed`
that the next upload reassigns, and copies its CLUT into a halfword
`flat_texture`; otherwise the shift crosses the next call and is scheduled
late. The mesh angle is a never-assigned zero-valued `angle`, and the texture
calls pass a literal zero so reload CSE does not hoist it into a saved register.

The funnel size store sits in its own `do { } while (0)` before the counter
increment: the loop notes stop the scheduler from hoisting the increment above
the store. The spiral colour test uses the inner counter, and the camera preset
call runs after both the initialization and update branches.

The header-474 linker bindings gain the GPU, `Square0`, `SquareRoot0` and
`GsGetLwUnit` names this entry calls, at their existing resident addresses.
There are no register pins, inline assembly or volatile workarounds.

## Acceptance

All four French and four Spanish images rebuild byte-identically with the
strict build-then-verify path. The attempts ledger records the intermediate
probes and the image hashes.
