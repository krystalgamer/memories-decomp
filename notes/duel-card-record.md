# Duel card record table (`D_801A7AD8`)

## Scope and evidence policy

The initial phase structured `D_801A7AD8`, the duel/card table at
`0x801A7AD8`. Primary evidence is the verified North American
`game/SLUS_014.11` target disassembly, exact-matching C users in `src/game`,
and the final byte-for-byte build comparison. GMS, the Unchiga tree, and the
old reference tree were used only to corroborate offsets and access widths;
their guessed declarations were not copied.

The staging-buffer contract below extends that evidence to the existing
base-relative views of the same records, without claiming a new allocation.

The generated `notes/global-usage.csv` snapshot lists matching-C and
assembly users; filter it on global address `0x801A7AD8`. The
typed-migration inventory below separately records which matching sources
have adopted the shared declaration. `Duel_CollectMatchingFieldCardObjects` now uses the shared
typed C declarations directly, including the `D_801A7B64` interior alias.
The generated report may predate a promotion until it is regenerated.

## Typed setup result and publication

`Duel_SetupCardRecord` exposes its existing `DuelCardRecord *` result in
`duel_deck_card.h`. Every return is the selected `gDuel_aCardRecords`
element already initialized by the body. Four callers discard that
pointer unchanged; `Duel_SetupFieldCardFromDeck` keeps it as a record and publishes the
new display object through its `object` member rather than a raw pointer
store through a byte cursor. The member retains its existing `G32`.

Field-slot and deck-index normalization remain separate, using 15 and 40
respectively. Full argument words, signed halfword loads, the separate
`0x48000` replay materialization, publication-before-object-index order
and both GPU uploads' addressing/captures remain unchanged.

Setup publishes the selected entry with typed indexing into
`gDuel_aDeckCardRecords`, rather than byte-base arithmetic. The normalized
deck index and six-byte stride are unchanged; the separate byte offset
is still retained for the staged-deck view's high-base/member
materialization. The destination `data` member keeps its opaque type.

The staged ID capture uses an explicit `u16` value view of the canonical
signed `id` member rather than reinterpreting its address. The unsigned
halfword load and captured bits remain unchanged; the later signed
stat-index conversion is still explicit. This does not collapse the
staged high base or alter the member's declared type.

The field-object wrapper reads the published data through
`DuelDeckCardRecord::id`, not a bare signed-halfword pointer. Its immediate
setup call stores a pointer to the selected six-byte deck record, and the
replay view reaches that same live record after identical slot-tag
normalization. This is a local view at that handoff: the global `data`
member remains opaque. The signed ID load, separate staging high base,
argument evaluation and object-publication order are unchanged.

Resume reconstruction uses the same local deck-entry view for its two
occupied-field traversals, records 5-14 and 20-29. Both rebuild arguments
read the canonical signed `deck_index` member rather than byte +0x02.
The `D_801A7B64` interior alias, record stride, flags/stat-modifier
snapshots and restores, helper ordering and object-flag refresh remain
unchanged; the global data member stays opaque.

## Battle preview replay IDs

The attacker and defender preview paths in `DuelScene_UpdateBattle`
read `DuelCardReplayRecordBlock::record.card_id` instead of a raw signed
halfword at `+0x36C0`. The existing block places the live record at
`+0x36B4`, and its canonical signed card-ID member is at `+0x0C`.

Each view is applied after the original byte-base, slot-times-`0x1C` and
`0x48000` high-base calculation. The padded block is an offset view, not
an array element or a new record stride. Both local base identities,
signed halfword loads, field-slot reads and preview-request ordering
remain unchanged. This does not retype the shared scratch buffer, change
request gating or replace unrelated width-specific battle accesses.

## Per-card-slot ground coordinates

`gDuel_aCardSlotGroundPositions` (originally `D_800908A0`, North American
`0x800908A0`) supplies one X/Z pair for each of the thirty live card slots.
The 120-byte initializer retains all entries, including the zero pairs
for hand records 0-4 and 15-19. This is not the 2-by-20 perspective-indexed
field-grid table.

The full-table projector `Duel_ProjectCardSlotScreenPositions` loads the pair into GTE X/Z,
supplies height 0, and writes separate biased screen outputs to
`gDuel_aCardSlotScreenPositions`. The single-X projector `Duel_ProjectCardSlotScreenX` supplies height -24.
Field-card setup uses the typed pair for the factory's X/Z translation,
whose Y is -24; debug effect setup reads slot 6's pair through halfword
indices 0xC/0xD and supplies the same height. Height belongs to those
consumers, not this table.

In the North American retail input, every field-grid map entry selects
the same pair from this per-record table as from the grid-coordinate
table. The namespaces remain separate: one indexes thirty live records,
the other two perspectives of twenty cells.

The owner/header retain both existing declaration arms: thirty
`DuelFieldPosition` entries for the typed users and an unsized `u16` view
for the measured halfword readers. Generic `x`/`y` member names, signed and
unsigned loads, pointer walks, projection height/bias and loop bounds are
unchanged. The unrelated French SDK label at numeric `0x800908A0` and all
seven binding addresses are preserved.

The projector's name describes only that operation. A bounded North
American retail scan found no direct resident J/JAL references or aligned
stored target words for `0x800177C4`; this does not exclude computed
references or establish runtime unreachability. No caller frequency is
assumed. Its original address-based source path and named
no-strength-reduction RTPS profile remain unchanged, as do the flat input
walk, thirty-record loop, side-specific bias and geometry-offset reset.

