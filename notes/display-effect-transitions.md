# Display-effect transition handlers

Five complete handlers in `display_effect_update_callbacks.c` use the
existing `MenuRecord *` step ABI. Their bodies, locals, casts, first-call
progression and cleanup order remain unchanged.

| NA identity | API | NA effect step |
|---|---|---|
| `0x8003A990` | `DisplayEffect_UpdatePositionTween` | 3 |
| `0x8003AAE4` | `DisplayEffect_FadeInRecord` | 7 |
| `0x8003AC48` | `DisplayEffect_FadeOutRecord` | 6 |
| `0x8003AD6C` | `DisplayEffect_CrossfadeResources` | 4 |
| `0x8003B054` | `DisplayEffect_UpdatePortraitTransition` | 5 |

## Position tween: retain the target arithmetic

The zero result from `DisplayEffect_MarkInitialized` runs initialization
and then continues the update in the same call. Keep the sole `r = p`
alias: retaining both names as independently live pointers changes the old
compiler's frame and register allocation.

Initialization divides a quarter turn by signed `field_44`, narrows the
step into `field_4A`, selects the original start angle from its sign, then
overwrites the overloaded distance fields. The divisor stays unvalidated.

The update keeps unsigned-halfword reads of angle and increment, the
stored-halfword wrap, unsigned `(t - 1)` endpoint test, signed cosine input
and sign-dependent distance complement. Completion publishes the exact
target position before clearing the step. It does not use the frame-step
multiplier; no interpolation helper, clamp or algebraic rewrite is added.

## Incoming and outgoing ramps are different lifetimes

The incoming step is the text command's alternate saved continuation.
It sets the existing position, configures the primary blend attributes,
builds a secondary row and stores ramp zero. That same call adds the
full-word frame step times eight.

At its original stored-member endpoint test, it clears the step, applies
the row-attribute helper with its existing zero argument, releases only
row one and clears bit `0x10`. The primary row and record remain. Do not
replace that helper call with guessed opaque attributes or add a final
color store that the branch currently skips.

The outgoing step starts at `0x80`, subtracts frame step times eight
through the original unsigned read and signed narrowed local, and on a
nonpositive result clears the step, releases row one, then releases the
record. Intermediate color stores and their order stay fixed.

Both retain same-call initialization/progression, all integer-row casts,
unchecked existing primary-row assumptions and the original branch/store
placement. The old incoming header's fade-out label is corrected, not
used to change its C body.

## Resource crossfade: four rows, one surviving primary

Initialization captures the old primary selector and depth before its
release. It builds new-selector resources into rows zero/one and
old-selector resources into rows two/three, retaining the original
attributes, depth offsets and construction order.

The signed halfword ramp/local and full-word frame step remain. While
active, old rows receive the current repeated RGB value and new rows its
`0x80 - ramp` complement. The two row pairs retain their independent
position offsets and the existing `field_3C` direction test.

Completion clears the step, restores the primary row's attributes and
position, releases rows one, two and three in order, clears bit `0x10`
and returns. It does not release the record or normalize all rows through
one generic cleanup path.

## Portrait transition: two directions and endpoint order

The portrait-ID path uses the existing image/grid/CLUT constants. With
`field_40` zero it creates a primary sprite and starts its signed
`field_60` value at `0x14`; otherwise it reuses the primary and starts at
zero. Both paths create the secondary blend sprite and continue the
update in that initialization call.

The source keeps both `(u16)D_8009B0D8` sites, low-halfword stores and
signed `field_60` tests. They are not replaced with the full-word views
used by the other ramps. The regional secondary CLUT-x macro remains
`0x200` by default and `0x280` in Europe.

At the lower endpoint it restores the primary's existing attributes,
neutral color and scale, releases/nulls only the secondary slot and then
clears the step. At the upper endpoint it releases the record **before**
the shared step-clear label, unlike the outgoing ramp's step-first order.
Intermediate scale, angle and color-byte stores remain in their original
order. No portrait-index, frame-step, allocation or divide guard is added.

## Native bindings and acceptance

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Position tween | `0x8003A990` | `0x8003A028` | `0x8003AB20` | `0x8003AD20` |
| Incoming fade | `0x8003AAE4` | `0x8003A17C` | `0x8003AC74` | `0x8003AE74` |
| Outgoing fade | `0x8003AC48` | `0x8003A2E0` | `0x8003ADD8` | `0x8003AFD8` |
| Resource crossfade | `0x8003AD6C` | `0x8003A404` | `0x8003AEFC` | `0x8003B0FC` |
| Portrait transition | `0x8003B054` | `0x8003A6EC` | `0x8003B1E4` | `0x8003B3E4` |

Remove only the five obsolete Japanese function macros. Retain the
Japanese frame-step alias, regional constants, old source/header paths,
grouped object order, all callback signatures and every initialized table
position. Shared record fields are not globally renamed or retyped.

All complete compiler/MASPSX outputs are compared after reversing only five
identifiers, without instruction, directive, relocation or filename
filtering. Full clean retail matching remains mandatory. Every unrelated
inventory field/row, symbol/import, profile, manifest and layout stays.
The complete accepted ledger gains five evidence/caveat rows. Tests,
controls and captures do not change; regional/cloud validation remains
PR-side.

The original five-name proof precedes accepted #7635 integration.
Its camera request/wrapper identities, native bindings, tests, trace names
and complete ordered ledger edits are retained. The combined-tree
comparison aligns only those two accepted names in the saved baseline,
then reverses these five transitions. Every whole artifact is still
compared, followed by a fresh clean retail gate.
