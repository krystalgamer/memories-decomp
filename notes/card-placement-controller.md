# Card-placement controller

`DuelScene_UpdateCardPlacement` at `0x80019D18` is the 5,044-byte resident
scene-state 7 controller. Its eight-state table at `0x800100F8` and five-state
combination subtable at `0x80010118` occupy another 52 bytes. Both tables and
all 1,261 instruction words were checked against the North American target.

The controller arranges the selected cards, resolves fusion and equipment in
order, requests their image resources, presents stat changes and fusion motion,
discards an unsuccessful combination, and selects the next scene. State 1
deliberately falls through to state 2 after its motion callback completes;
initialization returns without advancing the new phase.

## Source and ownership

The matching source uses the existing display-object, card-record, deck-card,
selection, hand, side-statistics, effect-request and resource layouts.
`D_800E9EF0` is shared work storage, not an object pool: this controller reaches
six consecutive pointer words, including the independently named word at
`0x800E9F04`. It must not be reduced to a five-element array.

`D_8009B150` retains the unsigned halfword used by card use. Placement also
requires a signed read of those same two bytes. `DuelPlacementCardIdCell`
expresses that read without allocating or enlarging storage. Bit 15 marks a
fusion result; the low twelve bits identify the resource. The six-byte
`DuelDeckCardRecord` supplies the signed index byte and image-block index.
Fusion readback is an 8-by-88-pixel rectangle into a 1,408-byte block.

The tentative `D_8009B206` definition and uniform
`gcc_2_8_1_g8_split_comm` profile preserve a real load-delay nop on equipment-id
publication. The initialized data owner remains unchanged. An extern-only
control loses that nop and fails the instruction execution comparison.

## Matching discriminators

- Retain the selected display object **after** its null guard. Assigning it in
  the short-circuit condition folds the load/copy and removes one instruction.
- Publish the primary object's flags through `D_800E9EF0[0]`, not a cached
  local work-base read. Old GCC otherwise forwards the cached pointer, removes
  retail's reload and emits a different fade-call tail.
- The successful first equipment check enters the resource request directly.
  Its shared entry prevents moving the unsigned id load into the final signed
  zero-test's delay slots.
- The two effect phases share the computed phase-clear store. The midpoint,
  stat-limit and alpha values are captured before their related publications.
- The signed velocity helper retains division rounding after fixed-point
  scaling. Loop-local bases and reused working values preserve register
  allocation; the indexed hand walk avoids a second advancing pointer.

The remaining unusual scopes and scalar halfword stores are measured
instruction-scheduling inputs, not attempts to introduce volatile state or
hardware-register bindings. No generated assembly or compiler checkout is
modified.

## Behavioral evidence

The local instruction differential exercises 16,929 cases and 24,378 callbacks,
including 1,526 cases with callback mutations. It visits every retail instruction
and rejects modified scene completion, an incorrect card-id mask, and the old
extern-only equipment publication.

The independent native ILP32 C comparison runs 702 selected cases at both `O0`
and `O2`, preserving all observed instruction paths and 276 branch/transfer
edges. It compares callback arguments, rectangle bytes, card/object/work state
and image-readback bytes. Wrong completion, fusion resource masking and motion
increments are rejected.

These are bounded tests with explicit ABI stubs, not full-game or SDK/GTE
emulation. Their sources and generated retail-derived fixtures stay under
local `tmp/`. Neither behavioral equivalence nor function-only byte equality
replaces the clean complete-executable match gate.

`tools/project/tests/test_card_placement_match.py` recompiles the tracked unit
with the production profile and checks its complete linked text and both
tables. It requires the legal target, local toolchain and a matched baseline;
it stores no retail bytes in the repository.
