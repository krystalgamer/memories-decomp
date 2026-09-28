# Duel-effect display quads and gradient bands

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
