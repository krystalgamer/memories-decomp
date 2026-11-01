# European MODEL images built from the French sources

The European (`SLES_039.47`) `MODEL.MRG` and `SU.MRG` archives keep the
French sector layout. With relocations masked, 1,189 European images are
byte-identical to the French image at the same sector and load address, and
2,378 more differ only in resident relocations. For #6258 the European
manifests in `config/sles_03947/overlays/` therefore reuse the French source
layout instead of adding European copies of the C: each image's `.yaml`,
`_functions.csv`, `_matching_c.json` and `_symbols.txt` copy the C segments,
function inventory and named compiler profile of the French module, and the
module takes the French module's name with the `european_` prefix. The
sources stay where the French manifests point (the US, French and Spanish
directories under `src/overlays/`), so no function is duplicated.

## Resident references

Resident calls and data references link through European
`*_linker_symbols.txt` files with the same names and entries as the French
files:

- named symbols map to their European address in
  `config/sles_03947/symbols.txt`;
- `D_800FE168`, the US-named word 0x40 after `GsWSMATRIX_ORG`, is
  `0x800FF0D0` (French `0x800FF4F0`, US `0x800FE168`);
- address-named entries whose address the French executable names (for
  example `func_80059B90` at French `0x8005CC98`,
  `Model_GetSlot0OffsetWorldPosition`) take that name's European address;
- address-named calls map through the call targets that differ between each
  image's French and European copies;
- an entry whose address disagrees with the source executable's own symbol
  table is an alias into an earlier resident layout and keeps its raw value.

One European image differs from its French donor outside relocations:
in `european_model_variant_485_stage10_slot1` the byte after header 89's
order table is `0x08` (French slot 1 has zero). Its European wrapper
`variant106_entry_slot1.c` sets
`MODEL89_ORDER_PAD` before including the US header-89 source; the slot-1
names come from the image's own symbols file.

## Coverage

Together with the header-174 images, whose entry, rings and coils helpers
come from the French `variant450_entry.c`, `variant450_rings.c` and
`variant450_coils.c`, every configured European MODEL and SU image now
builds from C. Every European image has a French counterpart at the same
sector; the eight French-only modules have no European copy.

`make european-match-overlays` builds and verifies every configured European
overlay, and the European build workflow runs it.
