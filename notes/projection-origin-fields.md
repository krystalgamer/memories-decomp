# Projection result origin fields

`ProjectionOut` remains the same eight-byte, separately typed API view.
Its old offset names now describe the existing byte roles:

| Old member | Name | Offset | Type |
| --- | --- | --- | --- |
| `f0` | `x` | `0` | `s16` |
| `f2` | `y` | `2` | `s16` |
| `f4` | `divisions` | `4` | `s32` |

Both projection helpers copy their screen-origin inputs into the first
two halfwords **before** the geometry calls. These are not transformed
vertices, depth or the returned NCLIP MAC0 value. `SetGeomOffset` still
receives the original full `s32` inputs; the stored coordinates retain the
same low-halfword conversion.

The packed-rotation helper stores zero divisions. The object helper
zero-extends the existing `field_65` byte into that word. Sprite-sheet and
quad/card callers pass this byte-compatible record to the packet
dispatcher, whose established `DisplayObjectPacketOrigin` view subtracts
X/Y and uses the same word for its `DivideFT4` choice/count. The two C
record types and their explicit casts remain separate.

The outline callback independently subtracts `(u16)r->x` and `(u16)r->y`
from its screen positions. Those deliberately unsigned views are retained.
The complete field-consumer census is main-game-only; callers that only
cast/embed the record require no edits.

Only member identifiers change in the two producers, outline consumer and
existing fixture expectations/mutations. Every input, type, offset,
constant, cast, expression, branch, operation order and fake-GTE/numeric/
byte/type/mutation oracle is unchanged. No function/callback ABI, storage
owner, layout, profile or metadata changes.

The existing packed-helper witness uses bounded scratch storage and
stubbed geometry/inline operations. The existing origin witness executes
only the actual three producer stores and reset statement, not the
renderer or GTE. These retain their measured limits; complete target
producer/outline compiler and MASPSX output and whole-executable matching
remain the acceptance gates.
