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

## Record dispatch and color pulse

`gSceneScript_RecordCallbacks` is the existing two-entry initialized table:
the empty `SceneScript_NoOpRecordCallback`, followed by
`SceneScript_UpdateColorPulse` (formerly `func_8002FED8`). Both native slot
sweeps keep their raw low-byte callback index and pass the original nonzero
first word as the second argument. There is no new selector mask or bound.

The native prototypes intentionally differ. The no-op stays `void(void)`;
the pulse stays `(SceneScriptSlot *, DisplayObject *)`. The table retains
its incomplete array and generic two-argument callback typedef with the two
existing initializer casts. Naming the entries does not normalize their ABI
or change call expressions, argument conversions or pointer storage.

The pulse tests bit `0x8000` of the signed halfword at +4. On first entry
it sets that bit and clears the signed phase at +6, then still advances by
32 in that invocation. The phase calculation stays
`((u16)phase + 32) & 0xFFF`, including the halfword store.

Intensity remains the signed expression `rsin(angle) * 24 / ONE`. Only
the existing exact values are adjusted: zero becomes one, and positive 24
becomes 23. This is not a clamp to a positive interval; negative sine
values retain their original signed arithmetic.

The supplied object's RGB bytes receive `intensity - 104` in byte order
2, 1, 0. The callback then independently loads slots zero and one, checks
each first word and writes `(intensity + 24) / 2 - 128` to each present
object in the same order. Those objects may alias the supplied object or
one another. Loads and repeated writes remain separate; no no-alias
assumption, merged store or newly skipped object is introduced.

The callback stays grouped immediately after the eight-byte no-op in
`scene_script_slots.c`, with its original `gcc_2_8_1_g8_split` profile and
0xFC-byte size in all seven inventories. Only its function-name field
changes; addresses, sizes, ownership/status, notes and order remain intact.
The public header moves from `func_8002FED8.h` to
`scene_script_color_pulse.h` without changing its declaration.

## Image-overlay tables and slot publication

`gScriptImage_OverlayPairs` remains an incomplete, mutable array of the
existing two-byte mask/value records. Its 44 initialized records occupy
0x58 bytes. `gScriptImage_OverlayPlacements` remains the existing aggregate
of thirteen six-byte rows and two trailing zero padding bytes, occupying
0x50 bytes. No const qualification, layout, record count, initializer byte
or padding changes.

`ScriptImage_RebuildObjects` first selects the supplied ID or retained
image-ID halfword using the original signed-negative test, then stores
the ID back. Its existing signed `n >= 0x200` and `n >= 0x100` branches
and packed-BCD low-byte decode remain unchanged. There is no new ID range
guard, decimal-digit validation, index clamp or generalized decoder.

For the placement branch the base object is created first, configured
with its original full-screen geometry, given the original attribute and
screen-space flag change, and published in slot zero with its original
field/value stores. Mask bit zero creates overlay one and writes X/Y;
bit one creates overlay two, writes X/Y and then the value halfword.
Only inside that second-overlay branch does bit seven set screen-space.
Unset mask bits do not newly clear old pointers.

For the lower-ID branch the original base-object creation remains. IDs
from 0x100 use the pair table; the local holding the decoded index is
still reused for the mask. Bits zero/one create the two optional overlays,
with the original request sizes, modes and slot-two value store. IDs
below 0x100 still explicitly zero the two optional pointers. These paths
are not merged with the placement path.

The table value reaches the halfword at set +0x2C, which is +4 of the
third 0x14-byte slot, not a DisplayObject field. Its low byte participates
in the same record-callback dispatch described above. The existing
`ScriptImageEntry.value` and `SceneScriptSlot.unk04` views remain separate;
no member retyping or new shared record is needed to describe the link.

Only the two obsolete European rebuild-table macros retire. All complete
compiler/MASPSX outputs retain their data directives and relocation
expressions after just the declared label substitutions. The shared source,
data placement, command table following the padding and unrelated native
bindings remain unchanged.

## Native identities and conservation

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Primary display-object slot | `0x8009B2A0` | `0x8009B190` | `0x8009C1F4` | `0x8009C614` |
| Scene/image slots | `0x800EAE98` | `0x800EAD78` | `0x800EBE00` | `0x800EC220` |
| Color-pulse callback | `0x8002FED8` | `0x8002FC80` | `0x8002FF40` | `0x8003012C` |
| Record callback table | `0x80090CAC` | `0x80090B5C` | `0x80091CB0` | `0x800920C4` |
| Overlay pair table | `0x80090BA8` | `0x80090A58` | `0x80091BAC` | `0x80091FC0` |
| Overlay placement table | `0x80090C00` | `0x80090AB0` | `0x80091C04` | `0x80092018` |

The six retired primary-object macros are exactly the Japanese and European
wrappers for package initialization, debug campaign entries and duel results.
Native addresses, pointer qualifiers, signed/data views, callback arguments,
array bounds, initialized tables and every object release/store remain.

The owners, record callbacks and image-overlay tables are atomic checkpoints
of one scene-presentation bundle. Accepted-master integration and cumulative
clean acceptance precede a single component-level PR; no checkpoint opens a
symbol-only PR. Each checkpoint retains all 940 complete compiler/MASPSX outputs after
only its declared identities, with full-source/declaration/native proofs.
The entire prior semantic ledger is retained with two evidence/caveat rows
per checkpoint. Inventories change only the reviewed callback name; all
other fields, compiler profiles, manifests, test controls, fixtures, traces
and tooling remain unchanged. Regional/cloud tests stay PR-side.
