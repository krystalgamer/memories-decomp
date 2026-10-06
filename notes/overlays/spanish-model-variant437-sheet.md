# Spanish MODEL437 growing sheet

Helper `+0x14A8..+0x1B08` is independently reconstructed matching C:
1,632 bytes, two physical instances, 3,264 instruction bytes, and one unique
routine. An independent second-slot compilation also matches. Screening 6,189
accepted regional C entries found no same-sized body; this is not a French or
other regional C port.

## Physical loads and ownership

An exhaustive scan identifies MODEL103, record 103, stages 9/10, sectors
28628/28638, with headers 437/587 and distinct complete-image hashes.
Command 603000 selects the 48-byte descriptor at `+0x302C`.

Five closed contiguous functions begin at `+4`, `+0xD4C`, `+0x14A8`,
`+0x1B08`, and `+0x25D4`; raw data begins at `+0x2F30`.
The functions at `+0xD4C` and `+0x1B08` are not entry-reachable. Only the
selected helper becomes C, leaving eight ASM instances, including all four
orphan instances.

## Geometry and phases

Entry `+0xBF4` passes the original context captured in `s2` at `+0xC`. The
entry later reuses `s2` as a byte constant, but only the `+0xC` definition
reaches this call. The call follows `+0x25D4` while the current time lies
within the descriptor's unsigned `+0xC..+0x10` window.

One 136-byte sheet at context `+0x800` holds four corner arrays and outer and
inner colors. It is drawn as four quads through the second initialized
`POLY_GT4` at `+0x1424`. The constructor owns the packet setup. The helper
writes RGB values and reversed XY output pairs only.

Two `ratan2` results are computed and discarded, as in the target. Translation
is origin plus path times the signed progress `/ 1024`. Before phase six the
scale is size plus a pulse of one eighth of the signed size on odd frames. Three
vertices use the inner color and one the outer color. From phase six both colors
fade by `(32768 - size) / 16384` and the pulse is dropped. Depth is scaled by
nine tenths, then nonnegative depth and flag gate 16-bit narrowed submission.

The phase machine uses unsigned descriptor times. Phase zero or one grows size
to 4096 over `+0x10..+0x14`, then sets phase two. Phase two grows to 5120 over
`+0x14..+0x1C`, then sets phase four. Phase four moves progress to 1024 over
`+0x1C..+0x20`. Phase five adds `step * 128` up to 16384. Phase six adds
`step * 1024` up to 32767, then sets phase seven.

## Stack frame

The target frame is 288 bytes. Every non-stack instruction already matched
with the natural 272-byte frame; the only remaining differences were 40 SP
displacements of 16 bytes, starting directly after the 80-byte coordinate.
Following the accepted repository convention for observed stack gaps (see
[Exodia helpers](exodia-helpers.md) and `variant432_draw.c`), an unused 16-byte
`VECTOR` local declared after the coordinate retains that gap. No runtime role
is asserted for it.

## Acceptance evidence

Both complete 20KiB images match using `gcc_2_8_1_g0_split`, the authoritative
GCC 2.8.1/MASPSX 2.81 pipeline. No forced registers or inline assembly are
present. The six-row ledger preserves both rejected experiments, the source and
second-slot compilations, and both terminal full-image results.

Regressions cover 37 layouts, repeated header inclusion, all physical loads
and descriptors, original-context reaching definitions, packet placement,
every selected relocation, all input/final function and data owners, and 35
resident bindings with loader/context ownership. The private state is only the
observed prefix through `+0x1640`; its stored pointer uses `*G32`.

Terminal dependency fingerprints hash the body, private header, shared
model-variant header, and `gpu_packets.h`, in that order.
