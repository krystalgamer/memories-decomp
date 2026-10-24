# Duel-effect textured quads

The contiguous functions at `0x80156C40..0x80156E58` are two textured-quad
helpers totaling 536 bytes: 272 and 264 bytes respectively. Their
complete source group uses the unchanged `gcc_2_8_1_g0_split` profile.

Both construct a SDK `POLY_FT4` followed by four SDK `SVECTOR` records on the
stack, a 72-byte aggregate. They initialize the vertices through the existing
size helper, translate each vertex by the caller's vector, and call the
preserved projected-quad routine at `0x80151218`. Neither routine defines
module storage or changes the projected helper's implementation.

The first uses caller-supplied RGB and a fixed `64 x 64` texture rectangle,
UV `(192,128)..(255,191)`. The second uses RGB `(128,128,128)` and a
32-pixel-wide atlas column selected by the unsigned-halfword index, with
UV row `0..31`. The texture-page/CLUT words come from four consecutive
halfwords at `D_8015B748 + 0x28..0x2E`. The private header is a view of that
observed prefix, not an allocation-size or C data-ownership claim.

## Recovered expression grouping

The existing SDK `addVector` macro expands the three field updates into a
comma expression. This matters to GCC 2.8.1's common-subexpression and loop
induction handling; three separate C statements did not produce the same
code, even when function sizes were correct.

| Candidate | Result |
|---|---|
| Separate polygon and vertex locals with direct field updates | Extra live array base changes allocation and saves. |
| Combined stack record and separate indexed updates | Correct 272/264-byte sizes, but 14 differing words per function. |
| Per-vertex pointers or mixed indexed/pointer updates | Adds four or eight bytes and changes loop induction setup. |
| Combined record with the existing SDK `addVector` macro | Both full instruction ranges match; complete 90,112-byte bank link and exact C symbol extents match. |

No register annotations, inline assembly, manual scheduling barriers or
one-off compiler flags were introduced. The owning header contains the
record declarations, following the repository's no-C-file-type-definitions
policy. The local projected callee and texture data remain real generated
assembly/data definitions, not absolute linker aliases hiding missing code.

The private proof was run independently against the accepted 15-function
Spanish baseline, replacing only this pair while preserving all other raw
bytes. Production matching likewise checks every complete terrain copy and
all seven configured Spanish module images. These results do not assert
that every runtime function, or even the complete duel bank, is decompiled.
After additive integration with the four accepted digit/primitive helpers,
Spanish has 21 matching bank functions / 4,084 bytes and 64 provisional
assembly functions; all 19 previously accepted entries remain unchanged.

## Independent regional proofs

The same unchanged pair also passes private complete-bank links in English
PAL, North America and Japan, together with the earlier 15 shared helpers:
17 actual C definitions / 3,120 bytes in each image. Every linked definition
has its expected function type, size, address and executable-section
ownership. All seven terrain copies in each release are equal to that
release's rebuilt representative.

| Release | First quad | Second quad | Texture-word prefix | Projected callee |
|---|---|---|---|---|
| English PAL | `0x80156C40` | `0x80156D50` | `0x8015B748` | `0x80151218` |
| North America | `0x80148E88` | `0x80148F98` | `0x8015B7E0` | `0x80151DF0` |
| Japan | `0x80164CD0` | `0x80164DE0` | `0x801697E0` | `0x8015F2AC` |

The data and local-callee bindings were recovered from the corresponding
retail HI/LO and JAL operands, checked for consistency across occurrences,
then verified by the complete-image comparison. They were not guessed from
a uniform regional address shift: the North American quad pair appears
before the utility groups rather than after them. Regional link order must
follow those measured addresses.

The complete images retain the independently measured regional hashes in
the [bank research](duel-effect-bank.md#regional-presence-and-spanish-integration):
90,112 bytes for English PAL and North America, 98,304 for Japan. These are
private portability proofs, not registered production builds or claims of
complete regional C coverage. Unmatched code and data remain raw fallback.

## Typed texture-word and quad views

`texture_semantic_views.h` names the complete texture-word/quad component
without modifying the original native headers, generated storage, linker
symbols, compiler profiles or grouped source extents. Its declarations use
the existing SDK `GsIMAGE` and `SVECTOR` types and bind to the original native
function symbols; they introduce neither wrappers nor machine exports.

| C view | Native binding | Evidence and limits |
|---|---|---|
| `DuelEffect_PackTexturePageClut` | `func_8014F564` | Writes a page word from the original image/mode expressions, followed by a CLUT word. Dispatch fills the 21-pair table; the tile effect fills its local pair. No image upload or allocation is performed. |
| `DuelEffect_PackFixedPageClut` | `func_8014F5D0` | Effect 11 supplies variant images. The page word retains variable image-depth bits and fixed low bits `0x3E`; the whole word is not constant. The CLUT word still comes from image coordinates. |
| `DuelEffect_DrawFixedTextureQuad` | `func_80156C40` | Trail callers supply RGB, size and vector offset. The fixed part is the UV rectangle/table pair, not the caller-sized geometry. It projects with bias `0`, mode `1`. |
| `DuelEffect_DrawAtlasColumnQuad` | `func_80156D50` | The complete body selects the next table pair, neutral RGB and a 32-wide UV column, then projects with bias `1`, mode `1`. No caller was found in the owning bank's C sources; runtime reachability is not asserted. |

The packers keep the exact halfword views, masks, signed shift grouping,
store order and final narrowing. The quad pair keeps the 72-byte stack
aggregate, local/definition order and SDK `addVector` comma expression.
The unsigned-halfword size/index domains and byte UV narrowing are unchanged;
the atlas index remains unchecked and may wrap its UV bytes. These views
add no bounds check, animation interpretation or gameplay claim.

All five configured bank inventories (NA, JP, UK, FR and ES) use the new C
names at their measured local addresses. Only the four NA overlay addresses
are appended to the scoped semantic registry: `0x8015013C`, `0x801501A8`,
`0x80148E88` and `0x80148F98`, respectively. Native suffixes are not physical
NA addresses. The complete three-function `texture_words.c` object retains
the already accepted `DuelEffect_PowS16` unchanged. The dispatch real-owner
regression recognizes the canonical packer call alongside native callee
names: it still requires exactly 25 distinct callees and checks their actual
inventory status. The canonical packer row must retain the duel-effects
module, regional address `0x8014F564` and size `0x6C`.
Both the canonical and original `func_8014F564` names are
forbidden as absolute linker aliases. All six data-owner checks and the
complete-pair, SDK/ABI and curve-fingerprint controls remain unchanged.
