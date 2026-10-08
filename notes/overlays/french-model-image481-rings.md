# French header-481 image rings helper

`func_8013C718` / `func_8017C718` (image offset `0x1718`, 0x638 bytes)
builds, fades and draws the two header-481 rings.
**No release had matching C for this body before.**

The body appears in four French raw images: models 624 and 248, in slot 0
(header `0x1E1`) and slot 1 (header `0x277`). Each raw layout gains its
first C segment at `0x1718`. The new
`model_variant481_linker_symbols.txt` binds only French resident functions.

The state starts with two 0x118-byte rings, each with 17 inner points `a`,
17 outer points `b` and a `scale`:
- **Shape:** while a ring's scale is positive, its points are rebuilt from the
  shared `spin` in 256-unit steps. Inner points sit at radius 64 in the plane.
  Outer points sit at radius 256 and are lifted by `rcos(sweep) * 128`, where
  `sweep` is 2048 for the first ring and 4096 for the second.
- **Placement:** the rotation uses the view angle plus a mode-dependent tilt
  (`-pitch - 768` or `-pitch - 1280`). The translation is `position` minus a
  96-unit wobble from `sweep`. The matrix scale is the ring's scale, plus
  1024 on odd frames.
- **Colour:** above a scale of 6144 the inner and outer colours fade with
  `(8192 - scale) / 2048`.
- **Drawing:** 16 shared POLY_GT4 quads per ring, sorted when the depth and
  flag are non-negative.
- **Growth:** a ring grows by `step * 64`, up to 8192, once the gauge level
  passes 900 for the first ring or 1100 for the second.
- **Spin:** at the end, `spin` advances by `step * 80`.

Three source details are required by the target bytes:
- **Discarded call:** a standalone `rsin(2048)` call whose result is never
  used.
- **Redundant branch:** an `angle < 2048` test whose two arms both compute
  `-angle + 1024`. The branch is present in the retail code; post-reload
  cross-jumping merges the identical tails. Every distinct spelling of the
  arms, and the ternary form, changes the bytes (ledger attempts 12–15
  and 27).
- **Ring-loop setup:** `i` and `sweep` are initialised in the ring loop's
  header. Pre-reload scheduling hoists those stores above the `rsin(2048)`
  call, and reload then copies `sweep` from the argument register.

The [attempt ledger](french-model-image481-rings-attempts.csv) records 27
measured probes.
