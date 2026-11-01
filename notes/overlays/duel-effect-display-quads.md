# Duel-effect display quads and gradient bands

## Scoped display C views

The complete three-function object exposes the following C names through
`display_semantic_views.h`; the symbol files bind them in place of the
former labels. The original `display_quads.h`, SDK records, grouping, definition
order and compiler profile remain unchanged.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `DuelEffect_DrawQuadrantTileQuads` | `func_801573A8` | `0x801495F0` | `0x80165438` | `0x801573A8` |
| `DuelEffect_DrawTranslatedTileRect` | `func_80157494` | `0x801496DC` | `0x80165524` | `0x80157494` |
| `DuelEffect_DrawGradientConnectionBands` | `func_801575CC` | `0x80149814` | `0x8016565C` | `0x801575CC` |

Effect 13 calls the quadrant helper for its background pass; effect 21
calls the translated rectangle with a per-slot tile value; effect 24
connects five pairs of target XY prefixes with the gradient helper.
The names describe these complete drawing bodies, not artwork, gameplay
animation identity, runtime coverage or world/screen-space dimensions.

The quadrant helper retains page/CLUT offsets `0x1C/0x1E`, fixed UVs
`(128,128)..(191,191)`, and four ordered generator/projector attempts for
indices zero through three. Every call forwards the original signed depth
and mode one; no acceptance result gates the next quadrant.

The rectangle retains page/CLUT offsets `0x20/0x22`, U endpoints
`128 + tile * 32` and `159 + tile * 32`, and V endpoints 192/255. Width
spans negative to positive X; height spans negative Y to zero. Keep direct
`drawing.vertices` initialization and projector arguments distinct from
pointer-relative `addVector` accesses. Both fixed projector arguments are
one. The caller cycles tiles modulo three, but the helper does not bound or
normalize its `u16` tile argument before the original byte UV stores.

The gradient helper creates two G4 bands, each with black outer vertices
and input-color inner vertices. Both offsets are along Y, not calculated
perpendiculars to the endpoint segment. Keep both ordered power calls per
band, their explicit `s16` narrowing, zero local Z, ignored projection
depth and independent flag-only acceptance. Accepted bands use the common
offset-quad submitter with mode one. The endpoint `DVECTOR` parameters
describe only the observed XY prefixes, not entire caller-owned records.

All four complete owner/caller sources retain their accepted-parent bodies
after reversing only the three names and removing one added include per
file. Actual named-profile GCC/MASPSX output, native symbols and
disassembly/relocations remain identical. The five bank inventories change
only fifteen name cells; three scoped records append at actual NA
addresses without changing earlier registry bytes. Native headers, tests,
data owners and all unrelated metadata remain unchanged. Full-image and
regional acceptance uses ordinary project/PR gates.

## Original matching evidence

`display_quads.c` owns the complete contiguous interval
`0x801573A8..0x80157794`, using the unchanged `gcc_2_8_1_g0_split` profile.

| Function | Bytes | Recovered behavior |
|---|---:|---|
| `func_801573A8` | 236 | Generate and submit four indexed quads using a fixed texture tile, supplied color, size and signed depth. |
| `func_80157494` | 312 | Construct a texture tile between negative height and zero, translate all vertices, and submit with depth/flags one. |
| `func_801575CC` | 456 | Draw two gradient bands around endpoint XY coordinates, alternating the signed width offset with the existing power helper. |

The first and third functions matched on their first candidates. The second
also had the correct size, but 42 instructions differed: pointer-relative
initialization changed store bases, color-store scheduling and the height
negation position. Separately negating the height did not resolve them.
Initializing `drawing.vertices` directly and passing that direct array to the
projection helper, while retaining pointer-relative `addVector` updates,
matched all 312 bytes. The three distinct experiments remain in the local
source/header and instruction-diff ledger.

The first two routines reuse the established 72-byte `DuelEffectTexturedQuad`
stack record and SDK setters. The texture view exposes observed page/CLUT
words at `0x1C/0x1E` and `0x20/0x22`, leaving `0x10..0x1B` and `0x24..0x27`
unknown. Earlier page/CLUT fields and the accepted `0x28..0x2E` fields retain
their exact offsets. Storage remains generated data, not a C definition.

The third routine reads only each endpoint's two halfwords at offsets 0/2;
`DVECTOR` describes that XY prefix, not the complete caller-owned record.
Callers in `func_8014D3E8` pass several subrecords separated by eight bytes.
Their unused trailing fields and complete record ownership are not inferred
from these calls. Local projected vertices have Z explicitly zeroed. Both
power results are narrowed to `s16` before multiplication, and only a
negative projection flag suppresses submission.

An independent private full-bank link first verified these three routines
against the accepted 43-function baseline: 46 C functions / 10,264 bytes.
The unpublished gradient/projection batch was then combined before any PR
was created. The combined seven-function addition is 2,332 bytes and retains
every accepted entry: 50 C functions / 11,592 bytes, with 35 explicit assembly
boundaries; configured Spanish overlays contain 174/209 matches / 67,444 bytes.
The complete 90,112-byte bank retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Production checks additionally cover every terrain copy, full module images,
compiled-object/linked-ELF function extents and real generated-data ownership.
No partial number-renderer candidate, unmerged dependency, new compiler flag
or inline assembly is included.
