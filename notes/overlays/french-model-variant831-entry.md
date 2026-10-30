# French header-831 bolt entry

`func_8013B004` (image offset `0x4`, 0x3804 bytes) is the loader entry of
the raw image `model_image_model_168816_8013b000`. The French and Spanish
copies are byte-identical; both now build
`french_model_variant/variant831_entry.c`.

The US image at the same sector has a different header (814) and is still
assembly.

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
    no negative signed value is left-shifted.
- **Data and bindings.**
  - The static scale vector is the C `.rodata` at `0x8013E808`.
  - Each image's `_symbols.txt` defines `D_8013E818` (texture images) and
    `D_8013E8A4` (command rows).
  - A new `model_variant831_linker_symbols.txt` per region binds the
    resident calls.

Both images rebuild exactly. The
[attempt ledger](french-model-variant831-entry-attempts.csv) records the
probes.
