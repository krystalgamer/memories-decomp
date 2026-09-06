# Runtime Overlay Sources

Matching C for runtime-loaded modules belongs under this directory, separate
from the resident executable sources in `src/game/`.

Create one module-scoped directory when its archive payload and load range are
verified:

```text
src/overlays/<module>/
```

When a screen has a verified package but its executable phase is shared with
another module, a documentation-only directory may record the screen-specific
entry points without duplicating matching source.

Use the stable module names recorded in
[`notes/overlays/module-crosswalk.md`](../../notes/overlays/module-crosswalk.md),
such as `free_duel`, `password`, `overworld`, and `main_menu`; `name_entry`
is the documented shared-image exception.
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

Overlay C must not be inserted into the resident `matching_c.json` or linked as
part of `SLUS_014.11`. Module-specific build manifests will extend the overlay
metadata as matching source is accepted.

Initialized module directories:

- [`free_duel/`](free_duel/) records the verified WA package, executable
  phase, and runtime range for the Free Duel module.
- [`password/`](password/) records the password-screen package and the
  related, non-identical password-shop phase in the main-menu package.
- [`overworld/`](overworld/) records the pre- and post-coup WA variants that
  reuse the same runtime code range.
- [`main_menu/`](main_menu/) records the verified SU phase and keeps the
  second, unidentified SU image outside the module scope.
- [`name_entry/`](name_entry/) records the verified WA package and entry
  points into the front-end image shared with `password/`; matching source is
  not duplicated under both directories.
