# Paired model coordinate helpers

This component names two existing game-owned matching-C helpers and all
fourteen corresponding native function records across seven images.
It introduces no new geometry implementation, type, validation, allocation,
compiler profile or source-file movement.

## View-relative coordinate builder

`Model_BuildViewRelativeCoordinateUnit`, formerly `func_800580D4`, retains
`void(s32, s32, SVECTOR *, struct _GsCOORDUNIT *)`.

The first argument selects `gModel_aSlots[index]`. The second requests an
80-byte coordinate entry. The original comparison remains strictly
`entry_count < requested`: that case substitutes `root_coordinate_index`. Equality,
negative requests and caller validation are not changed, and no anatomical
meaning is assigned to entry fifteen or the fallback.

The builder obtains that entry's local-world matrix, installs it in GTE,
and transforms the input vector. A translation parent starts with identity
rotation and the resulting point. The output combines cached view yaw/pitch
with the original negated slot-zero rotation and clears its local translation.
Translation uses the selected slot, but the orientation's repeated volatile
reads still come from slot zero, not that selected slot.

This describes a view-relative construction, not a guarantee of normalized
billboarding. All inline-helper structure, casts, full-turn constants,
matrix/GTE call order, flags and stack/store layout remain.

The legacy `super` assignment points to the builder's automatic parent
workspace. This lifetime-sensitive behavior is intentionally preserved,
not repaired or wrapped. The canonical name supplies no new stable-storage
or ownership guarantee.

## Slot-zero offset world position

`Model_GetSlot0OffsetWorldPosition`, formerly `func_80059B90`, retains
`void(s16, s16 *)`, not a retyped vector argument.

It zeros one `SVECTOR`, puts the signed input in its local X component,
and invokes the builder with slot zero and entry request fifteen. It then
asks `GsGetLwUnit` for the result and extracts low halfwords of the three
translation words. The single byte-backed SVECTOR/GsCOORDUNIT/MATRIX scratch
block, volatile halfword reads, narrowing and output extent stay intact.
The store `out[3] = 0` still precedes the final `out[2]` store.

Exodia's second-entry logic consumes this result as a world-space origin
at local X offsets 480 or zero before forming its direction. It includes
the coordinate entry's translation; it is not a projected screen coordinate,
camera position or view-dependent offset vector. No hand, bone or body-part
identity is inferred from the fixed entry request.

The SDK setter was checked independently: resident `GsSetLsMatrix` at
`0x800855D0` calls `SetRotMatrix` then `SetTransMatrix`. It is not the separate
`GsSetLightMatrix`. Reading that SDK evidence does not make it a game naming
candidate; SDK-owned `func_80058F10` likewise remains outside this component.

## Native and source contracts

| Image | Builder | Position helper |
| --- | --- | --- |
| NA | `0x800580D4` | `0x80059B90` |
| Japan | `0x8005AF04` | `0x8005C9C0` |
| SLES-03947 | `0x8005C614` | `0x8005E054` |
| Other PAL images | `0x8005B258` | `0x8005CC98` |

Every builder remains `0x1EC` bytes and every position helper `0x88`.
All retain game ownership, matching-C status and historical notes. The
builder stays grouped after the contiguous `0x19C`-byte
`Model_UpdateViewMetrics` in the same source definition order and named
`gcc_2_8_1_g8_split` profile. The position helper keeps its source filename
and `gcc_2_8_1_g0_split` registration. Address/source/profile manifests are
unchanged; no name-field migration is needed there.

The isolated canonical header preserves the old SDK/forward-tag declaration
forms. Every pre-existing header remains frozen, including the old numeric
builder prototype and the Exodia position prototype. Root compatibility
aliases retain those APIs, and Exodia C/import maps keep their numeric calls,
addresses and cardinalities. Existing ABI and French/Spanish import controls
remain byte-identical.

The resident source's obsolete candidate-status comment is updated to the
current matching-C interpretation. Frozen historical header comments are
not authoritative status inventories; `functions.csv` is.

No local cloud/full regional/overlay/witness suite, synthetic C fixture,
source-string harness or probe is added. Whole compiler/MASPSX, filtered
intermediate and complete fallback comparisons reverse only the two
identities, without instruction, directive, register or filename filtering.
Final clean resident matching, policy/ownership checks and fresh applicable
PR gates remain required.
