# Shared card-list setup

The three contiguous functions formerly grouped as `duel_reward_setup.c`
belong to shared card-list UI setup, not an evidenced duel reward-table load.
The group now lives in `src/game/card_list_setup.c`, with its existing regional
wrappers and exact compiler profile preserved.

| NA address | Identity | Direct evidence |
| --- | --- | --- |
| `0x80032184` | `CardList_LoadSharedGraphicsStage` | Transfer callback configures streamed VRAM phases and palette uploads |
| `0x80032328` | `CardList_LoadSharedGraphics` | Registers that callback, requests the package, and waits for transfers |
| `0x80032370` | `Library_PruneRecentCardDrops` | Filters recent IDs against current chest quantities, then stable-packs them |

## Graphics loading

Both `Main_RunTrade` and `BuildDeck_InitScreen` call the blocking loader before
constructing their card-list UI. The old header's claim that Trade was its only
consumer was incomplete. The callback uploads graphics and palettes rather
than awarding cards or selecting rewards. The names deliberately do not assert
the identity of individual textures, fonts, or card data.

The default request remains `0x2189`, `0x4C` sectors. Japan selects `0x2184`
with the same count. Their callback modes 0/1 configure the two streamed
graphics transfers, mode 2 reads four sectors into the staging buffer, and
mode 3 uploads the palette through `LoadImage2`. Preserve all original
descriptor stores, volatile flags operations, transfer lengths and rectangles.

Europe selects `0x2997`, `0x51` sectors and retains five modes: the first
streamed transfer, a five-sector staging read, the language-indexed upload
followed by the second streamed transfer, a four-sector staging read, and the
final palette upload. The regional phase order, language offset, RAM/VRAM
destinations and flag masks are intentionally not normalized to NA/JP.

`D_8009B0F4`, its absolute view, `D_8009B118`, the scratch rectangle and the
European language selector are shared infrastructure, not private card-list
state. Their identities, types, addressing arms and literal data selectors
remain unchanged.

## Recent acquisition pruning

`BuildDeck_InitScreen` and `BuildDeck_UpdateExitConfirmation` both prune the
accepted sixteen-halfword `gDuel_awRecentCardDrops` table. Its NA address
`0x801D07BC` minus `0x56C` is `0x801D0250`, the accepted chest quantity base.
For each nonzero signed ID, the reverse pass tests the byte at `ID - 1` and
clears the ID only if that current chest quantity is zero.

The forward pass stable-packs nonzero IDs and clears moved-from slots. It
does not deduplicate, sort, append new history, or test total ownership.
Moving the last chest copy into the deck may therefore remove its recent
marker even though the player still owns that card. Keep the signed halfword
views, unchecked indices, integer reverse cursor and final decrement; do not
replace them with a pointer formed before the array or a wider ownership test.

## Group and evidence preservation

All seven current native manifests select the complete three-function object
at `gcc_2_8_1_g8_split_comm`. NA/JP retain `0x1A4/0x48/0x88` extents; European
images retain `0x1FC/0x48/0x88` including the additional callback phase.
Historical G0 requester and G0-split compactor experiments are not current
compiler choices. Source definition order, object coverage and European
`.rodata` placement remain unchanged.

The common C/header and two regional C wrappers move together. Obsolete
Japanese/European callback-provider and European requester-provider/caller
spelling macros retire together; all remaining regional aliases stay intact.
No frozen overlay body, historical attempt, reuse fingerprint, unrelated
loader global, compiler flag or accepted acquisition-table identity changes.
The existing compaction regression control receives only identifier/path and
its directly related explanatory-comment updates, not new test machinery.
