# Duel-result controller caller views

`func_800218F0` keeps its address name and the existing reward/display helper
interfaces. Its source ordering is measured, particularly the adjacent colour
slot stores after the viewport and display-flag stores. Rank division remains
signed 32-bit, and the starchip-object X coordinate is also signed 32-bit.

## Backing and bounds

- `D_8009B1D8` and `D_8009B1DC` are adjacent pointer words in the existing
  `bss_image_after_viewport` image backing, not separate C allocations.
  `DUEL_SAVE_WINDOWS_AS_PAIR` exposes exactly two byte pointers, eight bytes
  on the target, preserving GP-relative indexed access. Default consumers keep
  their scalar names. Native fixtures must allocate a real pointer pair and
  map the second scalar identity to its second element.
- `Duel_InitScene` selects the two `0x1000`-byte windows beginning at
  `D_801D1200` in the no-opponent path, or the persistent state base in the
  normal path. `SaveDataState` already covers the required `0x5E4`-byte
  prefix. Its former four-byte padding at `+0x518` is now two `u16`
  duel totals: wins at `+0x518`, losses at `+0x51A`. Starchips occupy the
  word at `+0x5E0`. The controller's narrow increment/store happens before
  its clamp; this is not a wider saturating counter.
- The sole writer of `D_8009B1E8` points it at `gDuel_awRitualData`,
  `0x801799D8`. `DUEL_RITUAL_DATA_RESULT_VIEW` describes the full
  `0x40`-byte `DuelResultDisplayState` there, including ten child pointers.
  The span ends at `0x80179A18`, inside the existing `slot_80168000`
  image backing and before `slot_8017a000`. This is lifetime-based reuse,
  not an allocation sized from a short ritual recipe.
- `D_801D56A8` is a separate text-staging word, beyond the existing
  `D_801D5608` union's measured `0x80` bytes. Only element zero of its
  `s32 []` view is established. `0x801D56A8..0x801D56AB` lies in
  `tail_data`, before the colour slots at `0x801D5708`. The gap does not
  establish a larger array. Its incomplete-array spelling retains the
  split absolute store; a scalar `.data` declaration changes code generation.
- `D_801AF000` is an image-backed, reused display-resource bank, passed to
  the display helper as a resource address. `display_asset_banks.h` keeps
  the existing incomplete byte-array view without inventing a full extent.

## Addressing and declaration controls

The input repeat mask uses the new **nonvolatile scalar**
`GINPUT_PAD1_REPEAT_IN_DATA` arm. Existing volatile and default arms remain
unchanged. The message id uses `TEXT_STRING_ID_IN_DATA`; its existing
GP-relative readers keep the default declaration. Neither adds volatility
to force ordering. The scene flags and resource-bank declarations have moved
out of `unmatched.h` into their owners, with existing consumers retaining
compatible includes.

The final scratch source is normalized with
`centralize_basic_types.update_source` using its intended `src/game` path
before the exact probe and terminal evidence. Header migration does not
permit reordering the load-bearing viewport, flag, or colour writes.

## Existing candidate contract migration

The canonical contract hash includes both the declaration text and its owning
header path. Moving the unchanged `extern u16 D_8009B23A;` from `unmatched.h`
to `game/duel_scene_state.h` therefore changes its contract identity without
changing its type, qualifiers, addressing, or ABI. Candidate `0x80018FEC`
already includes the new owner and is the only configured candidate whose
contract references this declaration. Its per-symbol hash and aggregate
contract hash are updated together; its source hash and object/target
fingerprints are unchanged. The strict contract validator and that candidate's
existing fingerprint gate remain enabled.
