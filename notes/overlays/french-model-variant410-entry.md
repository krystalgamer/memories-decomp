# French MODEL410 entry

The loader entries `func_8013B004` / `func_8017B004`, at offset `0x4` in
MODEL54 stages 7/8, now use pure C with the existing `gcc_2_8_1_g0_split`
profile (GCC 2.8.1 and MASPSX 2.81). Each entry preserves all 2,728
instruction bytes and its 192-byte frame. The slot-1 wrapper only renames the
entry, its two called helpers and the data block. The three accepted helper
bodies (quads, ribbons and rings) and their registrations are unchanged.

The same entry also appears in the raw `model_image_model_15084` /
`model_image_model_15094` images of the other European releases. Those images
have no symbol files or matched helpers yet, so they keep their assembly.

## Measured context

| Offset | Initialized family |
|---|---|
| `0x0000` | One `0x2FC` quad record: four rows of ten corners, ten positions, a colour and five ten-word arrays |
| `0x02FC` | Nine `0x31C` ribbons |
| `0x1EF8` | Two `0x98` rings |

Packet templates follow: a `POLY_FT4` quad at `0x2028`, a `POLY_G3`, a
`POLY_G4`, two `POLY_GT4`s at `0x2090` and two `POLY_FT4`s at `0x2110`. The
part matrix is at `0x2170`, the target at `0x2190`, the direction at `0x2198`,
the screen delta at `0x21A8`, the view delta at `0x21AC`, the view angles at
`0x21BC`, then the frame counters, fade, configuration row and part. The phase
is at `0x21F0`, the tint at `0x21FC`, the slot at `0x2200` and the command at
`0x2202`.

Configuration rows are `0x10` bytes at `D_8013CD04 + 0xFC`: the part byte at
`+4` and the end frame at `+0xC`. The five texture images precede the table,
as in header 337.

## Source shape

The setup follows header 337, but only the first upload's packed word is kept:
its page and CLUT are copied out before the next uploads reassign `packed`, so
the CLUT copy survives as its own spilled halfword and the page takes the freed
textured-packet register. The quad packet uploads the last image again.

Each ribbon's progress row starts from a separate `base` that drops by 512 per
ribbon, which keeps the reversed inner loop and the retail increment order.
The progress loop uses the ring-loop index, and the ring loop reuses the quad
loop's inner counter; these choices reproduce the retail temporaries. The
update calls the ribbons helper from phase 2 and the quads helper from phase 1.
There are no register pins, inline assembly or volatile workarounds.

## Acceptance

Both French images rebuild byte-identically with the strict build-then-verify
path. The attempts ledger records the intermediate probes and image hashes.
