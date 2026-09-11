# Shared text-message staging views

`D_801D5608` is a reused message-input area, not a dedicated card record,
rank table, or save structure. Its address remains the linker assignment
`0x801D5608`; this change neither defines C storage nor establishes the
allocation's full extent. No semantic symbol rename is made.

[`text_staging.h`](../src/game/text_staging.h) owns all eleven C declaration
sites formerly spread across eight matching resident translation units and two
overlay units. Seven resident units replace their flat incomplete `s32` arrays
with incomplete arrays of `TextStagingValues`. Only element zero is used: the
union describes overlapping views at the original base, not repeated buffers.
Every named scalar in the resident views remains `s32`.

## Producer evidence

| Producer | Offsets | Observed input and selected view |
| --- | --- | --- |
| `func_80023144` in `duel_field_display_objects.c` | `+0`, `+4`, `+8` | `Duel_CalcCardStats` supplies signed ATK/DEF; the source-mode branches copy/clamp the side's existing rank value. Uses `card_stats.attack`, `.defense`, `.rank` before `TextBox_Create`. |
| `func_8002A2F4` | `+0` | The selected card ID returned by `func_8002A6B8`, narrowed through `s16`, is staged before text request 1. Uses `card.card_id`. |
| `func_80060E70` | `+0`, `+4` | Each two-halfword input row supplies its card ID and count before its text box is built. Uses `card.card_id` and `.count`; retains the separate direct-symbol store and cached destination pointer. |
| `func_80031E5C` in `build_deck_card_counts.c` | `+0`, `+4` | Copies screen words `+0x5A9C` and `+0x5AA0` before text `0xE`. The same TU's chest-return/deck-add paths update the respective totals. Uses `build_deck.chest` and `.deck`. |
| `func_8002BFCC` in `library_runtime.c` | `+0` | Counts the set per-card library story flags. Uses `library_count`, not a card ID interpretation. |
| `MemCardDialog_UpdateSave` in `mem_card_dialog_load_save.c` | `+0`, `+4` | Stages `MEM_CARD_BLOCK_COUNT - free_blocks` and the required block count before message `0xDB`. Uses `blocks.used` and `.needed`. |
| `SaveData_UpdateDuelLoad` in `save_data_transfer_runtime.c` | `+0x40` | A zero card ID in the left/right loaded deck publishes 1/2 before returning to the dialog. Uses `deck_validation.invalid_side`; the preceding sixteen words have no meaning assigned by this view. |
| `Duel_CalcRankScore` | `+0` through `+0x7C` | Writes sixteen rows of two side values through `D_801D5608[0].rank_rows`, keeping its side-first pointer walk and scoring order. |
| `FreeDuel_PlaceCursor` | `+0`, `+4` | Copies the selected duelist's two record halfwords into the shared unsigned words through `D_801D5608[0].pair`. |
| `Password_UpdateShopScreen` / `Password_RefreshStarchipDisplay` | `+0`, `+4` / `+0` | Message setup writes the table value and index through `D_801D5608[0].pair`; the starchip renderer uses the existing signed scalar alias. Neither becomes a resident card/count interpretation. |

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
  unchanged. `TEXT_STAGING_STARCHIPS_ALIAS` remains and exposes only the
  password source's existing `D_801D5608_starchips` alias; its assembler
  identity is still `D_801D5608`. The `#define` for it in
  `src/candidates/password/func_8016A37C.c` is inert -- a word-bounded grep
  finds the alias used only in `src/overlays/password/shop.c` -- and is left
  in place because that consumer is in an open pull request.
- The default remains an incomplete array, preserving its small-data
  classification. No tentative definitions, `.data` attributes, pins, compiler
  profiles or grouped function order are added, removed or relocated.
- The interior symbol `D_801D5648` is still declared by `mem_card.h` as an
  incomplete `s32` array. Its other dialog writers retain that relocation;
  the two-player deck check retains its base-plus-`0x40` access. Equal numeric
  addresses are not grounds for replacing a relocation symbol.
- `Main_RunCredits` remains an explicit-relocation assembly consumer. Its
  halfword inputs become two word stores, not a separate halfword output view.
  There is no C declaration to migrate.
- Five build-integrated candidates reference this family, not one:
  `src/candidates/func_80023144.c`, `func_8002A2F4.c`, `func_8003E854.c`,
  `func_80060E70.c` and `password/func_8016A37C.c`. The filter is
  `git grep -lw D_801D5608 -- 'src/candidates/**'`, and each of the five
  includes `src/game/text_staging.h` and declares nothing privately, so the
  password producer is one example of the population rather than the whole of
  it. The distinction that holds is the contract one. The probe over all 171
  entries of `config/slus_01411/candidates.json`, four of which carry a
  `module` and are the overlay candidates, finds neither `D_801D5608` nor
  `D_801D5608_starchips` in any entry's canonical contracts, so no entry
  declares either symbol privately; that file does not move when this header
  changes. The password candidate keeps its recorded build and target
  fingerprints and its other dependency contracts. The indirect text number
  reader (`func_80038148` via `func_80036D70`) retains its integer address
  boundary; the producer views do not establish a universal pointee type for
  that general script operand.

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
