# DuelEffect Shared Structures

This pass is limited to the shared records rooted at `D_800EB0F8` and
`D_800EB288`. The types in `src/game/duel_effect.h` describe verified storage
widths and offsets, not recovered original Konami names. Offset-based field
names remain where semantics or signedness are uncertain.

The separate canonical request now names its signed dispatch `phase` and
unsigned handler `progress` at `0x1A/0x1D`; see the
[complete resident/bank handoff](duel-effect-request-handoff.md). These are
not fields of the channel structures below or a blanket rename of display
and legacy partial-view members.

## `D_800EB0F8`: four `0x64`-byte channels

`DuelEffectChannel` has size `0x64`, and `D_800EB0F8` is declared as four
entries. The following local evidence establishes the stride and extent:

- `DuelEffect_InitEntry`, `TextBox_SetRect`, `func_8003D614`, and
  `MemCardDialog_CreateObject` independently index or advance entries by `0x64`.
- `MemCardDialog_CreateObject` scans exactly four entries.
- `D_800EB288 - D_800EB0F8` is `0x190`, exactly `4 * 0x64`; linker aliases
  such as `D_800EB15C` also land on `0x64` entry boundaries.
- Target assembly, GMS pseudocode, and Unchiga's same-address sources agree on
  the `25 * u32`/100-byte indexing. GMS and reference types are treated only
  as guesses; the local matching code and target instructions are decisive.

Verified channel fields used by matching C include:

| Offset | Shared field | Evidence |
|---:|---|---|
| `0x28` | `render_object` | primary entry-render object installed by `TextBox_RefreshDisplayObjects` |
| `0x2C` | `aux_object` | outline, custom trade sprite or name-entry caret; shared owned auxiliary slot |
| `0x30` | `prompt_object` | page/completion cursor or choice highlight; retained and released by channel state paths |
| `0x34` | `flags_34` | `0x10`, `0x2000`, and `0x8000` tests; reset by `DuelEffect_ResetState` |
| `0x36` | `initial_string_id` | constructor string ID consumed by initial `TextBox_BuildStep` bank lookup |
| `0x38`, `0x3A` | `pen_x`, `pen_y` | unsigned halfword pen initialization, stream commands and entry-position publication |
| `0x3C`, `0x40` | `rect_x`, `rect_y` | signed halfword origins set by `TextBox_SetRect` and `TextBox_SetPos` |
| `0x3E`, `0x42` | `rect_width`, `rect_height` | signed halfword extents set by `TextBox_SetRect`; wrap and display geometry consumers |
| `0x51` | `state_51` | low five bits dispatch `D_80090E64` in `TextBox_BuildStep`; the contiguous callbacks in `duel_effect_state_callbacks.c` latch `0x80` and write a new state |
| `0x52` | `delay_52` | reloaded from `field_53` in `TextBox_BuildStep` and from `0xFF` in `func_80037B40`, decremented once per tick, gating the rest of the tick while nonzero |
| `0x53`-`0x5B` | byte fields and `channel_index` | initialization sequence in `DuelEffect_InitEntry` |
| `0x56` | `explicit_newline_count` | explicit `Text_NewLine` accounting for page/choice state; automatic wraps do not increment it |
| `0x58` | `stream_58` | signed word index selecting which of the leading pointer words is the live byte stream; scaled by four in `TextBox_BuildStep`, [`duel_effect_object_commands.c`](../src/game/duel_effect_object_commands.c), and [`duel_effect_command.c`](../src/game/duel_effect_command.c) |
| `0x5C`, `0x5E` | `range_start_5C`, `range_count_5E` | adjacent `gDuelEffect_awEntryRangeBoundaries` bounds |
| `0x60` | `output_count` | NA byte count; PAL halfword pixel view includes +0x61 |
| `0x61` | `output_limit` | NA byte threshold cleared in `DuelEffect_InitEntry`; PAL high counter byte |

### Starting string and explicit-newline bookkeeping

The parser's initial_string_id names the original u16 field_36 at +0x36
in both shared and compact Japanese channels. TextBox_Create and
TextBox_CreateFlagged forward their string_id
to DuelEffect_InitEntry (0x80035AF0), which stores it with the original
halfword narrowing and regional initialization order. TextBox_BuildStep
(0x800393B0) reads that stored ID only in its original initial-build path
and applies the original region-specific bank thresholds, masks,
adjustments and address composition before publishing text_00.
The name identifies the starting/root string, not each later nested
stream or the live stream cursor. No lookup is hoisted or normalized.

explicit_newline_count names the shared channel's original u8 field_56
at +0x56. It counts Text_NewLine commands, NOT automatic line wraps,
glyphs, pixels or a universal rendered-row count. The primary command
table selects Text_NewLine for FE; that complete routine alone increments
the byte, retains pen_x=0x1000, calls the original wrap helper, applies
the regional page-wait guard and invokes the pending choice callback.
The European guard still requires count>=4 after wrap failure; other
regions retain their unconditional-on-failure state change.

