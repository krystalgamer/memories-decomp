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
