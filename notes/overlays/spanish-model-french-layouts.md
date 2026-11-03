# Spanish MODEL images built from the French sources

The Spanish (`SLES_039.51`) `MODEL.MRG` is byte-identical to the French one
at every sector: all 3,579 Spanish MODEL images match the French image at
the same sector and load address, relocations included, because the two
PAL executables share their resident layout. For #6459 the Spanish
manifests in `config/sles_03951/overlays/` therefore reuse the French
source layout instead of adding Spanish copies of the C: each image's
`.yaml`, `_functions.csv`, `_matching_c.json` and `_symbols.txt` copy the C
segments, function inventory and named compiler profile of the French
module, and the module takes the French module's name with the `spanish_`
prefix. The sources stay where the French manifests point (the US, French
and Spanish directories under `src/overlays/`), so no function is
duplicated.

Resident references link through Spanish `*_linker_symbols.txt` files with
the same names and entries as the French files. Because the images are
identical, every entry keeps its French address; named entries agree with
`config/sles_03951/symbols.txt`. Existing Spanish linker files are only
extended, never rewritten.

The 247 Spanish MODEL variant modules that still had `unmatched_asm`
rows take their French module's layout in the same way and keep their
Spanish names. Three of them (`spanish_model_variant_269_stage9_slot0`,
`_stage10_slot1` and `spanish_model_variant_238_stage8_slot1`) take the
French module with the same bytes at another sector
(`french_model_variant_43_*` and `french_model_variant_47_stage8_slot1`).
The Spanish copies of the header-414, header-415 and header-442 helpers
(`spanish_model_variant/variant414_bands`, `variant415_bands` and
`variant442_ribbons`) were wired only by those modules' raw layouts and are
removed; the French layouts compile the French helpers. The two header-447
images at sectors 109772 and 109782 keep their raw layouts but compile the
French header-685 entry (`french_model_variant/variant685_entry.c`) at `0x4`
beside `variant447_rings.c`; the rings body's five stale resident calls use
the `Rings447_*` names, so `model_image447_rings_linker_symbols.txt` carries
the French header-685 bindings unchanged.

The other 17 Spanish modules whose loader entry at `0x4` was still
assembly (headers 433, 440, 465, 476 and 477 of models 108, 117, 180,
262, 573, 631 and 712) now compile the French module's entry source with
its profile. Their symbol files take the French names at the same
addresses (entry, entry data and Psy-Q calls), and their linker files gain
the French files' Psy-Q bindings.

`make spanish-match-overlays`, which the Spanish overlay workflow runs,
builds and verifies every configured Spanish overlay.
