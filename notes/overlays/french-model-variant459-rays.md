# French MODEL459 ray helper

`func_8013CFB8` / `func_8017CFB8` (image offset `0x1FB8`, 0x64C bytes) is the
MODEL459 member of the "rays" helper family. **No release had matching C for
this body before.** In every image it is a closed stack-prologue function, and
no direct call from the entry was observed.

The accepted MODEL417 ray helper has the same body. A local MODEL459 header
reproduces it exactly on the first probe. The header differs in the following:
- the 0x6C-byte rays start at `+0x1758`, the pulse at `+0x2568` and the
  `POLY_G3` at `+0x2D20`;
- origin, direction and direction_b are at `+0x2EE0`/`+0x3054`/`+0x3068`;
- frame, step and timing are at `+0x3080`/`+0x308C`/`+0x309C`;
- `index` is a halfword at `+0x30BC`, `progress` a word at `+0x30D0`, `angle`
  is at `+0x30D8` and `mirror` at `+0x30F0`;
- `timing->count` is at `+0x24`.

It is registered in all 11 French MODEL459 layouts that contain the function
(12 physical images; models 47, 231, 238, 411, 417 and 620). `rcos`, `rsin`
and `RotTransPers` were added to the French MODEL459 binding file at French
resident functions. All French overlay images rebuilt exactly. The
[attempt ledger](french-model-variant459-rays-attempts.csv) records the
experiment.

The identical Spanish, German and Italian images can reuse this C without
decompiling the function again.