`Duel_ProjectCardSlotScreenX` (`0x8001B0CC`) has an observed resident
call at `0x8001B4C8` in the selected-card staging controller. The supplied
`gDuel_bCardMoveSlot` is a live-record index selected through the field-grid map;
the caller subtracts `0x1E` and uses the result as a sine-slide X target.
The helper keeps its full-word index and signed `s16` return, even though
the packed projected X member remains unsigned for other consumers.
Its fixed height -24, single-result behavior and viewport reset remain
distinct from the full-table projector's height 0 and biased pair writes.
The original source path, RTPS profile and all arithmetic are unchanged.

## Live-card upload rectangles

`gDuel_aCardUploadRects` (`D_80177EA4`, `0x80177EA4`) is the SDK `RECT`
array used by `Duel_SetupCardRecord`. Slot `i` selects element `2 * i`
for a 20-word-wide, 32-line transfer and element `2 * i + 1` for a
64-by-1 transfer. Both rectangles are filled immediately before their
`LoadImage` calls. The first source is the selected `0x580`-byte card-data
block at `gDuel_aCardThumbnailBlocks`; the second is the same block offset at `D_8018C7D8`,
`0x500` bytes into that data. This names the upload role, not the
interpretation of the auxiliary row's contents.

The table serves live records, not only field objects:
`DuelScene_UpdateDrawResolution` calls the setup routine before creating
a hand-card display object. The declaration therefore lives with the
lifecycle API and remains an unsized array. The Japanese wrapper retains
its `D_80187EA4` mapping; European resident imports retain their original
addresses.

## Copied thumbnail blocks

`gDuel_aCardThumbnailBlocks` is an unsized array of `DuelCardThumbnailBlock`, owned beside
the deck records in `duel_deck_card.h`. Each `0x580`-byte element holds
`0x500` bytes of 40-by-32 8-bpp thumbnail pixels and 64 halfword CLUT
entries. Header layout contracts retain that total stride and the CLUT's
`+0x500` offset. `Duel_PopulateCombinedDeckData` still copies the complete
block for each combined-deck entry; it advances a block pointer instead
of adding a byte stride.

The two setup uploads establish the split. The thumbnail path in
`Duel_DrawCardFrame` independently supplies `0x380, slot + 0xE0` as the
texture's CLUT coordinates, matching the setup routine's second upload.
This identifies the previously unnamed auxiliary row as the thumbnail
CLUT rather than the baked title.

The alternate writers preserve the same packed bytes:
`CardPreview_HandleResourceTransfer` uploads the package's thumbnail
window as an 8-by-88-word rectangle at `src_x + 0x38, src_y`.
Placement and ritual read that exact rectangle back into the indexed
thumbnail block. Neither path interprets or rearranges the payload.
Only their destination selection changes from byte multiplication to
block indexing.

`D_8018C7D8` remains the existing byte-addressed interior alias, `0x500`
bytes beyond the block array's base; setup retains its independent symbol materialization
and block-stride multiplication for the CLUT upload. No linker binding,
allocation, regional mapping, preview layout or opaque staging-buffer
lifetime changes.

## Marker data record

The marker refresh and `Duel_CreateHandCardDisplayObject` read data +0x04 through
`DuelDeckCardRecord::pre_shuffle_index`. `Duel_SetupCardRecord` supplies a pointer
into that six-byte record array, and `Duel_PopulateCombinedDeckData` fills
the same byte. The separate five-byte `DuelCardDisplayData` prefix is no
longer needed.

This reuse is limited to those two reads. The global `data` member stays
opaque. The byte is the pre-shuffle slot, not a flag mask or current
combined-deck index. The plus-one byte conversion, view-mode reads and
negative-mode override remain exact.

## Ritual deck-entry views

The ritual rebuild reads `DuelDeckCardRecord::deck_index` at data +0x02,
not an untyped byte-array element. This is the selected live card's deck
entry: setup publishes that pointer, and the preceding ritual readback
phase changes its ID and associated thumbnail block without replacing the
data pointer.
The existing signed `s8` member preserves the old signed byte load and
full-word argument promotion.

The readback phase selects the associated thumbnail block through
`DuelDeckCardRecord::data_block_index`, the unsigned byte at +0x03. This
view uses the existing reloaded local `data`, not a fresh field read through
`card->data`; unsigned values keep their original full-word promotion.

ID replacement writes the canonical signed `id` member after the existing
`u16 ritual` capture and an explicit `s16` value view. The same sixteen
payload bits, including high-bit values, are retained; the later live-card
ID mirror still uses the unchanged unsigned temporary.

The non-struct `u8 **` pointer reload and local `u8 *` pointer type are
deliberately untouched. These local member views do not retype the global
data pointer or alter helper reloads, slot/deck normalization, readback
geometry, object publication or state fallthrough.

## Combined-deck input window

During `Duel_PopulateCombinedDeckData`, the staging input contains 80
pre-shuffle index bytes at +0x1BB70 and 80 halfword card IDs at +0x1BBC4.
`Duel_ShuffleBothDecks` supplies two 40-entry runs: `Duel_ShuffleDeck`
initializes each byte to its pre-shuffle slot and permutes it together
with the ID. The population step copies those bytes into the existing
deck record's `pre_shuffle_index`. This names the paired original-slot
payload only; it does not introduce any flag behavior.