Automatic glyph wrapping calls TextBox_WrapLineIfNeeded directly from
BuildStep without incrementing this byte. Neither the wrap helper nor
the glyph/pixel counters are changed. Byte overflow/narrowing is retained;
no saturation, inferred row count or new layout policy is introduced.

BuildStep resets the count only in its initial-build path. The choice
command resets it before publishing the layout callback. The volatile
Text_TryCompleteChoiceLayout (0x80037CE0) compares it to the original
choice count, resets it at the same completion point, and retains all
volatile reads, ordinary flag-store cast, callback/control stores and
optional count=2 behavior. This is choice-label command accounting, not
confirmation of input or a promise that automatic wraps count as choices.
Text_CompletePageAdvance (0x8003741C) resets it only after the first-call
clear request and the active-entry wait have completed, together with
the original pen/state resets and region-specific field_62 store.
The channel initializer does not gain a new zero store.

Only five complete C units change under ten owner-qualified member
identifiers. The shared header changes two declarations and the existing
+0x56 assertion member/label; the Japanese header changes only its
+0x36 declaration. Its pad_55 still covers the +0x56 byte: no new member,
padding split, stride unification or pointer-prefix invention. All
types, widths, offsets, sizes, qualifiers, annotations, expressions and
numeric assertions remain. Other field_36 members are display/resource
geometry or velocity and remain untouched, including text-control's
DisplayObject e at +0x36. There are zero fixture edits.

### Retained channel display objects

The three stored DisplayObject pointers have high-confidence ownership
names from their complete local creation, placement, use and release chain:
render_object at +0x28 (formerly field_28), aux_object at +0x2C
(field_2C), and prompt_object at +0x30 (field_30). Their pointer types,
G32 annotations, order and offset assertions are unchanged. These are
qualified channel names, not a global rename of DisplayObject unions,
MenuRecord/DuelEffectResourceRecord payloads or FileTransferDescriptor.

TextBox_RefreshDisplayObjects (0x800391E4) lazily creates render_object,
installs TextBox_DrawEntries and the channel index, then publishes geometry,
ordering-table and narrowed depth values. An existing object is reused
without reinstalling its callback. The name denotes the channel's entry
renderer, not a claim that every channel or entry is text-only. Card-list,
library and save-dialog paths still alter this object's flags, ordering
table or animation through the original typed pointer and raw aliases.

The same refresh routine conditionally replaces aux_object with a Gouraud
outline. This slot is NOT universally an outline or shadow: trade-menu
constructors store custom sprites, and password/name entry stores its
separate animated caret there. TextBox_SetPos (0x80039934) retains the type-4
outline update versus direct-position branch; all custom sprite/caret
publication and subsequent casts remain in their original order.

Dialog_OpenChoice produces the lower-corner sprite used for page waits
and completion prompts. Dialog_UpdateChoice (0x800371A8) instead stores
a highlighted-choice quad in prompt_object. Highlight colour, regional
choice geometry, optional highlight retention, page-advance initialization,
countdown/input handling and CloseChoice's release/state/yield sequence
are unchanged. Existing callers polling this pointer still wait only for
its non-null publication, not for input confirmation or completed clearing.
TextBox_UpdateChannels (0x80039794) retains its separate +0x30
ChoiceChannelCursor, stored obj/flags view and original regional stride.

TextBox_BuildStep (0x800393B0) releases and nulls prompt_object and
aux_object before refreshing the renderer. TextBox_Destroy (0x80035B7C)
keeps prompt/aux/render release order followed by the original pointer
clears, including Europe's active-channel guard and occupancy differences.
DuelEffect_ResetState (0x80035A64) only clears the stored pointers; it
does not release those objects. No new releases or ownership transfer.
All zero stores, unchecked allocations, raw views and compact Japanese
0x60-byte traversal remain exact. The compact Japanese prefix declaration
itself stays unchanged; no new region-wide record view is introduced.

Twenty-five existing C units change only qualified member identifiers
and one existing build-deck comment identifier. The existing library layout
fixture changes two member identifiers in its offset/width checks; the
existing ritual fixture changes two member identifiers in its original
prompt-publication stubs. Every assertion, oracle, numeric expectation,
mutation, flag and scenario remains unchanged. No new behavioral harness
or local cloud-fixture execution.

The channel's depth_offset is the original raw u8 at +0x59 in both views.
It is narrowed only at the existing display-object depth calls, including
the original +/-1 expressions. The complete protocol and signed object byte
are documented in [Display-object Runtime](display-object-runtime.md).

### Text-channel pen and rectangle

The six channel halfwords have high-confidence names from their complete
local producer/consumer chain, independent of reference declarations:

