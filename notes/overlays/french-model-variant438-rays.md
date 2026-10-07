# French MODEL438 ray helper

`func_8013D3E8` / `func_8017D3E8` (image offset `0x23E8`, 0x64C bytes) is the
MODEL438 member of the "rays" helper family. **No release had matching C for
this body before.** It is a closed retained helper, and no call from the entry
was observed.

The accepted MODEL417 ray helper has the same body. A local MODEL438 header
reproduces it exactly on the first probe. The header differs in the following:
- the 0x6C-byte rays start at `+0x1410`, the pulse at `+0x1938` and the
  `POLY_G3` at `+0x2098`;
- origin, direction and direction_b are at `+0x2258`/`+0x226C`/`+0x2280`;
- frame, step and timing are at `+0x2298`/`+0x22A4`/`+0x22B4`;
- `index` is a halfword at `+0x22C8`, `progress` a word at `+0x22DC`, `angle`
  is at `+0x22E4` and `mirror` at `+0x22FC`;
- `timing->count` is at `+0x18`.

It is registered in all 12 French MODEL438 images (models 147, 211, 263, 525,
610 and 632, both slots). `RotTransPers` was added to the French MODEL438
binding file at the French resident function `0x80087868`. All French overlay
images rebuilt exactly. The
[attempt ledger](french-model-variant438-rays-attempts.csv) records the
experiment.

The identical Spanish, German and Italian images can reuse this C without
decompiling the function again.
