# Preview variant callback canonical object

`CardPreview_UpdateVariant` receives the actual `DisplayObject *`, not a
separately owned `CardVariantSource` record. The paired preview constructor
stores this callback in its primary object's `field_10`.
`DisplayObject_ProjectRotationFacing` calls that field as
`void (*)(DisplayObject *, s32)` with the same object pointer.

The canonical record is `0x70` bytes (`display_object_layout.h`), and its
`field_6A` is an unsigned byte at the same offset as the old variant view.
The former `CardVariantSource` shell claimed a `0x17A` extent but only
exposed that byte here; no source used its size for allocation or stride.
Removing that unused shell and its size assertion corrects a type claim,
not the real pool's allocation size or layout.

At the canonical-type checkpoint, only the parameter type and
`source->variant` spelling changed. Preserve the `DisplayObjectConfig *object`
narrow helper view, integer fallback `2`, negative-argument path, byte load,
current selector comparison, setter/update/rerun order and all casts.
The one-object `DISPLAY_OBJECT_UPDATE_COMMAND_STREAM_AMBIENT_ARGS`
selection remains; no values for ambient script/attribute arguments are
inferred.

The same selector requests no setter/update/rerun write. A changed selector
sets the resource variant, then updates the command stream, then writes
the existing signed word `D_8009B424` to `1`. No new null check, pointer
width, public callback ABI or resource-resolution guarantee is introduced.
The frozen `display_object.h` is unchanged.

## Controls

Supported-compiler probes accept the exact canonical callback pointer,
reject a byte-pointer callback, and verify the `0x70` extent plus the
unsigned one-byte `0x6A` member. The constructor-tail fixture only changes
its callback stub's parameter type; its address/byte and negative oracles
are unchanged.

The new fixture executes the **entire actual callback** with project types.
Resource setter and command-stream update are stubs; the setter changes
only the fixture selector, not real flags or resources. At both `-O0` and
`-O2`, 262,144 calls cross all requested/current selector bytes with four
sign/boundary arguments. Independent raw-offset/full object/rerun/guard
oracles and event checks protect unchanged paths and publication order.
Negative controls reject a wrong fallback, wrong source byte, treating zero
as negative, and rerun publication before update.

This does not execute real resource lookup, command streams, facing/GTE,
renderer, allocation or engine behavior. Exact supported compiler output
and complete executable matching remain the acceptance criteria.

## Facing area and requested variant

The later parameter/local names are independently grounded in the producer:
`DisplayObject_ProjectRotationFacing` runs
`RotAverageNclip3_nom` on its fixed reference triangle, then uses
`gte_stopz` to store coprocessor data register 24 (MAC0). The callback gets
that signed NCLIP area before the helper's negative-area mirror/reprojection
path. `gte_stopz` is not `gte_stotz`, which reads data register 7 (OTZ).
Both local inline-header forms retain this distinction.

The argument is therefore `facing_area`, not depth, object distance or
the area of the card's actual quad. The local target selector is
`requested_variant`: nonnegative area takes the stored unsigned byte,
negative area uses fallback `2`. Zero remains on the stored-variant path.

Only identifier spellings change in this semantic step. The scalar ABI,
`int` local type, exact operators/literals/casts, helper view, ambient
declaration and all call/publication order remain unchanged. The existing
262,144-call object/word/event oracles and four negatives are retained;
their source-identifier matching follows the new names. Added source
provenance checks do not claim GTE or projection runtime execution.
