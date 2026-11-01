# Current model-effect key and queue count

Two resident globals have high-confidence semantic names:

| Address | Name | Existing type |
|---|---|---|
| `0x8009B074` | `gModel_pCurrentEffectKey` | `Key *G32` |
| `0x8009B078` | `gModel_bEffectKeyCount` | `u8` |

The ten-record storage at `0x800F5788` is `gModel_aEffectKeys`, retaining
its existing `Key[10]` declaration. The append guard limits writes to ten
records, and the `0x190`-byte run ends at the next mapped handler registry:
ten times `sizeof(Key) == 0x28`. Its name distinguishes the array from the
current cursor and from the separate current-pose snapshot.

The cursor and count are not newly inferred allocations. Their initialized `.sdata`
definitions already belong to `src/game/model_transfer_flags_state.c`,
and their shared types remain in `model_transfer_flags.h`.

## Producer, stepper and consumers

`Model_ApplyEffectKeyQueueCommand` starts a queue by pointing the cursor at `gModel_aEffectKeys` and
zeroing the count. Its append path installs the requested endpoints in
`gModel_aEffectKeys[count]`, initializes that key's trailing fields and increments
the byte count, bounded at ten records. `Model_ResetEffectKeyQueue` clears both globals.

`Model_UpdateEffectKeyframes` in `model_keyframe_update.c` is the decisive evidence for
**current**: it reads and updates the pointed key's progress and duration,
advances the pointer by one `Key`, and compares its array index with the
count. Multi-key queues wrap to the base; a finished one-key queue clears
the cursor and count. The open-ended-key completion path clears them too.

The count is therefore the number of queued keys, not a countdown of keys
remaining after the cursor. `Model_EvaluateKeyframeSpline` independently uses it as the
ring modulus while choosing neighboring interpolation keys.
`Model_CaptureKeyframePose`, `func_8005FB30` and the key preparer consume the current
key's resolved/requested fields. The empty test and pointer getter also
use this count/cursor pair.

The existing `Key` layout is `0x28` bytes. Its requested and resolved
endpoint pairs, magnitude, duration, progress and readiness fields support
the effect-key role without assigning meanings to the neighboring
address-based control bytes. Those bytes are deliberately not renamed.

## Endpoint sequence input records

`Model_QueueEffectEndpointSequences` takes `ModelEffectEndpoint *` for both sequences
instead of storing those pointers as `void *` and casting them on every
use. Its own dispatch, one-record increments and post-step `kind` tests
establish the same eight-byte record view already accepted by
`Model_ApplyEffectKeyQueueCommand`. Both input pointers remain nullable.

The packed record definition is unchanged. The stop tests still take an
aligned `s16` view of `kind`, retaining retail's `lh` instead of the byte
loads that direct packed-member access would produce. The input records
must retain the same valid halfword alignment as before; the type change
does not add support for arbitrary unaligned byte buffers.

No count, counter, guard, termination, ordering or dispatch logic changes.
The helper's scalar arguments and pointer ABI are retained. Supported-GCC
owner checks accept the two typed sequence pointers and independently
reject reverting either one to the former `void *` contract; full
executable matching remains the code-generation gate.

## Storage and regional boundaries

The pointer/count widths, initial zero values, explicit `.sdata`
attributes, definition order and three individual padding bytes are
unchanged. No new declaration view, symbol alias, compiler flag or layout
is introduced. The semantic registry records the original addresses and
the producer/consumer evidence; the names are project names, not recovered
original symbols.

The European and Japanese source adapters change only the left-hand shared
identifier in their macros. Their existing `gEuropean_D_...` and
`gJapanese_D_...` target names and all regional fixed-address bindings
remain intact. The native keyframe and coordinate witnesses retain their
original fixture addresses and oracle behavior under the new identifiers.

The array itself uses a common name across builds. Each tracked regional
binding is renamed at its original value: `0x800F0230` in Japan,
`0x800F3AA8` in the European base, and `0x800F3EC8` in the four localized
European builds. The unrelated qualified French label at `0x800F5788`
is a different object and is not renamed. No new regional address or
additional code-generation alias is introduced. The array's existing
image-backed storage is retained; no C initializer or allocation is added
for it.

## Regional labels for the key cursor and count

The Japanese and PAL builds keep these two globals at other addresses. Their
symbol files bind `gModel_pCurrentEffectKey` and `gModel_bEffectKeyCount`
directly at those addresses (formerly `gJapanese_D_8009B074`/`gJapanese_D_8009B078`
and `gEuropean_D_8009B074`/`gEuropean_D_8009B078`), so `model_transfer_flags.h`
declares them the same way in every region, with no assembler label or
wrapper macro. Types, storage and every access are unchanged.

