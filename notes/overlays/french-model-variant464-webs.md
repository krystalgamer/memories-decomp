# French MODEL464 webs helper

`func_8013D320` / `func_8017D320` (MODEL464 family, image offset `0x2320`,
0x518 bytes) draws three line webs, each with 4×6 `GsGLINE` segments.

It is the header-422 webs body already used by the French MODEL439 and the
Spanish/North American header-422 images. Only the work-state layout differs:
- the line packet and all state words (`+0x18D0` to `+0x194C` in header 422)
  sit **0x2E0 bytes later**;
- the phase word (`+0x197C` in header 422) sits **0x2E4 bytes later**, at
  `+0x1C60`.

The raw comparison shows no other change apart from local jump targets. The shared
`model_variant/variant422_webs.c` now spells those displacements through
`MODEL_VARIANT_WEBS_SHIFT` and `MODEL_VARIANT_WEBS_PHASE_SHIFT`. Both default
to zero, so the existing header-422 objects are unchanged. The French
`variant464_webs.c` wrapper and its slot-one form set them to `0x2E0` and
`0x2E4`.

The helper is registered in all six French images that carry it: models 175
and 244 at stages 7/8, and model 182 at stages 9/10. `GsSortGLine` was added to
the French MODEL464 binding file at its resident address `0x800840B8`.
All French, Spanish and North American overlay images rebuild exactly. The
[attempt ledger](french-model-variant464-webs-attempts.csv) records the
comparison and the exact result.
