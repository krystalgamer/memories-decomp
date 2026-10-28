# French MODEL image 132118 swarm helper

`func_8017C2EC` (image offset `0x12EC`, 0x5CC bytes) in the French and
Spanish `model_image_model_132118_8017b000` images draws three groups of up
to 32 growing `POLY_FT4` quads. The slot-0 `model_image_model_132108_8013b000`
images carry the same body as `func_8013C2EC`; only absolute in-body jump
targets differ.

- **Relative.** It is the [header-429 swarm](french-model-image429-swarm.md)
  body widened to 32 quads per group and wrapped in an outer loop over three
  groups (stride `0x714`). Each group has its own origin matrix and
  per-quad direction row. The ring radius is `512`, growth is `step * 8`, and
  the last done quad of a group moves phase 2 to 5. The view lives in
  `french_model_variant/image132118_swarm.h`.
- **Register split.** `polygon` is taken before the `ot` call and `group`
  is set before the loop, which matches the target's stack slots and keeps
  `group` in `s4` and the state copy in `s5`. The unscaled colour branch
  reads blue through a `group->color` pointer copy; decomp-permuter found
  that shape, which also stops GCC hoisting the group-indexed offsets.
- **Slot 0.** `image132118_swarm_slot0.c` renames the function and includes
  the slot-1 source.
- **Bindings.** Calls use the current resident layout, so both regions reuse
  their `model_variant472_linker_symbols.txt`.

All four images rebuild exactly. The
[attempt ledger](french-model-image132118-swarm-attempts.csv) records the
probes.
