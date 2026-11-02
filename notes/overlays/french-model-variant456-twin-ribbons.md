# French header-456 twin ribbons helper

`FrenchModelVariant456_DrawTwinRibbons` (`0x8013C01C` / `0x8017C01C`, image
offset `0x101C`, 0x818 bytes) grows and draws two ribbons of three swaying
points. One source serves both slots; each image's symbols file binds the name
at its own address.

The body is identical in twelve images: the French and Spanish
`model_variant_163`, `model_variant_460` and `model_variant_536` stage
images (slot 0 and slot 1). All of them bind their region's
`model_variant456_linker_symbols.txt`.

- **Shape.** Each ribbon record holds three points:
  - the points grow along the record's direction by a clamped amplitude;
  - they gain a fixed 48-unit offset along the turn angle;
  - they are projected with RotTransPers4, with the last point reusing
    segment 1;
  - they sway by `rsin(phase)` and the wave.
  The quads are sorted while the record is still growing.
- **Register shape.** Several dead tests and stores steer register
  allocation:
  - **Outer loop top.** It opens with dead `i == 1` and `i == length`
    tests. Branch deletion removes them after reload, leaving only the
    index reload. Before that, they make loop optimisation hoist the
    constants 1 and 48 out of both loops in the target's order, and give
    48 the priority that places it in `s6`.
  - **Amplitude clamp.** A dead store in the clamp's copy arm stops jump
    optimisation from hoisting the `1024` store above the clamp test.
- **View.** The view lives in `variant456_twin_ribbons.h`.

All twelve images rebuild exactly. The
[attempt ledger](french-model-variant456-twin-ribbons-attempts.csv) records
the probes.
