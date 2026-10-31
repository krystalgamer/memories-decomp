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

`make spanish-match-overlays`, which the Spanish overlay workflow runs,
builds and verifies every configured Spanish overlay.
