# Card preview resource requester

`CardPreview_RequestResourceTransfer`, formerly `func_80029164` at
`0x80029164` in the North American executable, starts the card preview's
resource transfer. Its name is confirmed by the independent request
producer, callback consumer and callers; it does not construct display
objects or synchronously finish a GPU upload.

The producer first stores the low halfword of `card_id` into the selected
`D_800EA0E8` record. It then requests the original full-word card-art disc
position through `File_TryRequestAsyncTransfer`, passing
`CardPreview_HandleResourceTransfer`. The returned descriptor receives
the slot as `callback_data`; its status is published with the primary-active
bit. The exact descriptor is returned, although current callers discard it.

The callback independently consumes `callback_data` as the resource slot,
initializes its staging-buffer address words in mode zero, and uploads the
record's four image regions in nonzero mode. Card use, battle, ritual,
placement, the card viewer and Library pass actual card IDs to the
requester and gate later preview work on transfer state. The debug caller
supplies card 1. Password's existing call is updated only to retain the
resident API spelling; no overlay behavior or implementation is recovered.

| Configuration | Address | Size | Grouped source |
| --- | --- | --- | --- |
| `slus_01411` | `0x80029164` | `0x7C` | main |
| `slpm_86398` | `0x80028F44` | `0x7C` | Japanese wrapper |
| `sles_03947` | `0x80029198` | `0x78` | European wrapper |
| `sles_03948`–`sles_03951` | `0x800292DC` | `0x78` | European wrapper |

Both functions remain contiguous in each `duel_effect_resource_setup.c`
object, with the existing `gcc_2_8_1_g8_split` profile and constructor
definition order. No source owners move. US/Japan keep seven sectors per
card and `((card_id - 1) * 7 + CARD_COUNT)`; Europe keeps eight sectors and
its original `card_id * 8 + CARD_COUNT - 8` expression. The full signed
argument is not replaced with the already-truncated stored ID.

Password's separate resident import tables also bind the canonical name at
the same per-build address. North America's old address-based name had
been supplied by generated undefined-function output, so its import now
needs an explicit named assignment; the other six tracked imports add
the canonical name alongside the original address-based alias for unchanged
assembly. Every Password A/B manifest continues
selecting its existing import file. This is resident API wiring, not an
overlay implementation change.

This is a symbol-token-only source change. The requester body, constructor,
callback, flags, order, return type, scalar argument types and unchecked
failure assumptions remain unchanged. The native full-requester fixture
retains all 1,310,720 calls per optimization and its truncation, active-bit
and widened-store negatives. The ritual fixture changes only its stub's
name. Supported-compiler API controls reject opaque/byte returns, narrowed
card arguments and the wrong arity; caller visibility controls cover every
actual caller and detect a lost owning-header include. These do not
execute real I/O, GPU behavior or regional branches.

A two-instruction MIPS relocation probe links against each actual tracked
import script and checks the resulting absolute symbol address. Removing
only the named import must fail with the same undefined-reference error
seen in PR validation. This focused link probe does not build or execute
overlays; complete regional/overlay acceptance remains in PR checks.