| Offset | Original member | Canonical member | Retained storage |
|---:|---|---|---|
| `0x38` | `field_38` | `pen_x` | `u16` |
| `0x3A` | `field_3A` | `pen_y` | `u16` |
| `0x3C` | `field_3C` | `rect_x` | `s16` |
| `0x3E` | `field_3E` | `rect_width` | `s16` |
| `0x40` | `field_40` | `rect_y` | `s16` |
| `0x42` | `field_42` | `rect_height` | `s16` |

Both shared 0x64-byte and Japanese 0x60-byte channels retain these offsets,
types and declaration order. Unrelated DisplayObject unions and
DuelEffectResourceRecord/MenuRecord members keep their original names.
The existing offset/pen-width assertions retain every numeric expectation;
only member names and three assertion labels change. No fixture changes.

TextBox_SetRect (0x80035AB8) stores the original x/y/width/height arguments
without changing halfword narrowing or the compact regional channel stride.
TextBox_SetPos (0x80039934) changes the origins and separately updates its
three attached objects in the original order. Its lower-corner expression
is origin + extent - 16: width and height are not absolute right/bottom
bounds. TextBox_RefreshDisplayObjects publishes these origins and extents to
DisplayObject geometry and retains signed division by two.

DuelEffect_InitEntry (0x80035AF0) and Text_CompletePageAdvance
(0x8003741C) reset the two pen halfwords at their original points, with
unchanged regional guards and active-entry tests. Text_SetPenX
(0x80038388), the existing European Y setter, and the signed-Y/unsigned-X
byte-delta commands retain every stream read, publication and narrowing.
Text_NewLine (0x80038E1C) keeps the 0x1000 forced-wrap sentinel; no immediate
wrap, clamp or validation is added to the direct setters. Card-list slot
placement still writes pen_y and shifts rect_x through its unsigned alias.

DuelEffect_AppendEntry (0x80036C14) copies pen_x/pen_y to the original
entry-local positions. TextBox_BuildStep still advances X with the nominal
glyph advance plus the original signed European metric adjustment where
applicable. TextBox_WrapLineIfNeeded (0x80037C74) keeps explicit signed
views of the u16 pen, the extra line-advance bottom margin, and failure's
Y subtraction without undoing its X reset. No pixel/counter layout,
glyph publication or wrapping behavior changes.

Choice placement is unchanged. DuelEffect_UpdateObjectLayout (0x80036DBC)
retains its unsigned-halfword rectangle views, all regional row-spacing
branches and original display-object writes. The password/name-entry overlay adds
rect_x/rect_y to the entry-local glyph position in every regional branch;
Japanese VRAM publication and compact channel addressing remain exact.
Only typed channel accesses change across sixteen existing C units.

### Text-channel cursor advances

The paired u8 fields at +0x5A and +0x5B are glyph_advance_x and
line_advance_y, formerly field_5A and field_5B. Both the 0x64-byte shared
channel and 0x60-byte Japanese channel retain the same offsets and widths.
These are cursor increments, not DisplayObject timers or universal rendered
glyph dimensions. The unrelated DisplayObject/DisplayObjectStreamState
field_5A declarations and consumers remain unchanged.

DuelEffect_InitEntry establishes the regional defaults: North America
8/12, Japan 16/16, and Europe's existing four flag-selected size pairs.
Text_SetFontMode retains every branch, flag update, unsupported-value path
and Japan's original shifted-to-byte narrowing. Text_SetGlyphCellSize
reads the two raw operands through its original separate stream scopes:
publish the advanced stream pointer, then store X; reload/read/publish,
then store Y. No shared temporary, clamp or new size validation.

TextBox_BuildStep adds glyph_advance_x to the pen's horizontal halfword
after the glyph append in North America and Japan. Its European path
instead adds the ORIGINAL signed DuelEffect_AppendEntry result to this
nominal advance. That result is a metric adjustment and can be negative,
including the existing early-return result; it is NOT an absolute width.
Both the pen and pixel counter receive the same original computed sum.
All signed limit comparisons, extra advance in the limit check, halfword
narrowing and the distinct regional glyph/pixel counter layouts remain.
No glyph-result normalization or common counter representation is added.

TextBox_WrapLineIfNeeded retains its signed-halfword X comparison, X reset
and Y increment by line_advance_y. Its original bottom-edge test includes
one further line advance; failure subtracts the prior Y advance and returns
one without undoing the X reset. There is no new wrapping or clipping rule.

Reachable debug/library/duel presentation and password/name-entry producers
retain their original values, guards, channel-base locals, casts and compact
Japanese addressing. The name-entry variable called sprite is a
DuelEffectChannel pointer here, not a DisplayObject. Only these typed channel
member spellings change across twelve existing C units; the proportional-font
comment is corrected without changing its code.

The existing library layout fixture changes only eight member identifiers
in its original offset/width checks. Every assertion, numeric expectation,
oracle, mutation, scenario, control and flag remains unchanged. No new
behavioral harness or local cloud-fixture execution is introduced.

