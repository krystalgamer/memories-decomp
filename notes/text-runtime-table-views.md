# Text interpreter table views

`TextBox_BuildStep` dispatches both its primary stream-command table and its
nonzero-state callback table. The primary commands include
`Text_PushFormattedDecimal`, whose encoder reads the decimal divisor prefix.
Both tables are defined once under their canonical names:

| Former identity | Canonical name | Representation |
|---|---|---|
| `D_80090E0C` | `gText_DecimalDivisorView` | mutable, unsized `s32[]` view |
| `D_80090E64` | `gTextBox_apfnStateCallbacks` | unsized `TextBoxStateCallback[]` |

## The numeric view does not name the opaque backing

The NA definition, `gText_DecimalDivisorView`, stays in the same translation
unit as the Build Deck step table. Its 19 words remain exactly transcribed: the first
seven are `1, 1, 10, 100, 1000, 10000, 100000`, followed by twelve words
whose independent meaning is not established.

`Text_EncodeDecimalDigits` loads the count-indexed word into signed `d`,
divides the value by it and advances `d /= 10`. The name describes the
known divisor prefix; it makes no claim about the block's suffix.

The declaration stays mutable and unsized. No seven-element bound, const
qualification, clamp, guard or assertion that every word is a decimal power
is introduced. The stream command still passes its unclamped low-nibble
count. Existing zero/negative/large-count, buffer-write, signed division,
blanking and no-padding behavior remains unchanged.

The twelve bytes between this block and the callback table
are not classified by proximity. Neither the shared initialized-data translation unit
nor its source/profile/order is split or rearranged.

## State callbacks retain their native storage

The NA table still contains the same eighteen entries in order, including
the duplicate `Dialog_UpdateChoice` entries. The reader keeps its initial
nonzero-state test and exact five-bit index mask. Its public declaration
remains unsized; there is no new eighteen-entry guard or assumption that
all masked indexes are valid.

The table is defined as `gTextBox_apfnStateCallbacks` in
`text_box_build_step.c`; its pointer types, initializers and call
expressions are unchanged.

## Regional expansion and linker names

Every region binds the canonical names directly; the former `D_80090E0C`
and `D_80090E64` link aliases are gone. No claim about identical regional
callback contents or lengths follows. Native assembly, relocations,
initializer values and ordering are unchanged.

## Regional pen and state-callback interfaces

Three existing matching identities now agree with their regional inventories
and existing declaration owners:

| Function | Japan | UK | France / Germany / Italy / Spain | Bytes |
| --- | --- | --- | --- | --- |
| `Text_SetPenY` | not built in this unit | `0x80038168` | `0x80038368` | `0x28` |
| `TextBox_WaitForScriptCompletion` | existing canonical name | `0x80037544` | `0x80037744` | `0x1C` |
| `Dialog_OpenChoice` | `0x80036CAC` | existing canonical name | existing canonical name | `0xB0` in Japan |

The PAL-only pen helper consumes one little-endian halfword through
`TextStream_ReadU16LE` and stores it into channel `+0x3A`, the existing
unsigned pen-Y halfword. Its adjacent X setter and the reset-X/advance-Y
handler independently establish that role. Only the definition identifier
changes. `duel_effect_command.h` declares it under the existing
`VERSION_EUROPE` selector, and each PAL symbol file binds `Text_SetPenY` at
its own address (`0x80038168` UK, `0x80038368` elsewhere).

All operand values, stream advancement and the halfword store remain.
No signed coordinate, clamp, combined X/Y setter or new font/layout policy
is introduced. This name does not assign a secondary-command opcode or
claim the helper occupies NA's slot seven; native image adjacency is not
a table-position proof. See [text-layout-commands.md](text-layout-commands.md).

The script-completion callback already has its canonical C name, and
`duel_effect_state_callbacks.h` declares it identically for every region.
Each PAL symbol file binds `TextBox_WaitForScriptCompletion` at its own
address (`0x80037544` UK, `0x80037744` elsewhere) instead of the former
`func_80037544` asm-label alias. It tests the existing
shared byte `gText_ScriptDispatchGate` and clears state `+0x51` only when
that byte is zero; otherwise the state remains untouched.

The gate is not a per-channel token, timer or command-table index. Existing
image/tween producers, gate/command publication and the builder's separate
whole-command-word completion test remain unchanged. The initialized NA
state table keeps this callback at slot ten. No identical regional table
contents or lengths are inferred. See [text-script-bridge.md](text-script-bridge.md).

The Japanese `Dialog_OpenChoice` entry was already canonical in the real
C definition, `dialog_choice_state.h` and native linker symbols; only its
inventory name was stale. It still returns the unchecked cursor allocation,
configures the existing record-derived position, sets its sprite flags,
selects ordering table one and applies the signed-byte depth offset from
`field_59 + 1`. Cursor storage, input polling and release remain the callers'
responsibility. No new binding, alias, type or callback is added.
See [text-page-transition.md](text-page-transition.md).

All function bodies, layouts, stream/table bounds and initialized table
contents stay unchanged. Native conservation covers eight actual complete
source/profile routes: three regional command units with named
`gcc_2_8_1_g8_split`, three callback/constructor units with named
`gcc_2_8_1_g8`, and both NA initialized command/state-table owners with named
`gcc_2_8_1_g0`. Complete compiler/MASPSX assembly, symbols and
disassembly/relocations are compared. Coverage is 30 functions / 3,928
instruction bytes for NA, 30 / 3,676 for Japan and 31 / 3,988 for each PAL
image. No synthetic/generated-C/source-string harness or new testcase is
added; regional image and witness execution stays on the PR.
