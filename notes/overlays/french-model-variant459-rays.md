# French MODEL459 ray helper

`SpanishModelVariant459_DrawRays` (`0x8013CFB8` / `0x8017CFB8`, image offset `0x1FB8`, 0x64C bytes) is the
MODEL459 member of the "rays" helper family. **No release had matching C for
this body before.** In every image it is a closed stack-prologue function, and
no direct call from the entry was observed.

The accepted MODEL417 ray helper has the same body. A local MODEL459 view,
`Rays459State`, reproduced it exactly on the first probe; since #9563 the
helper takes `SpanishVariant459EntryState` directly. The layout differs from
MODEL417 in the following:
- the 0x6C-byte `rays` start at `+0x1758`, the pulse (`sheets[0].size`) at
  `+0x2568` and the `POLY_G3` (`packets.triangle`) at `+0x2D20`;
- origin, direction and direction_b (`matrices[0].t`, `aim.direction`,
  `aim.view_delta`) are at `+0x2EE0`/`+0x3054`/`+0x3068`;
- frame, step and timing (`clock.frame_count`, `clock.step`, `config`) are at
  `+0x3080`/`+0x308C`/`+0x309C`;
- `index` is a halfword at `+0x30BC`, `progress` a word at `+0x30D0`, the
  angle (`ray_angle`) is at `+0x30D8` and the mirror flag (`slot`) at
  `+0x30F0`;
- the ray count (`config->count`) is at `+0x24`.

It is registered in all 11 French MODEL459 layouts that contain the function
(12 physical images; models 47, 231, 238, 411, 417 and 620). `rcos`, `rsin`
and `RotTransPers` were added to the French MODEL459 binding file at French
resident functions. All French overlay images rebuilt exactly. The
[attempt ledger](french-model-variant459-rays-attempts.csv) records the
experiment.

The identical Spanish, German and Italian images can reuse this C without
decompiling the function again.
