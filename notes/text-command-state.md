# Shared text command publication and continuation state

Four existing identities connect the
[`TextBox_BuildStep`](../src/game/text_box_build_step.c) dispatcher,
its command handlers and choice-label layout. They are shared cells and
an interior table alias, not a new interpreter object or per-channel state.

| NA address | Name | Existing storage/view |
|---|---|---|
| `0x8009B33A` | `gText_CurrentStreamCode` | Volatile unsigned halfword |
| `0x8009B350` | `gText_CommandDisposition` | Volatile signed word |
| `0x8009B340` | `gText_ChoiceLayoutCallback` | Stored function pointer taking a volatile channel |
| `0x80090F18` | `gText_apfnPrimaryCommands` | Unsized raw-argument function-pointer array view |

## Current code and common-symbol ownership

The builder retains its **definition**, `volatile u16
gText_CurrentStreamCode;`, in the same translation unit. This tentative
common definition and its tracked linker override are matching inputs:
they are not replaced by an initialized object, moved into a header, or
converted to an ordinary external-only reference.

Only the external declaration moves from `unmatched.h` to the existing
`text_stream_commands.h` owner, where it now agrees with the definition's
volatile qualification. The extension handler retains its explicit local
`volatile u16 *code`, rather than relying on a new local or expression
shape. That declaration-only stage leaves every complete compiler/MASPSX
artifact raw-identical.

The builder stores the stream byte, reads it back for primary dispatch
and glyph lookup, then publishes it to the separate
`gText_PumpedGlyphIndex` when the glyph path is reached.
`Text_ExtendGlyphCode` combines an F0-F5 prefix and following byte into
this same word before publishing the negative disposition. Thus it is a
current command-or-glyph code, not only an opcode, choice index or
last-rendered glyph.

## Signed volatile disposition

The builder resets `gText_CommandDisposition` to zero before calling a
primary handler, then separately reads it for `>= 0` and `== 1`.

| Reader outcome | Behavior |
|---|---|
| Negative | Continue through the glyph path using the published code |
| Exactly one | Return from the builder, yielding this parse step |
| Other nonnegative value | Continue reading commands |

Known writers use zero, one and minus one, but the reader's full signed
branch domain remains. No Boolean conversion, enum validation, comparison
folding or new fallback is introduced. The shared declaration remains
`extern volatile s32`; both volatile reads, their order, and the
code-before-negative-disposition store order are retained.

## Choice-layout callback

`Text_HandleChoiceCommand` stores only `Text_TryCompleteChoiceLayout`
into `gText_ChoiceLayoutCallback`. New-box initialization clears it.
`Text_NewLine` is the only caller and retains the null check followed by
the original `CALL32` dispatch. The callback clears the cell when the
line count reaches the choice count.

The stored signature remains `void (*G32)(volatile DuelEffectChannel *)`.
This qualifies the callback's channel accesses, not the pointer cell
itself. In particular, the callback's volatile flags read and ordinary
halfword store remain distinct. There is no new callback array, ownership
transfer, reset policy or per-channel lifetime.

## Primary table is an interior alias

The alias remains within the existing initialized secondary-command table.
In NA, `0x80090F18 - 0x80090EAC = 0x6C`, so it starts at entry 27 of the
47-entry grouped initializer. Japan likewise uses `+0x6C`. European native
bindings instead use `+0x74`, entry 29: UK `0x80091F18 - 0x80091EA4`, and
the other European images `0x8009232C - 0x800922B8`.

All sixteen F0-FF entries and every enclosing initialized table position
stay unchanged. The existing unsized `void (*G32[])(u8 *)` declaration,
the dispatcher's raw local pointer/call view and explicit alternate-view
casts remain. No separate array is emitted, no shared region-wide table
offset is invented, and no callback signature or bound is strengthened.
The opcode mapping remains in [text-control-bytecode.md](text-control-bytecode.md).

## Native bindings and acceptance

| Identity | NA | Japan | UK | France / Germany / Italy / Spain |
|---|---|---|---|---|
| Current code | `0x8009B33A` | `0x8009B22A` | `0x8009C2BA` | `0x8009C6DA` |
| Disposition | `0x8009B350` | `0x8009B240` | `0x8009C2AC` | `0x8009C6CC` |
| Choice-layout callback | `0x8009B340` | `0x8009B230` | `0x8009C29C` | `0x8009C6BC` |
| Primary handler alias | `0x80090F18` | `0x80090DC8` | `0x80091F18` | `0x8009232C` |

Bind each canonical name at its actual native address and remove only the
seven obsolete Japanese disposition/table macros. Historical French
address-suffixed aliases remain. Source paths, profiles, record layouts,
function inventories, enclosing tables, GP/absolute access shapes and
accepted semantic-ledger bytes are retained.

The ownership-only stage compares every complete compiler/MASPSX output
raw-identically. The naming stage compares every whole output after
reversing only four global names, with no filename, instruction, directive
or relocation filtering. Full clean retail executable matching remains
mandatory. Tests, fixtures, diagnostic controls and runtime captures do not
change; no synthetic/generated-C/source-string harness is added. Regional
and cloud validation remain on the PR.
