# World-space field-card object

`Duel_CreateFieldCardObject` names North American `0x80024C1C`
(size `0x118`). Its existing `DuelCardDisplayObject *` return is a
shared partial view, not proof that this object uses screen coordinates.

The producer acquires a list-0 object and writes +0x30/+0x32/+0x34
from its two position arguments with the middle component fixed to -24.
`Duel_SetupFieldCardFromDeck` attaches that exact pointer to a live card record and
sets its record index. `Duel_DrawFieldCards` passes occupied records to
`Duel_RenderFieldCard3D` unless the separate 0x400 path is selected.

The complete 3D consumer independently resolves the producer's role.
It reads those three halfwords into matrix translation x/y/z. It uses
+0x5C/+0x5D for the card-face UV origin and +0x42 for the CLUT offset.
The producer initializes those fields from card type, including the
magic/trap/ritual/equip cases. This is the world-space field-card
representation, broader than the historical "card-type icon" wording.
The factory does not itself attach/publish the card record's pointer.

The existing `out_x`/`out_y` and icon-named partial-view members remain
unchanged: other users interpret the same physical fields differently.
The last position argument becomes world Z here, despite its retained
source parameter name. In particular, the 3D consumer's existing signed
X/Y/Z reads remain exact; no coordinate normalization,
width change, callback, resource or renderer rewrite is included.

All seven inventories/symbol lists follow the name. The Japanese primary
was `func_80024A48`, exported by a macro mapping the North American name.
That one obsolete mapping is removed so the shared semantic name is
defined directly; all other Japanese data/terrain aliases remain.
Each seven-function lifecycle object keeps its original order, extent,
profile and source owner. Historical attempt/reference records retain
their old names as evidence, not authoritative semantic labels.

The native witness executes only the actual factory with observing
allocation stubs, then isolated actual translation/front-UV/CLUT stores
over real guarded SDK records. It does not execute the 3D renderer,
projection/GTE, lighting, sorting, allocation runtime or engine.

## Field-card setup coordinator

`Duel_SetupFieldCardFromDeck` (`0x80024D34`, size `0x94`) combines live-record
setup and image uploads with field-object creation and publication. Its
second argument is an encoded combined-deck entry index, not a card ID;
the actual card ID is read through the initialized record's shared replay
view. The two full-word arguments retain their separate field-slot and
deck-entry tag rules.

Phase entry, battle, ritual completion and field transitions call
the coordinator before restoring any flags or stat modifiers they retain.
Old-object disposal is also outside this helper. The name does not imply
that those previous values or objects are preserved.
