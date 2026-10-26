# Secondary text composition and staged operands

This component connects decimal formatting, staged-string bank selection
and the shared digit-glyph map through the existing channel stream stack.
It adds no formatter, string allocator, bounds policy or bank abstraction.

## Pointer-driven formatted decimal push

`Text_PushFormattedDecimal` names secondary command three. The command
reads a little-endian word as a pointer, dereferences the original signed
word at that address, then reads the modifier byte. The pointed value does
not become an inline stream integer.

The local eight-byte digit buffer and `Text_EncodeDecimalDigits(value,
modifier & 0xF, buffer)` call remain. Formatting keeps the original full-byte
modifier branches: optional fixed width, glyph-zero versus zero padding,
and the separate leading-blank scan. The scan's full-byte behavior and the
later low-nibble loop count are not merged into a validated width.

Each selected glyph retains the existing map read and one-/two-byte
encoding. The wide-code branch stores `(glyph >> 8) - 0x10` then the low
byte; the narrow branch stores one byte. The original reverse-index
do/while and terminator store remain, including unsupported-width behavior
rather than a new guard, clamp, negative-value policy or safer loop.

After formatting, the channel's stream byte increments before the buffer
pointer is assigned to the selected slot. The command pushes the existing
`decimal_buffer` buffer; it does not allocate storage or directly rasterize glyphs.
The callback remains `void(DuelEffectChannel *)` in the generic table.

| Target | Callback address | Size |
|---|---|---|
| North America | `0x80038148` | `0x160` |
| Japan | `0x80037870` | `0x160` |
| UK | `0x80037EB8` | `0x160` |
| France / Germany / Italy / Spain | `0x800380B8` | `0x160` |

Only the obsolete Japanese decimal-function alias retires. The neighboring
string callback, digit-map alias, signed string-ID selector and all
regional source controls remain until their own exact integration.

All 940 complete compiler/MASPSX outputs match after only this callback
identity, with full C/header/native-inventory/ordered-ledger conservation.
Every function size, status, ownership, note, source group, profile and
manifest remains. No source path, test, witness, fixture, control, trace or
tool changes.

## Staged string push and native ID views

`Text_PushStagedString` reads `gText_StagedStringId`, not a stream operand,
and resolves the bank pointer before incrementing the stream byte. The byte
increment, store and signed `s8` slot index remain in their original order.
The function returns `u8 *G32 *`, the address of stored pointer storage;
the generic command-table cast still discards that result.

The staged ID retains all three original header arms and selectors:
signed `s16` for the Japanese reader, unsigned `.data` for the result
publisher and ordinary unsigned `u16` elsewhere. Result and Free Duel
publishers keep their original signed truncation, halfword stores,
availability/selection gates and textbox timing.

No common lookup helper replaces the native expressions:

| Region | Existing bank behavior retained |
|---|---|
| NA | Separate above-0xCFFF and bit15 ranges, then the original0x500 index adjustment; keep each distinct base/table and addition. |
| Japan | Signed ID read, bit15 test/mask, two banks and the original `0x801F0000` address mask with OR composition. |
| Europe | Original low/high ID branch,0x500 adjustment and separate +4 bases/tables; high bank retains0x8000 subtraction. |

There is no new ID mask, enum, slot bound, return-type normalization or bank
layout inference. The raw word and all repeated loads remain.

| Target | Push callback | Staged halfword |
|---|---|---|
| North America | `0x800383DC`, size `0xBC` | `0x8009B32E` |
| Japan | `0x80037AD8`, size `0x7C` | `0x8009B21E` |
| UK | `0x800381C0`, size `0xAC` | `0x8009C294` |
| France / Germany / Italy / Spain | `0x800383C0`, size `0xAC` | `0x8009C6B4` |

The shared Free Duel source now uses the canonical staged word. All seven
module import sets bind it to their unchanged native addresses, including
the explicit NA import that the numeric spelling did not previously need.
No module layout, profile, export address or image changes.

Both existing result-controller test files retain their complete contents
after reversing only the staged-ID identifier, including codegen symbol
checks and every expected value/control. All 940 complete resident outputs
and entire C bodies match after only the callback/ID names and one retired
Japanese callback macro. Header tokens preserve all type/view forms; the
full prior ledger gains exactly two evidence/caveat rows.

## Decimal glyph map and password imports

`gText_DecimalDigitGlyphMap` keeps the original incomplete `u16 []` view.
The decimal formatter retains its redundant member-address read casts, and
the password digit display retains its eight-digit loop and native
one-/two-byte output expressions. No bound or shared decoder is added.

NA/Japan copy the ten two-byte SJIS keys into the existing local byte
buffer, retain the signed pointer-order walk and search the const lookup
view for each key. A match stores its original one-based index; an empty
table or missing match does not gain a zero/fallback store or map clear.
The accepted stack byte-pointer declarations remain unchanged.

Europe instead decodes the existing terminated byte string: a byte from
0xF0 uses its low nibble as the high code byte,0xFF terminates early and
the existing ten-code limit ends the loop. An empty/short string leaves
unwritten entries untouched. This is not normalized to the index lookup.

| Identity | North America | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Decimal digit glyph map | `0x800EAFF8` | `0x800EAED8` | `0x800EBE70` | `0x800EC290` |

The resident Japanese alias and the password module's different alias
already identify the same native word range. Exactly three obsolete
producer/consumer macros retire after canonical wiring; every unrelated
alias stays. All seven password import sets bind the new name to their
original address, including an explicit NA import. Module source paths,
layouts, profiles and manifests remain unchanged.

The standalone Japanese initializer also spells the former resident alias
directly in its body. That producer reference must migrate with the macro
and declaration; removing only the macro leaves the old name undeclared.
Its canonical assignment retains the same pointer type, address, byte
cursor walk, lookup behavior and compiler profile.

All 940 complete resident outputs match after only this map identity.
Complete resident/overlay C bodies and header tokens retain every other
byte after the exact three macro removals; all existing tests and controls
remain untouched. The complete prior ledger gains one row with the
regional representation caveat.

## Bundle boundary

The decimal command, staged string views and regional decimal map/imports
form one component PR after cumulative
clean acceptance. Preserve the string callback's pointer-slot return and
generic-table cast, signed/unsigned/data arms and all native bank semantics.
The decimal map is an incomplete u16 array whose initialization differs
between lookup-index and encoded-code regions.

Only evidenced identities and declaration ownership change. Existing
controls may receive strictly proved identifier migrations where needed;
no harness is added and no cloud/regional/overlay suite is run locally.

## Decimal buffer and signed stream selector

decimal_buffer is the original channel-local u8 text_44[0x0D] at +0x44;
stream_index is the original s8 stream_58 at +0x58. The formatter still
reads its pointed signed value through the unchanged U32LE pointer
operand, consumes the modifier byte, uses the local eight-byte digit
buffer and writes the same one-/two-byte glyph encoding and terminator.

Keep the full-byte modifier branches and leading-blank scan distinct from
the low-nibble reverse do/while count. Unsupported widths gain no clamp,
guard, buffer extension or safer loop. After formatting, the original
signed index increment still precedes storing decimal_buffer into the
selected pointer slot; no allocator or glyph upload is substituted.

Staged-string push still resolves its regional bank BEFORE byte increment,
keeps the explicit u8 store and subsequent s8 slot cast, and returns the
address of pointer storage. Every regional ID mask/adjustment, OR versus
addition, independent table/anchor and generic-table return-value cast
remains. root_stream is slot zero's mutable cursor, not storage for either
of these nested stream pushes.
