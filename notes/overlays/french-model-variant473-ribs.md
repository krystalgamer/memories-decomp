# French MODEL473 ribs

The `0xC38` helper `func_8013CFB0` / `func_8017CFB0`, at offset `0x1FB0` in
MODEL385 stages 9/10, now uses pure C with the existing `gcc_2_8_1_g0_split`
profile (GCC 2.8.1 and MASPSX 2.81). It preserves all 3,128 instruction bytes.
The slot-1 wrapper only renames the helper. The byte-identical Spanish copies
of both images use the same sources, and the other header-473 sources are
unchanged.

## Measured layout and behaviour

Five `0x22C`-byte ribs start at `0x18D8`. Each holds a nine-point spine and a
view-shifted copy, their projections, the per-point angle, width, depth and
screen offset, two colour rows, and the size, fade and state that drive its
growth. The `POLY_GT4` packets start at `0x39D0`, the part matrix
translation is at `0x3AFC`, and the per-rib anchors are at `0x3B70`.

While a rib's size is positive, its spine grows from the origin toward its
anchor (state below 2), or runs from `fade / 1024` of the anchor to the
anchor (state 2 and above). Each point is bent by a sine of its length around
an angle that alternates around 1024 by rib. The copy is shifted along the
view by the spread. Both are projected, and each segment draws two halves
offset by the projected width; the projection flags live in a local array.
Sizes, fades, states, the two ripples and the phase advance as described in
the source comment.

## Source shape

The body follows header 479's ribs. Three choices reproduce retail register
use:

- The record loop's increment recomputes the odd-rib angle, a dead value that
  makes loop optimization hoist the `/ 5` constant as retail does.
- An empty loop before the projection deltas keeps the retail projection
  addresses and schedule.
- The per-point build locals are declared inside the build loop, which gives
  the hoisted constant the first spill slot.
- Each spine point is stored component by component: the first store's
  address makes the index copy `k = n`, the later stores index with `k`, and
  the z term reads `i`. Writing the copy inside a `setVector` argument would
  be unsequenced, because the macro repeats its pointer argument.

There are no register pins, inline assembly or volatile workarounds.

## Acceptance

All four French and Spanish images rebuild byte-identically with the strict
build-then-verify path. The attempts ledger records the intermediate probes
and image hashes.
