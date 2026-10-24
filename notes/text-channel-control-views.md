# Text-channel pacing, colour and regional array views

These names describe existing channel bytes and two PAL views of the shared
channel array. They do not change channel layout, storage, dispatch, input
handling or native code-generation identities.

## Per-channel control bytes

Both `DuelEffectChannel` and the compact `JapaneseDuelEffectChannel` retain
their existing unsigned bytes:

| Offset | Member | Evidence |
|---|---|---|
| `0x53` | `delay_reload_53` | `TextBox_BuildStep` reloads `delay_52` from this byte after the countdown gate. |
| `0x54` | `color_index_54` | `Text_SetColorIndex` publishes the effective colour; glyph creation and palette/mask readers consume it. |

The constructor stores two for the NA/JP delay reload and one for PAL.
Library, dialog and password paths retain their own stores of one. The
countdown is decremented only on the existing gated paths; input can replace
it with one and flags can bypass it. The pump can invoke the builder more
than once per display frame. No frame unit, universal per-glyph interval,
clamp or zero normalization is introduced. Byte decrement/truncation and
zero wrapping remain as before, as do the separate shared signed-halfword
wait counters.

`Text_SetColorIndex`, secondary command `0x0A`, consumes one byte. A literal
operand is used directly; when bit `0x80` is set, its low nibble selects
`gText_abColorSlots` instead. Selected-card modifiers copy the already-named
`gText_SelectedCardColorIndex`. Card-list, empty-card, dialog and Spanish
model-intro callers keep their existing values and conditional overrides.
The JP rasterizer indexes its halfword mask array with this byte; NA/PAL
entry creation and name-entry sprite CLUT selection retain their own forms.
No colour-table bound, new palette enum or validation of literal indices is
inferred.

The unrelated `DisplayObject.field_54` stays unchanged: it also carries
tail/command buffers, preview ownership pointers and packed vertex colour/UV
data. Neither it nor model-variant records are renamed by offset alone.
Only independently typed channel accesses change. The existing `0x53`
assertion is renamed with its member; no assertion, width or layout value
changes or disappears.

## PAL channel-array views

The password overlay creates channel 2, then builds the channel addressed
through `D_800F04F8`; its channel-3 path similarly uses `D_800F055C`.
The corresponding JP paths use pool offsets `0xC0`/`0x120` with their
`0x60` stride, and NA uses `&boxes[2]`/`&boxes[3]`. The name-entry caret
independently uses the PAL channel-2 import for the same existing box.

All five PAL image namespaces corroborate those roles:

| Image | Actual pool | `gText_Channel2View` | `gText_Channel3View` |
|---|---|---|---|
| SLES_039.47 | `0x800F0430` | `0x800F04F8` | `0x800F055C` |
| SLES_039.48 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.49 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.50 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.51 | `0x800F0850` | `0x800F0918` | `0x800F097C` |

The two offsets are `2 * 0x64 = 0xC8` and `3 * 0x64 = 0x12C`.
These are array-element views, not new standalone allocations, additional
channel slots or password-exclusive ownership. The canonical image exports
refer to the actual `gText_aChannels` provider plus those offsets. They do
not depend on an old password-only import being defined in the resident
image.

The `VERSION_EUROPE` header macros preserve each original scalar declaration
and native password import. No source pointer is replaced by an array
expression, no import is removed, and no unused canonical native imports
are added. NA/JP array/base forms and native symbols remain untouched.
The ten ledger records use `regional/<image>/global`, not NA addresses
guessed from the old C identifiers.

## Preservation and acceptance

The existing library layout witness changes only its eight member-name
references. Its emitted words, expected offsets/widths, compiler flags,
scenarios and controls stay unchanged. The preview pointer-ownership test
and its negative `secondary->field_54` mutation remain byte-for-byte intact.
No new C/source-string/native harness is added.

Complete affected compiler/profile contexts are captured from the accepted
parent before edits. Full preprocessed tokens are compared after reversing
only the two member names and the matching assertion name; the regional
view macros themselves retain the old tokens without projection.
Compiler/filtered/MASPSX assembly, native symbol tables and ordinary/fallback
assembly must remain identical. No NA exports are added, so both the
complete NA executable and its whole ELF must remain byte-identical.
The strict registry tool checks the ten actual scoped rows; regional and
cloud test execution remains on pull request validation.
