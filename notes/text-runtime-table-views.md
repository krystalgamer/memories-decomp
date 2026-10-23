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