`func_800383DC` returns the selected `TextStreamOwner::streams` slot as
`u8 *G32 *`, matching the stored byte-stream pointer it publishes there,
instead of exposing that pointer slot as an integer payload word.
Regional bank/address construction remains `u32`; conversion to `u8 *`
occurs only at the final store.

The original unsigned selector increment/store and subsequent signed
index remain separate, including their existing wrap behavior. The generic
command table still discards the result through its existing callback cast.
No stream bound, null guard, bank remapping, callback contract or shared
record layout changes.

### String-bank lookup result

`Text_LookupString` exposes its resolved address as `u8 *` in every
regional branch, with its declaration beside the existing bank contracts
in `text_constants.h`. Both signed word arguments remain, including the
currently unused first word; the string ID is not narrowed.

Bank masking and offset OR operations remain unsigned word arithmetic.
Conversion to a byte pointer occurs only after each complete expression.
All thresholds, ID rebasing, Japanese mask/selection rules and European
four-byte table displacements remain unchanged. No bank bounds, missing-ID
result, pointer guard, helper call or new allocation is introduced.

A bounded scan of the loaded North American image found no aligned
J/JAL candidates or stored words naming `0x8003B744`; this does not exclude
computed references or establish runtime unreachability. The helper and
its two-argument interface remain intact.

### Text-command counter and limit writes

output_count and output_limit name the NA channel's original u8
field_60/field_61 at +0x60/+0x61 and the compact Japanese channel's
original u8 pad_5E/field_5F at +0x5E/+0x5F. Japanese pad_5E was an
actively used counter, not unused padding. Both channel sizes, all
widths/offsets, the existing +0x61 assertion and range members remain.

DuelEffect_InitEntry clears only the existing limit byte in NA/Japan;
it gains NO counter store. The PAL initializer retains its raw u16+0x62
limit clear. TextBox_BuildStep clears the counter only in its original
initial-build path. Text_SetOutputLimit (0x800383B0; secondary slot 7)
clears the counter before consuming the operand, then stores the low byte
of TextStream_ReadU16LE as the NA/Japanese limit. It retains the two-byte
cursor advance and byte narrowing, with no clamp or high-byte validation.
Japan retains its actual 0x80037AAC command primary.

NA/Japan call append, increment the byte counter and test the original
nonzero limit using count>=limit, then advance the pen. The counter can
wrap as a byte and still advances when append rejects a glyph; it is not
a count of successfully allocated or currently active entries. Zero
disables only this threshold check. No done flag is cleared and other
completion paths remain unchanged.

PAL instead reads/writes a u16 pixel counter over output_count AND the
NA output_limit byte at +0x60/+0x61. Its distinct u16 limit remains over
address-named field_62 and pad_63 at +0x62/+0x63; channel field_62 also
stages NA icons, so it is not renamed to a universal limit. PAL consumes
one operand byte shifted left by three. The existing halfword casts
are retained, NOT replaced by the new byte members.

The PAL builder adds the signed append adjustment to glyph_advance_x,
updates the pen and halfword counter, and retains the exact signed
look-ahead test: for nonzero signed limit, done is set only when
limit < (s16)n + glyph_advance_x, not count>=limit. Preserve n's existing
width, halfword store, signed casts, strict comparison and ordering;
this is neither a universal glyph threshold nor a new pixel clamp.

Append's 0x200 scratch-buffer calculation uses the CURRENT counter
before the builder increments it, at the original 0x88-byte stride.
That staging index is distinct from Japan's queried glyph_index
occupancy slot. Neither a reservation nor a new scratch bound is added.

Six whole C units change fourteen qualified member identifiers only.
The whole shared and Japanese headers change four declarations, the
one existing offset assertion's member/label and one overlap comment.
No record, padding split, union, callback, macro name, expression,
operand width, narrowing, cursor advance, store/call order, compiler
profile or completion policy changes. DisplayObject field_60 and
unrelated pad_5E storage retain their original names and behavior.

Matching C users across the duel-effect and text-box paths include the shared
header and use `DuelEffectChannel`. In particular, `TextBox_BuildStep` passes
the record through `TextBoxStateCallback`, defined with the other game-owned
text/script value types in `ygo_types.h` and declared
`void (*)(DuelEffectChannel *)`; all eighteen entries of `D_80090E64` have
typed parameters and the table requires no function-pointer casts. The casts
that remain in `Dialog_UpdateChoice` mark calls to helpers that still take
`u8 *`, not uncertainty about the callback record.

`Text_SetFastPrintMode` (originally `func_800384E4`, `0x800384E4`)
selects the same prefix through `TextStreamOwner::streams`
and a `u8 *G32 *` local rather than raw `u8 **` owner indexing. Its original
register-qualified locals and their order remain; the operand capture is
still a full unsigned word, written with the equivalent `u32` alias.
The command clears flag bit `0x1000`, reads one operand byte, advances its
stream once and sets that bit for a nonzero value in the same order.
The flag keeps its numeric spelling; no helper, guard or callback change
is introduced.

