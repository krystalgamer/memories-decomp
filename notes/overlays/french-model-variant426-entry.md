# French header-426 loader entry

`func_8013B004` / `func_8017B004` (image offset `0x4`, 0xF20 bytes) is the
loader entry of the header-426 images. These are the French and Spanish
`model_variant_0_stage7_slot0` and `model_variant_0_stage8_slot1`, and all
four are byte-identical. `french_model_variant/variant426_entry_slot1.c`
renames the body, its helpers and its data label for the slot-1 images.

- **Base.** It is the header-425 entry (`variant425_entry.c`). The work-area
  tail, from the matrix onwards, sits 0x974 bytes higher. The changes are:
  - **Work area.** It holds four header-458 streamers, two funnels and a
    second FT4 pair (`flat2`) ahead of the matrix.
  - **Configuration.** Records are 0x34 bytes, and a new word at `+0x30`
    gates the helper at `0x13B0`.
  - **Textures.** A fourth texture page from `D_8013E874 + 0xE0` feeds
    the new FT4 pair.
  - **Mesh.** The rings drop the fixed height offset, and the row colours
    ramp by 8.
  - **Colours and extents.** The spirals use magenta outer colours, and the
    sheets span 256 units.
  - **Runtime.** It copies the other slot's position without the
    travel adjustment and calls the header-426 helpers.
- **Views.** `variant426_entry.h` reuses the header-425 entry views and adds
  `EntryConfig426` and `EntryState426`.
- **Bindings.** Both regions' `model_variant426_linker_symbols.txt` gain
  the named texture, primitive, `GsGetLwUnit`, `Square0` and `SquareRoot0`
  bindings used by the C entry.

All four images rebuild exactly. The
[attempt ledger](french-model-variant426-entry-attempts.csv) records the
probes.
