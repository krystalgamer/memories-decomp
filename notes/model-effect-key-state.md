# Current model-effect key and queue count

Two resident globals have high-confidence semantic names:

| Address | Name | Existing type |
|---|---|---|
| `0x8009B074` | `gModel_pCurrentEffectKey` | `Key *G32` |
| `0x8009B078` | `gModel_bEffectKeyCount` | `u8` |

They are not newly inferred allocations. Their initialized `.sdata`
definitions already belong to `src/game/model_transfer_flags_state.c`,
and their shared types remain in `model_transfer_flags.h`.

## Producer, stepper and consumers

`func_8005F91C` starts a queue by pointing the cursor at `D_800F5788` and
zeroing the count. Its append path installs the requested endpoints in
`D_800F5788[count]`, initializes that key's trailing fields and increments
the byte count, bounded at ten records. `func_8005FAE4` clears both globals.

`func_8005DBA4` in `model_keyframe_update.c` is the decisive evidence for
**current**: it reads and updates the pointed key's progress and duration,
advances the pointer by one `Key`, and compares its array index with the
count. Multi-key queues wrap to the base; a finished one-key queue clears
the cursor and count. The open-ended-key completion path clears them too.

The count is therefore the number of queued keys, not a countdown of keys
remaining after the cursor. `func_8005EBF4` independently uses it as the
ring modulus while choosing neighboring interpolation keys.
`func_8005F070`, `func_8005FB30` and the key preparer consume the current
key's resolved/requested fields. The empty test and pointer getter also
use this count/cursor pair.

The existing `Key` layout is `0x28` bytes. Its requested and resolved
endpoint pairs, magnitude, duration, progress and readiness fields support
the effect-key role without assigning meanings to the neighboring
address-based control bytes. Those bytes are deliberately not renamed.

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
