# Text display-effect continuation, transitions and partial release

The five remaining display-effect callbacks complete the named text-state
table family. They operate on one shared selected `MenuRecord`, not new
per-channel ownership or a list of all effects.

| Text slot | API | Existing action |
|---|---|---|
| 6 | `Text_ContinueDisplayEffect` | Wait for setup, optionally schedule the saved continuation once, then finish |
| 7 | `Text_WaitForDisplayEffectAndRelease` | Wait for step zero, release the record, clear text state |
| 8 | `Text_WaitForDisplayEffect` | Wait for step zero without releasing |
| 14 | `Text_RequestDisplayEffectFadeOut` | Gate on low activity bits, select effect step 6 and text state 8 |
| 9 | `Text_RequestDisplayEffectCrossfade` | Same gate, select effect step 4 and text state 8 |

Both initialized tables retain every original position, ABI, cast and
unsized declaration; their NA owners contain eighteen text entries and
thirteen effect-step entries.

## Selection comes from the existing command

`Text_HandleDisplayEffectCommand` selects an existing
`D_800EB010` arena record and stores it at the same three source sites
into `gText_DisplayEffectRecord`. Keep its current
`MenuRecord *G32` declaration, plain GP addressing and shared lifetime.
The independent script-menu selector `D_8009B274` is not merged with it.

The command's ordinary start path schedules setup step 2 and text state 6.
The saved continuation in `field_40` is 3, or 7 for its existing operand
case. The special ID path schedules step 5 instead. Removal and variant
branches keep their original state/step and operand priority.

The selector is not reset after release or text completion. It can still
point to an inactive fixed arena record; the name is not a guarantee of
active/live object ownership. No null check, selector validation or
per-channel cache is introduced.

## Continue once after setup

The state-6 callback keeps its initialization-latch store before polling
the selected record's step. A nonzero step returns immediately. At zero,
an already-set channel `0x40` continuation bit or stored signed identifier
at least `0x41` clears text state without another continuation.

Otherwise it sets channel `0x40`, reads the saved low byte of `field_40`
into the effect step, then overwrites that field with target X `0x68` or
`0xD8` according to the existing nonzero `field_3C` test. It also stores
target Y `0xB2` and `field_44 = -0x10`. The next zero-step observation
finishes through the already-used-bit path.

Preserve the initial pointer local and separate block-scoped global
reloads for subsequent stores. Do not collapse them into one cached
record or globally rename `field_40`: it is used as saved step,
destination and fade ramp by different handlers.

## Completion with and without release

Both wait callbacks keep their initialization-latch behavior and can
complete on the same call that sets it if the selected step is already
zero. State 7 calls `DisplayEffect_ReleaseRecord` before clearing text
state. State 8 clears text state without that call.

Neither adds a marker test, extra initialization delay, timeout, scan of
all records or selector reset. Existing repeated-release paths are not
normalized away.

## Fade-out and crossfade requests

Both request callbacks read `variant_update_flags` and do nothing while its low two
bits are nonzero. Once clear, they OR `0x10`, publish effect step 6 or 4,
and write literal text state 8. They do not introduce an initialization
latch or perform the renderer transition themselves.

The actual low-bit writers are in `DisplayEffect_UpdateMenuRecord`.
Its two primary resource-animation paths maintain those activity bits;
when bit `0x10` is present it clears the low bits and returns without
those variant updates. Preserve that raw gating and caller ordering,
not a new Boolean/enum or forced clearing in the text callbacks.

The initialized effect table maps step 6 to the handler that ramps the
object rows down, releases its secondary row and then releases the record.
Step 4 rebuilds the selected resource variant, crossfades/moves the old
and new rows, cleans up secondary rows and leaves the new primary row live.
Crossfade completion is therefore not record destruction.

## Release is deliberately partial

`DisplayEffect_ReleaseRecord` first stores `-1` to the signed marker at
record `+0x30`, then hands only `grid[0]` to the existing row helper.
That helper releases and nulls its three slots in index order 2, 1, 0.
The menu-record dispatcher skips negative markers.

The helper does not free the three-record arena, clear the step byte,
clear all four rows or reset either selector pointer. Other transition
handlers retain their own extra-row cleanup. Keep the original
void-pointer row view, mark-before-release ordering and all layout/type
contracts.

