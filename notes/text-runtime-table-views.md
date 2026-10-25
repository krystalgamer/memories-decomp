# Text interpreter table views

`TextBox_BuildStep` dispatches both its primary stream-command table and its
nonzero-state callback table. The primary commands include
`Text_PushFormattedDecimal`, whose encoder reads the decimal divisor prefix.
These two names expose existing interpreter data without new storage:

| Native identity | Canonical view | Existing representation |
|---|---|---|
| `D_80090E0C` | `gText_DecimalDivisorView` | mutable, unsized `s32[]` view |
| `D_80090E64` | `gTextBox_apfnStateCallbacks` | unsized `TextBoxStateCallback[]` |

## The numeric view does not name the opaque backing

The NA definition remains `D_80090E0C` in the same translation unit as the
Build Deck step table. Its 19 words remain exactly transcribed: the first
seven are `1, 1, 10, 100, 1000, 10000, 100000`, followed by twelve words
whose independent meaning is not established.

`Text_EncodeDecimalDigits` loads the count-indexed word into signed `d`,
divides the value by it and advances `d /= 10`. The canonical name is a
logical view of the known divisor prefix, not a semantic rename of the
whole backing block or its suffix.

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

This is an initialized native function-pointer table. The pointer policy
in `check_g32.py` deliberately retains native pointers for a name initialized
elsewhere in the sources, including its plain extern declarations.
The new interface therefore uses a simple header alias, not an additional
uninitialized pointer object or an incompatible guest-width declaration.
All existing pointer types, declarations, initializers and call expressions
remain intact after preprocessing.

## Regional expansion and linker names

The header aliases expand to the original identifiers. In Japan the
existing adapter then expands `D_80090E64` to `D_80090D14`; no North American
literal is pinned into that reader. Actual compiled NA/European readers
retain `D_80090E64`, while the Japanese reader retains `D_80090D14`.
No claim about identical regional callback contents or lengths follows.

Each image exports the canonical names as aliases of its existing native
providers. For the NA initialized objects these linker aliases inherit the
existing data section, rather than becoming absolute symbols or allocating
another table. Original symbol names, section/type/value metadata and data
ownership remain intact; only two canonical linker names are added.

The compiler-visible tokens are identical without identifier rewriting:
the aliases resolve before compilation. Native assembly, relocations,
initializer values and ordering are unchanged. Existing witnesses and
mutations retain their complete bytes; no new source-string harness,
fixture, pointer representation, SDK declaration or profile is added.

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
changes. `duel_effect_command.h` provides the explicit native `func_80038168`
assembly label under the existing `VERSION_EUROPE` selector.

All operand values, stream advancement and the halfword store remain.
No signed coordinate, clamp, combined X/Y setter or new font/layout policy
is introduced. This name does not assign a secondary-command opcode or
claim the helper occupies NA's slot seven; native image adjacency is not
a table-position proof. See [text-layout-commands.md](text-layout-commands.md).

The script-completion callback already has its canonical C name. Its
`duel_effect_state_callbacks.h` owner now supplies the PAL native
`func_80037544` assembly label, replacing only the private producer-wrapper
mapping. NA/Japanese declarations remain unchanged. It tests the existing
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
