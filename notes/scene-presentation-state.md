# Scene presentation object and slot contracts

This component connects campaign package initialization, debug presentation,
script images, duel-result panels and the per-tick record sweep. It is
separate from the script command cursor/latch/countdown state and does not
allocate a new presentation manager.

## Primary display-object slot

`gCampaign_PrimaryDisplayObject` remains the existing `DisplayObject *G32`
declared by `campaign_scene_package.h`. The ordinary and `.data` declaration
arms remain distinct: the debug campaign entry uses the latter for its
absolute load/store.

`Campaign_LoadScenePackage` clears the pointer before the async package
request, menu-record reset, slot initialization, transfer wait and portrait
uploads. It assigns `Campaign_CreatePrimaryDisplayObject` only after those
uploads. The factory still creates the original screen-space sprite and
selects ordering table one.

The debug campaign entry releases this object after destroying the text box;
it does not null the slot afterward. A later accepted selection creates and
assigns another object at the original point. No new cleanup, retain/release
pair or liveness check follows from the name.

`Script_OpDuelResult` checks this pointer before clearing its renderable bit
and again before restoring that bit. It does not replace the primary slot
with its temporary fill object. That fill/capture pointer remains the
separate retained script transition object; regional banner/panel geometry
and both object lifecycles remain independent.

## Shared scene/image slots

`gSceneScript_Slots` remains an incomplete `SceneScriptSlot []`, with the
original signed first-word storage and 0x14-byte stride. Image commands keep
the existing `ScriptImageObjectSet` casts; naming the address is not a
new union, packed struct, pointer conversion or exclusive ownership claim.

Package initialization still writes the signed halfword at slot three's
head (+0x3C) to -1, then clears only the first word and callback/state
halfword in slots zero through two. It does not clear the remaining bytes,
run image release, or extend that loop to the historical five-record count.

`Main_RunCampaign` sweeps exactly three slots before invoking the event
interpreter. The sweep passes a nonzero first word to the callback indexed
by the low byte at +4. There is no new callback-index bound, null-owner
conversion or change to the generic callback ABI.

The image-object view uses those same three 0x14-byte prefixes and the
image-id halfword at +0x3C. Release sets the image ID to -1 before releasing
each stored pointer, zeroing the pointer and value halfword in original
order. Other prefix bytes retain their original contents.

The result command writes its banner/panel objects into the first words of
these same records. North America/Japan retain two panels; Europe retains
its one-panel layout and explicit zero for slot two. The pulse callback
also reads the first two records independently. No separate result-only
table is introduced.

The existing five-record extent derived from earlier array views is not
changed or strengthened. The historical access at +0x64 remains an
unresolved boundary; the incomplete declaration, named count and measured
three-record sweep are preserved rather than inventing a new bound.

## Native identities and conservation

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Primary display-object slot | `0x8009B2A0` | `0x8009B190` | `0x8009C1F4` | `0x8009C614` |
| Scene/image slots | `0x800EAE98` | `0x800EAD78` | `0x800EBE00` | `0x800EC220` |

The six retired primary-object macros are exactly the Japanese and European
wrappers for package initialization, debug campaign entries and duel results.
Native addresses, pointer qualifiers, signed/data views, callback arguments,
array bounds, initialized tables and every object release/store remain.

This is one atomic checkpoint of a larger scene-presentation bundle.
Record callback contracts and image-overlay tables are completed before a
single component-level PR; this checkpoint does not open a symbol-only PR.
The entire accepted semantic ledger is retained with two evidence/caveat
rows. Inventories, compiler profiles, manifests, test controls, fixtures,
traces and tooling are unchanged. Regional/cloud tests stay PR-side.
