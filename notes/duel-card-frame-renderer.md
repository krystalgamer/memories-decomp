# Duel card-frame renderer

`Duel_DrawCardFrame` names the resident function at North American
`0x80016784` (size `0x580`). The name describes its framed screen-card
representation, not merely a border and not the separate lit 3D card path.

The complete matching implementation selects a `DuelCardRecord` through
the canonical display object's `field_6A`. The constructor
`Duel_CreateCardDisplayObject` in `duel_init_scene.c` derives that index from
`gDuel_aCardRecords`, sets the object's type byte from the card stats,
and installs `Duel_DrawCardAtOutputPosition` as its secondary callback.
That wrapper forwards the stored signed screen pair. The other caller,
`Duel_DrawProjectedFieldCardFrame`, computes a projected screen pair
before invoking the same renderer.

The renderer clips those supplied coordinates, optionally uses
`DisplayObject_ProjectRotationFacing`, and emits packets through
`DisplayObject_SubmitPacket`. Its branches submit numeric markers,
card-type graphics or digits from `Duel_CalcCardStats` and
`Text_EncodeDecimalDigits`, plus card-art and final frame packets.
This independent constructor/caller/body evidence supports the name
without guessing global meanings for the polymorphic display-object
fields or relying on reference-project declarations.

The source is now `duel_card_and_status_draw.c` (formerly
`duel_card_frame_draw.c`), grouped with the immediately following
`Duel_DrawCardAtOutputPosition` and the life-point and deck-count readout,
using the unchanged
`gcc_2_8_1_g8_split` profile. The Japanese and European wrappers retain
their existing source ownership and regional constants. All seven
resident inventories and linker bindings follow the semantic name;
the header becomes `duel_draw_card_frame.h`. Historical attempt records,
generated reports and frozen `display_object.h` comments intentionally
retain the old address spelling.

This is a symbol/header rename only. Coordinate types, callback APIs,
casts, scratchpad records, field accesses, qualifiers, packet ordering,
GTE operations, and all numeric/event/type/mutation oracles remain as
accepted. Name/owner probes do not execute graphics. Acceptance still
requires symbol-normalized complete compiler/MASPSX equality and a clean
whole-executable exact match, with regional validation on the PR.
