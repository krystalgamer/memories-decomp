# Save workspace and chest-only card lookup

`gSaveData_Workspace` names the resident save workspace at `0x801D0000`
(formerly `D_801D0000`).
`Library_CheckCardInChest` names the matching helper at `0x8002C570`
that reads the workspace's card-quantity byte. Neither name introduces
storage, a new layout or a new caller.

## Workspace evidence and views

The unchanged `save_data.h` places the observed `SaveDataWorkspace.state`
prefix at `+0x200`, with card quantities at state `+0x50`.
`Campaign_TestStoryFlag` reads its campaign flags through the recovered
structure. The existing campaign-bit writer
`Library_UpdateCardUsedFlag`, in `campaign_story_flags.c`, addresses the same
bank as bytes.
`Library_CheckCardOwned` reads the quantity array through that structure.
`Script_OpSavePrompt` uses the halfword base view for the saved campaign
scene index.

The isolated canonical header mirrors the existing declaration selector:
incomplete `u8[]` normally, incomplete `s16[]` when
`SAVE_DATA_WORKSPACE_AS_HALFWORDS` is defined. The prompt keeps that
selector and its halfword access, which also touches adjacent `field_5DD`;
it is not rewritten as a byte store. All existing headers stay unchanged.

The reusable live-state workspace is distinct from
`gSaveData_aTransferBuffer`. The recovered prefix is not proof that every
workspace byte is always a serialized save image. No fixed array extent,
new `SaveDataWorkspace` global, allocation size or stride is asserted.
Password Name Entry's existing `0x3000` clear/debug output remains outside
the change; it is not used to enlarge the resident declaration.

Keep separate base and interior relocation targets. The canonical base
alias does not replace or rebind `gDuel_awPlayerDeck` or
`gLibrary_abCardChest`, and no access is folded into an interior symbol.
Free Duel and password overlays retain their numeric view, old headers
and imports. Canonical aliases allocate no replacement workspace.

## Chest-only result

The original helper forms `(u32)base + offset`, then reads at
`SAVE_DATA_HEADER_SIZE + SAVE_DATA_CARD_QUANTITIES_OFFSET - CARD_ID_FIRST`.
For a one-based card ID this is the quantity byte at
`workspace + 0x250 + card_id - 1`. It returns **+1 for a nonzero byte and
-1 for zero**, not a normalized Boolean or a count.

The independent `Library_CheckCardOwned` implementation performs the same
chest-byte test, then falls back to `Duel_FindPlayerDeckCard` when it is
zero. `Library_CheckCardInChest` has no deck fallback: its name does not
claim ownership across chest and deck, nor a newly found Library caller.
No original gameplay use or unreachability is inferred.

The exact pointer/integer expression, byte access, offset constants and
sentinel values remain unchanged. There is no input-range validation;
this naming change adds no guard, clamp or invalid-ID result. The
definition retains its existing `int(int offset)` spelling, compatible
with the frozen `s32(s32)` declaration and the new canonical prototype.
The helper stays first in its existing effect-request-pool source group:
the source filename does not establish an effect-pool data role.

The old `Library_UpdateCardUsedFlag` name is not evidence of flags packed
inside a quantity byte: that function actually manipulates the campaign
flag bank. Its accepted name and behavior are not opportunistically
changed here.

## Native bindings and preservation

| Image | Chest-only helper | Workspace base |
| --- | --- | --- |
| North America | `0x8002C570` | `0x801D0000` |
| Japan | `0x8002C33C` | `0x801D0000` |
| SLES_039.47 | `0x8002C600` | `0x801D0000` |
| Other PAL images | `0x8002C744` | `0x801D0000` |

All seven provider sets separately retain the deck base at `0x801D0200`
and chest at `0x801D0250`. Provider equality is not independent proof of
regional initial values or retail bytes. The seven function records keep
game ownership, matching-C status, notes, `0x28` extent and the named
`gcc_2_8_1_g8_split` profile. Old function aliases remain available.

All source groups, definition order and profiles remain unchanged. All
pre-existing headers, overlay C/imports and tests stay byte-identical.
No harness, C fixture, probe or case is added. Full regional images and
cloud/witness execution remain PR-side validation.
