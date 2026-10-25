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

## Integrated acceptance

The review base is accepted master
`aa6afb058de76644bb05ae20f8823baabf27a021` (#7657). A fresh clean build
at that base supplied 940 complete compiler/MASPSX reference outputs.
The integrated bundle matches every output after reversing only its six
identities, without filtering instructions, directives, relocations or
filenames. The already accepted runtime identities need no normalization.

All nineteen changed C files otherwise retain every byte after the eight
exact macro removals and two declared header-include substitutions. All ten
header noncomment token sequences retain declaration forms after the named
identity, guard and path substitutions. The exact nineteen-file config
delta preserves all unrelated bindings and every inventory field except
the single reviewed pulse name in each of seven native inventories.

The complete ordered 629,829-byte accepted ledger prefix, including all
thirteen runtime rows, remains intact before the six scene-presentation
rows. Final clean acceptance reproduces retail SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`;
the seven related repository policy gates pass. No generated artifacts,
retail inputs, test harnesses or controls are included.

## Slot callback-control and pulse-angle member names

The separate slot views now name six high-confidence member identities:

| Original member | Canonical member | Unchanged representation |
| --- | --- | --- |
| `SceneScriptSlot.unk00` | `object_address` | signed word at `+0x00` |
| `SceneScriptSlot.unk04` | `callback_control` | signed halfword at `+0x04` |
| `SceneScriptSlot.unk06` | `pulse_angle` | signed halfword at `+0x06` |
| `ScriptImageEntry.value` | `callback_control` | signed halfword at `+0x04` |
| `ScriptImageOverlayPair.value` | `callback_index` | byte at `+0x01` |
| `ScriptImageOverlayPlacement.value` | `callback_index` | byte at `+0x05` |

The per-tick sweep supplies the nonzero `object_address` word as its
existing second argument, indexing the callback table with only the raw
low byte of `callback_control`. The pulse callback uses that same
halfword's high `0x8000` bit as its initialization latch, clears
`pulse_angle` on first entry, and then advances the angle in that same
invocation. The control is not a Boolean, an image ID or a bounded enum.
No index check, mask, callback prototype or argument conversion changes.

Image construction seeds the halfword with one for mode two and zero
otherwise, independently of its display-object blend attributes. Rebuild
may replace the third slot's control with the overlay table's byte;
the existing unsigned-byte load and signed-halfword store clear the
other control bits exactly as before. The two table fields name only
the index byte, not the pulse latch. Releasing an image clears each
pointer and callback-control halfword after setting the separate image-ID
halfword at set `+0x3C` to minus one. These stores retain their order.

The integer object-address view is not retyped as a pointer or merged
with `ScriptImageEntry.pointer`. Its role is established for the three
dispatched records; slot three's separately cast image-ID head and the
historical five-record/unresolved-boundary caveats above remain distinct.
Unused words, `ScriptImageEntry.field_10`, its padding and the explicit
signed-halfword image-ID view stay unchanged. No reader or stronger role
is invented for the creation marker at `+0x10`.

Only the twenty-five typed member accesses in the five production C
units change. The complete remaining C source, signed/unsigned views,
arithmetic, repeated alias-sensitive object loads/writes and regional
branches are exact. Both initialized tables, their padding and callback
casts retain every byte/relocation. Existing controls, inventories,
native bindings, profiles and the entire semantic registry are untouched.
This field-relative naming evidence does not invent linker addresses or
new registry kinds. No harness, testcase, source-string control, new
guard or local cloud-test execution is introduced.

## Image operand, command-phase and overlay-mask domains

Nine names distinguish three existing bit domains without changing a
literal's type or value:

| Domain | Names and original values |
| --- | --- |
| Image operand | `SCRIPT_IMAGE_OPERAND_ID_MASK` = `0xFFF`, `SCRIPT_IMAGE_OPERAND_SKIP_FADE_IN` = `0x4000`, `SCRIPT_IMAGE_OPERAND_HAS_VIEWPORT` = `0x8000` |
| Command control | `SCRIPT_SHOW_IMAGE_CONTROL_TRANSFER_PHASE` = `0x4000`, `SCRIPT_SHOW_IMAGE_CONTROL_REBUILD_PHASE` = `0x2000`, `SCRIPT_LOAD_IMAGE_SCENE_CONTROL_FADE_PHASE` = `0x800` |
| Overlay mask | `SCRIPT_IMAGE_OVERLAY_FIRST_PRESENT` = `1`, `SCRIPT_IMAGE_OVERLAY_SECOND_PRESENT` = `2`, `SCRIPT_IMAGE_OVERLAY_SECOND_SCREEN_SPACE` = `0x80` |

The two `0x4000` names are deliberately not interchangeable. The operand
bit suppresses `Fade_StartInKeepOverlay` in the show-image path, still
subject to the separate white latch. The command-control bit selects its
transfer/setup phase; it is published before that phase's work, not a
new assertion of successful transfer completion. The rebuild and
load-scene fade-phase markers retain their own stores before callbacks.
Other commands' uses of the shared command word remain unrelated.

All three viewport-bit producers are wired. `Script_OpStageImage` retains
the complete operand and consumes two little-endian halfwords when the
bit is set, storing the pending viewport pair. `Script_OpLoadImageScene`
also consumes two halfwords, but updates the live viewport and masks the
operand only inside that optional-coordinate branch. Its no-coordinate
path still passes the original word. The show-image path masks at its
existing request site instead. No common decoder or unconditional
normalization is introduced.

`Text_StageScriptImage` uses the same presence bit but a different
payload: one unsigned byte followed by a little-endian halfword. Its
stream-slot reloads, cursor advances, complete operand store and dispatch
state stores remain exact. Scalar and unsized-array views of the same
retained script state are not merged. A viewport flag does not promise
one universal bytecode width or an exclusively owned scratch word.

Overlay bits zero and one create the first and second optional objects;
the screen-space bit is tested only inside the second-overlay branch.
It is not applied to the base or first overlay. Missing mask bits do not
newly clear stale pointers. Packed-BCD decode, signed family comparisons,
unchecked indices, table callback controls, request sizes, coordinates,
blend attributes, release/fade ordering and regional screen positions
are unchanged.

Only seventeen production literals in four real C units become names.
The nine definitions keep their original unsuffixed decimal/hex literals.
Every other source byte and all initialized overlay, command and text
callback data remain exact. Existing controls, numeric oracle values,
native bindings, profiles, inventories and the entire semantic registry
are untouched. No new guard, enum, shared decoder, testcase, generated-C
harness, source-string control or local cloud-test execution is added.
