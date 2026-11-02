# Fade flag and step bytes

The fade flags and step are the `flags` (`+0x06`) and `step` (`+0x07`)
members of `gFade_State` (`FadeTransitionState`):

| Image | `gFade_State` | Flags | Step |
|---|---:|---:|---:|
| NA | `0x800E9EC8` | `0x800E9ECE` | `0x800E9ECF` |
| Japan | `0x800E9DA8` | `0x800E9DAE` | `0x800E9DAF` |
| UK | `0x800EAE28` | `0x800EAE2E` | `0x800EAE2F` |
| Other PAL | `0x800EB248` | `0x800EB24E` | `0x800EB24F` |

They used to carry their own interior labels (`D_800E9ECE`/`gFade_bFlags`
and `D_800E9ECF`/`gFade_bStep`), declared as incomplete `u8[]` arrays in the
-G8 units and as scalars in the overworld overlay through two selectors. That
was a second identity for each member, not a separate object. Every consumer
now accesses the members directly. A relocation against `gFade_State+6`
links to the same bytes as one against the old label, and the -G8 units
need no array spelling because the aggregate is larger than the small-data
threshold.

`Script_OpFadeOut` had kept a local byte pointer to the flags label. With
the members it is written plainly; a pointer to `gFade_State.flags` would
let GCC reach the step as `base + 1` and hold the base across the call.

## Flags are not a Boolean

`Fade_Update` tests and clears `FADE_FLAG_ACTIVE` through the flag member.
Scene, model, library, script and overworld users wait on bit `0x80`. The
member covers the full flag byte, including the other fade masks.

## Step is not a duration or guaranteed fixed speed

The updater multiplies the stored step by its volatile base-frame factor
before ordinary level movement. In/out initializers use step values `0xC`
and eight; external callers override with two and six. The script command
overrides with its nonzero low six bits after `Fade_InitOut`.

## Overlay linking

The overworld overlays bind `gFade_State` in each region's
`overworld*_linker_symbols.txt` (NA through a `PROVIDE`).

See [fade-transition-state.md](fade-transition-state.md) for the complete
aggregate contract.