The shuffle API uses optional `const u16 *` source lists, `u16 *` ID
outputs and `u8 *` original-slot outputs. Each ID step remains two bytes;
these lists are not the six-byte `DuelDeckCardRecord` array. The caller
retains the raw saved-buffer globals and applies halfword views only at
the shuffle boundary. Null sources still select weighted generation,
and raw sixteen-bit payloads, output offsets and RNG order are unchanged.
The opponent generation weights have a separate incomplete `u16` array
view and a halfword cursor. The three-sector transfer is unchanged; no
new table bound, scan length, copy limit or RNG operation is introduced.

`gDuel_awPlayerShuffledDeck` exposes the ID storage as an incomplete
`u16` array, and the two-deck wrapper holds that halfword base directly.
The neighboring permutation outputs still use explicit byte views at
`-0x54` and `-0x2C`; the second ID run remains `DECK_SIZE` halfwords
after the same base. No bound, combined allocation or new alias is
introduced, and the raw saved/staging buffer declarations stay unchanged.

An input view in `duel_card_staging.h` names those two arrays instead of
raw offset loads. Its use applies only to this population lifetime; the
shared buffer stays an unsized byte array. The separate source/image pointers,
unbounded unique-ID search, block-copy order and output-record stride
remain unchanged.

## Shared staging-buffer views (`D_8015C424`)

`src/game/duel_card_staging.h` owns the declarations of `D_8015C424`,
`gDuel_awCombinedDeckCardIds`, and `gDuel_awUniqueDeckCardIds`. The staging
base is not exclusively a card database: `Duel_RequestCombinedDeckData`
passes it through the file-transfer descriptor's integer `position` field,
`Duel_PopulateCombinedDeckData` copies card-image blocks from it, trap
resolution uses scratch slots, and `func_8001944C` uses the base for GPU
readback. The default declaration therefore remains an incomplete byte
array. No size or single lifetime is asserted for that raw buffer.

The typed views are established by address arithmetic and producers, not by
the base symbol's name:

| Staging-relative offset | Absolute address | Evidence-backed view |
| --- | --- | --- |
| `0x48000 + 0x36B4` | `0x801A7AD8` | `DuelCardRecord`: the same field-card table declared in `duel_card.h` |
| `0x48000 + 0x39FC` | `0x801A7E20` | `DuelDeckCardRecord`: `gDuel_aDeckCardRecords`, filled by `Duel_PopulateCombinedDeckData` for `COMBINED_DECK_SIZE` (`80`) entries |
| `0x1BC68` | `0x8017808C` | `gDuel_awCombinedDeckCardIds`, the sort source |
| `0x1BD0C` | `0x80178130` | `gDuel_awUniqueDeckCardIds`, the deduplicated list and transfer callback data |

`DuelCardReplayRecordBlock` retains its padded offset view, now shared by
card lifecycle, draw resolution, phase entry, AI selection setup, trap
presentation, and the integrated `DuelScene_UpdateExodiaResult` candidate.
`DuelStagedDeckRecordBlock` provides the corresponding small-displacement
deck-record view for `Duel_SetupCardRecord`. These views preserve the
historical high-base materialization followed by a small field displacement;
replacing them with a direct interior-symbol reference would change the
relocation identity.

The old Exodia-only six-byte record is removed. Its `type` at `+2` was not
read as a type by either consumer: the producer writes the deck index there.
Both Exodia detection and the AI exporter now use `DuelDeckCardRecord` in
`DuelCardStagingDeckView`, with the actual 80-record extent rather than an
indexed one-element placeholder. Compile-time assertions check all six-byte
record fields and the three staging-view offsets.

Every C consumer of this family uses the shared header: nine matching
resident translation units when this change landed, including the Exodia
result, which now shares `src/game/duel_draw_resolution.c` with the draw
unit. This removes
12 private declaration sites (including the candidate and the draw unit's
same-symbol alias). The matching-resident headerless inventory decreases
from 109 names / 205 sites to 106 / 194 on the Tick388 base.

The following boundaries remain deliberate:

- Both typed-view consumers share one declaration: `D_8015C424_cards`, an
  `asm("D_8015C424")` alias beside the byte view, with no guard on either.
  The AI exporter used to take a second declaration of the same type under
  the base name; the alias reproduces its addressing at `gcc_2_8_1_g0_split`
  as well, so both guards are gone. It still introduces no new linker
  symbol -- `objdump -t` on the exporter's object lists one undefined
  `D_8015C424`.
- Deck ID reads in setup remain unsigned, while the draw path still reads
  `index_02` as signed. Typed fields do not normalize load widths or signs.
- Byte offsets for image blocks, GPU pixels, ID staging lists and trap
  scratch remain byte views. They are not fields of the field-card record.
- No tentative definitions, C storage ownership, compiler profiles, register
  pins, integer ABI boundaries, or grouped function ordering change.

