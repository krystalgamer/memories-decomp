# Pen offsets and regional glyph-metric commands

The four secondary handlers in
[`duel_effect_command.c`](../src/game/duel_effect_command.c) keep the same
positions in the
[`dispatch table`](../src/game/duel_effect_command_table.c). Their names
describe the complete matching bodies, without treating the regional operand
domains as a universal font-mode enum.

| NA address | Secondary index | API |
|---|---|---|
| `0x800380D4` | `0x01` | `Text_ResetPenXAndAdvanceY` |
| `0x80038110` | `0x02` | `Text_AdvancePenX` |
| `0x800382A8` | `0x04` | `Text_SetFontMode` |
| `0x80038334` | `0x05` | `Text_SetGlyphCellSize` |

## Operand and storage domains

The channel-local pen at `+0x38/+0x3A` remains two unsigned halfwords.
Reset/advance-Y first stores zero to X, consumes one byte, publishes the
advanced stream pointer and adds the explicit `s8` operand to Y. The signed
quantity is the delta, not a newly signed pen member. X advance instead
consumes an unsigned byte and adds it to X. Both retain the old low-halfword
wrapping; no clamp, sign normalization or implicit line height is added.

Glyph width/height at `+0x5A/+0x5B` remain byte storage. The explicit-size
handler preserves two separate block-scoped stream/current/value lifetimes:
read and publish width first, then read and publish height. Zero and all
other byte values remain accepted, and it does not clear font flags or
reset the pen.

The canonical `DuelEffectChannel` layout gains only the missing pen and
height offsets and pen/metric width contracts. The existing width offset and
all record-size/pointer contracts remain. Fields are not globally renamed
into text-only meanings, and no storage is allocated, widened or initialized.

## Font modes are regional

| Region | Recognized behavior |
|---|---|
| North America | Mode 1 stores `8x8`; mode 2 stores `8x12`. Other operands leave the metrics unchanged. Bit `0x100` is cleared first and is raised again only for operand 1. |
| Japan | Every operand stores `operand * 8` into both metric bytes, retaining low-byte truncation. Bit `0x100` is cleared first and raised only for operand 1. |
| Europe | Mode 0 stores `8x16`; 1 stores `8x8` and low flag 1; 2 stores `12x16` and low flag 2; 3 stores `16x16` and low flags 3. The two low flag bits are cleared before the switch. |

An unrecognized NA/European operand is therefore not a whole-command no-op:
its pre-switch flag clear still happens even though the metric stores do
not. The European `u8` operand local and NA/Japanese `u32` local remain
unchanged, as do the inlined reader, flag expressions, switch order,
unsigned metric stores and scoped lifetimes. No fallback size, lookup table,
mode validation or new flag is introduced.

These modes are distinct from symbol selection, the existing
`Text_SetOutputLimit` and the direct two-byte cell-size command. Width and
height here describe glyph metrics, not a replacement text-box rectangle or
new bitmap allocation.

## Native identities

| API | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Reset X / advance Y | `0x800380D4` | `0x800377FC` | `0x80037E44` | `0x80038044` |
| Advance X | `0x80038110` | `0x80037838` | `0x80037E80` | `0x80038080` |
| Font mode | `0x800382A8` | `0x800379D0` | `0x80038018` | `0x80038218` |
| Explicit cell size | `0x80038334` | `0x80037A30` | `0x800380EC` | `0x800382EC` |

The three obsolete European address-name macros and Japanese font-mode
macro are removed only after wiring their actual native names. Japan's
different function at numeric `0x800382A8` is not the font-mode handler and
is left untouched. Inventories retain every address, size, status, row
position, owner and note; profiles, source grouping, channel layouts and
table positions do not change.

The storage-contract-only stage reproduces the complete command and
initialized-dispatch compiler/MASPSX units raw-identically. The naming stage
compares those whole artifacts after reversing only four function names,
without filtering instructions, directives or relocations. Full clean
executable matching remains required. No test, fixture, synthetic source,
scenario, diagnostic control or runtime capture changes; regional/cloud
validation remains on the PR.
