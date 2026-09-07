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
| `0x00` | `tint_r` | 1 | `Fade_DrawOverlay` subtracts `level` and writes the result to the red box channel |
| `0x01` | `tint_g` | 1 | the same exact renderer writes the green channel |
| `0x02` | `tint_b` | 1 | the same exact renderer writes the blue channel |
| `0x03` | `pad_03` | 1 | no exact C field access establishes a role |
| `0x04` | `level` | 1 | byte loads/stores throughout the family; current fade brightness in `Fade_DrawOverlay` |
| `0x05` | `target_level` | 1 | byte comparison and initialization in the transition setup paths |
| `0x06` | `flags` | 1 | byte bit tests/writes for `0x01`, `0x02`, `0x04`, `0x10`, `0x20`, and `0x80` |
| `0x07` | `step` | 1 | setup values `8` and `0x0C`; `func_800151D8` uses this byte for both band spacing and the scaled head advance |
| `0x08` | `field_08` | 2 | band-ramp head: `func_800151D8` starts its walk from `(s16)field_08`, then advances the stored halfword; setup initializes it to `0` or `0xFF` |
| `0x0A` | `band_levels[30]` | 30 | `func_800156B8` fills offsets `0x0A..0x27`; `Fade_DrawOverlay` renders exactly 30 bands |

The end of `band_levels` gives a minimum record size of `0x28`.
`D_800E9EF0`, the next linker symbol, is exactly `0x28` bytes after
`D_800E9EC8`, independently fixing the extent. C89 typedef assertions verify
the total size and every modeled field offset.

Target assembly across `func_800151B0`, `func_800151D8`,
`func_80015310`, `Fade_DrawOverlay`, `func_800156B8`, the setup functions, and
the flag-setting wrappers establishes the access widths and offsets. GMS
corroborates the same byte labels, the halfword at `0x08`, and the 30-byte
span, but its generated scalar and function types are treated as guesses.
Unchiga's same-address sketches independently use the same byte fields,
halfword, and array span. The exact matched `Fade_DrawOverlay` implementation is
the semantic basis for the tint, level, flags, and band names. No Psy-Q
declarations were used.

## Band-ramp mechanics

The matching [`func_800151D8`](../src/game/func_800151D8.c) establishes
**high-confidence static semantics** for `field_08` in band mode: it is a
signed sweep head, not another byte brightness value. The shared declaration
remains `u16` to preserve the existing exact C; the walker explicitly casts
the starting value to `s16`, so values below zero and above `0xFF` can
participate before clamping.

Let `H` be that signed starting head, `L` the current `level`, and `T` the
`target_level`. Each iteration writes the same clamped value to band `i` and
band `29 - i`:

| Condition | Pair traversal | Value before clamping | Stored head after all 15 pairs |
|---|---|---|---|
| `L < T` | `i = 14..0`, middle pair to outer pair | `H - (15 - i) * step` | old halfword plus `step * D_8009B0D8` |
| `L >= T` | `i = 0..14`, outer pair to middle pair | `H + (i + 1) * step` | old halfword minus `step * D_8009B0D8` |

Both paths clamp to the interval between `L` and `T`. The unscaled `step`
sets the spacing between unclamped pair values; only the head advance is
multiplied by `D_8009B0D8`. The head update therefore uses the opposite sign
to the within-pass accumulator. A post-call sample of `field_08` already
contains the next head, whereas the band array was generated from the old
one.

`level` is not advanced for every pair. It is assigned `T` only when the
**last** clamped pair reaches `T`: the outer pair for increasing levels, or
the middle pair for decreasing levels. With the unsigned, nonnegative
`step`, that also means every other pair has reached the target. The walker
does not clear the active flag; transition completion remains the separate
responsibility of `func_80015310`.

[`Fade_DrawOverlay`](../src/game/fade_draw_overlay.c) draws array index `i`
at `y = i * 8` with height 8 and intensity `0xFF - band_levels[i]`.
Thus indices 14/15 are the two center bands and 0/29 are the top/bottom
bands. The **eight-pixel band height is not a fixed eight-unit ramp step**.
The direction-dependent traversal also means a description of both paths
as filling from the edges toward the middle is incomplete.

## Setup overrides and evidence limits

The matching setup chains explain why the configured step must be read
rather than inferred from a caller's name:

| Setup path | Initial head / target | Default setup |
|---|---|---|
| `func_80015780` in [`fade_in.c`](../src/game/fade_in.c) | `0` / `0xFF` | fills all bands with the current level, sets flags `0x80` and step `0x0C` |
| `Fade_InitOut` in [`fade_out.c`](../src/game/fade_out.c) | `0xFF` / `0` | fills all bands with the current level, sets flags `0x80` and step `0x0C` |

`func_800157DC` and `Fade_StartOut` call those initializers, then request
step `8` and flag `0x01` (band mode). However, both call a color helper
**after** that request. When `D_8009B145` is nonzero,
[`func_8001572C`](../src/game/func_8001572C.c) replaces the flags with `0x90`,
while [`func_80015870`](../src/game/fade_color.c) replaces them with `0xB0`.
Both helpers write white tint, restore step `0x0C`, and clear band mode by
replacing the entire flag byte. The wrappers therefore do not unconditionally
start an eight-unit banded transition.

These are code-derived conclusions, not a new emulator result or a semantic
rename. The older [screen-fade observations, F87-F92](research/Unchiga_Symbols/findings.md#screen-fade-to-black-circle-out-of-free-duel-x-back-in----session-2026-09-02)
describe particular menu runs; their eight-unit step and approximate
48-frame duration are not universal API guarantees. Which screens select
each setup path, and which values `D_8009B0D8` takes during them, still need
caller-specific evidence and human context before assigning fixed timings
or screen-specific meanings.

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

- `func_800151B0`, `func_800151D8`, `Fade_DrawOverlay`, `func_800156B8`,
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

- `Fade_DrawOverlay` keeps `D_800E9EC8_arr` as the pointer passed to
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
`Fade_DrawOverlay` also retains its oversized raw `D_800E9ECE` declaration
because the target independently materializes that address; folding it into
the typed base would change exact code generation.

## Validation

The migrated source retained the complete executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
with `MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also pass after regeneration of the tracked usage
reports.
