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
