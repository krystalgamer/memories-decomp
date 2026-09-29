# Complete French duel effect 10

`func_8014C8FC` at `8014C8FC..8014D378` is a complete 2,684-byte
lifecycle, independently recovered from French instructions. Its first
candidate reproduced every function byte with the existing
`gcc_2_8_1_g0_split` profile and project headers. No reference-project
source, types or flags, forced registers or inline assembly were used.

All 63 accepted French bank entries remain unchanged. This independent
batch reaches 64/85 C functions / 25,628 bytes and 21 assembly boundaries;
configured French overlays contain 188 C instances / 81,480 bytes.
Pending registrations are excluded.

Integration through accepted master `42dd72c8` preserves all 71 accepted
French entries and this unchanged effect-10 source/header: **72/85 bank C
functions / 39,376 bytes**, 13 assembly boundaries, and **196 configured
French C instances / 95,228 bytes**. Accepted lifecycle bindings, owners,
evidence and tests are retained, with the common matrix getter deduplicated.
The implementation is also now accepted through the Spanish batch; all
game sources and build inputs equal master. The accepted authenticated CI
input repair is included without changing any retail hashes.

Integration through master `c125c88b` also retains accepted French effects
4/5 and Spanish effects 15/1. All 73 accepted French entries remain verbatim;
adding only effect 10 gives **74/85 bank C functions / 43,728 bytes**,
11 assembly boundaries, and **198 configured French C instances /
99,580 bytes**. No source redesign or pending-branch history is introduced.

## Exact image and real ownership

The candidate reproduced the complete 90,112-byte bank before promotion,
and the production link retains SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
All seven French images, all seven bank copies and 188 C owners remain
exact. All 30 bank C objects have complete inventoried extents; 19 named
overlay routines, including this entry, resolve to executable sections.

Three data owners retain exact generated-input and final-ELF types,
addresses, extents and retail bytes:

| Symbol | Bytes | Role |
| --- | --- | --- |
| `D_80146138` | 16 | Initial vector `{4096,4096,4096,0}` |
| `D_8015A8C8` | 588 | Separate 21-record `GsIMAGE` table |
| `D_8015AB14` | 84 | Six 14-byte configurations |

The image table equals the primary `D_8015A1E4` table byte-for-byte but
retains its own original storage. Entry 12 supplies the mode and coordinates
for `getTPage`; the result updates pair 12 of the already-owned
`D_8015B748` output table. It is not a new absolute alias at `8015B778`.
The new resident binding is the existing 12-byte matching
`Model_GetLightSourceMatrix` at French `8005C328`.

## Target layouts and bounds

Fifty-one target-GCC constants verify the configuration/work/image layouts
and array extents. The work size is `68C` hex:

| Offset | Field |
| --- | --- |
| `000` | Configuration pointer |
| `004` / `044` | Two four-vector rings / four-vector plane |
| `064` / `264` | 64 dissolve particles / 64 column positions |
| `464` / `4E4` | 64 speed / age halfwords |
| `564` / `566` | Ring height / stage |
| `568` / `56A` / `56C` | Color-ready / ring-ready / spawned halfwords |
| `570` / `574` / `578` | Frame / tick / cross-effect counter |
| `57A` / `57E` / `582` | Main / screen / ring colors |
| `586` / `686` | 64 column colors / plane color |

The deliberately unaligned color offsets are retained. The readiness check
reads the two adjacent halfwords as the original word value `00010001`.
The configuration contains color at 0 and halfwords at 4/6/8/A/C for height
step, maximum height, particle speed, draw mode and an uninterpreted field.
All offsets above are hexadecimal.

All six retail records use step 4, maximum height 128, speed 4 and final
field 90. Five modes are 1; the sixth is 2. Their colors are green,
pale yellow-green, gray, pale green, blue and light gray. With the resident
nonnegative `rand` result, computed dissolve speeds lie in 2..5.
All 21 image records are valid complete `GsIMAGE` records, and the accessed
index 12 is in bounds. Fixed particle counts and the capped column count
fit their 64-element arrays; each column has four layers of four vertices.

## Preserved lifecycle

Phases 0..5 select configurations; larger phases use the crossed-line
fallback. The two initial rings preserve the unsigned 16-bit height-offset
expression. Before dissolving, the plane and ring colors approach their
targets and the outer ring rises. Mode 1 additionally transitions the
plane color and uses its result for readiness.

Columns appear every fourth tick, capped at 64. Each cycles four textured
layers with alternating color/gray brightness and per-layer signed division.
Every vertex adds its column position, then the layer displacement, then
subtracts age. Ages advance by 12; after 160, colors fade and reset only
once zero. The quad width/height casts preserve the target sign extensions.

Readiness marks the canonical request but does not itself start the dissolve:
stage zero requires `phase < -1`. Mode 2 first resets the main color to 64.
The next updates draw and move all 64 dissolve particles while the main
color fades by 8 and the screen color fades by 15. Completion requires both
colors to be zero and stage one. Frame accumulates the captured resident
step; tick increments once per update.

No existing C, shared contract, other regional registration, compiler
profile or hash gate changes. Boot ownership, MODEL/SU loads and the
overworld tail remain unresolved; configured totals are not exhaustive
runtime completion.
