# French header-449 image bands helper

The `0x75C` helper at image offset `0x2458` is first-ever game code: no release
had matching C for its body. Four French images carry it: MODEL715 stages 9/10
(`169940`, `169950`) and MODEL370 stages 7/8 (`88500`, `88510`). Slot 0 owns
`func_8013D458` from `variant449_bands.c`; slot 1 owns `func_8017D458` through
a wrapper that only renames the function.

One `0xB4`-byte band sits at work offset `E88`, directly after the accepted
sheet at `E00`. It holds three points per row (`a`, `b`, `c`), their projected
words (`sa`, `sb`, `sc`), a signed drift word per point at `6C`, two colour
rows at `78/84`, and three depths at `A8`. The view reuses the measured
header-449 fields (second `POLY_GT4` at `1A24`, origin at `1B14`, factor at
`1B90`, scale at `1B98`, delta at `1BB0`, flags at `1BDC`) and adds the
spread word at `1B9C`, the half-word axis pair at `1BC0`, and a half-word
direction at `1C4C`.

Each point uses the MODEL422 band shape at radius 128. The transform adds
`delta * progress / 1024` to the origin, where progress is the factor
clamped to `0..1024`; point 0 also adds `scale / 16 * spread / 1024`. The
`z` translation gains the signed drift scaled by `scale / 4096`, and the scale
is `scale` plus a flag-selected `scale / 8` bias. `RotTransPers3` projects the
three points. The draw loop emits two `POLY_GT4` faces per segment through the
second packet. The first segment uses the forward orientation; the second is
mirrored. A face is submitted only when its depth is positive.

Exactness depended on two source details. The first draw arm indexes `[j]`
and `[j + 1]`, which keeps the retail `band + 84` / `band + 4` induction
bases. The factor clamp is written as a `<= 0` / `< 1024` / otherwise chain,
which places the zero move in the retail branch delay slot. The model
records one band; the loop bound and stride follow the target.

Bindings for `RotTransPers3`, `rcos` and `rsin` are added to the shared
header-449 linker file; every binding is a French resident function.

The opt-in `MODEL_VARIANT437_BANDS` view changes only the initial padding for
the [MODEL103 stage 9/10 reuse](french-model-variant437-bands.md). Without the
flag, the body and all MODEL449 offsets above remain unchanged. The complete
French production gate replays all four accepted donor images; the attempt
ledger appends the updated shared-header fingerprint without altering history.
