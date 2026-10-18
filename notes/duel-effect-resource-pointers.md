# Effect-resource object pointers

The two leading stored pointers of `DuelEffectResourceRecord` refer to
`DisplayObject` pool entries, not opaque allocations. The existing paired
constructor `func_800291E0` acquires both objects with the normal pool API,
publishes the allocated-second primary in `primary_object` and the secondary
in `secondary_object`, then links the primary from the secondary. The release helper
passes both values to the unchanged generic release API before clearing
them; the separate clear helper writes only null pointers.

Both members therefore use `struct DisplayObject *G32`, with only a forward
declaration in the record header. The `0x40` record, pointer offsets 0/4,
rectangle start at 8, source coordinates, scalar tail and every existing
offset remain unchanged. No pool allocation, null-safety or exclusive
physical-memory ownership guarantee is added.

The main-game Library consumes the primary as a display object and retains
its explicit mixed-width flag/attribute views. Other typed record consumers
use the rectangle/scalar fields. The ritual controller's separate
`D_800EA128` halfword alias accesses only the rectangle/source-coordinate
prefix beyond the two pointers; its measured accesses are unchanged.

The Password overlay stores the record pointer and sets texture source
coordinates/dimensions without accessing either object member. No overlay
implementation or public factory/release signature needs changing. Existing
constructor casts, byte views, pointer reloads and publication/release order
are deliberately retained. The original recovery changed only stored pointee
types and target-layout/type controls; the later member-name recovery follows.

## Primary and secondary member roles

| Original member | Offset | Name | Stored type | Confidence |
| --- | --- | --- | --- | --- |
| `object_00` | `0` | `primary_object` | `struct DisplayObject *G32` | confirmed |
| `object_04` | `4` | `secondary_object` | `struct DisplayObject *G32` | confirmed |

The constructor publishes the first allocation as the secondary before
acquiring the second allocation; it returns and publishes the second as
primary, then links it through secondary `field_54`. The Library captures
and moves that primary while retaining separate mixed-width visibility
operations on the secondary. All twelve Library member uses follow these
same roles; no byte/halfword/word view or pointer reload is collapsed.

The release helper still calls primary then secondary, with both pointers
published during both calls, then clears secondary before primary. The
clear-only helper keeps the same clear order and makes no release calls.
Nullable words, possible aliases and partial views gain no new ownership
or allocation-safety guarantees. The stored types, four-byte widths,
offsets, `G32` annotations and total record extent are unchanged.

The retired `test_preview_object_roles.py` executed the actual complete release/clear
helpers and extracted constructor link tail with real project types. Its
80 cases per `-O0`/`-O2` run cover four valid slots, nullable and aliased
pointer pairs, and both non-null constructor-role orientations. Independent
raw-offset/full guarded byte oracles and a release stub reject reversed
release calls, clearing before release and a wrong link target. The stub
observes the original published handles during both calls.

These are pointer/byte/call-shape controls, not a claim that aliased handles
are safe to release in the real pool. There is no actual allocation,
release, variant callback, Library transition, renderer or engine execution.
Existing target pointer-width/type rejection and layout controls remain
unchanged apart from following the confirmed member names.
