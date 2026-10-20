# Ritual controller caller views

`DuelEffect_ApplyRitual`, formerly `func_800262D4`, passes `0x800E9EF8` to
`Duel_CheckRitual`. The matched
callee writes three tribute-object pointers and clears its result's word at
`+0x0C`, so this output covers `[0x800E9EF8, 0x800E9F08)`. Relative to
`D_800E9EF0`, that means slots 2–4 and the separate word `D_800E9F04`.
The earlier five-slot consumer `func_80018FEC` also clears `D_800E9F04`
after filling the pointer slots (retail instruction at `0x8001913C`).

The canonical work-slot count remains **five**, not six.
`DISPLAY_OBJECT_WORK_RITUAL_VIEW` selects a 24-byte union containing the
five-pointer view and a two-pointer prefix followed by a complete
`DuelRitualResult`. Passing the address of that result does not overrun
a five-pointer allocation. Native fixtures must allocate the whole union;
an independently allocated five-pointer array is insufficient backing.
The default incomplete-array declaration is unchanged for existing
consumers and their measured absolute relocations.

The retail resident image, through the existing `bss_image_after_viewport`
binary-backed section, supplies the storage. Existing absolute linker
identities cover `D_800E9EF0`, its interior labels, and `D_800E9F04`.
No new C allocation or linker alias is required. The next mapped label
after the clear word is `D_800E9F10`; this view does not claim that gap.

`D_800EA128` is also image-backed, with no C definition. The controller
writes the four halfwords at offsets `+8` through `+0x0E` as a `RECT`,
and reads source coordinates at `+0x28/+0x2A`. Its owner declares a
**22-halfword measured prefix**, ending at `0x800EA154`; it does not infer
the total object size from the next mapped label, `gLibrary_State`.

## Integer card-ID API

`gDuel_RitualResultCardId`, formerly `D_8009B1A0`, retains the signed low
halfword of the ritual result;
`D_8009B1C0` retains a display-object pointer. The initializer passes the
former, still explicitly sign-extended through `s32`, to
`Duel_MarkCreatedCardKnown(s32 card_id)`, formerly `func_80019CC8`.
Placement independently passes its existing
unsigned word conversion of `D_8009B150 & 0xFFF`. Only the outer `void *`
carrier casts are removed; the input narrowing/mask/sign extension remain.

The helper never dereferences an object. It adds the card ID to
`CAMPAIGN_FLAG_LIBRARY_CARD_BASE` (`0x120`) and passes the full resulting
word to the existing packed flag updater. Unsigned 32-bit addition followed
by the original signed flag-word view preserves carries and wraparound
without object-pointer arithmetic or signed overflow. No ID clamp,
validation or new interpretation of flag-command bits is introduced.

The side-index-zero, negative controller-byte and nonnegative opponent-byte
gates retain their original signedness, order and addressing views. No
global, return type, wrapper, named profile or semantic function name changes.
The helper and both caller compiler outputs remain identical.

The actual-helper native fixture checks all 256 patterns of each gate byte,
then every low-halfword pattern under five high-word patterns, including
signed and unsigned carry boundaries: 17,104,896 calls at each of `-O0`
and `-O2`. Independent byte-domain gates, a wider arithmetic oracle, exact
call counts and guarded input bytes check the unchanged flag argument and
read-only conditions. Negatives reject ID narrowing and either reversed
signed gate. Supported-compiler/native API probes accept the integer
signature, reject the old pointer signature, and retain arity/owner-loss
checks. Existing ritual events and numeric expectations are unchanged;
their stub changes only its parameter type. The flag setter and actual
save bank/engine are not executed by this helper witness.

## Stored ritual result name

The two controllers store the low halfword of `Duel_CheckRitual`'s return
at `0x8009B1A0` and retain the existing nonzero test. The value supplies
the preview's card ID, the known-card helper argument, and the final
result card/data ID. Its owned declaration remains `s16`, not a wider
word or a new valid-ID type.

The verified resident direct-GP census finds exactly these overlapping
accesses: `sh` at `0x80026250` and signed `lh` at `0x80026288` in the
starter, then `sh` at `0x80026300` and unsigned `lhu` at `0x800266C0` in
the applying controller. There is no overlapping wider direct-GP access,
aligned literal pointer word or memory/addiu/ori literal-low reference
to this address. This is a direct/literal census, not a claim to resolve
arbitrary indirect ranges. Matching C independently has the same two
controller owners.

