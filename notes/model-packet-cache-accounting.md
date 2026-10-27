# Model packet and lit-color-cache accounting

The unit scan, packet-space predicate, slot work layout and handwritten
renderers establish one packet/cache accounting contract. This component
names its five existing `ModelSlot` fields without changing their
representations or introducing storage:

| Original member | Canonical member | Preserved type/offset |
| --- | --- | --- |
| `field_DF0` | `lit_color_cache_address` | `s32`, `+0xDF0` |
| `field_DF4` | `lit_color_cache_end_address` | `s32`, `+0xDF4` |
| `field_E00` | `packet_budget_bytes` | `u16`, `+0xE00` |
| `field_E02` | `lit_color_cache_entry_count` | `u16`, `+0xE02` |
| `field_E04` | `prepared_primitive_count` | `u16`, `+0xE04` |

The record remains `0xE20` bytes. Address words remain signed integers,
not new pointer members. Counts remain halfwords, including their existing
truncation and wrap behavior.

## Producer and packet-budget consumer

`Model_InitSlot` clears these fields and scans HMD units through the
existing typed dispatcher. Ordinary and shared textured-unit cases call
`tent_Model_RebaseTexturedPrimitives`, accumulate its return value, and
accumulate its packet-byte contribution. The helper's supported kinds
contribute `0x20`, `0x28` or `0x34` bytes per accepted record, matching the
existing textured GPU packet forms. Unknown kinds and unsupported modes
retain their original zero-return behavior.

The slot stores that byte total as `packet_budget_bytes`.
`Model_HasInsufficientBufferSpace` consumes it as bytes: it derives the
remaining bank space from the packet cursor and byte-buffer base, subtracts
one budget (two for the existing type-5 path), and applies the unchanged
`MODEL_DATA_MIN_FREE_BYTES` margin. This is a pre-cull packet-space estimate,
not a count of packets actually emitted or visible polygons.

`prepared_primitive_count` is the original sum of accepted decoder record
counts across the two unit categories. No new downstream use is inferred.
It is not asserted to count all file primitives, rendered polygons,
successful GPU calls or culled output. Its original zeroing, accumulation,
halfword storage and unsupported-case behavior remain.

## Lit-color cache index and layout

The ordinary-unit call supplies the running index maximum; the shared-unit
call deliberately passes a null maximum pointer. `Model_InitSlot` stores
that scanned bound plus one as `lit_color_cache_entry_count`.
The initial bound still gives one when no ordinary record raises it, and
the final halfword conversion is unchanged. No new minimum, overflow check
or normal-vector validity guarantee is introduced.

The handwritten FT3/GT3 renderers independently identify the index role.
They take vertex positions from `GsARGUNIT_NORMAL.vertop` at `+0x18`, and
use the other halfword indices against `nortop` at `+0x1C` for lighting.
The latter normal-derived offset is divided by two when addressing the
four-byte lit-color cache. FT4/GT4 handlers use the corresponding normal
fields and cache addressing. These routines keep their handwritten
classification and unchanged instruction bytes.

`Model_LoadSlot` and staged-load case 4 preserve the original per-part
work-span calculation after `animation_snapshot_buffer`. They store that resulting address
as `lit_color_cache_address`, then derive `lit_color_cache_end_address`
by adding `lit_color_cache_entry_count * 4`. This names the existing
accounting interval, not a new allocation or a validated address range.

`Model_DrawSlot` publishes the base through the already named
`gModel_dwLitColorCacheBase`; existing draw modes can disable that global.
The handwritten lighting paths write cached GTE color words there and their
mode-zero paths read them back.

## Reset and lifetime limits

Full slot initialization clears both cache-address words. The animation
table reset deliberately clears only the base, leaving the end word's
existing state alone. The names do not establish an always-live interval
or justify clearing extra fields, caching pointers across callbacks,
changing the per-part sum, or adding bounds checks.

All reset order, arithmetic, casts, signed comparisons, tint-type selection,
packet-buffer selection, margins and raw-offset address construction remain
unchanged.

## Scope and evidence

Only `ModelSlot` and its proven consumers adopt these names. Private
Spanish variant-408 and French variant-393 work structures also have
members named `field_DF0`/`field_DF4`; those are unrelated records and
remain unchanged.

Primary evidence is in `model_init_slot.c`, `model_slot_control.c`,
`model_slot_row_tables.c`, `model_draw_slot.c`,
`model_has_insufficient_buffer_space.c`, and the existing handwritten
FT3/GT3/FT4/GT4 renderer assembly. The local packet/normal accounting does
not require importing guessed reference declarations or changing SDK types.

`tent_Model_RebaseTexturedPrimitives` remains tentative overall. Later
retail-package observations now also link the measured PAL slot-2 palette
conversion to its auxiliary upload and leave high-row palettes in their
streamed image bank; see [primitive context evidence](model-primitive-context.md).
The unexercised low-X branch and uncovered input/selector cases remain
separate research. Descriptor/local names do not promote the function,
change its ABI or introduce a packet-capacity guarantee.

There are no new global symbols or semantic-map rows: these are runtime
record members, not independent fixed-VMA objects. The existing third-slot
mutation target adopts the corresponding member name; its exact-one-match
assertion, injected guard, oracle, O0/O2 variants and runners are unchanged.
Other tests, profiles, source groups, SDK headers and linker inputs remain
unchanged. No synthetic/generated-C/source-string harness is added, and
cloud/native witness suites remain PR validation rather than local execution.
