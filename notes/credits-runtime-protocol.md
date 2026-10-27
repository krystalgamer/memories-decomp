# Credits module views and resident handshake

The resident credits controller loads the North American SU credits package
from sectors `[1223, 1239)`, initializes it, starts group zero and later
advances its lines. The module's matching implementations support these
scoped C names:

| Native entry | NA credits C view | Existing ABI |
|---|---|---|
| `func_801807B0` | `Credits_InitPresentation` | `void(void)` |
| `func_80180A24` | `Credits_UpdateLines` | `s32(void)` |
| `func_80181C4C` | `Credits_StartLineGroup` | `void(s32 group)` |

## Module behavior

`Credits_InitPresentation` retains all three 16-entry CLUT values, separate
GPU wait/upload retry loops, two line-pointer clears and finished-byte reset.
It does not start a group itself.

`Credits_UpdateLines` advances and draws the two occupied lines through
fade-in, hold, fade-out, blank/page advance and following-group handoff.
A held line whose first row has group `0x63` sets `gCredits_bFinished`;
the function returns that sticky byte. It is not a side-effect-free poll.
The existing graphics base-step reads, signed timers, brightness arithmetic,
draw order and line-release behavior remain unchanged.

`Credits_StartLineGroup` scans the two lines in order and attempts layout
on the first free line. Successful layout retains the original placement,
centering, state, timer and auxiliary-byte reset. It returns after that
first free line even if layout returns zero; both-busy retains the existing
no-op behavior. No deferred queue or success result is introduced.
Its source remains contiguous with `Credits_IsFinished` in `lines.c`.

## The resident's two independent bytes

`ModelDebugState.fields.credits_update_enabled` is byte zero of the existing
`D_8009B004` word. The resident's initialization phase clears it. The controller increments
it from zero after the existing sound-status `0x80` bit test and first-model
activity check; nonzero enables the module update.

`credits_exit_requested` is byte one, separately cleared by that resident phase.
After the module update returns nonzero, the controller still requires the
exact signed-halfword sound status to differ from `128` before incrementing
this byte and selecting phase two. This is not an inverse bit test.
The byte also suppresses further peer-model loading and selects the ending
path when the active model's presentation sequence ends. It is not an alias
for the module's own finished byte.

Both members remain `u8`, including their original guarded increments and
all callback/status reloads. The upper half retains its independent signed
debug-height and unsigned-update views. Neither the whole `D_8009B004`
record nor its `ModelDebugState` type is semantically renamed.

## Scope, native identity and conservation

The NA module's canonical prototypes use `__asm__` to retain the original
native entry names, function types and sizes. Explicit legacy prototypes
remain available. The module inventory records the scoped semantic names;
its Splat symbol file, matching manifest, compiler profiles, source order
and all native providers remain unchanged. No linker aliases are added.

The resident deliberately keeps the address-based declarations and calls
in `model_scene_imports.h`. Other packages reuse this address range;
in particular the main-menu package is not the credits package. The
Japanese and European resident imports likewise keep their existing
bindings. The NA private header is not substituted into those callers,
and these names do not qualify an uninspected regional implementation.

That earlier handshake component preserved its then-existing scene-transition
and debug-controller witnesses: one protected the upper half while exercising
the low credits bytes; the other protected those low bytes while editing
height. Its ownership probe changed only the member identifier. These are
historical observations, not a request to restore retired gameplay fixtures
or a claim of new fixture/cloud execution.
`ModelSlot.field_E15` remains unnamed because its consumers mix one-shot
latches, a `0xFF` sentinel and credits counter behavior.


## Resident Exodia and credits local views

Three complete resident definitions now name existing local/input views:
Model_UpdateExodiaPresentation (NA 0x8004FE2C),
Model_UpdateCreditsMonsterSlot (0x80050584), and
Model_UpdateCreditsPresentation (0x800507D0). No function identity changes.

The Exodia controller retains camera_offset, primary_slot and both module
entry captures BEFORE the frame-step override call. slot0_module_handler
and slot1_module_handler name the addresses from which those callbacks are
formed, not exclusive target slots. In particular, the second handler's
poll STILL sets active slot zero and forwards the slot1 data address.
Its completion path still stops slot0 playback, cuts to white and selects
the original phase. Callback types, casts, argument values, live data
reads and one-shot/sentinel gates remain distinct and in original order.

The load_slots base and later tint_slot pointer remain separate lexical
bindings. Keep both initialization-handler calls, the two field_E15 clears,
all transfer/readiness gates and the signed tint test. Camera offsets retain
their original eight-byte copy and per-phase component writes; no zeroing,
fourth-halfword interpretation or cached-pose policy is added. stop_position
captures the original duration-row threshold BEFORE Model_GetFrameStep,
then still subtracts the later live playback_speed times frame_step.
The position test and playback-stop call are unchanged.

The credits loader distinguishes its slot_index definition argument,
initial_base/initial_slot, later ready_base/ready_slot and
peer_slot/peer_coordinate. Preserve the signed unchecked index, OR80 load
argument, transfer tests, rand() rejection loop, all card/model gap tests,
and the original model_id narrowing/domain. The shared t scratch STILL
holds random input THEN biased yaw; m1 STILL supplies the same minus-one
binding to four properties. Neither is split or assigned a false single
purpose. yaw_work retains the initial quarter turn, quotient THEN signed
remainder reconstruction, optional peer coordinate and original order.

The ready slot's coordinate writes remain under the null test, but the
following Model_UpdateCoordinateUnitMatrix call STILL occurs OUTSIDE that
test. Model_RunSlotHandlers and the field_E15 clear follow at their original
sites. No null repair, stronger coordinate ownership, input validation,
random-uniformity or loop-termination guarantee is introduced.

The credits controller distinguishes load_check_base, crossfade_base,
active_lookup_base and fade_slot_base without merging their read sites.
active_slot_words remains a raw s32 pointer, not a new ModelSlot local or
changed cast/stride. slot_tick_count is the existing unsigned snapshot of
field_E15 used by modulo/division and animation_row selection; it is NOT a
universal frame duration or handler-state name. peer_slot_index retains the
live byte index and XOR1. slot0_tint and slot1_tint retain separate aliases.

All three random color samples, signed byte steps, low-two-bit gates,
904*index THEN *4 forms, separate byte-offset form, timer wrap, fade/crossfade
stores, module/sound polls and repeated exit/update-enable byte reads stay.
The shared tick label still increments the LATER LIVE active slot; it is
not replaced with the earlier captured pointer or made unconditional.
The deliberately mixed result scratch, phase-value/next-phase bindings,
labels and goto structure remain. Numeric resident module imports remain
numeric, with their original per-package ABI; no NA credits name is imposed
on the other packages or regional implementations.

Only scoped bare locals and the loader's definition argument change.
All C comments, members, types, declarations/order/lifetimes, casts,
operators, constants, labels, calls, other grouped functions, private
tables, regional macros, public headers, profiles and storage remain.
The entire semantic ledger stays byte-identical: this component promotes
no function/global/member identity and adds no NOTES churn. No fixture,
control, oracle or mutation change, retired-fixture revival, local cloud
execution or synthetic/generated-C/source-string behavioral harness.
