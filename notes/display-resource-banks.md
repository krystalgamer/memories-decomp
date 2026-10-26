# Paired reused display-resource banks

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_801AF000` | `gDisplay_abResourceBank0` | separate incomplete `u8[]` |
| `D_801AF800` | `gDisplay_abResourceBank1` | separate incomplete `u8[]` |

These names identify existing resource-bank addresses, not two new
allocations, DisplayObject records or a contiguous bank-pair structure.
Packages reuse their contents.

## Producers and resource consumers

Resident package callbacks load serialized resources into bank 0;
`MainMenu_LoadPackageStage` loads bank 1. Boot, result, Options, game-over
and display-transition consumers pass bank 0 to existing sprite-resource
configuration functions. Independent overlay consumers corroborate the
roles: Free Duel, Overworld and PAL Options use bank 0; Main Menu frontend
and value-setup code use bank 1. Those overlay sources and imports remain
untouched.

`DisplayEffect_BuildResourceObjects` chooses bank 0 when `field_3C == 0`,
otherwise bank 1. It passes the selected base to
`DisplayEffect_HasResourceEntry`, which follows three nested halfword-offset
tables, then builds up to three sprite objects using that same resource.
The existing zero/nonzero predicate is preserved, not restricted to a new
0/1 enumeration.

The suffixes label the two address identities. They do not assert player
sides, permanent left/right positions, language IDs or fixed package
contents.

## Transfer and cache directions

`DisplayEffect_UpdateResourceAcquisition` uses the same zero/nonzero
selection when setting the resource transfer's `callback_data`. The final
`DisplayEffect_LoadResourceStage` phase loads the serialized resource at
that destination after its existing image and CLUT phases.

On a cache hit, the selected slot restores its extra resource bytes into
the chosen bank; after a newly completed transfer, the cache saves the
chosen bank into the slot. These copy directions, the selected cache slot,
GPU/CLUT rectangles, callbacks and completion flags remain unchanged.

The existing copy argument is `0x800`; `Util_CopyWords` consumes a byte
length, so that path copies a `0x800`-byte window. It is not evidence for a
new complete allocation bound. Both declarations remain unsized, with
their original independent relocation labels rather than an array of
banks or an address-plus-offset substitution.

## Native identities

| Image | Bank 0 | Bank 1 |
| --- | --- | --- |
| NA | `801AF000` | `801AF800` |
| Japan | `801BF000` | `801BF800` |
| Other tracked PAL bindings | `801AF000` | `801AF800` |
| English PAL | original automatic providers | original automatic providers |

Seven root scripts add canonical aliases to the original logical providers.
Twelve native bindings have local numeric evidence. English PAL's two
automatic providers are retained without guessed tracked numeric
assignments; its retail input/split map is unavailable locally. Japanese
addresses are independently established rather than copied from NA.
French literal aliases and every old provider remain.

## Declaration owners and controls

The isolated canonical header preserves both incomplete byte-array views
and follows the existing declaration owners. The old
`display_asset_banks.h` and `unmatched.h` headers stay frozen, as do all
regional wrappers, overlays/imports, source groups, compiler profiles and
earlier accepted namespaces.

Only the existing result-controller witness needs a literal namespace
migration; whole-file reversal reconstructs its original, including all
cases, assertions, mutations, generators and compiler/setup logic.

The existing ownership test intentionally checks the unchanged legacy
`display_asset_banks.h` declaration using old names. It remains unchanged,
as do French/Spanish Options controls tied to frozen overlay imports.
These are not blanket-renamed merely because a search finds the symbols.
No harness, fixture, probe or testcase is added.

Cloud/regional/overlay/witness suites are not executed locally. Native
compiler conservation is not independent full regional-image proof;
fresh exact-head PR checks and maintainer review remain required.


## Serialized resource selector view

DisplayObjectConfig names its unchanged u8 bytes at +0x67/+0x68/+0x69
resource_root_index/resource_branch_index/resource_variant. These names
describe table levels, not player sides, languages, object kinds or a
permanent resource taxonomy. DisplayObject_UpdateCommandStream starts at
field_54, indexes the root u16-offset table with +0x67, indexes the next
table with +0x68, then indexes the final table with +0x69. EVERY offset is
relative to the original resource base. DisplayEffect_HasResourceEntry
independently walks the same three base-relative tables.

The existing setter/configuration parameter names follow that order.
SetResourceVariant always stores the s32 argument through the original
u8 byte and clears resolution bit 0x10. UpdateResourceVariant retains its
comparison of the promoted old u8 against the FULL s32 argument before
storing/truncating; it is not changed to compare narrowed values.
SetResourcePath retains three u8 parameters and its existing flag clear.
ConfigureSpriteResource and ConfigureSpriteAtPosition retain their s32
parameters, all stores, return views, texture arithmetic and callback ABI.
Canonical DisplayObject still uses polymorphic field_67/68/69 names,
including the untouched command-stream resolver and its ambient arguments.

PAL Options changes the branch index to select a language, then calls the
existing unconditional variant setter with its unchanged variant to
invalidate the resource. The order and apparently redundant calls remain.
CardPreview_UpdateVariant retains its signed facing test, fallback 2,
full comparison, setter/stream/retry order and already-selected fast path.
The three existing fixtures migrate FIVE owner-qualified member words
on FOUR lines only. Independent expected offsets, flags, truncation,
event order, backing, cases, scenarios, assertions and all mutation
controls remain. No fixture is run locally and no harness is added.
