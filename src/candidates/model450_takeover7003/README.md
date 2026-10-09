# MODEL450 takeover probes

These C files preserve bounded source-form experiments from the stale
Japanese/North-American MODEL450 takeover. They are not the reviewed PR
source and are not exact matches. In particular, the rejected NTSC streamer
source with the artificial self-store and byte-identical `if (poly)` arms is
not included.

`streamer02.c` removes both rejected constructs and compiles to 2,084 bytes
against the `0x834`/2,100-byte target, with 383 position-wise
relocation-masked differences under `gcc_2_8_1_g0_split`.
`streamer03.c` moves offset stores into the existing terminal/nonterminal
geometry branches; it compiles to 2,104 bytes with 390 such differences.
Neither is exact or promotable. The small local header records independently
checked `0x378` candidate-layout correspondence to the accepted French coil
view; it does not make that view a proven NTSC target type.

The quad/line files are natural initialization-form probes. Their best
bounded variants remain one instruction short: quad objects are 1,480 vs
1,484 bytes, and line objects 900 vs 904 bytes, with 21 and 30 positional
masked differences respectively. Alternate argument ordering, builtins,
zero-first forms, and `-fno-schedule-insns2` did not resolve the differences.
These probes do not replace or invalidate the independently reviewed
Japanese quad/line integration in #7003.

The French `variant450_coils.c` implementation is structural evidence only,
not a direct donor: its target differs substantially from the Japanese/NTSC
streamer body. Resume only with genuine source recovery under the current
GCC 2.8.1/MASPSX 2.81 policy. Preserve the Japanese quad/line work, and do
not use a historical compiler profile or allocation-control artifacts.
