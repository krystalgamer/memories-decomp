# Fade Transition State

This phase is limited to the `0x28`-byte transition/fade record
`gFade_State` and its same-address linker alias `D_800E9EC8_arr`. It does not
group neighboring globals or offset symbols into the shared declaration.
`src/ygo_types.h` records verified offsets and widths, while
`src/game/fade.h` owns the fade API and globals.

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
| `0x0A` | `band_levels[30]` | 30 | `func_800156B8` fills offsets `0x0A..0x27`; the band loop in `Fade_DrawOverlay` renders those 30 entries |

The end of `band_levels` gives a minimum record size of `0x28`.
`D_800E9EF0`, the next linker symbol, is exactly `0x28` bytes after
`gFade_State`, independently fixing the extent. C89 typedef assertions verify
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

### Frame-advance factor

The matching [`Graphics_SyncFrame`](../src/game/graphics_frame.c) now establishes
the producer of `D_8009B0D8`. The frame-sync helper optionally waits for the
GPU with `DrawSync(0)` (unless `D_8009B098 & 0x8000` is set), then spins
while the volatile counter `D_8009B0C8` is below the byte threshold
`D_8009B0C0`. [`Main_VBlankCB`](../src/game/main_frame.c) increments that
counter on each callback.

After the wait, the helper assigns the counter to the **byte**
`D_8009B0C1`, changes any nonzero byte to `1`, and publishes that byte plus
one as the word `D_8009B0D8`. For the sampled counter value `n`, the
publication is exactly:

```text
D_8009B0D8 = 1 + ((u8)n != 0)
```

Thus this writer produces only `1` or `2`: samples `0`, `1`, and `2`
produce `1`, `2`, and `2`. The byte conversion matters; a sample of `256`
has low byte zero and produces `1`, so this is not an unbounded elapsed
VBlank count or a saturating conversion of the full counter. The helper
then resets `D_8009B0C8` to `-1`, calls `VSync(0)`, and increments
`D_8009B0CC`. Its separate `D_8009AFA4` override affects `D_8009AFA3`, not
`D_8009B0D8`.

[`Main_ResetFrontendRuntime`](../src/game/main_reset_frontend_runtime.c)
clears the wait threshold `D_8009B0C0`, but that does not force the sampled
counter to zero or guarantee a factor of `1`. Actual GPU/render work and
VBlank timing still determine the sample.

The standard [`func_80012D4C`](../src/game/main_frame.c) pump calls
`func_8001306C` **before** `Graphics_SyncFrame`.
[`func_8001306C`](../src/game/func_8001306C.c) starts with `Fade_DrawOverlay`,
so the fade reads the factor already present at draw time, not the one
published later by that pump's sync call. A trace captured after sync must
not attribute that newly published factor to the preceding band update
without also establishing the draw-time value.

With this published factor, step `8` advances the head by `8` or `16`,
while the unclamped band-pair spacing remains `8`. The producer and its
output range are code-backed; the sequence of factors during a particular
visible transition still needs runtime evidence.

## Setup overrides and evidence limits

The matching setup chains explain why the configured step must be read
rather than inferred from a caller's name:

| Setup path | Initial head / target | Default setup |
|---|---|---|
| `Fade_InitIn` in [`fade_in.c`](../src/game/fade_in.c) | `0` / `0xFF` | fills all bands with the current level, sets flags `0x80` and step `0x0C` |
| `Fade_InitOut` in [`fade_out.c`](../src/game/fade_out.c) | `0xFF` / `0` | fills all bands with the current level, sets flags `0x80` and step `0x0C` |

`Fade_StartIn` and `Fade_StartOut` call those initializers, then request
step `8` and flag `0x01` (band mode). However, both call a color helper
**after** that request. When `D_8009B145` is nonzero,
[`func_8001572C`](../src/game/func_8001572C.c) replaces the flags with `0x90`,
while [`func_80015870`](../src/game/fade_color.c) replaces them with `0xB0`.
Both helpers write white tint, restore step `0x0C`, and clear band mode by
replacing the entire flag byte. The wrappers therefore do not unconditionally
start an eight-unit banded transition.

