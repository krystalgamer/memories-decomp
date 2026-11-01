# Model unit-dispatch views

This component names the handler-key registry and the image-context fields
used by the existing `Model_InitSlot` unit scan. Polygon/shared units bind
and register handlers; image units prepare upload records, call the SDK
image handler, wait for completion, and replace the unit callback with the
existing sentinel. The registry accessors, reverse maps, and image-context
pass retain their grouped source and compiler profiles.

| Original view | Canonical view | Preserved representation |
| --- | --- | --- |
| `tent_ModelHandlerRegistry` | `gModel_aHandlerKeyRegistry` | mutable `ModelHandlerRegistryEntry[80]` |
| `ModelType2Scratch.indices` | `clut_data` | `u8 *G32`, `+0x18` |
| `ModelType2Record.field_00` | `vram_x` | `u16`, `+0x00` |
| `ModelType2Record.field_02` | `vram_y` | `s16`, `+0x02` |

## Handler-key registration and reverse lookup

The eight-byte registry entries keep `handler_value` at `+0` and `key` at
`+4`. `Model_RegisterHandlerKey` scans in the original order. An existing
key returns immediately without updating its handler; an empty entry must
have both words zero. A full table with no match remains a no-op.

`Model_FindHandlerKey` and the two primitive remapping functions keep their
complete 80-entry reverse scans. The specific `GsU_00000000` function
sentinel is not a null pointer: zero values, empty entries, missing keys,
and the SDK sentinel must not be collapsed into one case.

Ordinary/shared primitive binding supplies the visible registration calls
during `Model_InitSlot`. The registry name describes its key/handler
contract without claiming that every possible key must be a polygon kind.
The signed numeric handler word is not converted to a function pointer,
and no callback ABI, map implementation, validation, or storage is added.

`model.h` declares only the canonical fixed-count view, with no assembler
label. The seven resident linker inputs export the canonical name as an
alias to the retained native provider:

| Image | Existing provider address |
| --- | --- |
| North America | `0x800F5918` |
| Japan | `0x800F2FF0` |
| United Kingdom | `0x800F6880` |
| France, Germany, Italy, Spain | `0x800F6C88` |

The NA provider is an absolute symbol. It is not replaced by a C definition
or assigned a fabricated ELF size. The 80-entry type remains the existing
view, not a new allocation or exclusive-ownership claim.

## Image records and the CLUT-data pointer

The authoritative `GsARGUNIT_IMAGE` declaration places `imagetop` at `+0x14`
and `cluttop` at `+0x18`. The local `ModelType2Scratch` prefix keeps the
existing pointer-bearing layout, padding, and `u8 *G32` view at `+0x18`;
it is not replaced by a full SDK argument structure.

The existing NA `GsU_02000001` SDK routine at `0x80089ED0` provides a direct
consumer-side discriminator. It passes each record pointer to `LoadImage`
as the rectangle and forms the data pointer from the record's `+8` word
offset shifted by two. After the 12-byte image record, it does the same for
the CLUT record using context `+0x18`. This establishes the coordinate
roles and CLUT-data pointer independently of guessed reference-file types.
The SDK routine remains SDK assembly; no SDK decompilation or attempt
record is introduced.

The local record remains 12 bytes. `vram_x` deliberately remains `u16`
despite the SDK rectangle's signed coordinate view, and `vram_y` remains
`s16`. There is no substitution of `RECT`, signedness change, range
guarantee, or new bound. The separate
`ModelType2RecordHeader.descriptor_word_count` keeps its original `u16`
representation. The SDK scanner consumes it as the number of 32-bit
descriptor words following the preceding type word; it is not the
number of image rows, columns, records or pixel-data words.

The processor keeps its original per-row data writes, including the
first-entry distinction and bit `0x8000` writes to later entries. Its byte
cursor and explicit word/halfword casts remain. Null-pointer handling,
conditional extra-record advances, signed row/column comparisons, PAL
coordinate adjustments and row clipping retain their exact order and
behavior. New names do not add a valid-record assumption or normalize
unusual coordinates.

## Deliberate limits

`tent_Model_RebaseTexturedPrimitives` stays tentative overall. Actual
retail-package observations now tie the measured PAL slot-2 palette
conversion to its auxiliary upload, and distinguish unchanged high-row
palettes in the streamed image bank. The measured ordinary-model palettes
likewise agree with the loader/tint path. See
[primitive context and palette evidence](model-primitive-context.md).
The unexercised low-X branch and uncovered input/selector cases are not
promoted into observed behavior or a broader input-domain guarantee.

The existing `column_count`, `row_count`, and `index_offset` fields,
primitive-context types, ModelSlot accumulators and SDK declarations
remain unchanged. Both descriptor-length views retain their distinct
byte-pair versus scalar representations, sizes and offsets. The unrelated display-object
command table is not included to enlarge the bundle.

No synthetic/generated-C/source-string harness or new control is added.
Retained tooling/ABI policies remain unchanged; retired primitive-context
controls are not revived. Cloud/native witness suites and
regional/overlay image acceptance remain PR validation, not local execution.
