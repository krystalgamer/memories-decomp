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

`D_8009B1A0` retains the signed low halfword of the ritual result;
`D_8009B1C0` retains a display-object pointer. The initializer passes the
former, sign-extended through `s32`, to the existing `void *` interface of
`func_80019CC8`. That argument encodes a card identifier, not a dereferenced
display object. No broader callee type or semantic rename is implied.

The controller installs `func_8001EC70` through the guarded `u8 *`
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
