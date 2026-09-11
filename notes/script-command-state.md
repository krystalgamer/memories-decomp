# Script command runtime contracts

`src/game/script_state.h` owns the script engine's command state and its
text-channel entry views. `src/game/menu_record.h` owns the selected menu
record pointer because its complete observed target type already lives there.
These declarations describe accesses, not C storage allocation.

## Evidence and boundaries

`Script_RunTick` fetches the command through `D_8009B290` and dispatches the
low five bits of `D_8009B27C`. `func_8003767C` and `func_8003771C` in
`duel_effect_state_callbacks.c` install commands 5 and 7 through the same
globals, and `TextBox_BuildStep` dispatches them until the command clears.
Thus the text-channel views belong to the same contract as the script
handlers, not independent copies of the state.

The following access forms remain intentional:

| Consumer | Shared-header selection | Preserved form |
| --- | --- | --- |
| Existing script handlers | Default | Unsigned scalar command, image operand, duration and viewport targets |
| `duel_effect_state_callbacks.c` | `SCRIPT_STATE_TEXT_CALLBACK_VIEWS` | Unsized unsigned arrays; signed `.data` duration and scalar aliases |
| `text_box_build_step.c` | `SCRIPT_STATE_COMMAND_IN_DATA` | Unsigned `.data` command scalar |

`Base2_8009B2A8` and `Base2_8009B2AA` remain separate linker identifiers at
the target halfwords' addresses. Their stores occur after calls that the
first `D_8009B2A8/AA` stores precede. Do not replace the Base2 identifiers
with C macros or remove the existing scalar `asm` aliases: address retention
and absolute versus small-data addressing are compiler inputs here.

`Script_UpdateViewportTween` initializes `D_8009B284/288` from the current
viewport in 16.16 form, computes `D_8009B294/298` by dividing the target
distance by the duration, and adds those deltas on each frame. The signed
reads used for interpolation and unsigned final halfword copies remain
explicit. These four words do not become one struct: the separately used
command latch and script cursor lie between the two pairs.

`func_8002F968` seeds signed countdown `D_8009B278` from two script bytes and
clears the command when it reaches zero. `Script_OpShowImage` and
`func_8002F630` store a freshly allocated `DisplayObject *` in `D_8009B280`
and later release it through `func_8004036C`. The forward declaration in
`script_state.h` uses the existing `struct DisplayObject` tag; it does not
create a second object layout or import unrelated guarded graphics globals.

## Typed menu command

`func_8002EB78` selects element zero or one of `D_800EB010` using bit 7 of
its second operand. This is the same `0x4C`-byte `MenuRecord` array walked
by `func_80039E9C` and `DuelEffect_MarkObjectIfActive`, not an inferred new
allocation. Its retained pointer `D_8009B274` now has type `MenuRecord *`.

Seven accesses use the existing members rather than raw byte offsets:
the signed marker at `+0x30`, writes at `+0x31`, `+0x30`, `+0x3C`, `+0x33`
and `+0x34`, and the later `+0x33` completion poll. Compile-time assertions
cover each accessed offset and the unchanged record stride. The signed
marker load, byte stores and halfword store are preserved. Calls into the
older `u8 *` and `DisplayEffectState *` lifecycle interfaces retain explicit
boundary casts; changing those APIs is not required to type the record.

## Migration and exactness

Tick392 removes 19 private declarations across seven resident translation
units:

| Source | Declarations moved |
| --- | ---: |
| `duel_effect_state_callbacks.c` | 10 |
| `text_box_build_step.c` | 1 |
| `script_update_viewport_tween.c` (now the `Script_UpdateViewportTween` candidate) | 4 |
| `script_control_commands.c` | 1 |
| `script_op_show_image.c` | 1 |
| `func_8002F630.c` | 1 |
| `func_8002EB78.c` | 1 |

All current matching and candidate sources were searched for these fourteen
linker identifiers; none of the nineteen integrated candidate sources needs
a declaration migration. The narrower resident linker-symbol diagnostic
drops from 99 headerless names / 159 sites to 95 / 155; it does not count all
the moved declarations, including names which already had a shared default
view.

The complete resident image matches after the guarded declarations and typed
menu accesses, with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
No compiler profile, tentative definition, grouped translation unit, assembly
body, register pin, relocation identity or data mapping changes. Candidate
source and fingerprint metadata remain unchanged.

Source-reclassification work must carry the selected header arms if these
functions move to candidate sources. Frontend mode bytes and live save
workspace contracts are separate campaigns, even where their declarations
share a consumer.
