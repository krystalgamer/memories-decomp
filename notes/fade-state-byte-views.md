# Independent fade flag and step byte views

## Existing aggregate fields, independently labeled consumers

| Original NA identity | Canonical view | Existing aggregate field |
|---|---|---|
| `D_800E9ECE` | `gFade_bFlags` | `gFade_State.flags`, offset `+0x06` |
| `D_800E9ECF` | `gFade_bStep` | `gFade_State.step`, offset `+0x07` |

The existing `FadeTransitionState` assertions establish both field offsets.
Tracked numeric bindings independently establish the same relationship in
every resident image:

| Image | Aggregate | Flags | Step |
|---|---:|---:|---:|
| NA | `0x800E9EC8` | `0x800E9ECE` | `0x800E9ECF` |
| Japan | `0x800E9DA8` | `0x800E9DAE` | `0x800E9DAF` |
| UK | `0x800EAE28` | `0x800EAE2E` | `0x800EAE2F` |
| Other PAL | `0x800EB248` | `0x800EB24E` | `0x800EB24F` |

These are names for existing external interior labels, not new storage,
members, arrays or allocations. The aggregate type, grouped fade runtime,
typed member accesses, COMMON tint definitions and array-base alias remain
unchanged. External consumers keep independently materialized labels rather
than being folded into member expressions.

## Flags are not a Boolean

`Fade_Update` tests and clears `FADE_FLAG_ACTIVE` through the existing
aggregate flag field. Scene, model, library, script and overworld users
wait on bit `0x80` through the independent byte label. The canonical name
covers the full flag byte, including the other existing fade masks; it
does not replace that byte with a Boolean or normalize any value.

`Script_OpFadeOut` retains its original local byte-pointer view and busy
handling. The command byte's bit `0x80` still selects whether to wait;
the fade byte's bit `0x80` still controls completion on later calls.
No new wait, timeout, progress guarantee or clearing behavior is added.

## Step is not a duration or guaranteed fixed speed

The updater multiplies the stored step by its existing volatile base-frame
factor before ordinary level movement. In/out initializers retain step
values `0xC` and eight; external callers retain overrides of two and six.
The script command retains its nonzero-low-six-bits override after
`Fade_InitOut`. A zero operand still keeps the initializer's existing step.

All banded branches, clamping, frame-factor reads, calls, stores and
completion tests remain unchanged. The name does not imply FPS, elapsed
time, a new unit conversion or identical timing across banded and
non-banded paths.

## Two independent C selectors

The narrow canonical header follows the old `unmatched.h` owner in six
root sources. It retains both independent selectors:

- `D_800E9ECE_AS_SCALAR` selects `u8 gFade_bFlags`; the default remains
  incomplete `u8[]`.
- `D_800E9ECF_AS_SCALAR` selects `u8 gFade_bStep`; its default independently
  remains incomplete `u8[]`.

The incomplete arrays are measured addressing views, not declarations
of a larger allocation. No bound, qualifier, pointer conversion, shared
selector or first-include change is introduced. The old owner and all
regional wrappers stay unchanged.

Actual CPP and named-profile objects cover 67 source/profile contexts
and 385 registrations, including both overworld variants in every image.
The canonical declarations retain 106 scalar views and 28 incomplete-array
views. Twenty-nine imports are active: sixteen flags imports and thirteen
step imports. The other 105 declarations remain unused, with no fabricated
imports or storage definitions.

## Native providers and compatibility

Twenty physical linker files serve the seven residents and fourteen
overworld images; NA before/after share one linker file. Canonical names
point to the existing providers and preserve all accepted linker prefixes.
The automatic NA overlay providers use conditional `PROVIDE` aliases,
retaining production automatic resolution without eagerly introducing
dependencies into unrelated standalone import links. No numeric provider
is guessed or pinned.

The distinct French `D_800E9ECE_French` and `D_800E9ECF_French` shadows at
NA-number addresses remain untouched. All existing tests, physical
ownership controls, compiler profiles, source groups and generated storage
remain unchanged. No new harness, fixture, probe or testcase is added.

See [fade-transition-state.md](fade-transition-state.md) for the complete
aggregate contract and unchanged transition behavior.
