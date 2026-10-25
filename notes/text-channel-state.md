# Text channel ownership and completion acknowledgement

The four records initialized by `DuelEffect_InitEntry`, reset by
`DuelEffect_ResetState` and pumped by `TextBox_UpdateChannels` are
`gText_aChannels`. This is the existing channel pool, not a new allocation.
`text_channel_state.h` provides canonical declarations without changing any
existing header or frozen overlay C.

## Native views and storage

The North American pool starts at `0x800EB0F8`. The complete channel records
remain `0x64` bytes in North America and Europe and `0x60` bytes in Japan.
Every existing native stride, cast, field access and declaration arm remains
unchanged.

Canonical includes follow the original selector-dependent declarations.
Moving `duel_effect.h` ahead of existing per-source selector macros can change
old-GCC external-symbol directives even when the C instruction text looks
unchanged; complete compiler and MASPSX output must both be preserved.

`gText_Channel1` retains the separately addressed second-record view formerly
named `D_800EB15C`. It is not replaced with an array expression: the original
callers depend on how the base and offset reach the compiler.

`gText_dwChannel0State` retains the existing word view at pool offset `0x34`:
the low halfword is the channel flags and the high halfword is its string ID.
The existing `D_800EB12C_IN_DATA` selector still chooses the `.data`
declaration used by `Script_RunTick`. No halfword load replaces the original
word loads or masks.

Each image's linker aliases refer to its original providers rather than
allocating replacement storage. In Japan the canonical pool aliases
`gJapanese_D_800EB0F8`, at `0x800EAFD8`, **not** the literal `D_800EB0F8`
address. The Japanese quit path's existing assembly-named byte view still
addresses channel 3 at that latter address. The channel-1 and channel-0-word
aliases retain their existing native providers independently.

The PAL password imports `D_800F04F8` and `D_800F055C` were outside this
pass's naming scope. The later
[channel-control views](text-channel-control-views.md) identify them as
pool elements 2/3, not standalone allocations, and provide the regional-only
`gText_Channel2View`/`gText_Channel3View` source aliases. Existing numeric
declarations, native bindings and overlay imports remain available. No
historical dependency fingerprint is refreshed.

## Completion acknowledgement

Secondary text command `0x16`, formerly `func_800389C4` at North American
`0x800389C4`, is `Text_DisableCompletionWait`. It clears channel flag `8` in
North America/Japan or `0x10` in Europe. It does not consume an operand,
change the done bit, advance a frame, destroy an object or yield the current
text pass.

The flag's owner is visible in `TextBox_UpdateChannels`
(`src/game/text_box_update_channels.c`). When building first reaches done with this flag
set, it creates the existing completion prompt with `Dialog_OpenChoice`.
On a later update, confirmation clears the flag, releases that prompt and
plays sound `0xB`. `Script_RunTick`, the memory-card result path and
`BuildDeck_UpdateExitConfirmation` test their existing completion masks for
done set **and this flag clear**. `Script_OpShowDialog` sets the flag by
default and clears it for its existing high-bit operand option.

The command disables that acknowledgement wait; it does not itself perform
confirmation cleanup. The distinct choice-input suppression flag is `4` in
North America/Japan and `8` in Europe and is not renamed or changed.
All raw literals, regional selectors, prompt timing, button masks and sound
calls remain as before.

## Preservation boundary

The completion command remains inside the complete existing
`duel_effect_object_commands.c` groups. Function order, sizes, compiler
profiles and every sibling body remain unchanged, including Europe's extra
player-name stream command.

Resident consumers use the canonical names. Existing controllers receive only
the same data-identifier substitutions in their existing stubs and assertions.
All pre-existing headers and overlay C remain byte-identical. Validation
compares complete ordinary compiler/MASPSX output and actual native
translation units, retaining zero substitutions for frozen outputs, and
requires a clean full-executable match. Regional/overlay full-image acceptance
remains on the PR checks; local source probes do not replace it.