The retail secondary command slot 12 at `0x80090EDC` names this handler.
Its script-controlled bit is included in the `0x1C00` consumer mask:
the builder bypasses its delay/input-acceleration block, the pump bypasses
the ordinary yield comparison, and append selects handler zero for new
entries. This supports fast-print mode, not universal instant completion
or clearing already active entry handlers. State callbacks, wrapping and
page-wait handling still run; operand zero clears only `0x1000`, leaving
other acceleration bits `0x400`/`0x800` untouched.

Japan retains its existing `0x80037BA0` binding. Europe names the actual
`func_800382B8` primary at `0x800382B8` (translated releases
`0x800384B8`), removing only the obsolete common-name wrapper macro.

The public text-box build/wait pair and `TextBox_SetPos` now also take
`DuelEffectChannel *`. Typed producers such as `TextBox_Create` and
`DuelEffect_CreateChannel` pass their results directly. A few exact-code
consumers retain raw byte cursors internally and cast only at the call
boundary; `TextBox_SetPos` likewise keeps its repeated member casts because a
typed local changes the GCC 2.8.1 prologue schedule. The occupancy-release
helper `DisplayObjectFade_ReleaseChannel` takes `DuelEffectChannel *`, which removes the
incompatible-pointer calls from both fade callbacks. It reads the channel's
byte at `0x10`, `field_10`, as the object's index into the `D_800EAF08`
occupancy table and clears that table entry. It then zeroes the next byte,
`field_11`, as part of the release.

### Stream pen-X command

`Text_SetPenX` (originally `func_80038388`, `0x80038388`) is secondary
text-command slot 6; the retail pointer at `0x80090EC4` names that entry.
It reads two little-endian stream bytes into the existing unsigned
halfword `pen_x`. Glyph append copies this value to each entry's local
`x_0C`, and the renderer adds the display-object X origin.

This is the local glyph pen, not the text-box origin set by
`TextBox_SetPos` or the script-stream cursor changed by
`Text_SetCursorOffset`. Wrapping remains in `TextBox_WrapLineIfNeeded`,
which retains the signed halfword view, compares against box width,
resets X and advances Y. The setter introduces no immediate wrap or clamp.
The pen's unsigned storage, operand width, stream advancement, glyph
publication and rendering stay unchanged. Japan uses its actual `0x80037A84` primary;
the separate European-only Y command keeps its existing name.

### Stream color-index command

`Text_SetColorIndex` (originally `func_80038498`, `0x80038498`) is
secondary text-command slot 10, verified by the retail pointer at
`0x80090ED4`. Its one-byte operand is a literal index unless bit 7 is
set, in which case the low nibble selects a value from
`gText_abColorSlots`. The result becomes channel byte `field_54`;
the command does not modify that lookup table or store an RGB word.

North American glyph/icon paths publish the byte into entry palette
selection, while Japanese bitmap generation uses it as a glyph-mask
lookup index. The shared field keeps its neutral name. All literal and
lookup behavior, stream advancement, unchecked indices and regional
rendering stay unchanged; no table bound, mask or guard is added.

Japan retains its existing `0x80037B54` binding. Europe names its actual
`func_8003826C` primary at `0x8003826C` (translated releases
`0x8003846C`), removing only the obsolete common-name wrapper macro.
All seven function sizes and binding addresses remain unchanged.

## `D_800EB288`: 620 `0x1C`-byte entries

`DuelEffectEntry` has size `0x1C`, and `D_800EB288` is declared as 620
entries. Exact local C repeatedly uses 28-byte pointer increments, while the
target assembly computes the same stride as `(index * 8 - index) * 4`.
`DuelEffect_ClearMatchingMarker` and `DuelEffect_ResetEntryMarkers` each scan
620 records. The resulting modeled extent is `0x43D0` bytes; the next linker
symbol, `gInput_abRawPadBuffers`, begins 16 bytes later, so the intervening
bytes are not claimed by this type.

Verified entry fields are:

| Offset | Shared field | Evidence |
|---:|---|---|
| `0x11` | `flags_11` | bit `0x80` gates exact scans; target `Text_CompletePageAdvance` and `DuelEffect_ProcessEntries` agree |
| `0x12` | `owner_channel_tag` | original channel index plus one; marker clear leaves the tag unchanged |
| `0x13` | `handler_state` | complete-byte activity test; low-five-bit handler selector plus latch/phase flags |
| `0x15` | `render_mode` | entry draw selector written by append/control/callback paths |
| `0x18` | `layout_selector` | range write in `DuelEffect_SetEntryRangeLayout`; `DuelEffect_UpdateObjectLayout` reads it |

