# Italian MODEL images built from the French sources

The Italian (`SLES_039.50`) `MODEL.MRG` is byte-identical to the French one
at every sector: all 3,572 Italian MODEL images match the French image at
the same sector and load address, relocations included, because the two
PAL executables share their resident layout. For #6512 the Italian
manifests in `config/sles_03950/overlays/` therefore reuse the French
source layout instead of adding Italian copies of the C: each image's
`.yaml`, `_functions.csv`, `_matching_c.json` and `_symbols.txt` copy the C
segments, function inventory and named compiler profile of the French
module, and the module takes the French module's name with the `italian_`
prefix. The sources stay where the French manifests point (the US, French
and Spanish directories under `src/overlays/`), so no function is
duplicated.

Resident references link through Italian `*_linker_symbols.txt` files with
the same names and entries as the French files. Because the images are
identical, every entry keeps its French address; named entries agree with
`config/sles_03950/symbols.txt`. Existing Italian linker files are only
extended, never rewritten.

The Italian `WA_MRG.MRG` duel-effect bank (sector 7193 and its six
duplicates) and PAL options module (sector 10170 and its four duplicates)
are likewise byte-identical to the French modules, so `italian_duel_effects`
and `italian_options` are registered with the French layouts, sources and
linker bindings.

`make italian-match-overlays`, which the Italian overlay workflow runs,
builds and verifies every configured Italian overlay.
