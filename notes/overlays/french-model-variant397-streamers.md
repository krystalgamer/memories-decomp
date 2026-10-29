# French MODEL397 streamers

The `0x8E4` helper `func_8013D06C`, at offset `0x206C` in MODEL21 stage 7 and
MODEL258 stage 9 (both slot 0), now uses pure C with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81). It preserves all
2,276 instruction bytes and its 352-byte frame. The header-397 entry and sheets
helpers are unchanged; the `0xDE4` helper is covered by
[the beams note](french-model-variant397-beams.md).

## Measured layout and behaviour

Two `0x378`-byte streamers start at `0x1130`. Each holds a seventeen-point
spine and a view-shifted copy, their projections, and per point the screen
angle, width, colour, projection flag at `0x2AC`, depth at `0x2F0` and the
width's screen offset. The `POLY_G4` packet is at `0x18C8`.

For every record counted by the command row (`+0x24`), the helper:

- spreads a size and rotation offset of `record * 4096 / count`, with the
  odd-frame rotation variant as in header 320;
- coils both streamers from that record's base angle (`0x1BC8`) and length
  (`0x1BA0`), then advances the base;
- builds the record's matrix from its position (`0x1A70`) and rotation words
  (`0x1BB4`/`0x1BC8`/`0x1BDC`);
- projects both streamers and draws sixteen strips each. A strip is skipped
  when its depth or projection flag is negative. A long streamer uses widths
  of 2 (last point) or 3 instead of the projected width.

From phase 6 a record's length drops by `step * 64`. When the last record
reaches zero, the phase moves to 7.

## Source shape

The body follows header 320's French streamers. As there, each branch of the
last-point test computes the width offset itself. Cross-jumping merges the two
copies only after reload, and the extra copy is what moves the projection
argument's reload onto the retail register. The radius is a variable that
holds 128 and is rematerialized at each use. The inner counter is cleared before
the streamer and base setup. Typed state arrays keep the retail operand order
of the per-record addresses. There are no register pins, inline assembly or
volatile workarounds.

## Acceptance

Both French images rebuild byte-identically with the strict build-then-verify
path. The attempts ledger records the intermediate probes and image hashes.