Exact C for `DuelEffect_ProcessEntries` (`0x80039D64`) copies complete
seven-word (`0x1C`) records and advances both pointers by `0x1C`,
independently confirming that the named bytes belong to one record rather
than parallel arrays. Unchiga's same-address sketches and GMS pseudocode
corroborate these accesses but do not determine the shared types.

## Glyph lookup entry views

`TextGlyphLookupEntry` exposes the four-byte glyph lookup entry as one
packed word or two halfwords: the Shift-JIS key at `+0` and an otherwise
neutral upper halfword at `+2`. Layout contracts preserve its four-byte
size, word alignment and both offsets. The upper halfword is not given a
universal flags meaning merely because the channel pump reads its low
three bits.

The unsized `tent_GlyphLookupTable` base and separate const entry-1 alias
`tent_GlyphLookupTableEntries` retain their original bindings. Encoders
and decimal lookup use the named key; the builder retains its packed-word
mask, and the pump retains the upper-halfword kind read. The decimal
guard stays const. Whole-word zero sentinels and Europe's key-only zero
test remain distinct, as do all pointer starts and four-byte steps.
No table bound, enum, normalization, helper or new storage is introduced.

The North American and Japanese decimal maps walk the local copied key
block with byte pointers rather than storing those addresses in `s32`.
The independent low-byte index, high-byte cursor, word-aligned block copy,
initialization order and two-byte advances remain unchanged.

The loop boundary deliberately compares signed 32-bit address views.
A natural pointer comparison changes the target's `slt` to `sltu`;
the explicit casts retain the original signed comparison while memory
access uses real byte pointers. This is not a count-loop rewrite or a
change to the const glyph-table search. The European stream path is
unchanged.

## Text-record lookup result

`Text_FindRecordById` exposes its existing payload cursor as `u8 *`,
including the legacy `func_80036BCC` declaration, rather than returning
that address through `s32`. The full-word ID parameter, big-endian key
assembly and zero-before-match termination remain unchanged. A match
returns the same byte address; failure still returns zero.

Key and payload cursors advance by 2 and `0x1E` independently. The North
American `D_801D9174_b` and `D_801D9174` aliases both bind `0x801D9174`;
Japan binds the ID cursor at `0x801D9BAC` and payload cursor at
`0x801D9BE4`. These bindings and strides do not establish a prefixed or
interleaved record struct, a table bound or separate allocations.

The Japanese bitmap consumer retains its existing `u16 *` payload view
and unchecked miss behavior. No record type, pointer guard, normalization,
search bound, linker alias or regional layout changes.

## Dialog and active-handler control bytes

`duel_effect.h` also owns three scalar control contracts used by the resident
state machine. `D_8009B244` is the message identifier passed to
`TextBox_Create` by `func_80028310`. `D_8009B248` is that dialog's two-stage
latch: bit `0x80` records that the box was created, and bit `0x40` records that
its choice object was opened. `DuelEffect_UpdateState` clears the latch when a
new request starts. `D_8009B24A` holds the active handler index copied from
`D_8009B254` before the request byte receives its started bit.

The header names only the two proven `D_8009B248` flags. It leaves the three
symbols separate because the bytes between their addresses are live or
unexplained; this is declaration centralization, not a claim that they form a
single packed structure.

The Build Deck mode-7 entry pair has a related but separate byte at
`gBuildDeck_bConfirmMode`. `BuildDeck_EnterWideConfirmMode` sets its high bit before
entering state 7; that bit selects the wide confirmation dialog in
`BuildDeck_UpdateExitConfirmation`. In narrow mode, a nonzero choice cancels the pending exit.
The flag lives beside `BuildDeckTransitionState` rather than in local
magic-number uses. Its local scalar declarations remain because the byte also
has candidate-only array views whose relocation contracts are reviewed
separately.

`DuelEffect_UpdateObjectLayout` now provides an additional exact-C read of
`DuelEffectEntry.layout_selector`. It selects the display-object coordinate layout
from `layout_selector % 10` after resolving the entry through the channel's
`gDuelEffect_awEntryRangeBoundaries` range index.

The complete `D_80090F68` display-effect step table now carries
`void (*)(MenuRecord *)`, matching `DisplayEffect_ProcessMenuRecords`, which
selects its callback from a `D_800EB010` record. The easing callback, portrait
callback, three dialog transitions and dialog-channel transition expose that
type directly. The lifecycle and VRAM callbacks retain explicit table casts
because the two repeated lifecycle slots and the single VRAM slot expose
narrower public views of the same storage.
Handlers whose accepted bodies depend on byte arithmetic keep those
expressions through preprocessor aliases rather than introducing a second
live pointer that could change GCC 2.8.1 register allocation.

`DuelEffect_PlaySoundCommand` is now exact C in the effect-handler dispatch
family. It consumes one 16-bit script value, uses the high bit to select the
flagged sound path, and arms effect state `0x11` with a follow-up value.

