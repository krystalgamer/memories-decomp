# Card preview resource-transfer callback

`CardPreview_HandleResourceTransfer`, formerly `func_800289BC`, is the
typed phase callback registered by `func_80029164`. The requester stores the
resource slot as descriptor `callback_data`; the callback uses that slot
to select `D_800EA0E8`.

Mode zero sets the original transfer size and both buffer words, applies
the original volatile flag mask and sets the descriptor's `done` member.
Nonzero mode uploads the four image regions in their existing order using
the actual `RECT *` / `u32 *` `LoadImage` interface. The name covers both
initialization and upload, not just the main card image or a file request.
No more specific meaning for the `s32 mode` argument is introduced.

| Configuration | Address | Size | Source owner |
| --- | --- | --- | --- |
| `slus_01411` | `0x800289BC` | `0x14C` | main C |
| `slpm_86398` | `0x8002879C` | `0x14C` | Japanese wrapper |
| `sles_03947` | `0x8002895C` | `0x16C` | European wrapper |
| `sles_03948` | `0x80028AA0` | `0x16C` | European wrapper |
| `sles_03949` | `0x80028AA0` | `0x16C` | European wrapper |
| `sles_03950` | `0x80028AA0` | `0x16C` | European wrapper |
| `sles_03951` | `0x80028AA0` | `0x16C` | European wrapper |

Every entry retains `gcc_2_8_1_g0_no_split`, separate from the neighbouring
renderer's split profile and variant updater's G0 profile. The three actual
owners use `card_preview_handle_resource_transfer.c`. The Japanese data
alias `D_8009B0F4 -> gJapanese_FileTransferFlags` is preserved. The European
function alias becomes obsolete with the canonical name, but all original
language-dependent offsets, phase size `0x4000` and fourth-image offset
`0x3560` remain unchanged; the main defaults stay `0x3800` / `0x2AE0`.

The complete main callback body changes **one function identifier only**.
Its descriptor operations, mode-zero return, integer buffer views, signed
and unsigned coordinate accesses, rectangle writes, upload order, volatile
flag alias/mask and null/failure assumptions are untouched. The current
whole-requester fixture only follows the callback identity in its stub and
pointer check; its byte, argument, ordering and negative oracles do not
change, and it still never invokes this callback.

The semantic-map append preserves every historical row and duplicate key.
The regional symbol updates preserve unrelated aliases. Historical attempt
records and optional generated reports are not rewritten. No overlay
implementation or frozen French-options fingerprint input changes.

Focused controls cover the seven address/size/profile/owner bindings,
registration and data-alias boundaries, and the exact callback type with
negative pointer/arity probes. They do not execute the callback's transfer
phase, upload VRAM or emulate I/O/GPU behavior. Complete retail executable
matching remains the acceptance gate; regional execution belongs to normal
PR validation.
