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

## Bundle boundary

The staged string callback and raw ID views, plus the regional decimal map
and overlay imports, join this command in one component PR after cumulative
clean acceptance. Preserve the string callback's pointer-slot return and
generic-table cast, signed/unsigned/data arms and all native bank semantics.
The decimal map is an incomplete u16 array whose initialization differs
between lookup-index and encoded-code regions.

Only evidenced identities and declaration ownership change. Existing
controls may receive strictly proved identifier migrations where needed;
no harness is added and no cloud/regional/overlay suite is run locally.
