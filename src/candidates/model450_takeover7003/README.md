# MODEL450 NTSC streamer candidate

This directory retains only the best available clean source candidate for
`func_8013E0A8`, the `0x834`/2,100-byte Japanese/North-American streamer
target. The rejected PR source with an artificial self-store and
byte-identical `if (poly)` arms is not included.

`streamer02.c` removes both rejected constructs and compiles to 2,084 bytes
against the target, with 383 position-wise relocation-masked differences
under `gcc_2_8_1_g0_split`. It is not exact or promotable. The private local
header records independently checked `0x378` candidate-layout correspondence
to the accepted French coil view; it does not make that view a proven NTSC
target type.

No quad/line or alternate streamer source forms are retained here. The
independently reviewed Japanese quad/line integration remains in #7003.
French `variant450_coils.c` is structural evidence only, not a direct donor:
its target differs substantially from the Japanese/NTSC streamer body.
Resume only with genuine source recovery under the current GCC 2.8.1/MASPSX
2.81 policy; do not use historical compiler profiles or allocation-control
artifacts.
