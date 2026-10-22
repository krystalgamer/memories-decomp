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
