# Spanish MODEL variant 337/487 ring helper

These six independent ten-sector images share the 1,216-byte ring helper at
offset `0x1278`. Slot zero loads at `0x8013B000`; slot one at `0x8017B000`.
Each slot has its own GCC 2.8.1 / MASPSX 2.81 compilation under the named
`gcc_2_8_1_g0_split` profile. Each full image is separately linked and hashed.

| Model | Compact record | Stages | Archive sectors | Normal command |
|---:|---:|---|---|---:|
| 110 | 110 | 9/10 | 30560/30570 | 1 |
| 159 | 159 | 9/10 | 44084/44094 | 4 |
| 410 | 360 | 7/8 | 99540/99550 | 7 |

Header families span loader stages: model 410's stages 9/10 are a different
family and are not registered by this change. None of these six images is
treated as an identical-image duplicate.

## Boundaries and preserved scope

| Offset | Bytes | Status |
|---|---:|---|
| `0x4` | 2464 | Game-owned unmatched assembly |
| `0x9A4` | 2260 | Game-owned unmatched assembly |
| `0x1278` | 1216 | Matching C |
| `0x1738` | 2084 | Game-owned unmatched assembly |

Direct-call traversal reaches every instruction in these contiguous
intervals, with one return per function and no unresolved indirect jump.
This does not prove the absence of other runtime entry points. The header
and complete 12,452-byte suffix at `0x1F5C..0x5000` retain real generated
storage. The suffix remains **unclassified**, not excluded code or C coverage.
Only six function instances / 7,296 instruction bytes are added as C; the
other eighteen function instances remain explicitly unmatched.

## Local layout and behavior evidence

Two 152-byte ring records start at context `0x3A4`. Each contains four rows
of four canonical `SVECTOR`s, two `CVECTOR`s at offsets 128/132 and a signed
scale at 136; its final twelve bytes remain unknown. The helper reuses a
single canonical `POLY_GT4` at `0xC38` for four projected quads per ring.
Three vertices receive the second color and the fourth the first. Negative
depth or projection flags suppress submission; depth is multiplied by eight
and divided by ten in that order.

The entry passes context `0xCCC` to `GsGetLwUnit`, establishing a canonical
`MATRIX` whose translation starts at `0xCE0`. Its end is the next `SVECTOR`
at `0xCEC`, not an overlapping guessed sixteen-byte position vector.
The first ring uses that translation and the second the target vector.
Their rotations differ by a half turn about Y and the sign of Z rotation.
The shared angle advances by `step * 64` once per ring.

Odd frames add scale divided by eight to each scale component. The first
ring grows to 4096 using unsigned elapsed/configuration interpolation, then
shrinks during state three. The second grows to 8192 at `step * 1024` in
state two and shrinks at `step * 64` in state four. The observed clamps and
state transitions are retained, including unsigned division semantics.

The entry computes its configuration pointer as image base plus `0x2058`
plus command times 36. The final metadata sector of each ordinary model
record supplies requests 503001, 503004 and 503007. The resident's secondary
dispatch passes each request modulo 1000, selecting configuration views
1, 4 and 7 inside the real suffix owner. This establishes those load paths
and access windows, not global array capacity or exclusive runtime use.

Forty-nine target-compiled size/offset values verify the local and SDK
layouts. The partial state size `0xD70` is not caller allocation proof.
All nine helper callees have real selected resident input objects and sized
function symbols whose final bytes match the legal Spanish executable.
No inline assembly, fixed-register declarations or synthetic instruction
storage is used. The two exact source experiments are recorded in
[`spanish-model-variant337-attempts.csv`](spanish-model-variant337-attempts.csv);
fingerprints cover the scratch source/header pair before include-path and
type-prefix promotion. The regional manifests follow existing overlay
integration conventions; the resident-only integrator is North-American-only.
