# Packed dialog runtime controls

## Choice command, layout and confirmation control

`gDialog_bChoiceControl` names the existing `u8 D_8009B34C` view shared
by the text-choice producer, layout helpers, confirmation callback and
script save prompt.

`Text_HandleChoiceCommand` stores the command byte masked with `0xF0`
in its ordinary prompt branch. The command's bit-`0x80` branch instead
uses the selected offset and does not take that store path. The script
save prompt separately clears the control byte.

The choice display-object layout reads bits `0x30` for the existing
entry-specific and regional row-displacement calculations. The North
American paths retain their distinct divisions, multipliers and signed
locals; Japanese and European paths retain their original shifts,
spacing and wrapper-defined row step. `Text_TryCompleteChoiceLayout`
uses the same masked field after layout completion and keeps the
existing choice-count store of two when the field is nonzero.

`Dialog_UpdateChoice` tests bit `0x40` after accepting an enabled choice.
When that bit is clear, the existing tail releases and nulls the
highlight object and stores channel state three. Naming the byte adds
no cleanup, lifetime guarantee or new interpretation of other bits.
The full byte is a packed control, not a Boolean, row index or
release-only flag. All literal masks, callbacks, repeated reads and
state-store ordering remain unchanged.

The canonical declaration retains the original plain `u8` or
`D_8009B34C_IN_DATA` `.data` selection. The old owner in `unmatched.h`,
its selector spelling and its first-include behavior remain unchanged.

## Shared callback state, not work-only or per-record state

`gDialog_bCallbackState` names the existing plain `u8 D_8009B3C1`.
Its users span the generic dialog transitions, memory-card work
controller and prompted/unprompted load, save and trade modal callbacks.
The name groups those already shared callback uses; it does not assign
exclusive ownership to the work controller or introduce a new lifecycle.

The work controller dispatches `gDialog_apfnTransitionSteps[state & 0xF]`
when the low nibble is nonzero, retaining the existing precedence over
ordinary controller advancement. It writes selectors one and two for
the open and close paths. The mask is not proof of sixteen valid
callbacks: the existing five-entry caller view and two-argument call
signature remain unchanged.

Bit `0x80` is the first-entry latch in the generic transition and modal
step users. Individual transition callbacks interpret `0x40` and `0x20`
through different existing branches; no common enum or universal
completion meaning is introduced. The contraction callback still
clears on a later invocation after its endpoint latch, combined open
retains its prompt handshake, and message-only open retains its
separate completion test.

`MemCardWork_InitializeOperation` and `MemCardDialog_Start` still clear
the shared byte. It remains distinct from `gMemCardWork_OperationState`,
`gMemCardDialog_OperationState`, `gMemCardDialog_StepIndex` and
`MenuRecord.display_effect_step`. No callback ABI, dispatch bound,
initialization order, polling, timeout, state normalization or error
handling changes.

See [dialog-transition-family.md](dialog-transition-family.md) and
[memory-card-runtime.md](memory-card-runtime.md) for the preserved
transition and modal behavior.

## Native and compatibility boundaries

Both canonical names alias the original external provider in each of
the seven images. All fourteen bindings have tracked numeric evidence;
the original numeric assignments and legacy symbols stay unchanged.
The old declaration-owner headers and regional wrappers remain frozen.
Narrow canonical headers are included only in the corresponding
actual consumers, after their existing owners.

Actual compiler-expanded declarations and named-profile objects cover
26 source/profile contexts and 302 registrations. Each role has
thirteen active canonical declarations/imports. Three choice-control
views retain `.data`; the others retain plain scalar declarations.
Twenty-two unused original declarations stay legacy-only, and four
absent choice-control declaration/reference cases remain absent.

The existing memory-card controller witness receives only the complete
callback-state identifier migration. Its unchanged source-prefix
extraction picks up the new header automatically. Cases, assertions,
mutations, fixture layouts, generator and oracle behavior are unchanged;
no new harness, fixture, probe or testcase is added.

The Password overlay's raw `D_8009B34D` choice-index view remains
deliberately separate. That byte is already `gDialog_bChoice` in the
resident tree; it is not another unknown symbol or part of either new
packed control.

## Script save-prompt command phases

`SCRIPT_SAVE_PROMPT_CONTROL_*` names seven existing bits of
`gScript_wCommandWord` only while `Script_OpSavePrompt` owns the command.
These are not text-channel flags or universal meanings for the shared
script command word. Each definition retains its original unsuffixed
literal; the source changes only seventeen literal occurrences.

| Bit | Control | Existing producer and consumer |
| --- | --- | --- |
| `0x80` | `CARD_DIALOG` | Choice zero requests a save, then selects the early `MemCardDialog_Poll` branch. |
| `0x200` | `DECK_ERROR` | The zero-entry deck predicate fails; the later branch opens text `0x1C`. |
| `0x400` | `EXIT_CONFIRM` | Choice two selects the channel-two confirmation using text `0x12`. |
| `0x800` | `SUBPHASE_STARTED` | Shared first-entry latch for exit confirmation, deck error and slide-out. |
| `0x1000` | `SLIDE_OUT` | Choice three selects the slide-out that eventually destroys channel three and clears the command. |
| `0x2000` | `SLIDE_IN` | The existing `0x6000` store selects channel three's entrance animation. |
| `0x4000` | `FOLLOWUP_PHASE` | Set after initial channel readiness and inactive-entry checks, before testing the deck or creating the choice box. |

These bits select work, not successful completion. In particular, polling
zero retains `CARD_DIALOG`; any nonzero poll result restores four choices,
selects zero, clears the separate choice control, and applies the original
`0xFF7F` mask. There is no new distinction between save success, cancellation
or failure, and no rollback of the live-workspace scene stores.

The initial readiness test still examines the channel's distinct `0x2000`
flag and its active effect entry. `FOLLOWUP_PHASE` is published before the
deck check and all following UI work. `SUBPHASE_STARTED` likewise precedes
its existing box creation or position-save calls. It is one shared latch,
not three independent flags or an assertion that those calls succeeded.

Exit confirmation still clears exactly `0xC00` through the literal
`0xF3FF` mask, destroys its prompt, and takes the existing main-mode handoff
only for a nonzero choice. The deck-error branch retains its regional
completion test, including its original polarity. Slide-in still clears
only `0x2000` through `0xDFFF`; slide-out still clears the entire command
only after its signed endpoint test. The mixed `0x6000` store and all three
clear masks remain literal and unchanged.

Preserve branch priority, the first-invocation return, blocking box-build
loops, callback/store ordering, signed halfword animation reads, narrowing,
position aliases, and the load-bearing one-pass choice loop. Both scene
workspace writes and the build-deck-only byte narrowing remain intact.
Japanese channel stride, button mask and box geometry, and the European
helper binding and completion flags, retain their original wrapper paths.
No dispatch bound, valid-choice guard, enum, shared decoder, initialized
table, provider declaration, compiler profile or executable control changes.