Six pure-C functions now use the shared declaration:
`DuelEffect_ClearMatchingMarker`, `DuelEffect_ResetEntryMarkers`,
`DuelEffect_HasActiveEntry`, `func_800373C8`, `Text_CompletePageAdvance`, and
`DuelEffect_SetEntryRangeLayout`.

## Typed effect-request allocation and retained ownership

The existing `DuelEffectRequest` layout is a tagged `0x20`-byte record.
Its forward declaration lets `duel_effect_allocate_request.h` return the
actual request type without importing the pool globals or creating an
include cycle. `DuelEffect_AllocateRequest` returns the selected initialized
record or zero when all eight entries are active. Every resident allocation
caller now receives that type directly; redundant record-cast macros are
removed, not replaced with new byte-buffer aliases.

`gDuelEffect_aRequests` (originally `D_800EAD88`, `0x800EAD88`) remains
the same eight-record pool. Reset still clears flags and writes ID `-1`;
lookup still returns the first free entry; allocation still initializes
only the original fields and marks the record ACTIVE. The distinct
`DuelEffect_CreateRequest` wrapper alone raises the pool-status ACTIVE
latch on success. These entry flags and status latch are not conflated.

`gDuelEffect_pTrackedRequest` (originally `D_8009B17C`, `0x8009B17C`)
is the request retained by duel phase/effect controllers across updates.
It is not the dispatcher's current `gDuelEffect_pActiveRequest` pointer or the only active
entry in the pool. All publication, null/unchecked-full behavior and
completion polling retain their original order.

Dispatch still captures `field_1A` before the first-call reset to `-1`,
sets INITIALIZED, calls the same handler with the captured value, restores
the same geometry state and clears flags on the original completion test.
NONBLOCKING still affects the blocking-status bit without skipping dispatch.
No callback, flag, count, phase or lifetime policy changes.

The three payload halfwords keep their neutral names: their meaning
depends on the effect ID. The stored `G32` handle, opaque shared `buffer`,
id-specific/larger partial views and measured field-address bridges remain
explicit. A typed request result does not turn that payload into a display
object or introduce new allocation, zeroing, pointer guards or ownership.
All request sizes, offsets, eight-entry bounds, fourteen regional data
bindings and compiler profiles remain unchanged.

## Runtime state and channel lifecycle

Matching `DuelEffect_UpdateState` defines the global dispatcher lifecycle.
State zero is inactive. On the first update of a nonzero state, the function
copies its value to `gDuel_bActiveEffectState`, sets bit `0x80` in
`gDuel_bEffectState` as the initialized latch, clears
`gDuel_bEffectHandlerFlags`, and reports active. A later bit `0x40` completes
and clears the state; otherwise the latched index dispatches through
`gDuelEffect_apfnStateHandler`. The complete five-slot contract is documented
in [`duel-effect-state-runtime.md`](duel-effect-state-runtime.md).

That dispatcher byte is separate from `D_8009B3C1`. In the later callback
state family, `DUEL_EFFECT_STATE_FLAG_INITIALIZED` (`0x80`) is the shared
one-shot entry latch. The matching callbacks in `dialog_transition.c`
and the load/save callbacks in
`mem_card_dialog.c` test and set it before their first-frame object or
companion-state setup, so subsequent frames skip that initialization. This is
a state-machine flag, not the unrelated `0x80` bit in
`DuelEffectEntry.flags_11`.

`DuelEffect_CreateChannel` resets the current dialog choice to `-1`, creates a
`DuelEffectChannel` through `TextBox_Create` using the low 15 bits of its
request value, and stamps `field_59` from the shared sequence byte minus one.
A nonzero setup argument adds flags `0x1008`; otherwise a high request bit
selects the alternate `func_80039A14` setup path before the channel is
returned.

`DuelEffect_MarkObjectIfActive` scans the separate three-entry table at
`D_800EB010` from the last entry backward. It sets object flag `0x2` when any
entry has a nonnegative signed marker, and leaves the object unchanged when
all three markers are negative. `DisplayEffect_ProcessMenuRecords` walks the
same three records forward, applies their transition update when field
`0x32` requests it, and dispatches `display_effect_step` through the mapped
`D_80090F68` lifecycle table. `DuelEffect_InitEntryDefaultFlags` is the
zero-flags wrapper around `DuelEffect_InitEntry`, making the default entry
construction path explicit without broadening either shared structure.

## Deferred and exact-code exceptions

Matching-C functions implemented with inline assembly remain unchanged:

- `Main_RunCredits` references `D_800EB0F8`;
- `DebugMenu_UpdateCampaignEntry` references both globals through explicit relocations.

