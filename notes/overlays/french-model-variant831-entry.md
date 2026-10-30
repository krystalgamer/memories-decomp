# French header-831 bolt entry

`func_8013B004` (image offset `0x4`, 0x3804 bytes) is the loader entry of
the raw image `model_image_model_168816_8013b000`. The French and Spanish
copies are byte-identical; both now build
`french_model_variant/variant831_entry.c`. Their slot-1 twins
`model_image_model_168826_8017b000` (header 928) build the same C through
`variant831_entry_slot1.c`, which remaps `func_8013B004` and the
`D_8013E808`/`D_8013E818`/`D_8013E8A4` labels to `0x8017…`.

- **Base.** It is the header-792 bolt entry (`model_variant/variant792_entry.c`)
  reworked for three bolts:
  - **Work area.** Per-bolt arrays have three entries. A per-bolt flare
    scale follows the flight counters. The rays become one shared ring of
    16 inner/outer pairs, built once from the configuration radii, and the
    flare texture page and CLUT follow the five textures.
  - **Configuration.** Rows are 0x48 bytes. They add the ring radii and
    depth, byte fade steps, near and far flare growth rates, and a third
    launch time. All five texture pages use CLUT mode 1.
  - **Runtime.** It has new camera presets, a single-sided depth jitter, a
    third launch and a strip spin of four 512-unit steps. The arrived bolt
    draws the glow, the sparks and a growing flare over the ray ring instead
    of the two ray fans. The phase sequence runs 0, 1, 2, 4, 5.
    The doubled negated z-step is computed as `(s32)((u32)-vz << 1)`, so
    no negative signed value is left-shifted. The flare scale is read and
    grown at `flare_scale[i - count]`, which for three bolts aliases
    `flight[]`; the source keeps and comments that retail subscript.
- **Data and bindings.**
  - The static scale vector is the C `.rodata` at `0x8013E808`.
  - Each image's `_symbols.txt` defines `D_8013E818` (texture images) and
    `D_8013E8A4` (command rows).
  - A new `model_variant831_linker_symbols.txt` per region binds the
    resident calls.

Both images rebuild exactly. The
[attempt ledger](french-model-variant831-entry-attempts.csv) records the
probes.

## North American images

The US MODEL.MRG images at sectors 168816/168826 (headers 814/911, modules
`model_variant_711_pos0_slot0` and `model_variant_711_stage8_slot1`) carry the
same entry; besides the resident call targets, which
`config/slus_01411/overlays/variant814us_linker_symbols.txt` supplies, the
only difference is the flare CLUT row (`0x3D20` instead of the PAL `0x3D28`).
The wrappers `src/overlays/model_variant/variant814us_entry.c` and
`variant814us_entry_slot1.c` set `VARIANT831_FLARE_CLUT` and include this
source; the French default is unchanged. Both US images reproduce their
retail SHA-256.
