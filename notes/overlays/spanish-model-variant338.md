# Spanish MODEL headers 338/488

Sixteen independently verified images reuse the accepted French symbol-renaming
wrappers and their shared header-321 rings/strand bodies without modifications.
Both helpers in each slot compile separately with the named GCC 2.8.1 / MASPSX
2.81 `gcc_2_8_1_g0_split` profile. Regional portability is demonstrated against
the legal Spanish images, not assumed from the source directory.

Models 164, 165, 210, 424 and 609 use stages 7/8; models 34, 443 and 459 use
stages 9/10. The [instance ledger](spanish-model-variant338-instances.csv)
records every compact record, sector, header, command and independent hash.
Slot loads are `0x8013B000` and `0x8017B000`.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xBA0` | 2972 | Unmatched assembly | Yes |
| `0xBA0..0x16D8` | 2872 | Unmatched assembly | Yes |
| `0x16D8..0x1B90` | 1208 | Rings C | Yes |
| `0x1B90..0x1ED4` | 836 | Strand C | No |
| `0x1ED4..0x270C` | 2104 | Unmatched assembly | Yes |

The strand is retained game code, not a reachable-entry claim or an exclusion.
Its boundary was separately seeded and every instruction in its interval was
checked, as were the other four spans. All have one terminal return and no
unresolved indirect jump. Entry calls only `0xBA0`, `0x16D8` and `0x1ED4`.
This does not establish every possible runtime entry point.

All 327,680 full-image bytes match. The 32 compiler-owned C instances contribute
32,704 instruction bytes; **48 function instances / 127,168 bytes remain
explicitly unmatched assembly**. Each four-byte header and **10,484-byte
unclassified suffix** at `0x270C..0x5000` retains a real raw storage owner.
No suffix bytes are classified as non-code merely to improve progress.

## Independent layout and ownership evidence

Thirty instruction anchors per Spanish image verify context capture, pointer
formation, descriptor indexing, strides and bounds. Forty-six target-compiled
constants check the canonical SDK and shared local layouts.

The two 152-byte rings start at context `0x1234` and end at `0x1364`.
Six 132-byte strand records follow, each exposing thirteen `SVECTOR` points;
their end is `0x167C`. The quad is at `0x1DE0`, line at `0x1EB4`, transform
at `0x1EC4`, target at `0x1EF4`, and visible-range halfwords at
`0x1F4A/0x1F4C`. Timing/state accesses agree with the shared declarations.

Entry selects a 52-byte configuration view at image `0x2808` plus the initial
command times 52. Each normal metadata request is decoded independently,
reduced modulo 1000 by the resident dispatcher, and checked inside its
owning suffix. Its start/end and fade intervals are positive. These views
do not establish a generic array capacity.

All 35 module-level resident callees have real selected input objects, sized
final function symbols and retail-identical bytes in a clean matching Spanish
resident. The initializer, controller and loader likewise have sized matching
C owners. Their context pointers at resident `0x80010024/28` resolve to
`0x80136000/0x80176000`. The pointer words belong to the selected
`spanish_raw_80010000.o` data subsection, even though its final `.main`
output section also contains executable code.

The final observed entry halfword at `0x1F7A` establishes a minimum 8,060-byte
view that does not overlap the selected model, primary or secondary loads.
The partial rings structure is 8,044 bytes. Neither observation proves a
reserved allocation capacity or whole-game lifetime isolation.

The [terminal attempt ledger](spanish-model-variant338-attempts.csv) identifies
each selected wrapper by its source hash. The existing bodies and their
earlier recovery history remain shared; no opaque instruction arrays,
fixed-register declarations or alternate compiler versions are introduced.
This partial family coverage is not exhaustive Spanish runtime completion.