`DuelEffect_UpdateObjectLayout` is also matching C, but it retains a minimal
local `EffectEntry` view and is not counted among the six shared-header users.
`DuelEffect_ProcessEntries` likewise remains on its local complete-record view
so its accepted source does not broaden the shared declaration.
`func_8003DA40` is matching C as well, and now takes its dialog channel as a
`DuelEffectChannel *`, both the one `DuelEffect_CreateChannel` returns and the
`D_800EB0F8` element it selects later (`flags_34`, `prompt_object`). The index it
selects that element with is still the raw byte at `+0x1A` of its
`MenuRecord`, which `dialog_transition.c` reads the same way.
`func_8003D74C`, which coordinates the duel-intro card reveal, likewise
takes its channel as a `DuelEffectChannel *` and selects it by the same
`+0x1A` byte.

The changing assembly-user lists for `D_800EB0F8` and `D_800EB288` are not
duplicated here. [`global-usage.csv`](global-usage.csv) is the generated
authority: filter by `global_address` and a `function_status` other than
`matching_c`. Regenerate it with `make global-usage` whenever source or symbol
changes affect these globals.

`DuelEffect_HasActiveEntry` and `func_800373C8` in
`duel_effect_entry_control.c` are the sole raw-view exceptions among migrated
pure-C users. Both include the shared header and use its typed global
declaration, but retain byte-pointer field expressions. Replacing those
expressions with direct `DuelEffectEntry` member access shortened the two
exact functions by eight bytes in total and failed the resident text-size
check. The retained expressions preserve GCC 2.8.1's address-adjustment and
register schedule; they are not a competing layout declaration. The third
function in that grouped source, `Text_CompletePageAdvance`, uses the shared
`DuelEffectEntry` fields directly.

The shared field syntax was exact-tested successfully in the other migrated
users, including direct indexing in `DuelEffect_InitEntry` and both marker
functions in `duel_effect_entry_occupancy.c`.

## Validation

The completed pass retained the full executable SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88` with
`MAKEFLAGS=-j2 make match`. `make basic-types`, `make global-usage`, and
`make check-global-usage` also passed after the final source changes.

## PAL binding for the active-entry test

Every region declares `DuelEffect_HasActiveEntry` with the same plain
prototype in `duel_effect_entry_control.h`. Each PAL symbol file binds the
name at its own address (`0x800370E0` in the UK image, `0x800372E0` in the
other PAL images), so the former `func_800370E0` asm-label alias and the
`VERSION_EUROPE` declaration split are gone.

## Channel indices and entry ownership tags

The channel's original u8 index_57 at +0x57 is channel_index in both
the shared 0x64-byte and compact Japanese 0x60-byte records.
DuelEffect_InitEntry (0x80035AF0) uses its index argument to select that
same gText_aChannels element, stores the narrowed byte, and selects its
entry-range boundaries. The original regional arithmetic and store order
remain exact; no bounds check, new default or layout unification is added.

DuelEffect_AppendEntry (0x80036C14) writes channel_index + 1 into the
entry's original field_12. owner_channel_tag names that byte: it is NOT
the raw zero-based channel index. The North American/Japanese byte is at
+0x12, while the European member with the same original name is at +0x10.
The shared, Japanese and European entry types retain their original
0x1C, 0x18 and 0x16 sizes and alignment. Every append mode and rejection
keeps the tag store at its original point, including writes that precede
a rejected glyph. The +1 expression and byte narrowing are unchanged.

The Japanese DuelEffect_RasterizeGlyph (0x800362AC in SLPM-86398)
publishes the same channel_index + 1 to the occupancy slot. Zero remains
the occupancy allocator's free-slot value; this does NOT establish that
an inactive entry has a zero ownership tag. DuelEffect_ClearOccupancyValue
(0x80035CA8) increments its original integer argument before comparing
all 240 occupancy bytes. DuelEffect_ClearMatchingMarker (0x80035DB8)
likewise increments its argument before scanning owner_channel_tag,
and clears flags_11 rather than the tag. The entry reset also clears
flags, not ownership; compaction retains the whole original record.
Neither clear routine gains a narrowed comparison or a new tag reset.
European entry count/stride and the absence of its occupancy clear remain.

TextBox_Destroy, the initial-build cleanup and choice setup supply the
same stored channel_index to these clears, retaining every regional
guard and order. DuelEffect_UpdateObjectLayout indexes the channel's
entry-range boundaries with it. TextBox_RefreshDisplayObjects copies it
into its newly acquired renderer's DisplayObject.field_67, and
TextBox_DrawEntries uses that byte to select gText_aChannels, including
the compact Japanese record view. Reused renderers do not gain a new
binding write. field_67 remains generic: preview resources, deck-list
labels, card markers and command-stream selectors give it other meanings.
No DisplayObject byte is globally renamed or given a text-only type.

Only seven complete C units change under eighteen owner-qualified
non-comment member identifiers and one existing marker comment's member
identifier. Three whole headers change five declarations and the two
existing shared offset assertions' member/label identifiers. All types,
widths, offsets, numeric assertions, sizes, qualifiers, G32 annotations,
expressions and other record owners remain. There are zero fixture edits.
