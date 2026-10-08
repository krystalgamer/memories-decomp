# French header-383 image rays helper

`func_8013D8FC` / `func_8017D8FC` (image offset `0x28FC`, 0xB6C bytes)
builds, projects and draws sixteen rays.
**No release had matching C for this body before.**

The body appears in the two French raw images of model 363 that also hold
the header-383 strip, ribbons and quads helpers.

The code follows the French header-373 rays helper (`variant373_rays.c`) over
the header-383 layout (rays at `+0xB74`, quad at `+0x138C`), with these
differences:
- **Clip flags:** each ray segment keeps its own flag (`flag[16][2]`), the
  edge projection uses a separate flag, and quads are sorted when both the
  depth and that flag are non-negative.
- **Scale:** 4352 (pulse) or 3840.
- **Angle rates:** advance by `step * 12` / `step * 24` from phase 7, and by
  `step * 8` / `step * 16` before.

The [attempt ledger](french-model-image383-rays-attempts.csv) records three
measured probes.
