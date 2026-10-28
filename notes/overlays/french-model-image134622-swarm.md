# French MODEL image 134622 swarm helper

`func_8017C490` (image offset `0x1490`, 0x6EC bytes) in the French and
Spanish `model_image_model_134622_8017b000` images draws up to sixteen
growing `POLY_FT4` quads. The loader entry reaches it.

- **Relative.** It belongs to the
  [header-455 swarm](french-model-image455-swarm.md) family, but with
  sixteen entries and a `fade` array before the origin matrices. In phase 4
  and later, the colour scales by the clamped fade, and each fade then counts
  down by `step * 8`. The last fade to reach zero moves phase 4 to 5. A
  drift vector, scaled by a value that grows from phase 3, offsets every
  quad, and a spin angle pulses the size. The view lives in
  `french_model_variant/image134622_swarm.h`.
- **Fade temporary.** The fade clamp is tested through a second local, and
  that same local carries `step * 8` for the decrement. Only then does GCC
  keep the target's `lw v0; move a1, v0` copy. decomp-permuter found this
  shape.
- **Bindings.** Calls use the current resident layout, so both regions reuse
  their `model_variant472_linker_symbols.txt`.

Both images rebuild exactly. The
[attempt ledger](french-model-image134622-swarm-attempts.csv) records the
probes.

## Opposite-slot image 134612

The French `model_image_model_134612_8013b000` helper `func_8013C490`
has the same `0x1490` offset and `0x6EC` size. Its original body differs
from image 134622 at only two internal absolute jumps, at helper offsets
`0xCC` and `0x538`, whose destinations move down by `0x40000`.

`image134612_swarm.c` remaps only the function name and includes the
accepted image-134622 source. The local declarations, expressions and
`gcc_2_8_1_g0_split` profile are unchanged. The slot-0 compilation is
exact; the [reuse ledger](french-model-image134612-swarm-attempts.csv)
records the native probe and complete-image checks.

Both French 20,480-byte images were rebuilt before and after the
registration. Image 134612 retains SHA-256
`eaad64467eca27fec6f107312fc2964d8cd9673c29e768899cfdaaef8ce81ccb`;
image 134622 retains
`a5e5b11a69a8b885a815f2935c0e17d94e310a1f4766a95086429537c0f585c3`.
The original loader-entry assembly and raw tail of image 134612 remain
unchanged, as do the existing French and Spanish image-134622 owners.
The Spanish `model_image_model_134612_8013b000` image (SLES-03951) is
byte-identical and registers the same `image134612_swarm.c` wrapper
with the same profile and region bindings. Its complete image rebuilt
from `game/spain/DATA/MODEL.MRG` keeps SHA-256
`eaad64467eca27fec6f107312fc2964d8cd9673c29e768899cfdaaef8ce81ccb`.
