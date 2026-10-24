# Shared text-message staging views

`gText_StagedValues` (legacy `D_801D5608`) is a reused message-input area,
not a dedicated card record, rank table, or save structure. Its NA address
remains `0x801D5608`; the canonical alias neither defines C storage nor
establishes the allocation's full extent.

The isolated `text_message_staging.h` declares the existing incomplete
`TextStagingValues[]` view and the separate
`gDuel_DroppedCardTextValue` (legacy `D_801D56A8`) incomplete `s32[]` view.
The old declaration owner, union members and assertions stay unchanged.

[`text_staging.h`](../src/game/text_staging.h) owns all eleven C declaration
sites formerly spread across eight matching resident translation units and two
overlay units. Seven resident units replace their flat incomplete `s32` arrays
with incomplete arrays of `TextStagingValues`. Only element zero is used: the
union describes overlapping views at the original base, not repeated buffers.
Every named scalar in the resident views remains `s32`.

## Separate result text value

`DuelScene_UpdateResultRewards` copies the selected dropped card into its
existing selected-card/result fields and also sign-extends the ID into
element zero of `gDuel_DroppedCardTextValue` before showing result pages.
This separate display argument is outside the union's `0x80` view.
Branches that do not stage a dropped card retain the old value; no new
clearing or normalization is introduced.

Only the written word is established. The gap to the following colour
slots does not establish an array extent, persistent reward record or
exclusive allocation. The declaration remains an incomplete `s32` array:
the original split absolute store is not replaced by a scalar `.data`
view that would select a different store register.

## Producer evidence

| Producer | Offsets | Observed input and selected view |
| --- | --- | --- |
| `DuelField_CreateCardInfoTextBox` in `duel_field_display_objects.c` | `+0`, `+4`, `+8` | `Duel_CalcCardStats` supplies signed ATK/DEF; the source-mode branches copy/clamp the side's existing rank value. Uses `card_stats.attack`, `.defense`, `.rank` before `TextBox_Create`. |
| `Library_RefreshSelectedCardText` | `+0` | The selected card ID returned by `Library_GetGridCursorCardId`, narrowed through `s16`, is staged before text request 1. Uses `card.card_id`. |
| `func_80060E70` | `+0`, `+4` | Each two-halfword input row supplies its card ID and count before its text box is built. Uses `card.card_id` and `.count`; retains the separate direct-symbol store and cached destination pointer. |
| `BuildDeck_RefreshCountDisplay` in `build_deck_card_counts.c` | `+0`, `+4` | Copies screen words `+0x5A9C` and `+0x5AA0` before text `0xE`. The same TU's chest-return/deck-add paths update the respective totals. Uses `build_deck.chest` and `.deck`. |
| `Library_InitializeScreen` in `library_runtime.c` | `+0` | Counts the set per-card library story flags. Uses `library_count`, not a card ID interpretation. |
| `MemCardDialog_UpdateSave` in `mem_card_dialog.c` | `+0`, `+4` | Stages `MEM_CARD_BLOCK_COUNT - free_blocks` and the required block count before message `0xDB`. Uses `blocks.used` and `.needed`. |
| Modal card-number producers in `mem_card_dialog.c` | `+0x40` | Where their existing branches do so, stage `(gMemCardDialog_Port >> 4) + 1` through `gMemCardDialog_CardNumberArg[0]`; this remains a module-specific relocation view of the shared word, not new storage or a universal field interpretation. |
| `SaveData_UpdateDuelLoad` in `mem_card_dialog.c` | `+0x40` | A zero card ID in the left/right loaded deck publishes 1/2 before returning to the dialog. Uses `deck_validation.invalid_side`; the preceding sixteen words have no meaning assigned by this view. |
| `Duel_CalcRankScore` | `+0` through `+0x7C` | Writes sixteen rows of two side values through `gText_StagedValues[0].rank_rows`, keeping its side-first pointer walk and scoring order. |
| `Main_RunCredits` | `+0`, `+4` | The current matching C runner publishes its two input halfwords through the existing `pair` view before the secret-number text request; the old assembly-only exclusion no longer applies. |
| `FreeDuel_PlaceCursor` | `+0`, `+4` | Copies the selected duelist's two record halfwords into the shared unsigned words through `D_801D5608[0].pair`. |
| `Password_UpdateShopScreen` / `Password_RefreshStarchipDisplay` | `+0`, `+4` / `+0` | Message setup writes the table value and index through `D_801D5608[0].pair`; the starchip renderer writes the union's `starchips` member. Neither becomes a resident card/count interpretation. |

