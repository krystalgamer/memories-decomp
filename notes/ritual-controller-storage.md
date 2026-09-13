# Ritual controller caller views

`func_800262D4` passes `0x800E9EF8` to `Duel_CheckRitual`. The matched
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
the total object size from the next mapped label, `D_800EA1E8`.

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
