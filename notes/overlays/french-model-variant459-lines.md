# French MODEL459 line helper

`func_8013EFE0` / `func_8017EFE0` (image offset `0x3FE0`, 0x568 bytes) is the
MODEL459 member of the "lines" helper family. **No release had matching C for
this body before.**

The body is the same as the
[MODEL438 line helper](french-model-variant438-lines.md), apart from three
things:
- the state layout (groups at `+0xCA8`, line at `+0x2EB8`, origin at
  `+0x2EE0`, target at `+0x304C`, directions at `+0x3058`/`+0x3068`, time,
  step and timing pointer at `+0x3084`/`+0x308C`/`+0x309C`, phase at `+0x30DC`);
- the timing window at `timing+0x34`/`+0x38`;
- the function address.

The same C body with a MODEL459 header was exact on the first probe.

It is registered in all 11 French MODEL459 layouts that contain the function
(12 physical images; models 47, 231, 238, 411, 417 and 620). `GsSortGLine` was
added to the French MODEL459 binding file at the French resident function
`0x800840B8`. All French overlay images rebuilt exactly. The
[attempt ledger](french-model-variant459-lines-attempts.csv) records the
experiment.

The identical images in the Spanish, German and Italian archives can reuse this
C without decompiling the function again.