All seven physical bindings retain their existing addresses. Japanese
and European English data labels receive the canonical name; existing
North American and other European linker aliases remain intact beside
the new name. The French `D_8009B1A0_French` label at the North American
address is a distinct alias and remains untouched; the actual French
result lives at `0x8009C59E`.

Only identifier spelling changes in existing source and native/type
controls. Low-halfword retention, sign extension, nonzero behavior,
unsigned readback, globals/layout/profiles and every existing event/
numeric/mutation oracle are unchanged. There is no new initialization,
range check, allocation or exclusive-storage guarantee.

## Known-card helper name

The name is supported by both the result-card producers and the independent
Library consumer. The only resident calls are fusion-result placement at
`0x8001AC54` and ritual-result presentation at `0x80026314`. The helper
updates the flag at `card_id + 0x120` under its unchanged three byte gates.

`Campaign_TestStoryFlag` normally returns the stored bit mask. The Library
initializer queries that exact card flag, counts it and sets
`LIBRARY_CARD_FLAG_KNOWN`, then separately calls `Library_CheckCardOwned`
and sets `LIBRARY_CARD_FLAG_UNOWNED` when appropriate. The grid's confirm
path requires `KNOWN` to select the card. `Library_MarkOwnedCards` seeds
the same bitset from chest and deck entries. Thus this helper marks normal
created/result-card IDs **known**, not owned, and does not add a card copy.

The new name is `Duel_MarkCreatedCardKnown`. The main owner is
`duel_mark_created_card_known.c`; the Japanese wrapper keeps its original
player-state alias and includes that same body. All seven names, owners and
regional split references retain the original `0x50` size and
`gcc_2_8_1_g8` profile. Only identifiers/filenames change in existing source
and tests; the scalar ABI, modulo-word behavior, original gates and all
native numeric/event/mutation oracles are unchanged. No new validity or
range guarantee is implied for arbitrary packed words.

The controller installs `DisplayObject_UpdateCosineSlide` through the guarded `u8 *`
`DisplayObjectCallback` view; direct callers and the implementation keep
the existing `DisplayObject *` prototype. The selected-card and input
globals retain their existing scalar `IN_DATA` arms. State 3 deliberately
loads the card data address as a raw word at `DuelCardRecord + 4`; replacing
that measured word view with the pointer member changes old-GCC scheduling.

## Borrowed preview resource view

The linked image resolves `D_800EA128` exactly `0x40` bytes after
`D_800EA0E8`, one independently verified `DuelEffectResourceRecord` stride.
The ritual starter requests preview resource slot 1; state 1 constructs
the paired objects from that same slot. State 3's readback accesses match
the resource producer and upload consumer:

| Previous halfword index | Byte offset | Existing record member |
| --- | --- | --- |
| 4 / 5 | `0x08` / `0x0A` | `rects[0].x` / `rects[0].y` |
| 6 / 7 | `0x0C` / `0x0E` | `rects[0].w` / `rects[0].h` |
| 20 / 21 | `0x28` / `0x2A` | `src_x` / `src_y` |

The local now borrows that actual record type through the original
`D_800EA128` address. The global remains the measured 22-halfword prefix:
there is no new definition, global alias, array bound or allocation claim.
The prefix ends at `0x800EA154`; the later `gLibrary_State` label is at
`0x800EA1E8`, not at that prefix end. Neither label is used to infer an
otherwise unproved array extent.

The `src_y` capture and x/width/height/y write order remain unchanged.
`RECT`'s signed coordinate halfwords retain the original low-halfword
bytes, including X wraparound. The stored card-ID views, raw card-data
reload, state publication, page-byte selection and `1408`-byte destination
stride remain untouched. The final `StoreImage` receives the same first
rectangle at the same address, now through its actual `RECT *` type.

The whole-controller native witness now owns a complete 0x40-byte resource
with before/after guards rather than allocating only its measured prefix.
Every existing numeric/event expectation and regression mutant remains;
unwritten resource bytes are compared before/after every call and the
unobserved tail remains canaried. Added negatives detect another rectangle
write, an unobserved metadata write and an overrun. The fixture's C-only
preinclude preserves the source's original owner views and leaves the
freestanding assembly startup untouched.

These are controller and stubbed-helper controls, not executions of the
constructor, real readback/GPU or regional engine behavior. Complete target
compiler and MASPSX output remain identical; clean retail matching remains
the acceptance gate.
