# Duel card display constructor

`Duel_CreateCardDisplayObject` names North American `0x80017F04`
(size `0x100`). It creates a display-pool object for an existing duel
card record; it does not allocate a new card record, load preview
resources, publish `card->object`, or acquire an existing cached object.

The complete matching factory obtains a free general slot and acquires
it with class value 6. It derives the card-stat index, binds the type
byte and `gDuel_aCardRecords` index, copies the card-data byte at +2,
sets the two initial screen halfwords and 8bpp attribute, installs
`DisplayObject_SetResourceVariantFromSign`, selects ordering table 1,
and installs `Duel_DrawCardAtOutputPosition`. The existing marker and
orientation/colour helpers run in their original order before it returns.

Hand and field actions, card placement, the ritual controller and the
debug effect screen use the returned display object. Those callers,
the allocation/configuration body and the installed drawing callback
independently support the name. No broader ownership or meaning for the
polymorphic fields is inferred.

## Ambient-position boundary

The owner declaration returns `DuelCardDisplayObject *` from
`(DuelCardRecord *, s32, s32)` and exposes the actual card-rendering view;
byte/canonical clients convert explicitly.
`Duel_CreateHandCardDisplayObject`, now in the same unit, calls the constructor
through an unprototyped cast and passes only its record word, leaving incoming
`$a1/$a2` untouched for the constructor's initial position. That cast replaces
the former `FUNC_80017F04_AMBIENT_POSITION_ARGS` declaration arm, which could
not sit beside the real definition. Its `(u8 *)card` cast, parameter count and
all three-argument callers remain unchanged.

The North American four-function scene group and European three-function
display-state group retain order, extent and compiler profile; the European
group has since joined `Duel_InitScene` in `european/duel_init_scene.c`. The
Japanese build first gave the constructor an isolated wrapper,
`japanese/duel_create_card_display_object.c`; it now compiles with the
rest of the scene group through `japanese/duel_init_scene.c`. Seven resident inventory and
linker bindings follow the name.

The existing ritual witness retains every oracle while its observer
uses the factory's card-rendering return view.
Supported-compiler controls cover the normal and ambient owner arms,
arity, return view and declaration ownership. They do not execute the
factory, graphics, allocation runtime or engine. Complete affected
compiler/MASPSX output, after only identifier normalization, and a clean
whole-executable exact match remain required. Historical attempt/research
evidence, generated reports and frozen headers are not rewritten.
