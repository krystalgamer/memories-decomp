# Dialog transition callbacks and their shared table view

The existing four callbacks cover spotlight contraction, joint display
closure, combined spotlight/dialog opening and a message-only opening.
These names follow their complete bodies, the spotlight renderer,
position interpolation, channel/choice code and memory-card caller.
They do not infer behavior from the older header's reversed radii prose.

| NA identity | API |
|---|---|
| `0x8003D518` | `Dialog_UpdateSpotlightContraction` |
| `0x8003D614` | `Dialog_UpdateCloseTransition` |
| `0x8003D74C` | `Dialog_UpdateOpenTransition` |
| `0x8003DA40` | `Dialog_UpdateMessageTransition` |
| `0x80090F88` | `gDialog_apfnTransitionSteps` |

## Shared state is not a new per-record lifecycle

All four callbacks use the existing `u8 D_8009B3C1`: bit `0x80` is their
first-entry latch, while the individual handlers interpret `0x40` and
`0x20` through their original branches. The memory-card controller also
uses its low nibble as a dispatch selector. No new type, enum, flag
definition or dialog-only global identity is imposed on this shared byte.

The controller calls `gDialog_apfnTransitionSteps[state & 0xF]` with
`MemCardWorkRoot *` and `MemCardWorkSlot *`. Keep that existing five-entry
`G32` two-argument caller declaration. The stored callbacks keep their
existing one-argument `MenuRecord *` signatures; they ignore argument two.
Their touched root offsets fit the existing caller view. This is a
measured target ABI, not a claim of interchangeable ISO C prototypes.

The available NA retail executable confirms the five words in order:
message, open, close, contraction, message. They are entries eight through
twelve of `gDisplayEffect_apfnSteps`, starting at `+0x20`. The alias adds
no storage, changes no initialized word and does not widen the five-entry
view to sixteen. The full table's thirteen-entry unsized declaration and
`step & 0x1F` dispatcher remain separate and unchanged.

The callbacks read the channel index through byte offset `0x1A`.
`MemCardWorkRoot` already calls that byte `text_index`; the callback's
integer `MenuRecord` row view remains as written. No new writer, saved
channel index, union, pointer-grid interpretation or allocation is invented.
Completion clears shared flags, not `MenuRecord.display_effect_step`.

## Spotlight contraction retains one-call-delayed completion

On first entry, the contraction callback creates the existing kind-six
object in `grid[0][1]`, centers it, seeds inner/outer radii `128/224`,
selects ordering table one, derives signed-byte depth from the existing
aggregate byte view and installs `DisplayObject_RenderSpotlightMask`.

The renderer consumes `+0x48/+0x4A` as inner/outer radii. Contraction
subtracts eight from both on the initialization call as well as subsequent
progress calls. The original `u16` reads, halfword stores and signed
inner-radius endpoint test remain. Reaching zero writes radii `0/64`
and latches `0x40`; only a later invocation already seeing `0x40` clears
the shared state. The mask object survives. This is not an expanding
frame, an immediate-completion callback or a release operation.

## Close progresses and releases two objects independently

Initialization still saves the primary sprite position without a new null
guard and seeds its signed-halfword phase to `0x400`. The same call
decrements that phase by `0x40` through the original unsigned-halfword
view. A nonpositive signed result releases the sprite and then nulls its
slot. Otherwise `Widget_SlideSine` moves toward `(DIALOG_BOX_X, -0x40)`
and the text box follows the resulting sprite position.

Independently, the existing mask object gains eight in each radius and
is released/nullified when the signed inner radius reaches `0xC0`.
Primary-sprite work precedes mask work; shared state clears only once
both slots are null. There is no channel destruction, full-record release,
new endpoint position store or cleanup of all four integer rows.

## Combined open preserves both endpoints and the prompt handshake

Initialization constructs the mask and sprite through two distinct
allocator calls, stores their existing row slots, snapshots the sprite
position and seeds phase `-0x400`. It creates text channel
`(D_8009B3C7 & 1) | 0xD0`, sets the regional input-skip flag and repeatedly
pumps `TextBox_UpdateChannels` until the channel's `field_30` object
exists. The helper `DuelEffect_CreateChannel` itself resets the global
choice to minus one. No bound, timeout or changed readiness test is added.

Progress continues on that first call. While phase is negative, it
advances by `0x40`, retaining the unsigned read and signed endpoint/local
tests. The endpoint fixes the existing onscreen position and phase zero;
otherwise the sine helper computes position. The channel follows those
stores. Mask radii separately contract by eight, then clamp to `0/64`.
Input is unmasked and shared bit `0x40` is set only after the sprite
phase is also zero.

An invocation already seeing `0x40` pumps text instead of moving objects.
On `TEXT_BOX_FLAG_DONE`, it ORs shared `0x20`; if the choice is exactly
zero it creates channel `0x80D4` and then stores choice zero again,
after the creation helper's minus-one write. The existing cached `f`
tests and later `g` reload remain. An invocation already seeing `0x20`
clears shared state and returns before channel lookup. It does not release
either display object or close a newly created follow-up channel.

## Message-only opening stays a different callback

The message callback creates only the kind-two dialog sprite and fixed
channel `0xD0`; there is no mask allocation or mode-byte selection.
Initialization pumps until `field_30` is nonnull and then proceeds with
phase `-0x400` in steps of `0x20`, not the combined open's `0x40`.

It retains the `u16` phase read, signed narrowed endpoint, existing sine
helper path and channel positioning. At the onscreen endpoint it sets
shared `0x40` and clears the regional input-skip flag. Only a subsequent
invocation in that branch pumps until raw channel flag `0x2000` is set,
then clears shared state. There is no bit-`0x20` handshake or display-object
release here. No callback in this family gains frame-step scaling.

## Native identities, regional constants and acceptance

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Contraction | `0x8003D518` | `0x8003CADC` | `0x8003D4E4` | `0x8003D6E4` |
| Close | `0x8003D614` | `0x8003CBD8` | `0x8003D5DC` | `0x8003D7DC` |
| Open | `0x8003D74C` | `0x8003CD08` | `0x8003D714` | `0x8003D914` |
| Message | `0x8003DA40` | `0x8003CFF4` | `0x8003DA04` | `0x8003DC04` |
| Table view | `0x80090F88` | `0x80090E38` | `0x80091FA0` | `0x800923B4` |

Regional addresses come from tracked native inventory/symbol bindings,
regional wrappers and the full-table interior relationship. Only the NA
retail table is inspected locally; regional exact-image acceptance stays
with PR checks. The unrelated French `D_80090F88_French` alias at the
NA-number address remains untouched.

Spotlight center y remains 120 in NA/Japan and 128 in Europe. The channel
input-skip flag remains four versus eight. Japan retains the `0x60`
channel stride and its array/depth aliases. The existing Spanish-family
message wrapper retains `DIALOG_BOX_X = 0x08`; other wrappers keep their
own established default. No wrapper is unified or moved.

All 940 complete compiler/MASPSX outputs match after reversing only these
five identities, without filtering instructions, directives, relocations
or filenames. The full seven inventories preserve non-name fields/order;
unrelated bindings, profiles, manifests, source/header paths, layouts and
all table entries remain. The complete accepted semantic ledger gains
five evidence/caveat rows.

Clean full-executable retail matching remains mandatory. No tests,
fixtures, controls or captures change, and no synthetic/generated-C or
source-string harness is introduced. The accepted model-tint header
rename/retirement from #7636 is already part of this component's baseline.
See [memory-card-work-controller.md](memory-card-work-controller.md) for
the controller's workspace and calling-view boundaries.