These are code-derived conclusions, not a new emulator result. The older
[screen-fade observations, F87-F92](research/Unchiga_Symbols/findings.md#screen-fade-to-black-circle-out-of-free-duel-x-back-in----session-2026-09-02)
describe particular menu runs; their eight-unit step and approximate
48-frame duration are not universal API guarantees. The frame-sync producer
now bounds its published `D_8009B0D8` values to `1` or `2` as described
above. Which screens select each setup path, and the sequence of factors
during those transitions, still need caller-specific evidence and human
context before assigning fixed timings or screen-specific meanings.

## Draw eligibility and box submission

[`Fade_DrawOverlay`](../src/game/fade_draw_overlay.c) calls `func_80015310`
**before** testing whether to draw. Its condition uses the updated state:

```c
(flags & 0x80) || (D_8009B141 != 0 && level != 0xFF)
```

| Active flag `0x80` | `D_8009B141` | `level` | Submits boxes |
|---|---|---|---|
| set | any | any | yes |
| clear | zero | any | no |
| clear | nonzero | `0xFF` | no |
| clear | nonzero | not `0xFF` | yes |

Thus a nonzero `D_8009B141` does not unconditionally keep a black overlay on
screen, nor does `level == 0xFF` unconditionally stop drawing. The untinted
input color is `0xFF - level` (or the corresponding band level); `0xFF`
therefore supplies zero color, not a general "fully faded" or "finished"
sentinel. Completion is based on the current level reaching its target,
which need not be `0xFF`.

The matching [`func_80015310`](../src/game/func_80015310.c) confirms that this
separate control byte is not a simple record of fade-in versus fade-out.
When an active update
enters with `level == target_level == 0xFF`, `0x80015384..0x800153C8` clears
the active flag and calls `func_80015CFC` to write `D_8009B141 = 1`; the
renderer still submits nothing because the level is `0xFF`. Conversely, an
equal-zero update entering with flags `0x80` calls `func_80015D0C` and leaves
the control byte zero. Other zero-target flags can retain it or write
`0x80` (`0x800153D0..0x80015404`), and the tinted completion path can re-arm
the active flag (`0x80015414..0x80015478`). The entry check at
`0x80015340..0x80015358` also preserves a control byte whose high bit is set
instead of forcing it to `1`. These previously recorded instruction-level
conclusions are now also expressed by matching C.

When the draw gate passes, the low two flag bits choose these submissions.
The coordinates below belong to the scratchpad descriptor:

| `flags & 0x03` | Band-loop submissions | Tail submission | Tail depth |
|---|---|---|---|
| `0x00` | none | one `320x240` box at `(0,0)` | `4` |
| `0x01` | 30 `320x8` boxes at `(0,8*i)`, `i=0..29`, depth `4` | none | not applicable |
| `0x02` | none | one `320x240` box at `(0,0)` | `D_8009B140`, or `0x3F` if zero |
| `0x03` | the same 30 boxes at depth `4` | one `320x8` box at `(0,240)` | `D_8009B140`, or `0x3F` if zero |

The combined `0x03` path does **not** restore height or y after the band
loop. Its extra submission is not another full-screen cover. The resident
`GsSortBoxFill` at `0x80084240` reads the descriptor geometry without writing
it back (`0x800842B4..0x800842FC`), so the SDK does not restore those fields
either. It adds the position offsets at `D_800FE0BC`/`D_800FE0BE` to x/y
while building the packet. Consequently, `(0,240)` here is not by itself
evidence that the resulting rectangle is invisible or clipped.

The scratchpad `FadeBox.tag` is a `GsBOXF.attribute` word, not a GPU packet
tag or command byte. The SDK translates `0x60000000` and `0x50000000` into
draw-mode words `0xE1000240` and `0xE1000220`, respectively
(`0x80084258..0x80084280`); both produce rectangle command byte `0x62`,
including semi-transparency, rather than `0x60`
(`0x8008429C..0x800842AC`). The tinted path changes the attribute and clamps
each input channel to `max(tint_channel - level, 0)`.

These corrections are high-confidence static conclusions from matching C,
the resident assembly, and the existing `GsBOXF` layout in
[`libgs.h`](../src/psyq/libgs.h). Which callers actually combine band mode
with `0x02`, and what the extra tail box contributes on screen, remain
runtime questions; no trace result or behavior change is implied here.

## Shared declarations and migrated users

`fade.h` declares the typed base from `src/ygo_types.h`:

```c
extern FadeTransitionState gFade_State;
```

It also preserves the only same-address raw linker alias:

```c
extern u8 D_800E9EC8_arr[FADE_TRANSITION_STATE_SIZE];
```

Matching pure-C users migrated to this shared header include:

- `func_800151B0`, `func_800151D8`, `func_80015310`, `Fade_DrawOverlay`,
  `func_800156B8`, `func_800156DC`;
- `func_8001572C`, `Fade_InitIn`, `Fade_StartIn`;
- `Fade_InitInColor`, `func_80015870`, `Fade_InitOut`;
- `Fade_StartOut`, `Fade_InitOutColor`, `Fade_Wait`;
- `func_80015A50`, `func_80015A94`, `func_80015B50`, `func_80015B94`;
- `func_80015BD8`, `func_80015BF0`;
- `func_80015C0C`, `func_80015C48`, `func_80015C84`, `func_80015CC0`.

The colour initializers and their blocking wrappers now carry an explicit
`s32 color` parameter, replacing the old reliance on an incoming `$a0`.
`func_80015C84` deliberately keeps an unspecified parameter list: one exact
caller passes the white-mode byte and another passes nothing, while the
callee consumes neither form. A stricter invented parameter would make one
of those known call sites false.

The later exact pure-C matches for `func_800151D8` and `func_80015310`
removed the band walker and transition updater from the assembly exception
list. Both include `fade.h`; the updater retains the raw views described
below for exact addressing.

## Exact-code exceptions

The remaining assembly exception documented here is `func_800218F0`, which
reads `flags`, writes `level`, and calls the band fill during its larger
assembly-only flow.

There are no matching inline-assembly users of the base global in this phase.

Some migrated C deliberately retains raw expressions without retaining local
competing declarations:

- `Fade_DrawOverlay` keeps `D_800E9EC8_arr` as the pointer passed to
  `func_80015310`, while the tail uses typed `gFade_State` fields. The two
  same-address symbol views preserve the target's fresh address
  materialization in the exact draw-screen-fade implementation.
- `func_80015310` keeps its `u8 *` parameter and the offset-symbol array
  `D_800E9ECC[]`; the latter preserves absolute `%hi/%lo` addressing for the
  final level store instead of the small-data form. Its byte definitions
  for `D_8009B142`, `D_8009B143`, and `D_8009B144` are assembler-addressing
  controls, not additional fields of `FadeTransitionState`.
- `func_800156B8` casts the typed base to a byte pointer and keeps
  `*(p + i + 0xA)`. The equivalent array-member expression changes the MIPS
  `addu` operand order and does not match.
- `func_8001572C`, `Fade_InitInColor`, `func_80015870`, and
  `Fade_InitOutColor`
  retain explicit `s32` stores through the typed base because target assembly
  proves one whole-word write at offset zero before byte field accesses.

`D_800E9ECC` and `D_800E9ECE` are offset symbols at `+0x04` and `+0x06`, not
same-address aliases. Remaining assembly users of those symbols are unchanged.
`Fade_DrawOverlay` also retains its oversized raw `D_800E9ECE` declaration
because the target independently materializes that address; folding it into
the typed base would change exact code generation.

## Validation

The migrated source retained the complete executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`
with `MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also pass after regeneration of the tracked usage
reports.
