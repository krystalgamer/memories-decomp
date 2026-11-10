# Text channel ownership and completion acknowledgement

The four records initialized by `DuelEffect_InitEntry`, reset by
`DuelEffect_ResetState` and pumped by `TextBox_UpdateChannels` are
`gText_aChannels`. This is the existing channel pool, not a new allocation.
`text_channel_state.h` provides canonical declarations without changing any
existing header or frozen overlay C.

## Native views and storage

The North American pool starts at `0x800EB0F8`. The complete channel records
are `0x64` bytes in North America and Europe and `0x60` bytes in Japan.
`DuelEffectChannel` in `src/ygo_types.h` carries both shapes: under
`VERSION_JAPAN` its tail after `line_advance_y` is the byte-sized
`entry_range_start`, `entry_range_count`, `output_count` and `output_limit`
(`0x5C`-`0x5F`), and the North American/PAL `field_62`/`pad_63` do not
exist. Every Japanese unit that touches the pool therefore defines
`VERSION_JAPAN`, and native strides follow `sizeof(DuelEffectChannel)`.

Canonical includes follow the original selector-dependent declarations.
Moving `duel_effect.h` ahead of existing per-source selector macros can change
old-GCC external-symbol directives even when the C instruction text looks
unchanged; complete compiler and MASPSX output must both be preserved.

Channels 1-3 are reached through `TEXT_CHANNEL(index)` in `duel_effect.h`,
which is `&gText_aChannels[index]` with the regional record size. The
constant offset folds into the
relocation exactly as the former per-channel names (`D_800EB15C`,
`D_800EB1C0`, `D_800EB224`, `gText_Channel1`, `gText_Channel3`) did, so no
separate channel symbol is defined in any image.

Channel 0's word at pool offset `0x34` (formerly the separate
`D_800EB12C`/`gText_dwChannel0State` names) is read as
`*(u32 *)&gText_aChannels[0].flags_34`: the low halfword is the channel flags
and the high halfword is its string ID. No halfword load replaces the
original word loads or masks.

Each region's text-box unit defines `gText_aChannels` through the shared
`src/game/duel_effect_text.c` as `DuelEffectChannel[4]`; in Japan that is
`0x180` bytes at `0x800EAFD8`. The literal `D_800EB0F8` address is channel 3
(`0x800EAFD8 + 3 * 0x60`). Overlays bind only `gText_aChannels` itself.

The PAL password channel 2/3 imports (formerly `D_800F04F8`/`D_800F055C`)
are the same pool elements; see the
[channel-control views](text-channel-control-views.md).

## Completion acknowledgement

Secondary text command `0x16`, formerly `func_800389C4` at North American
`0x800389C4`, is `Text_DisableCompletionWait`. It clears channel flag `8` in
North America/Japan or `0x10` in Europe. It does not consume an operand,
change the done bit, advance a frame, destroy an object or yield the current
text pass.

The flag's owner is visible in `TextBox_UpdateChannels`
(`src/game/duel_effect_text.c`). When building first reaches done with this flag
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
`duel_effect_text.c` group. Function order, sizes, compiler
profiles and every sibling body remain unchanged, including Europe's extra
player-name stream command.

Resident consumers use the canonical names. Existing controllers receive only
the same data-identifier substitutions in their existing stubs and assertions.
All pre-existing headers and overlay C remain byte-identical. Validation
compares complete ordinary compiler/MASPSX output and actual native
translation units, retaining zero substitutions for frozen outputs, and
requires a clean full-executable match. Regional/overlay full-image acceptance
remains on the PR checks; local source probes do not replace it.

## Playback cadence ownership

playback_state, delay_countdown and glyph_delay replace the original
state_51/delay_52/delay_reload_53 names without changing the byte fields,
the two existing offset assertions or either regional layout. The shared
NA/PAL channel and compact Japanese view keep glyph_delay at +0x53.

DuelEffect_InitEntry stores a reload of two in NA/Japan and one in PAL;
it does not gain initialization stores to playback_state or the countdown.
Library_UpdateCardView, the existing duel-effect card-view paths and the
password shop message helper retain their explicit reload-of-one writes,
including all original regional guards, creation flags and cleanup.

State producers include the raw stream byte, wrap/page/choice transitions,
sound waits and the existing narrow EffectObject.state view. Consumers
retain full-byte activity before masked dispatch, callback-specific latch
tests, first-call early returns, shared script/media/viewport cells and
their original completion boundaries. No state enum, validation, renamed
callback table, per-channel wait allocation or new cadence control is
introduced. The entry handler/render state bytes are separate owners.

The existing library layout fixture changes only four member identifiers
for its +0x53 offset/size probes. Every numeric expectation, assertion,
scenario, mutation and control remains byte-for-byte the same. It is not
executed locally; ordinary PR validation remains authoritative.
