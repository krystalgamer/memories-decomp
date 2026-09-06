# Fade Transition State

This phase is limited to the `0x28`-byte transition/fade record rooted at
`D_800E9EC8` and its same-address linker alias `D_800E9EC8_arr`. It does not
group neighboring globals or offset symbols into the shared declaration.
`src/game/fade.h` records verified offsets and widths; names are conservative
descriptions of behavior, not recovered original Konami identifiers.

## Layout and extent evidence

The shared `FadeTransitionState` layout is:

| Offset | Field | Width | Local evidence |
|---:|---|---:|---|
| `0x00` | `tint_r` | 1 | `func_800154E4` subtracts `level` and writes the result to the red box channel |
| `0x01` | `tint_g` | 1 | the same exact renderer writes the green channel |
| `0x02` | `tint_b` | 1 | the same exact renderer writes the blue channel |
| `0x03` | `pad_03` | 1 | no exact C field access establishes a role |
| `0x04` | `level` | 1 | byte loads/stores throughout the family; current fade brightness in `func_800154E4` |
| `0x05` | `target_level` | 1 | byte comparison and initialization in the transition setup paths |
| `0x06` | `flags` | 1 | byte bit tests/writes for `0x01`, `0x02`, `0x04`, `0x10`, `0x20`, and `0x80` |
| `0x07` | `step` | 1 | byte step values `8` and `0x0C`; target `func_800151D8` loads it with `lbu` |
| `0x08` | `field_08` | 2 | exact halfword writes in `func_80015780` and `Fade_InitOut`; target `func_800151D8` uses both signed and unsigned halfword reads |
| `0x0A` | `band_levels[30]` | 30 | `func_800156B8` fills offsets `0x0A..0x27`; `func_800154E4` renders exactly 30 bands |

The end of `band_levels` gives a minimum record size of `0x28`.
`D_800E9EF0`, the next linker symbol, is exactly `0x28` bytes after
`D_800E9EC8`, independently fixing the extent. C89 typedef assertions verify
the total size and every modeled field offset.

Target assembly across `func_800151B0`, `func_800151D8`,
`func_80015310`, `func_800154E4`, `func_800156B8`, the setup functions, and
the flag-setting wrappers establishes the access widths and offsets. GMS
corroborates the same byte labels, the halfword at `0x08`, and the 30-byte
span, but its generated scalar and function types are treated as guesses.
Unchiga's same-address sketches independently use the same byte fields,
halfword, and array span. The exact matched `func_800154E4` implementation is
the semantic basis for the tint, level, flags, and band names. No Psy-Q
declarations were used.

## Shared declarations and migrated users

`fade.h` declares the typed base:

```c
extern FadeTransitionState D_800E9EC8;
```

It also preserves the only same-address raw linker alias:

```c
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];
```

Every matching pure-C user reported for `D_800E9EC8` now includes the shared
header. The current generated usage report lists:

- `func_800151B0`, `func_800151D8`, `func_800154E4`, `func_800156B8`,
  `func_800156DC`;
- `func_8001572C`, `func_80015780`, `func_800157DC`;
- `func_8001581C`, `func_80015870`, `Fade_InitOut`;
- `Fade_StartOut`, `func_80015944`, `func_80015998`;
- `func_80015A50`, `func_80015A94`, `func_80015B50`, `func_80015B94`;
- `func_80015BD8`, `func_80015BF0`;
- `func_80015C0C`, `func_80015C48`, `func_80015C84`, `func_80015CC0`.

The later exact pure-C match for `func_800151D8` moved its symmetric
30-band update out of the exception list while preserving the typed
`FadeTransitionState` accesses.

## Exact-code exceptions

Two assembly users remain untouched:

- `func_80015310` advances and completes transitions;
- `func_800218F0` reads `flags`, writes `level`, and calls the band fill during
  its larger assembly-only flow.

There are no matching inline-assembly users of the base global in this phase.

Some migrated C deliberately retains raw expressions without retaining local
competing declarations:

- `func_800154E4` keeps `D_800E9EC8_arr` as the pointer passed to
  `func_80015310`, while the tail uses typed `D_800E9EC8` fields. The two
  same-address symbol views preserve the target's fresh address
  materialization in the exact draw-screen-fade implementation.
- `func_800156B8` casts the typed base to a byte pointer and keeps
  `*(p + i + 0xA)`. The equivalent array-member expression changes the MIPS
  `addu` operand order and does not match.
- `func_8001572C`, `func_8001581C`, `func_80015870`, and `func_80015944`
  retain explicit `s32` stores through the typed base because target assembly
  proves one whole-word write at offset zero before byte field accesses.

`D_800E9ECC` and `D_800E9ECE` are offset symbols at `+0x04` and `+0x06`, not
same-address aliases. Assembly users of those symbols remain unchanged.
`func_800154E4` also retains its oversized raw `D_800E9ECE` declaration
because the target independently materializes that address; folding it into
the typed base would change exact code generation.

## Validation

The migrated source retained the complete executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
with `MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also pass after regeneration of the tracked usage
reports.
