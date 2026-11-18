# Runtime Overlay Sources

Matching C for runtime-loaded modules belongs under this directory, separate
from the resident executable sources in `src/game/`.

Create one module-scoped directory when its archive payload and load range are
verified:

```text
src/overlays/<module>/
```

When a screen has a verified package but its executable phase is shared with
another module, document its screen-specific entry points in that module's
README rather than creating a second directory or duplicating matching source.

Use the stable module names recorded in
[`notes/overlays/module-crosswalk.md`](../../notes/overlays/module-crosswalk.md),
such as `free_duel`, `password`, `overworld`, and `main_menu`; the
`name_entry` build module is the documented shared-image exception and builds
from `password/`.
Do not create a module directory from an address alone: different overlays
reuse the same VRAM ranges, especially `0x80168xxx`, so an address-based source
path would merge unrelated code.

The binaries generated at `tmp/splat/assets/overlays/slot_*.bin` are snapshots
of the resident executable's load banks. They are not module payloads or
editable source. Keep extracted archive payloads and comparison artifacts
under `tmp/` until their disc range and runtime destination are verified.

`config/slus_01411/overlays.json` records verified module slices independently
from the resident executable build. Run `make overlays` to extract the tracked
module images under `tmp/overlays/`, then `make verify-overlays` to confirm
their archive range and SHA-256. Run `make match-overlays` to rebuild every
module with its independent layout and require an exact binary match.
Module builds are independent, so `overlay_build.py build` honors the make job
count (`MAKEFLAGS=-jN`). Each module still runs Splat, assembly, and linking on
its own; parallel builds fork a fresh process per module so Splat state is
never shared. Whole-image MODEL baselines use a Splat `bin` subsegment, which
is converted with `objcopy -I binary` and linked at its original address,
instead of spending time disassembling raw bytes as `data`.

Overlay C must not be inserted into the resident `matching_c.json` or linked as
part of `SLUS_014.11`. Module-specific build manifests will extend the overlay
metadata as matching source is accepted.

Directories are named after the module, never after a region: one directory
holds every build's source for that module, and region-specific behaviour is
selected by the per-build `*_matching_c.json` manifests under `config/`.

Module directories:

- [`credits/`](credits/) holds the credits overlay: `credits.c` is the
  USA/Japanese runtime and `pal_presentation.c` the PAL (European, French,
  German, Italian and Spanish) presentation.
- [`duel_effects/`](duel_effects/) holds the duel effect overlay shared by
  every build: the effect dispatcher, the numbered `effect_<N>` handlers and
  their drawing and geometry helpers.
- [`free_duel/`](free_duel/) records the verified WA package, executable
  phase, and runtime range for the Free Duel module.
- [`main_menu/`](main_menu/) records the verified SU phase and keeps the
  second, unidentified SU image outside the module scope.
- [`model_variant/`](model_variant/) holds every MODEL.MRG image source: the
  per-model variant stages, the `primary<N>` MODEL primaries, the Exodia
  helpers (`exodia_slot0.c`, `exodia_slot1.c`) and the `image<N>_*` images
  named by disc sector. See [MODEL variant file names](#model-variant-file-names).
- [`options/`](options/) holds the PAL options screen built by the five
  European executables.
- [`overworld/`](overworld/) records the pre- and post-coup WA variants that
  reuse the same runtime code range.
- [`password/`](password/) records the password-screen package and the
  related, non-identical phase in the name-entry package, including the
  name-entry entry points into the shared front-end image.
- [`safecheck2003/`](safecheck2003/) holds the safechk 2.0.0.3 boot
  protection module built by every region.

## MODEL variant file names

`model_variant/` merged the former `french_model_variant/`,
`spanish_model_variant/`, `spanish_model_primary/` and `model_exodia/`
directories (#9498, #9500, #9504, #9506). A file's base name is
`<effect>_<role>`, where `<role>` is `entry` or a helper such as `rings` or
`sheets`, and `<effect>` names the effect image the source was written for:

- `e<NNN>` (for example `e036_entry.c`): the developers' effect name, taken
  from the `bin/eNNN_0.bin`/`bin/eNNN_1.bin` build-script residue in
  MODEL.MRG and confirmed against the retail image. Byte-identical copies
  share the lowest confirmed name.
- `h<NNN>` (for example `h143_entry.c`): an image without a confirmed name.
  `<NNN>` is the image's slot 0 North American MODEL header word (Japan is
  one lower, the PAL builds 17 higher).
- `e<NNN>_h<HHH>` (for example `e503_h320_entry.c`): a confirmed second image
  of `eNNN`, an older build at the name's linear header slot that coexists
  with the canonical `eNNN_*` image. `<HHH>` is its slot 0 North American
  header word.

[MODEL effect names](../../notes/model-effects.md) and
[`model-effects.csv`](../../notes/model-effects.csv) give the evidence, map
every `eNNN` to its header words in each region, and define the naming rule.
Some sources keep a suffix:

- `*_usa.c` (for example `exodia_slot0_usa.c`, `e027_entry_usa.c`): the
  USA build of a shared source. Each is a thin wrapper that sets a
  configuration macro (`MODEL_EXODIA_USA`, `GRAPHICS_DEFAULT_HEIGHT 240`, a
  work-size `printf`, ...) and includes the shared `.c`; it has no header of
  its own. Japan reuses most of these wrappers.
- `*_pal` (for example `e585_entry_pal.c`): a PAL-only build that would
  otherwise share its name with the North American/Japanese source of the
  same image and role. It goes before any `_slot1`.
- `*_slot1`: a slot 1 build that differs from the slot 0 source.
- `*_r<rec>` and offset tags (for example `e585_strand_r163.c`,
  `h452_sheets_1b98.c`): a leftover helper linked at one record, or one of
  two same-role helpers told apart by image offset.

Only source files were renamed: module and config names
(`model_variant_<rec>_...`) stay position-based, and symbols still carry
their old `ModelVariant<N>_`, `FrenchModelVariant<N>_` or
`SpanishModelVariant<N>_` header-number prefixes.