The candidate's `+0x36C0` load now names `record.card_id`, retaining its
index-first address arithmetic. Its object fingerprint remains
`5b8db9b3dbaf377624bf31a2890b934f9fff263f86a4f6db4db586c8d30d6615`.
Only the removed private `D_8015C424` dependency and its aggregate contract
hash change in `candidates.json`, following the
[build-integrated candidate contract](build.md#unmatched-declarations-and-data-ownership).
All 19 candidate
fingerprints and the complete resident executable remain unchanged.

## Conservative shared layout

### 3D field-card renderer (`0x80015EF4`)

`Duel_DrawFieldCards` passes the same `DuelCardRecord *` it walks through
both occupied field rows to `Duel_RenderFieldCard3D` (formerly
`func_80015EF4`). The callee now takes that shared type rather than `void *`
and the redundant `DuelCardRenderHolder` prefix. Its only record reads are
`object` at `+0x00` and `table_index` at `+0x18`. The latter is explicitly
cast to `s8`: retail's `>= 15` half-turn test uses a signed byte, so values
`0x80..0xFF` must not acquire the unsigned interpretation of the shared
field. This does not settle the signedness of every other consumer.

The object translation now uses the existing `DisplayObject` halfword
members at `+0x30`, `+0x32`, and `+0x34`. The first two need `s16` casts
because the shared union exposes unsigned halves while this renderer
sign-extends them into the matrix. Texture coordinates and colour channels
retain byte views of their named members, not byte offsets from the object
base, preserving retail's byte loads without inventing a new object layout.

The semantic name is supported by the whole rendering path: lit card-face
corners go through `RotColorDpq` into a `POLY_GT4`, while copies flattened to
`y = 0` go through RTPS into a `POLY_FT4`. Both are submitted to ordering
table 2. Cards with flag `0x400` instead take `Duel_DrawProjectedFieldCardFrame` in the caller;
the name does not claim that separate path or a universal sprite renderer.

### Field layout

`src/game/duel_card.h` defines `DuelCardRecord`, with a checked size of
`0x1C`, checked offsets for every exposed field, explicit unknown padding at
`+0x19..+0x1B`, and an unsized typed extern. The known
initialization loop covers 30 records; the extern remains unsized because
several functions use interior aliases or derived subranges.

| Offset | Width | Shared field | Exact evidence |
| --- | ---: | --- | --- |
| `+0x00` | 4 | `object` | `Duel_ResetCardRecords` clears it with `sw`; `DuelEffect_ApplySwords`, `func_8002778C`, `func_800278A0`, and `func_80027DF8` load it as an object pointer. `Duel_CollectFieldRowCardObjects` exports the same word as an opaque value. |
| `+0x04` | 4 | `data` | `Duel_ResetCardRecords` clears it with `sw`; `Duel_SetupCardRecord` stores a pointer into `gDuel_aDeckCardRecords`; `Duel_UpdateCardDisplayMarker` loads it and then reads a byte from the pointed-to object. |
| `+0x08` | 2 | `screen_x` | `Duel_DrawProjectedFieldCardFrame` stores the projected x coordinate minus `0x1A` and passes it to the card-frame renderer. |
| `+0x0A` | 2 | `screen_y` | The same path stores projected y minus `0x1E`, with the existing side/selector-dependent one-pixel adjustment, before drawing the frame. |
| `+0x0C` | 2 | `card_id` | `Duel_SetupCardRecord` stores it with `sh` and later uses `lh`; `Duel_CollectFieldCardsByType` uses `lh`; `func_80027DF8` uses both `lhu` for copying and `lh` for signed table indexing. The shared field therefore fixes the width while exact users retain explicit signed views where required. |
| `+0x0E` | 2 | `attack` | `Duel_SetupCardRecord` stores the first card-stat component with `sh`; exact stat calculation users consume the record. |
| `+0x10` | 2 | `defense` | `Duel_SetupCardRecord` stores the second card-stat component with `sh`; exact stat calculation users consume the record. |
| `+0x12` | 2 | `stat_modifier` | `Duel_SetupCardRecord` clears it with `sh`; the exact stat comparison path passes the record to the stat calculators. |
| `+0x14` | 2 | `terrain_modifier` | `Duel_SetupCardRecord` stores the terrain result with `sh`. Some state tests intentionally load a 32-bit word beginning here, spanning this field and `flags`. |
| `+0x16` | 2 | `flags` | Repeated `lhu`/`sh` accesses in `Duel_UpdateCardDisplayState`, `Duel_ApplyCardObjectFlags`, `func_8001EFD4`, cursor/effect helpers, scans, and mutators establish the width and bit masks. |
| `+0x18` | 1 | `table_index` | `Duel_SetupCardRecord` stores the selected table index with `sb`. |
| `+0x19` | 3 | padding | No field type is asserted. |

The projected-frame path now takes `DuelCardRecord *` directly, removing its
duplicate `DisplayProjectionTrackedObject` view and both caller casts.
`table_index` remains unsigned in storage; this consumer explicitly casts it
to `s8` for both orientation comparisons, preserving the signed treatment of
`0x80..0xFF`. Its second object-pointer read remains volatile at the same
expression boundary, and the display-object fields retain their original
unsigned halfword reads. The shared `object` member remains opaque; this
consumer takes a `DisplayObject` view without retyping every other owner.

The target initialization at `0x8001778C` advances both cursors by `0x1C`
and stops at 30. Independent record address calculations use the same
stride: `Duel_SetupCardRecord` scales an index by 28,
`Duel_CollectFieldCardsByType` selects
five-record slices within 15 records per side, and `func_80027DF8` selects
the two 15-record side blocks.

The encoded field-card slot paths in `DuelCard_DecodeTaggedSlotIndex`,
`Duel_SetupCardRecord`, `Duel_SetupFieldCardFromDeck`, and `DuelScene_UpdateDrawResolution` add
`DUEL_CARD_SIDE_RECORD_COUNT` (`15`) after masking a tagged value with
`0x7F`. The `0x80` tag test is unchanged, and untagged values are still
passed through without that normalization or new bounds checks.
This offset belongs to the 30-entry field-card table. The separate deck-card
index normalization in `Duel_SetupCardRecord` still uses `DECK_SIZE` (`40`).

Matching `Duel_CheckRitual` scans the five records at indices `5-9` for a
zero `gDuel_bActiveSide`, or `20-24` for any nonzero value. These are expressed as
`DUEL_FIELD_ROW_SIZE` and `DUEL_CARD_SIDE_RECORD_COUNT + DUEL_FIELD_ROW_SIZE`,
with `DUEL_CARD_FLAG_OCCUPIED` selecting eligible records. Its local `Card`
view remains unchanged; it has not adopted the shared `DuelCardRecord` type.
There is no face-down or defence-position test in that eligibility filter:
matching occupied cards in either orientation remain candidates.

The ritual table has a separate five-halfword recipe layout: one ritual ID,
`DUEL_RITUAL_TRIBUTE_COUNT` (`3`) tribute IDs, and one result ID, giving
`DUEL_RITUAL_RECIPE_HALFWORD_COUNT` (`5`). The search stops at a zero ritual
ID rather than a fixed recipe count. Each matched candidate is cleared from
the temporary list before the next tribute search, so repeated tribute IDs
must come from distinct field records. The optional output contains the three
matched records' first words; the return value is the recipe's result ID.
The clearing is confined to the temporary candidate list, not the actual
field records. The normal ritual execution routine removes the exported
cards later; see the selection/removal distinction in
[`the-game.md` §5.7](research/the-game.md#57-traps-and-rituals).

The draw-phase entry `DuelScene_UpdateDrawPhase` scans `DUEL_CARD_RECORD_COUNT` (`30`)
records at `DUEL_CARD_RECORD_SIZE` (`0x1C`) strides to reset per-turn flags.
That bound belongs to this record table, not to the separate
`DUEL_FIELD_SIDE_GRID_SLOT_COUNT` layout. Occupied records retain their
other flags while `DUEL_CARD_FLAG_USED_THIS_TURN` is cleared; unoccupied
records have their flag word cleared.

`DUEL_CARD_FLAG_USED_THIS_TURN` is also the shared action-eligibility latch
for monsters. `Duel_SetupCardRecord` initializes a new field record with only
`DUEL_CARD_FLAG_OCCUPIED`, so placement does not impose summoning sickness.
The state-5 attack filter requires occupied, attack position, and used clear;
state 9 sets used on the attacker. The L1/R1 position-change helper likewise
requires used clear, while its callback toggles only the defense-position
bit. This one lifecycle therefore enforces both one attack per monster and
the post-attack position lock. See
[`duel-turn-action-eligibility.md`](duel-turn-action-eligibility.md) for the
instruction-level state transitions and masks.

Its hand snapshot and rebuild loops use `HAND_SIZE` (`5`). The active side's
base index is `side * DUEL_CARD_SIDE_RECORD_COUNT`, with 15 records per side.
The remaining draw count is still `HAND_SIZE - n` after compacting valid hand
entries. These names preserve the local byte views, record traversal, and
object creation. The pending-object table's extent is `DUEL_SIDE_COUNT`
(`2`); it is still indexed by the active side.

Corroboration agrees without defining the shared type:

- GMS declares a guessed `int[]`, but consistently indexes records as seven
  words, uses halfword 6 for the `+0x0C` ID, and the high half of word 5 for
  the `+0x16` flags. Its guessed declaration was rejected.
- Unchiga sources independently use a `0x1C` stride and the same
  `+0x00`, `+0x04`, `+0x0C`, and `+0x16` widths. Its recompilation records
  for `Duel_UpdateCardPickCursor` (`0x800240B0`, originally
  `cursor_card_pick_tick`) and
  `find_table801a7ad8_entry_matching_id` report zero differing words.
- The old reference assembly and C repeat these accesses, but were treated
  only as a cross-check against the target and exact local build.

## Stat computation and comparison

Matching [`Duel_CalcCardStats`](../src/game/duel_card_and_status_draw.c) uses
signed halfword views of `attack`, `defense`, `stat_modifier`, and
`terrain_modifier`. It independently clamps
`attack + stat_modifier + terrain_modifier` and
`defense + stat_modifier + terrain_modifier` to `0..9999`, packing ATK in
the low halfword and DEF in the high halfword. The modifiers are summed
before that clamp, not applied through separate saturation steps.

[`Duel_CalcGuardianStarBonus`](../src/game/duel_battle_stats.c)
chooses each record's first or second packed star using
`DUEL_CARD_FLAG_USE_GUARDIAN_STAR_2`. For valid star IDs, the cycle helper
returns `+500`, `-500`, or zero relative to the left record. A null right
record returns zero without a matchup.

The [battle-stat helpers](../src/game/duel_battle_stats.c) add that signed
result to the selected left pre-matchup stat, then apply only an upper
limit of `9999`. Matching
[`func_8001EFD4`](../src/game/duel_battle_stats.c) compares that adjusted left
value against the right pre-matchup ATK or DEF; each position flag chooses
which stat its side contributes. It does not also apply a guardian
adjustment to the right value.

| Left pre-matchup value | Right pre-matchup value | Left guardian adjustment | Returned difference |
|---:|---:|---:|---:|
| `9999` | `9900` | `+500` | `99` |
| `9999` | `9999` | `-500` | `-500` |
| `100` | `0` | `-500` | `-400` |

These are source-derived arithmetic examples, not new gameplay traces.
The intermediate left value can be negative; these helpers do not write
that value back into either record.

With no right object, `func_8001EFD4` returns the left pre-matchup ATK
without consulting its position or guardian matchup. With two objects,
unequal values return their signed difference. Equal values return zero
if either card is in defence position, or `-1` if both are in attack
position. This is a return convention, not a claim that edited values
could never produce an arithmetic difference of `-1`.

The retail battle sequencer `DuelScene_UpdateBattle` calls the comparison at
`0x8001FBFC`. Its negative-result path tests for a result below `-1` at
`0x8001FD2C` before reaching the LP update at `0x8001FD5C`; a returned
`-1` is not processed there as one point of LP damage. This corroborates
a concrete consumer of the convention without auditing every trap,
presentation, or outcome branch.

## Card-information text payload

Matching
[`DuelField_RefreshCursorCardInfo`](../src/game/duel_field_display_objects.c) maps the
source's row/column through the active side's `D_800907D8` field view and
passes the resulting card-record index to
[`DuelField_CreateCardInfoTextBox`](../src/game/duel_field_display_objects.c). This is a field-card text
request path, not by itself evidence for every button or visibility mode.

The constructor clears `gText_BranchSelectorA` and `gText_BranchSelectorB` on entry. For an
occupied record it captures the card ID in `gDuel_wSelectedCardID`. Only
the occupied-monster branch (packed type below `20`) writes
the words at `D_801D5608 + 0` and `+4`, now expressed as
`D_801D5608[0].card_stats.attack` and `.defense` through
[`text_staging.h`](../src/game/text_staging.h): they receive the ATK and DEF returned by
`Duel_CalcCardStats`. No opponent-dependent guardian adjustment is added
to these two assignments.

The base text ID is `0x50`, or `0x51` for an occupied non-monster.
Opaque source-mode conditions add `2` or `4` before the value is passed
as `TextBox_Create`'s `string_id` argument. These are text-request
variants; their full scripts and user-visible meanings are not decoded
by this constructor alone.

When source byte `+0x17` is `3` and `gText_BranchSelectorA` is nonzero, a separate
call to [`DuelField_GetGuardianStarDisplayCode`](../src/game/duel_field_display_objects.c)
supplies `gText_SelectedCardColorIndex`. That helper maps its guardian comparison to `4`
for neutral, `1` for negative, or `6` for positive. The selector is
separate from the two pre-matchup stat words, not an extra term in them.

The non-monster and unoccupied-record branches contain no clearing of
those stat words. Later selector changes can also leave `gText_BranchSelectorA` zero
after the data was prepared; `TextBox_Create` is still called after the
conditional setup. Matching
[`FreeDuel_PlaceCursor`](../src/overlays/free_duel/screen_runtime.c) reuses
the same two words for its selected duelist's win/loss values. Interpret
`D_801D5608` with its current caller and text-request state, not as
dedicated persistent ATK/DEF storage. No conclusion about hidden-card
visibility or a complete scratch-buffer layout follows from these writes.

## Per-side view-mode handoff

The value editor's shared option has a confirmed resident consumer, even
though its visible caption remains unassigned. Matching
[`Duel_InitSideStates`](../src/game/duel_state_init.c) first clears
`DuelSideState.card_view_mode` at byte `+0x1F` of both `D_800E9FF0` side
records. These are `0x20`-byte records, not the `0x1C`-byte card records
described above:

| Side | Record base | View-state byte |
|---:|---|---|
| `0` | `0x800E9FF0` | `0x800EA00F` |
| `1` | `0x800EA010` | `0x800EA02F` |

Only the branch with both `D_8009B360[0] < 0` and
`gDuel_bOpponentID < 0` copies the low byte of `gDuel_wTwoPlayerCardViewMode` into both
positions. For a nonnegative opponent ID, the initializer instead sets
the side-1 byte to `0xFF` (signed `-1`) while side 0 stays zero.
The remaining negative-opponent/nonnegative-`D_8009B360` case leaves both
cleared. Do not collapse these branches into a universal two-player copy.

The initializer points `D_8009B1C8` at the active side record; the matching
[turn-switch helper](../src/game/duel_scene_battle.c) refreshes that pointer
when changing sides. Matching
[`Duel_CreateHandCardDisplayObject`](../src/game/duel_init_scene.c) reads `card_view_mode`
through a signed view, after calling the base card-object constructor:

| Active-side value | Post-construction operation |
|---|---|
| `0` | Skips this mode-dependent remap. |
| Positive | Sets the card-record display-marker flag (`0x2000`) and writes the associated data byte `+4`, plus one, to the returned display object's byte `+0x67`. |
| Negative | Takes the same flag-setting path, then overrides display-object byte `+0x67` with `0xFF`. |

Thus the option value `1` is not a request for resource index `1`; the
positive-mode resource value depends on the associated data. Skipping the
mode-dependent branch is not itself an assignment of resource index zero.
The per-side mode, card-record flag, and display-object resource byte are
distinct storage and must not be conflated with the
[Millennium Eye tutorial's resource-index edits](modding-tutorial-evidence.md#millennium-eye-opponent-card-display).

The same active-side byte also gates a text-selector override in
[`DuelField_CreateCardInfoTextBox`](../src/game/duel_field_display_objects.c). For an occupied field
record on the acting side, zero forces `gText_BranchSelectorA = 2`, including after
the face-down path selected `3`. A nonzero byte leaves that override
disabled; this does not decode what either text variant displays.
These consumers read copied side state, not `gDuel_wTwoPlayerCardViewMode` directly.
No complete visibility policy, option caption, or additional buffer layout
is inferred here, and no new runtime trace is claimed.

A later `duel_display_callbacks` trace (#418; interpreter CPU, setup
confirmed) matched the nonnegative-opponent branch in a Free Duel against
Villager2. The opponent's card already carried flag `0x2000` in hand record
15 and kept it on field record 20, through a set, a defense switch and two
turn changes. The player's records never had it.
`Duel_UpdateCardMarkerRemovalTurn` did not run. Its only installer, in
`DuelScene_UpdateCardPlacement`, skips single-card placements (scene flag
`0x4000`) and turns only marked cards in the work slots. That suggests the
marker is removed when the CPU places several hand cards at once, such as
a fusion, or by `DuelEffect_UpdateRevealCard`. Neither was observed, and a
player cannot make the CPU fuse on demand, so the flag keeps its neutral
name.

## Typed migration snapshot

The following matching C sources include `duel_card.h` and use its typed
extern:

`Duel_ResetCardRecords`, `Duel_UpdateCardDisplayMarker`, `Duel_UpdateCardDisplayState`, `Duel_CreateCardDisplayObject`,
`Duel_ApplyCardObjectFlags`, `func_80019BD0`, `func_8001D240`,
`func_8001EFD4`, `Duel_UpdateTrapPresentation`, `DuelField_GetGuardianStarDisplayCode`,
`Duel_UpdateCardPickCursor`, `Duel_SetupCardRecord`,
`DuelEffect_ApplyStopDefense`, `DuelEffect_UpdateRevealCard`,
`DuelEffect_ApplyDarkPiercingLight`, `DuelEffect_ApplySwords`,
`DuelEffect_ApplyHarpiesFeatherDuster`,
`Duel_FindFreeFieldSlot`,
`Duel_CollectFieldCardsBelowType`, `Duel_CollectFieldCardsByType`,
`func_8002778C`, `func_800278A0`, `func_80027DF8`,
`Duel_CollectFieldRowCardObjects`, and
`Duel_CollectMatchingFieldCardObjects`.

Raw local views retained for exact code generation:

- `DuelField_GetGuardianStarDisplayCode` keeps byte-addressed construction for its first record
  pointer because it only scales the index before passing that pointer to the
  guardian comparison helper; it does not access a shared field directly.
- `Duel_FindFreeFieldSlot` keeps the manual byte-scaled base construction so
  the record-scale result and final pointer share the retail live range, then
  walks a typed `DuelCardRecord` cursor for the flags test.
- `Duel_ResetCardRecords` keeps the independent `+0x16` flags cursor that preserves
  retail allocation. Its `+0x04` word store derives the backward distance
  between `DuelCardRecord::flags` and `::data`, and its increments and bound
  use the shared size/count constants.
- `Duel_UpdateCardDisplayMarker` keeps the target's 32-bit read beginning at `+0x14` so the
  `0xA0000000` mask remains a single `lw`-based test.
- `Duel_CreateCardDisplayObject` uses the shared table declaration for its record-index
  calculation, but keeps the incoming record as a byte pointer so its
  field loads and pointer-difference expression preserve the accepted code.
- `DuelEffect_ApplySwords` likewise keeps the target's 32-bit `+0x14` read for the
  `0x90000000` state test.
- `DuelEffect_ApplyStopDefense` uses the shared record and flags, but keeps
  the target's 32-bit `+0x14` read for its `0x88000000` state test.
- `Duel_CollectFieldRowCardObjects` keeps its explicit byte-address
  construction and fixed
  register variables, then uses `DuelCardRecord` once the address is formed.

Any matching-C report user not listed in the typed inventory above still
retains a local or raw record view. Derive that changing set from
`notes/global-usage.csv` rather than duplicating it here.

`Duel_CollectMatchingFieldCardObjects` replaces the former inline-assembly
implementation with matching C and the shared `DuelCardRecord` view. Its
negative-selector path retains `D_801A7B64` for the original two-side
traversal and relocation; this is an interior view of the existing table, not
another allocation.

The output is a zero-terminated list of object addresses in `u32` words.
A negative selector can append two occupied cards per iteration across ten
field zones, so the maximum is **21 words including the terminator**.
Other selectors inspect one five-card row, so their maximum is **six words
including the terminator**. The function has no capacity argument.

## Which row the two object collectors read

Both collectors in `src/game/duel_field_card_objects.c` start from
`gDuel_bActiveSide ? 5 : 20`, which is the opposite side's block, not the selecting
side's: `gDuel_abHandCardRecordIndices` in `src/game/duel_field_layout.c` maps the two hands to
records `{0..4}` and `{15..19}`, and `gDuel_abFieldGridCardRecordIndices`
paired with `gDuel_aFieldGridGroundPositions` place records 5-9 and 20-24
on the row at ground `|z| = 95` and records 10-14 and 25-29 on the row at
ground `|z| = 161` behind it. So records 5-9 belong to side 0
and 20-24 to side 1, and selecting `5` when `gDuel_bActiveSide` is `1` reads side 0.
`Duel_CollectFieldCardsByType` scales the same table by
`gDuel_bActiveSide * DUEL_CARD_SIDE_RECORD_COUNT` and its `Duel_SelectEquipPair`
caller asks for equip cards at offset 0 and any card at offset 5, which is
the selecting side's own hand and own front row; the two object collectors
omit that scale, so they read the other side.

`Duel_CollectFieldRowCardObjects`'s second argument adds another
`DUEL_FIELD_ROW_SIZE`, moving it from that front row to the `|z| = 161` row
behind it.

## The traced overlay callers

The duel overlay is not split into this project, so `WA_MRG.MRG` is the only
record of the callers. Three overlay functions between them call the two
collectors, at file offsets `0xBB3B18`, `0xBB5C20` and `0xBBBE84`, which are
addresses `0x80150B18`, `0x80152C20` and `0x80158E84`: the `j 0x150bec` at
file offset `0xBB3BD8` lands on file offset `0xBB3BEC`, which fixes the image
base at `0x7F59D000`. Every call site repeats at six further offsets, once per
language image, which accounts for the 21 `jal` sites to
`Duel_CollectMatchingFieldCardObjects` and the 14 to
`Duel_CollectFieldRowCardObjects` that the file holds.

The first caller takes a record pointer and a second argument it rejects
unless it is below 8, then indexes an eight-entry table at `0x8015ACD4` whose
entries are `{u8, u8, u8, u8 pad, s32 selector}`:

| Index | First three bytes | Selector |
|---:|---|---:|
| 0 | `ff 40 00` | 3 |
| 1 | `ff 00 ff` | 2 |
| 2 | `60 60 ff` | 14 |
| 3 | `ff 00 00` | 1500 |
| 4 | `20 ff 20` | 9 |
| 5 | `ff ff ff` | 18 |
| 6 | `ff ff 60` | 12 |
| 7 | `ff 70 70` | 999 |

Selectors 3, 2, 14, 9, 18 and 12 are `card_constants.h`'s
`CARD_TYPE_WARRIOR`, `CARD_TYPE_ZOMBIE`, `CARD_TYPE_MACHINE`,
`CARD_TYPE_INSECT`, `CARD_TYPE_ROCK` and `CARD_TYPE_FISH`; 1500 is above
`CARD_TYPE_MAGIC`, so it takes the attack comparison instead. The caller
passes the selector to `Duel_CollectMatchingFieldCardObjects` with the fixed
output buffer `0x8015B838` and then counts the result by walking it to the
terminator, but it diverts the `999` entry to
`Duel_CollectFieldRowCardObjects` with a nonzero
second argument, so that entry reads the row behind the front row unfiltered
rather than filtering on an attack of 999.

The second caller, at file offset `0xBB5C20`, calls
`Duel_CollectFieldRowCardObjects` with a nonzero second argument and nothing
else, then fills mirrored screen offsets of `140`, `190`, `4` and `384`
chosen by the sign of the halfword at `0x8015B7D4` and walks the list to count
it. With the `999` branch above, that is both of the row collector's traced
call sites, and both ask for the back row, so its front-row case has no traced
caller.

The third caller rejects a second argument that is not below 2, indexes a
two-entry 16-byte-stride table at `0x8015B7A8`, and branches on the halfword
at that entry's `+0x0E`, which is `0` in the first entry and `1` in the
second: zero calls `Duel_CollectMatchingFieldCardObjects` with selector `-1`
and nonzero calls it with selector `0`. So the negative both-sides path does
have a traced caller, and all three callers share the one output buffer.

That buffer's capacity is consistent with the 21-word worst case. From
`0x8015B838` the overlay image is zero for `0x58` bytes, up to the next
initialised data at `0x8015B890`, which is 22 words.

## Current assembly users

The assembly-user set changes whenever another function is integrated.
Consult the `D_801A7AD8` rows in `notes/global-usage.csv` rather than copying
that volatile list here.

## Original deck provenance and display metadata

pre_shuffle_index is the unchanged u8 at DuelDeckCardRecord +0x04.
Duel_ShuffleDeck seeds a 0..39 original-slot byte in both supplied-deck
and weighted-generated paths, then swaps that byte together with its ID.
Duel_ShuffleBothDecks supplies two separate forty-entry runs; population
copies the shuffled byte from DuelCombinedDeckInput.pre_shuffle_indices.
This differs from deck_index at +0x02, which population seeds with the
current combined-deck index, and data_block_index at +0x03.

The card-specific display view names +0x67 deck_number_marker.
The screen factory initializes it to zero; marker refresh and the hand
factory copy pre_shuffle_index + 1, including the original u8 conversion.
The negative view mode still performs the nonzero-mode data read before
overriding with 0xFF. Mode zero can retain the old byte. The renderer draws
digits only for 1..40; any nonzero marker still selects its marker frame,
so 0xFF is not normalized to zero or treated as a visible decimal value.
Marker-flag clears, the packed record +0x14 gate and helper order stay.

card_type at display +0x68 retains the original stat-word extraction and
AI comparisons. The world-space factory uses its own signed shift before
the existing mask; neither factory gains a new interpretation or clamp.
The display view's u8 deck_index at +0x6B caches the constructor's original
unsigned data[2] read. Its source record's canonical deck_index is s8:
those distinct declarations, casts and promotions are not unified.
Scene-move and battle readers pass that cached byte back to the field
setup path. The world-space factory still does not initialize it.

These names apply only to DuelDeckCardRecord and DuelCardDisplayObject.
Canonical DisplayObject, resource/configuration, preview, text and parser
views retain their polymorphic offset names. One non-card row-set write
retains field_68 through the existing generic DisplayObject view: it is an
enabled flag, not card_type. No new type, union alias, default, clear,
callback, group boundary or argument change follows.
The shared sign-selector byte +0x69 and the two helpers' different +0x21/
+0x22 mappings remain unchanged, including stale orientation bytes.
