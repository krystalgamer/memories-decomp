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

## Scoped texture-state C views

`texture_state_semantic_views.h` names the three complete texture-state
owners through typed extern bindings to unchanged native symbols. It reuses
the canonical SDK image type, `DuelEffectImageInputs` and
`DuelEffectTextureTable`; it defines no storage, replacement layout or
absolute linker alias. Original native declarations remain in their
existing headers.

| C view | Native symbol | Bytes | NA address | JP address | UK / FR / ES address |
|---|---|---:|---|---|---|
| `gDuelEffect_TextureImages` | `D_8015A1E4` | 588 | `0x8015A270` | `0x80168270` | `0x8015A1E4` |
| `gDuelEffect_TextureModesAndVariants` | `D_8015A430` | 180 | `0x8015A4BC` | `0x801684BC` | `0x8015A430` |
| `gDuelEffect_TexturePageClutPairs` | `D_8015B748` | 84 | `0x8015B7E0` | `0x801697E0` | `0x8015B748` |

The dispatcher consumes 21 image records and 21 mode halfwords to build
the 21 packed page/CLUT pairs for nonnegative phase. Mode 20 overlaps the
low halfword of variant image zero's `pmode`; the same 180-byte owner also
contains five phase-selected images used by effect 11. The two views must
remain one overlapping union, not independent arrays or an invented
42-byte owner. No read is hoisted, cached or made conditional on a guessed
gameplay purpose.

The packed table retains both the complete `pairs[21][2]` view and its
48-byte named prefix. It is mutable, not a permanent copy of the input
images: effect 10 replaces slot 12's page word, and effect 13 rewrites the
first page word during its existing lifecycle. Tile selection retains its
original cast/index arithmetic. Named prefix fields, SDK packet fields,
all casts, load/store order and every caller's unchecked slot arithmetic
remain unchanged.

All twenty complete C consumers migrate only the three identifiers and
one added semantic include per file. All native headers, symbol maps,
generated-data owners, matching manifests and five function inventories
remain unchanged. Three scoped registry records identify actual NA data
addresses. These are mechanical table-role names, not texture artwork
names or claims that unobserved variants executed.

## Human texture observations (#418)

The NA texture trace accepted in #7865 captured 18 numbered episodes and
72,468 watched table reads, with observed execution breakpoints and no
foreign-module hits or foreign reads. The human action/handler associations
and capture limits are recorded in
[the dispatch note](duel-effect-dispatch.md#north-american-human-route-evidence-418).
The three tables retain their distinct owners: image records at actual NA
`0x8015A270` (native `D_8015A1E4`), overlapping modes/variant images at
`0x8015A4BC` (`D_8015A430`), and packed word pairs at `0x8015B7E0`
(`D_8015B748`).

These eight packed-word slots were read in that route. Horizontal image
coordinates and widths are VRAM **halfwords**, not texels; Y and height are
rows. The last column gives the decoded texel dimensions. These values
describe the captured table, not a promise that uploads or all future
routes retain this layout.

| Slot | Observed decimal effect IDs | Page / CLUT words | Image VRAM `(x,y,width,height)` | Texels / depth |
|---|---|---|---|---|
| 0 | `0,1,2,10,17` | `0x00B8 / 0x3C00` | `(512,256,32,64)` | `64x64`, 8-bit |
| 1 | `0,1,2` | `0x0038 / 0x3C40` | `(544,256,8,64)` | `32x64`, 4-bit |
| 2 | `1` | `0x00B8 / 0x3C80` | `(512,320,32,64)` | `64x64`, 8-bit |
| 3 | `1,2,10,17` | `0x0038 / 0x3C41` | `(552,256,16,64)` | `64x64`, 4-bit |
| 4 | `3` | `0x0038 / 0x3C42` | `(544,320,32,64)` | `128x64`, 4-bit |
| 9 | `2` | `0x0038 / 0x3C47` | `(568,256,8,64)` | `32x64`, 4-bit |
| 12 | `10` | `0x0038 / 0x3C4A` | `(568,448,8,32)` | `32x32`, 4-bit |
| 15 | `17` | `0x0079 / 0x3C4F` | `(600,256,16,64)` | `64x64`, 4-bit |

Slot 0's reading PCs were `0x80147E44` and `0x8015624C`; slot 1's were
`0x80148008` and `0x80154150`. The other reading PCs were respectively
`0x80148218`, `0x801482E0`, `0x8014AD20`, `0x80148AF4`,
`0x8014DC18` and `0x80159818` for slots 2, 3, 4, 9, 12 and 15.
Slot 9 was observed in all three direct-attack episodes (phases four/five),
not in this run's monster-attack episodes. That does not establish global
exclusivity.

These are **use associations, not artwork identities**. The human's VRAM
inspection recognized framebuffers and a large card preview, but did not
identify individual effect-slot images. An effect uses several shared
slots; seeing flames in an effect that reads slot 4 does not independently
identify every pixel or frame in that image. Mechanical table-role names
do not identify individual artwork; unconfirmed artwork names remain
unchanged. Slots absent from the read summary are not proven unused.

The repeated `variant0` rows are especially limited: every one was read at
dispatcher PC `0x8014633C`. Mode 20 overlaps the low halfword of variant
image zero's `pmode`, as represented by `DuelEffectImageInputs`. The
dispatcher initializes all 21 pairs for nonnegative phase, so this watched
read is not evidence that effect 11 ran or that variant image zero was
drawn. No effect 11 episode was observed.

## Atlas reachability remains unresolved

The separate atlas run in #7865 saw effects 1/0/14, phases 0/0/3, over
three completed episodes: 2,107 resident control hits and 152 guarded
dispatch calls. The atlas helper at actual NA `0x80148F98` had zero
calls, but so did its sibling control at `0x80148E88`. Neither this route
nor the other two human routes exercised effects 6 or 7, the sibling's
known static callers.

This is not a controlled negative result for either helper and does not
prove dead code, global unreachability or unused atlas artwork. The broad
atlas probe and its result were retired rather than requesting another
untargeted duel. A future probe first needs an actionable short route that
exercises the sibling, then must distinguish the atlas count from that
positive control. The existing static atlas-column interpretation is
unchanged.

The targeted follow-up in #7930 (`duel_effect_followups`, part A) supplied
that control. Hinotama ran effect 6 with phase 1. During it, the sibling
`DuelEffect_DrawFixedTextureQuad` was called 1,008 times: 360 from
`0x80155B90` with sizes 1..32 and 648 from `0x80155D24` with size 4. Over
the same 10-episode duel, which also covered effects 1, 2, 3, 17, 18 and
21, the atlas helper at `0x80148F98` was called zero times. Effect 7 still
has not been seen. This is controlled negative evidence for those routes
only. It does not prove that the atlas helper is dead code.
