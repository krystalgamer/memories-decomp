# Japanese MODEL images built from the US sources

The Japanese `MODEL.MRG` and `SU.MRG` archives keep the US sector layout: an
image at a given sector and load address has the same extent and, apart from
the module header word and resident relocations, the same code as its US
counterpart. For #5551 the Japanese manifests in `config/slpm_86398/overlays/`
therefore reuse the US source layout of the matching US module rather than
adding Japanese copies of the C.

## What a Japanese image carries

For each wired image, the Japanese `<image>.yaml`, `_functions.csv`,
`_matching_c.json` and `_symbols.txt` copy the C segments, function sizes and
named compiler profiles of the US module at the same sector. The sources stay
under `src/overlays/model_variant/`, `src/overlays/spanish_model_primary/` and
the other existing US directories; no function is duplicated. Each image
selects the same profile as its US counterpart, including
`gcc_2_7_2_cdk_g0` where the US manifest selects it (see
[MODEL variants](model-variants.md#compiler)).

Resident calls and data references resolve through Japanese
`*_linker_symbols.txt` files with the same names as the US files. Every name
maps to its Japanese address from `config/slpm_86398/symbols.txt`; the only
unnamed resident symbol, `D_800FE168`, sits between `GsWSMATRIX_ORG` and
`GsIDMATRIX2` in both executables and moves with them from `0x800FE168` to
`0x800FB858` (confirmed from the low halves in the header-628 and header-632
images). Address-named calls (`func_80058F10`, `func_8005B260`) map through the
call targets that differ between the US and Japanese copies of each image.

## Not yet built from C

- Images whose US source emits a rodata padding byte that differs in the
  Japanese copy (one word, for example the byte after the header-635
  `E_DATA size` string is `0x0C` instead of `0x24`). They need a Japanese
  selector for that byte and stay generated assembly.
- The header-141 primary images (`model_image_model_39136_8013a000` and
  `model_image_model_39138_8017a000`), whose Japanese code differs in 139
  words.
- The six images that use `variant668_linker_symbols.txt`, whose aliases to
  US resident addresses inside other functions have not been mapped.
- US images that are still generated assembly.

`make japanese-match-overlays` builds and verifies every configured Japanese
overlay, and the Japanese build workflow runs it.