## Native identities and preservation

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Continue | `0x800377C8` | `0x80036F80` | `0x80037560` | `0x80037760` |
| Wait and release | `0x8003787C` | `0x80037034` | `0x80037614` | `0x80037814` |
| Wait only | `0x800378D8` | `0x80037090` | `0x80037670` | `0x80037870` |
| Request fade-out | `0x80037914` | `0x800370CC` | `0x800376AC` | `0x800378AC` |
| Request crossfade | `0x80037950` | `0x80037108` | `0x800376E8` | `0x800378E8` |
| Release record | `0x80039FD4` | `0x80039654` | `0x8003A164` | `0x8003A364` |
| Selected record pointer | `0x8009B328` | `0x8009B218` | `0x8009C28C` | `0x8009C6AC` |

Japan's existing native `D_8009B218` selector is distinct from the unrelated
`gJapanese_D_8009B218` at `0x8009B108`. Native-address selection preserves
that alias and every other unrelated binding. Remove only the eleven
obsolete callback/selector wrapper macros.

All complete compiler/MASPSX outputs are compared after reversing only
seven identifiers, without filename, instruction, directive or relocation
filtering. Full clean retail matching remains mandatory. Inventories
preserve every other field and row; profiles, manifests, grouping, paths,
layouts, initialized tables and unrelated symbols/imports stay fixed.
The complete accepted ledger gains seven evidence/caveat rows. No tests,
controls or captures change; regional/cloud validation remains PR-side.

The initialization-return polarity, partial reset, exact three-slot
row helper and shared step-dispatch table are described in
[display-effect-lifecycle-helpers.md](display-effect-lifecycle-helpers.md).

The actual tween, incoming/outgoing ramp, resource-crossfade and portrait
handlers are detailed in
[display-effect-transitions.md](display-effect-transitions.md).

The original seven-name proof precedes accepted #7631 integration.
Its Library source/header/guard move, native manifests and retained test
include-path update are preserved. The combined-tree proof aligns only
that accepted source filename in its two complete outputs, then reverses
the seven owned names. No output is dropped, and a fresh clean gate
covers the complete combined tree.


## Shared resource-variant controls

MenuRecord +0x32 is variant_update_flags, also named in the existing
DisplayEffectState and DisplayEffectVramState views. Acquisition sets
0x40 only after its later primary-row construction, and the dispatcher
tests that bit before updating variants. This is separate from the
acquisition state byte's 0x40 bit and the step/initialization byte at +0x33.

The low bits describe the two existing child paths: 0x01 for grid[0][1]'s
randomized variant-zero/one cycle, and 0x02 for grid[0][2]'s requested
variant path. The update's 0x10 arm clears only those low two bits and
returns, retaining 0x10 itself. Text adjustment can set or clear 0x10;
fade/crossfade setup and idle-finish/request callbacks set it, while
fade-in/crossfade completion clears it. Fade-out/release does not gain
a clear. The flags stay a raw byte: no enum, Boolean or invented bits.

MenuRecord +0x3A is requested_variant: the narrowed last explicit value
passed to DisplayObject_SetResourceVariant for grid[0][2]. Its only
named C stores are global reset and this publication; it is not read back
to choose a variant. The slow path's request for live variant zero does
not rewrite it, nor does the partial lifecycle reset. It is not a cache
of the live object's selector or the first resource-table entry index.
The setter retains its own byte store and resource-resolution-bit clear.

MenuRecord +0x3B is variant_update_counter, not a pure countdown or frame
clock. Under bit 0x02 it is decremented and stored before the child
field_5A gate, then tested through the original signed-byte local cast.
A nonpositive request can clear it and bit 0x02; an explicit request
stores six and publishes requested_variant. With that bit clear and a
negative input, the slow path instead increments the same byte and tests
its stored value as s8 against six before conditionally requesting zero.
There is no new saturation, reset at six, underflow repair or reordering.
Both byte wrap and the distinct pre/post-narrowing signed tests remain.

TextBox_UpdateChannels supplies its existing glyph-kind selection,
including previous-kind reuse and the negative no-glyph sentinel.
Script_OpShowMenu supplies zero; the debug caller supplies its existing
Rand_GetInterval result. Calls can occur per active text channel, so this
counter does not establish a once-per-frame rate or an audio dependency.
No facial-expression, speech or blink meaning is claimed.

Global menu reset retains zero stores to all three named bytes. The
partial lifecycle reset clears only variant_update_flags among these
three; record release adds no control reset. Missing-child returns,
child field_5A gates, RNG timing, the separate variant_countdown halfword,
all raw masks, caller order, object rows and selectors remain unchanged.
