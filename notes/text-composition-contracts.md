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
`text_44` buffer; it does not allocate storage or directly rasterize glyphs.
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
