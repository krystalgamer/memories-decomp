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

Existing scene-transition and debug-controller behavioral witnesses retain their complete bytes:
the former protects the upper half while exercising the low credits bytes,
and the latter protects the low bytes while editing height. Only the
corresponding member identifier changes in the existing debug ownership
probe; its widths, positive/negative cases, diagnostics and runner remain
identical. No new harness, fixture, mutation or callback model is introduced.
`ModelSlot.field_E15` remains unnamed because its consumers mix one-shot
latches, a `0xFF` sentinel and credits counter behavior.
