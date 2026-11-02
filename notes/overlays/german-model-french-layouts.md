# German MODEL images built from the French sources

The German (`SLES_039.49`) `MODEL.MRG` is byte-identical to the French one
at every sector: all 3,572 German MODEL images match the French image at
the same sector and load address, relocations included, because the two
PAL executables share their resident layout. For #6510 the German
manifests in `config/sles_03949/overlays/` therefore reuse the French
source layout instead of adding German copies of the C: each image's
`.yaml`, `_functions.csv`, `_matching_c.json` and `_symbols.txt` copy the C
segments, function inventory and named compiler profile of the French
module, and the module takes the French module's name with the `german_`
prefix. The sources stay where the French manifests point (the US, French
and Spanish directories under `src/overlays/`), so no function is
duplicated.

Resident references link through German `*_linker_symbols.txt` files with
the same names and entries as the French files. Because the images are
identical, every entry keeps its French address; named entries agree with
`config/sles_03949/symbols.txt`. Existing German linker files are only
extended, never rewritten.

The German `WA_MRG.MRG` duel-effect bank (sector 7193 and its six
duplicates) and PAL options module (sector 10170 and its four duplicates)
are likewise byte-identical to the French modules, so `german_duel_effects`
and `german_options` are registered with the French layouts, sources and
linker bindings.

The four German images at sectors 48224, 48234, 109772 and 109782 were
registered earlier as `german_model_image_model_*` for the MODEL450 and
header-447 rings takeovers, which put only their helpers in C. They are
the French `model_variant_174` and `model_variant_447` slot 0 and slot 1
images byte for byte, so they now take those French layouts and are named
`german_model_variant_174_*` and `german_model_variant_447_*`. The takeover
ledgers keep the historical module names. `model_variant450_linker_symbols.txt`
gains the French names of the eleven Psy-Q entries it already bound under
placeholder names.

`make german-match-overlays`, which the German overlay workflow runs,
builds and verifies every configured German overlay.