The union's `0x80` size comes from the rank producer's existing table, not an
inferred allocation boundary. Assertions also cover the named members at
`+4`, `+8` and `+0x40`. The field-card path does not clear its stat words for
unoccupied/non-monster records, and other producers overwrite the same words.
The [card-information payload analysis](duel-card-record.md#card-information-text-payload)
explains why these are not persistent ATK/DEF globals.

## Retained addressing and contract boundaries

- `TEXT_STAGING_AS_RANK_ROWS` and `TEXT_STAGING_AS_PAIR` are gone. Both
  selected a spelling the union already carries as a member, `rank_rows` and
  `pair`, and neither was separated from it by addressing: the rank table and
  the union are both outside small data at -G8, and the two `Pair` consumers
  compile at `gcc_2_8_1_g0_split`, whose `compiler_flags` and `maspsx_flags`
  in `config/slus_01411/compiler_profiles.json` both carry `-G0`. Written
  through the members, the resident SHA-256 and all five overlay images are
  unchanged. `TEXT_STAGING_STARCHIPS_ALIAS` and its `D_801D5608_starchips`
  linker alias are gone too: `Password_RefreshStarchipDisplay` writes the
  union's `starchips` member, which builds the same `-G0` store, so no unit
  reaches `D_801D5608` through an `asm` label.
- The default remains an incomplete array, preserving its small-data
  classification. No tentative definitions, `.data` attributes, pins, compiler
  profiles or grouped function order are added, removed or relocated.
- The interior symbol `gMemCardDialog_CardNumberArg` is still declared by `mem_card.h` as an
  incomplete `s32` array. Its other dialog writers retain that relocation;
  the two-player deck check retains its base-plus-`0x40` access. Equal numeric
  addresses are not grounds for replacing a relocation symbol. Its
  card-number name describes the modal producer's view only: the same word
  is `deck_validation.invalid_side` and also lies within `rank_rows`.
  It is not persistent card state or an exclusive allocation.
- `Main_RunCredits` is now matching C using the same incomplete union-array
  declaration and `pair` member. Its halfword inputs still become two word
  stores, not a new halfword output view. The historical matching record is
  preserved; only its current scoped namespace/include changes.
- No build-integrated candidate still references this family. Five consumers
  are now matched:
  `DuelField_CreateCardInfoTextBox` in `src/game/duel_field_display_objects.c`, `func_80060E70`
  in `src/game/card_list_deck_box.c`, `MemCardDialog_UpdateSave` in
  `src/game/mem_card_dialog.c` and `Library_RefreshSelectedCardText` in
  `src/game/library_card_grid.c`, plus `Password_UpdateShopScreen` in
  `src/overlays/password/shop.c`. The filter is
  `git grep -lw D_801D5608 -- 'src/candidates/**'`. The distinction that holds
  is the contract one. The probe over all configured
  entries of `config/slus_01411/candidates.json`, including the overlay
  candidates that carry a `module`, finds `D_801D5608` in no entry's
  canonical contracts, so no entry declares it privately; that file does not
  move when this header changes. The indirect text number
  reader (`func_80038148` via `TextStream_ReadU32LE`) retains its integer address
  boundary; the producer views do not establish a universal pointee type for
  that general script operand.

## Canonical names and native preservation

The two names describe existing staging roles, not universal pointee types
for text-script address operands. Resident producers retain every union
member, cast, side-first rank walk, cached pointer and original store order.
The unoccupied/non-monster branches still leave their old staged bytes.

Seven root scripts add aliases to the original logical providers for both
views. Twelve bindings have local numeric evidence; Japan's two original
automatic providers remain symbolic because its retail/split map is not
available locally. No numeric assignment is guessed or generated file
manually edited.

Free Duel and Password overlay producers remain unchanged and keep their
old symbols. The independently relocated
`gMemCardDialog_CardNumberArg` interior view also remains: equal or
overlapping addresses do not justify folding its accesses into the new
base name or assigning a universal card-number interpretation.

The existing result-controller witness receives only literal namespace
substitution and reconstructs its complete original after reversal.
Its cases, assertions, mutations, fixture layout, generators and compiler
setup remain. Legacy declaration/type/HI-LO ownership checks deliberately
retain their old-name header references and remain unchanged.
All old headers, wrappers, overlays/imports, profiles, groups and other
identities stay frozen. No new harness, fixture, probe or testcase is added,
and cloud/regional/overlay/witness suites are not executed locally.
Native compiler conservation does not replace fresh PR-side regional
full-image validation.

Actual compiler-expanded declarations retain 50 identical active pairs.
The four other source/profile pairs remain wholly absent under the
viewer's existing regional guard, with neither old nor canonical compiled
references. No include is moved outside that guard to manufacture a
declaration in a region that did not use it.

## Tick389 measurement

The first typed-view experiment reproduced the complete resident executable
under the unchanged named profiles, using an unchanged clean baseline followed
by an immediately seeded incremental cache. All five overlay images also
matched. The final clean resident gate reproduces SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.

Matching-resident headerless declarations decrease from 106 names / 195 sites
to 105 / 187. The three additional removed sites belong to the two overlays
and are outside that checker's resident inventory. Seventeen constant-index
resident staging writes/updates now select observed fields. The rank producer
already has its typed two-dimensional view and keeps it unchanged.

Tick389 starts independently at master `18abd9eb4`. It does not modify the
pending duel-card staging, AI pending-selection, frontend mode, or Free Duel
display-object contracts. The latter shares `screen_runtime.c` but a different
symbol family. Pending TU regrouping or source reclassification must carry
the new header/guard along with any moved consumer. Local experiment records
and build logs remain under ignored `tmp/tick389/`.
