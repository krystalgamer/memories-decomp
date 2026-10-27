# Frontend CD-BGM and saved script commands

This bundle names two existing parts of the frontend BGM command interface:
`func_8003FFD8` becomes `SD_CdBgmPlay`, and the word at `D_8009B404`
becomes `gSD_dwSavedBgmCommand`. It changes neither playback behavior nor
the command drivers, script dispatch, data storage or recovered declarations.

## CD-BGM submission

`SD_CdBgmPlay(u32 value)` retains the exact expression
`SD_BGMPlayDefaultStep((value & SD_COMMAND_VALUE_MASK) | 0xA000)`.
The lower entry masks to sixteen bits, forwards the driver's default step,
and calls `SD_BGMPlayWithStep`. Bit 15 selects that function's CD path:
stop the non-CD BGM through `SD_BGMStop(SD_BGM_COMMAND_BASE)`, then call
`SD_EnqueueCdPlay`, rather than `SD_RequestBgmPlayback`.

`SD_EnqueueCdPlay` checks the existing source flags, selects its command
table from the high nibble, and submits command `0x24`, resets CD pan and
updates the driver flags. `SD_StepActiveCommand`'s command-36 arm reads the
selected entry's sector/channel information and requests its sector range
through `File_RequestSecondarySlot`. This is the BGM-facing CD submission
wrapper, distinct from the neighboring `SD_CdSoundPlay` SE wrapper.

The input remains all sixteen low bits followed by **bitwise OR** `0xA000`.
It is not a normalized twelve-bit index, and other incoming class bits can
remain set. Neither the high nibble nor the selected table/index is
strengthened beyond the actual decoder. The forwarded default step is
unused by `SD_EnqueueCdPlay`; no fade-in or timing meaning is invented.
Existing gates and queue behavior remain, so a call is not a guarantee of
audible playback or successful enqueueing. Unlike `SD_BGMPlay`, the wrapper
does **not** publish `gSD_dwCurrentBgmCommand`. No named caller, original
gameplay purpose or unreachability claim is asserted.

## Saved command word

`Script_OpSound` establishes the saved-word role. When its low six command
bits are nonzero, it performs these operations in order:

| Mask | Operation |
| --- | --- |
| `0x01` | Pass the saved word to `SD_BGMPlay` |
| `0x02` | Read an explicit little-endian two-byte command into the word |
| `0x04` | Copy `gSD_dwCurrentBgmCommand[0]` into the word |

The word is not the current-command tracker: a script can save an explicit
command instead of a snapshot. Combined masks preserve replay, explicit
save, then snapshot ordering. Stream advances, busy handling, masks and
all thirty-two-bit accesses remain unchanged.

`DuelEffect_ProcessBgmCommand` contains corresponding expressions, but its
outer low-six-bit test has the opposite sense. Those inner `0x01/0x02/0x04`
arms are under the branch that requires all three bits to be clear. They
are not evidence of live duel-effect save/replay behavior. This naming
change deliberately preserves that pre-existing control flow rather than
repairing or interpreting it.

The old `sound.h` stays frozen, including the `D_8009B404_IN_DATA` selector.
The isolated canonical header mirrors its `.data`/plain `u32` declaration
arms; both saved-word consumers retain the `.data` arm. Numeric root
providers remain, with canonical aliases rather than new storage.

## Native bindings and preservation

| Image | CD-BGM wrapper | Saved command |
| --- | --- | --- |
| North America | `0x8003FFD8` | `0x8009B404` |
| Japan | `0x8003F6B8` | `0x8009B2F4` |
| SLES_039.47 | `0x800400FC` | `0x8009C360` |
| Other PAL images | `0x800402FC` | `0x8009C780` |

All seven wrapper records remain game-owned matching C, `0x24` bytes, under
the named `gcc_2_8_1_g8` profile. The entire frontend source group retains
definition order, ABI and registrations. Old function aliases remain
available. The French literal `D_8009B404_French` at `0x8009B404` is
separate from its logical saved-command word and remains unchanged.

No initial data value is inferred from these bindings. No layout, extent,
type, SDK or overlay contract is changed, and all existing test modules
remain byte-identical. No harness, fixture, probe or case is added.
Full regional images and cloud/witness execution remain PR-side validation.


## Packed command argument views

The public preload/play/stop bodies and sequenced-playback request now
name command_id and step where their argument roles are established.
These are command words, not normalized track indices or a new enum.
SD_PreloadBgmAndReplaceValueLink instead names its u32 packed_request:
the high halfword goes to preload and the low halfword to value-link
replacement, in that order. No caller or input-domain claim is added.

Preload and play-with-step retain the data_source_flags bit-0x02 gate.
Their saved_command remains u16; masked_command remains the separate
u32 used for the base comparison. Bit 0x8000 is still tested on the
original word. The non-CD arm compares the saved low halfword with
SD_BGM_COMMAND_BASE but conditionally subtracts from the ORIGINAL s32
command_id, then narrows at the original call. Do not substitute the
saved/masked copy, normalize early, clamp or rearrange those operations.

The CD preload arm still seeks only. The CD play arm still stops the
sequenced path before forwarding to SD_EnqueueCdPlay. Both play arms
retain their explicit s16 step assignment and original call position,
but the CD callee does not read that argument: no CD fade or timing
meaning is inferred. Preloading a different sequence package can reset
current music state; preload is not a promise of silence or playback.

The public default-step wrapper retains its low-sixteen-bit mask; the
direct sequenced default wrapper retains its s16 command cast. Both
read the same existing field_164B byte, without a new cache, store or
interpretation of its 0xFF initializer. The untouched frontend retains
its bitwise class ORs and current-command publication differences.

SD_RequestBgmPlayback calls its original s16 local signed_command.
Its negative test still resets music state and returns before queueing.
Otherwise the same package comparison/load precedes command 0x48,
the original value and signed-step stores, ignored enqueue result, BGM
step store and target 255. The local SDCommand remains partially
initialized. Its value MEMBER is not renamed with the parameter.
The unchanged command pump later transfers the queued step into the
sequence fade ramp only on its existing successful-open branch.

SD_BGMStop retains its low-word mask and literal -32 forwarding.
The unchanged stop-with-step helper chooses CD stop or sequenced
request -1; neither stop path consumes the forwarded step. No fade
is invented. The SOUND_INIT_S16_VIEW declaration retains its s32/s16
parameter types, guard and exact assembler alias, while the normal
request declaration stays s32/s32. All casts, widths, default reads,
reloads, source gates, masks, constants, storage and queue behavior
remain; the broad sound.h and the frontend source stay untouched.
