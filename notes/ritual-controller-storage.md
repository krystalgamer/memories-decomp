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
the total object size from the next mapped label, `gLibrary_State`.

## Integer card-ID API

`D_8009B1A0` retains the signed low halfword of the ritual result;
`D_8009B1C0` retains a display-object pointer. The initializer passes the
former, still explicitly sign-extended through `s32`, to
`func_80019CC8(s32 card_id)`. Placement independently passes its existing
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

The controller installs `func_8001EC70` through the guarded `u8 *`
`DisplayObjectCallback` view; direct callers and the implementation keep
the existing `DisplayObject *` prototype. The selected-card and input
globals retain their existing scalar `IN_DATA` arms. State 3 deliberately
loads the card data address as a raw word at `DuelCardRecord + 4`; replacing
that measured word view with the pointer member changes old-GCC scheduling.
