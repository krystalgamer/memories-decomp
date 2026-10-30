# French MODEL397 beams

The `0xCB8` helper `func_8013BDE4`, at offset `0xDE4` in MODEL21 stage 7 and
MODEL258 stage 9 (both slot 0), now uses pure C with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81). It preserves all
3,256 instruction bytes and its 328-byte frame. With the streamers helper
(see [the streamers note](french-model-variant397-streamers.md)), every
function in both images is now C.

## Measured layout and behaviour

The work area matches the North American header-380 entry's view: five
`0x2D4`-byte beams, five sheets at `0xE24`, six `POLY_FT4` packets at
`0x18C8`, five matrices at `0x19C8`, directions at `0x1AC0`, and per-record
halfword length, mode and done counters at `0x1B70`, `0x1B84` and `0x1B8E`.
Each beam holds a thirteen-point spine and its view-shifted copy, their
projections, and per point the angle, width, projection flag (`0x238`),
depth (`0x26C`) and screen offset (`0x2A0`/`0x2BA`); its colour is at `0x1A0`.

For every record the helper coils the spine from the record's matrix along its
direction. The twist alternates around 1024 by record, and each inner point
bends with a wave that advances by 1300 per point. The copy is shifted along
the view by `width * 40 / 1024`. Both are projected, and the
helper draws `length` strips from the packet pair chosen by the record's mode
(3 and 4 select the second and third pair; odd strips use the pair's second
packet). Modes of 5 and above stop drawing.

In phases 1 and 3 or later, the length grows by the frame step up to 12. That
clears the sheet's shown flag and moves phase 3 to 4. A full-length record's
mode then advances. Past the row's frame thresholds, a finished record is held
at length 12, mode 5 and done; otherwise it restarts. When every record is
done, phase 1 moves to 2 and phase 4 to 5. Phase 2 staggers the lengths by
eight per record and moves to 3 after the row's hide frame. The two wave
offsets advance by the frame step shifted left by 7 and by 5.

## Source shape

The body follows header 376's ribbon. The view shift is an `s16` local, and
each branch of the last-point test computes its own width offset. The done
counter is cleared after the discarded second `ratan2`, which keeps the first
loop's entry test on that counter. The sheet pointer is taken from the state
pointer before the first call. There are no register pins, inline assembly or
volatile workarounds.

## Acceptance

Both French images rebuild byte-identically with the strict build-then-verify
path. The attempts ledger records the intermediate probes and image hashes.
