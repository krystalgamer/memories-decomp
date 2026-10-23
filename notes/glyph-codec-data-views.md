# Glyph lookup and decimal key data views

## One codec cohort, three retained views

| Original NA provider | Canonical view | Original C contract |
|---|---|---|
| `tent_GlyphLookupTable` (`0x801D9000`) | `gText_aGlyphLookup` | Mutable incomplete `TextGlyphLookupEntry[]` |
| `tent_GlyphLookupTableEntries` (`0x801D9004`) | `gText_aGlyphSearchEntries` | Const incomplete `TextGlyphLookupEntry[]`, independently labeled |
| `tent_DecimalDigitSjisKeys` (`0x80010330`) | `gText_DecimalDigitSjisKeys` | By-value `TextDecimalDigitKeyBlock`, twenty bytes |

The lookup entry remains the existing four-byte, word-aligned union:
packed word, low key halfword and upper metadata halfword. No new
record fields, bounds, alignment, allocation or data format is introduced.
The old headers, type definitions and declarations remain unchanged.

## Mutable base and const interior are not interchangeable C expressions

SJIS conversion searches from index zero or one, emits a matching
glyph index and retains its packed-word terminator/missing-key behavior.
Text building reads the same packed records with `0x8FF0FFFF`; the
channel pump retains its `upper & 7` metadata-kind read. Other upper
fields are not given guessed meanings.

The decimal initializer reaches the first search entry through a separate
const-qualified external label. NA/Japan providers place it at base
plus four. Both the qualifier and independent relocation are codegen
inputs: the const first-word guard hoists outside the digit loop.
It is not folded into `&gText_aGlyphLookup[1]`, and the mutable base is
not made const. The const view is not a new claim that physical storage
cannot be loaded or changed elsewhere.

The twenty-byte SJIS key block keeps its exact struct type and by-value
copy. Ten local two-byte keys are combined big-endian and searched with
the existing one-based match index. Pointer ordering, signed address
comparison, guard placement and partial initialization remain unchanged.
An empty table or missing key does not acquire a new default output.

The legacy NA storage symbol reports ELF size `0x18`, larger than the
twenty-byte copied C view. That original metadata/backing remains
untouched; the canonical alias preserves class/address without inventing
a size or classifying the remaining bytes. No storage is shortened to
match the caller's struct.

The converter's backward count scan, all-zero-input behavior, signed
count/index casts, single-byte/extended encoding, zero-on-miss path and
final `TEXT_STRING_TERMINATOR` store remain. No decoder, bounds check,
normalization or new fallback is added.

## Independent Japanese bodies and PAL boundaries

The Japanese decimal initializer and SJIS converter contain complete
implementations, not includes of the NA bodies. Both are explicitly
migrated and captured under their actual profiles. The paired old
declaration owners precede the narrow canonical headers.

The Japanese lookup/search providers remain `0x801D8600` and
`0x801D8604`, with key block `0x800102AC`. NA keeps `0x801D9000`,
`0x801D9004` and `0x80010330`. PAL keeps its existing lookup base
at `0x801D9000`, but has no runtime import for the const interior or
SJIS key block; no provider is invented for their retained declarations.

PAL digit initialization still decodes its separate terminated byte
string `D_801BF870`. Its existing extra helper `func_8003B758` increments
from the lookup base before comparing low keys, retaining its own
zero-key termination rather than the non-PAL full-word guard.
The string, function identity/prototype and all guards remain unchanged.

## Supplied NA data observations are not new bounds

The supplied executable contains twenty key bytes decoding to SJIS
`0x824F..0x8258`. Each appears uniquely before the observed packed-word
terminator in the inspected lookup window. Their glyph indices are
`56, 61, 58, 65, 74, 66, 78, 69, 87, 89`, not a presumed linear digit
range.

Record zero is zero; the next zero packed word in the bounded window
is record 92 at `0x801D9170`, before the next existing label
`0x801D9174`. These are observations of provided bytes, not a complete
allocation bound, universal regional count or interpretation of unknown
metadata. Both lookup declarations remain incomplete arrays.

## Declaration and provider conservation

Actual CPP/compiler objects cover eleven contexts and thirty-three
registrations across six roots. The canonical views retain sixteen
declarations: thirteen active imports and three unused views.
One other unused mutable-base declaration remains legacy-only in the
Japanese decimal unit; sixteen role/context absences remain absent.

All eleven active image/role bindings use their original tracked
providers. Seven resident linker files keep their accepted prefixes;
no numeric address, storage or ownership is guessed. No selected data
owner or transitive overlay C consumer exists in the audited closure.

All old headers/types, source groups, compiler profiles, wrappers,
existing tests and ownership controls remain unchanged. No new harness,
fixture, probe or testcase is introduced.
