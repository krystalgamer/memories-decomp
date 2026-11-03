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

The password overlay creates channel 2, then builds it, and its channel-3
path destroys channel 3; the name-entry caret uses channel 2 and the PAL
name-entry teardown destroys channel 3. These were once separate PAL imports
(`D_800F04F8`, `D_800F055C`, and a `D_800EB1C0` bound to channel 3's
address), but every image's addresses are pool elements:

| Image | Actual pool | Channel 2 | Channel 3 |
|---|---|---|---|
| SLES_039.47 | `0x800F0430` | `0x800F04F8` | `0x800F055C` |
| SLES_039.48 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.49 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.50 | `0x800F0850` | `0x800F0918` | `0x800F097C` |
| SLES_039.51 | `0x800F0850` | `0x800F0918` | `0x800F097C` |

The offsets are `2 * 0x64 = 0xC8` and `3 * 0x64 = 0x12C`. All regions now
write `TEXT_CHANNEL(2)`/`TEXT_CHANNEL(3)` and bind only `gText_aChannels`.

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
