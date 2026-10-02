# Spanish MODEL headers 424 and 574

Thirty independently extracted Spanish secondary images select the unchanged
local MODEL424 petals wrappers. They contain 29 distinct complete images.
Every 20,480-byte image matches its own retail slice. The entry remains genuine
generated assembly and the suffix remains unclassified raw storage.

## Instances and ownership

Stages 7/8 select models 68, 96, 186, 297, 376 and 595. Stages 9/10 select
165, 242, 294, 352, 358, 399, 465, 520 and 621. The
[instance ledger](spanish-model-variant424-instances.csv) records each model,
compacted record, stage, slot, sector, command and complete-image hash.
Identical images are not collapsed: their actual selection commands can differ.
The [attempt ledger](spanish-model-variant424-attempts.csv) identifies each of
the 30 exact results, including wrapper and shared-body fingerprints.

| Image-relative span | Selected owner | Bytes per image |
| --- | --- | --- |
| `0..4` | Raw header | 4 |
| `4..12D8` | Generated entry assembly | 4,820 |
| `12D8..18B4` | Petals C | 1,500 |
| `18B4..5000` | Unclassified raw suffix | 14,156 |

The named profile is `gcc_2_8_1_g0_split`: authoritative GCC 2.8.1 and MASPSX
2.81. The existing `VERSION_FRENCH` wrapper selects expression/declaration
ordering that independently compiles exactly against Spanish bindings; it is
not an assumption that resident ABI or dependencies match between releases.
No C, header, compiler profile or French registration changes are needed.

The 30 images cover 614,400 bytes: 45,000 compiler-C bytes, 144,600 generated
instruction bytes and 424,800 raw bytes. Per-image input objects, selected
linker sections, final ELF symbol sizes and retail ranges establish the actual
owners. All 1,205 generated entry words per image were checked independently.
Both function CFGs are contiguous and complete; the entry's only local callee
is petals at `+0x12D8`, and the helper has no local calls.

## Loader and resident evidence

All 33 bindings were independently recovered from accepted Spanish metadata.
A fresh exact Spanish resident build verifies their actual selected objects,
four loader/dispatcher owners and eight pointer-storage words, not merely
labels or a whole-image hash. The resident SHA-256 is
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.

The selected loader at `0x8005967C`, transfer callback at `0x80059EF4`, slot
setup at `0x8004FC2C` and dispatcher at `0x80058B4C` establish both alternates.
The loader stores `p4 != 0` in slot `field_DFE` when `p4 >= 0`, then copies
that selector to transfer `position`; `callback_data` carries the slot index.
The callback phase counts are
`(96,48,2,1,16,1,16,10,10,10,10,2,2,1,50,1)`, totaling 276 sectors.
Stages 7/8 select alternate zero and stages 9/10 alternate one, at record
sectors 180/190/200/210 respectively. Skipped phases still consume sectors.

Load-pointer storage `0x80010014/18` selects `0x8013B000/0x8017B000`.
Context-pointer storage `0x80010024/28` selects `0x80136000/0x80176000`.
The first phase and primary-module pointer owners were also checked.
Final record-sector words `+0x110/+0x114` reach slot command words
`+0xD08/+0xD0C`; the dispatcher selects `commands[field_DFE]`. Initialization
receives the actual command modulo 1,000 and updates receive minus one.

Actual commands are 590000, 590001, 590002 and 590004 through 590013.
The descriptor address is image `+0x19B0 + (command % 1000) * 68`, inside
retained raw storage. Petals reads radius as unsigned halfword `+0x1E`,
step as unsigned halfword `+0x20`, and duration as unsigned word `+0x28`.

## Layout, lifetimes and gates

A fresh target compilation verifies 57 constants in 228 read-only bytes,
including canonical primitive, pointer, SDK matrix/coordinate/packet and
independently recovered accessed-view layouts.

| Context offset | Accessed storage |
| --- | --- |
| `0/180/300/480/600` | Five arrays of 48 eight-byte SVECTORs |
| `780` | RGB bytes |
| `794/854/914/9D4` | Four 48-dword lanes: scale, done, unresolved, reset |
| `2320` | Stable 36-byte POLY_G4 |
| `2380` | Position words |
| `2398/239C` | Angle-input words |
| `23A4/23A6` | Angle-input halfwords |
| `23A8/26A8` | 48 initial-position and 48 delta sixteen-byte records |
| `29C4/29CC/29D4/29FC` | Frame, frame step, descriptor pointer, phase |

The entry initializes all five vector arrays; the helper projects only the
first four. Entry context accesses establish a minimum view of `0x2A10`,
not an allocation size or whole-game lifetime-isolation claim. That minimum
does not overlap the selected first-phase, primary or secondary load spans.

Regressions freeze 20 complete preserved-register write sets per image:
`s0..s7`, stack and frame pointers across both functions. The entry frame is
248 bytes; its command home at `sp+252` is a caller argument home, not an
out-of-frame local. The helper frame is 336 bytes.

Original context reaches entry `s2` at `+0xC` and `s6` at `+0x14`.
Initialization later reuses `s2`; a delay-slot-aware reaching-definition walk
proves the original definition reaches the helper call at `+0x1170`, whose
argument move is at `+0x1174`. There are 74 entry and 16 helper call sites.
The helper keeps original roots in `s1/s3` and packet pointer in `s6`.
Projection output pairs occupy packet `+8/+16/+24/+32`, and direct RGB stores
occupy `+4/+5/+6`. Stack `p` and flag are separate at 224/228; the saved
ordering-table pointer at 232 and matrix pointer at 288 remain intact.

Drawing requires positive scale, nonnegative depth and projection flag, and
a zero done lane. Sorting narrows depth to its low 16 bits; it does not clamp
negative depths or clear projection flags. Scale above 512 fades RGB using
`(1024-scale)/512`. The descriptor step times frame step is shifted right
logically by one. Duration/frame comparisons are unsigned. Completion or
recycling updates each lane, and the product of all 48 done lanes advances
phase two to five. The fifth initialized vector array is not a fifth quad
projection input.

## Reproduction

Use the legally supplied Spanish inputs and the existing targets:

```sh
MAKEFLAGS=-j4 make spanish-match spanish-match-overlays
tools/environments/python/bin/python -m unittest \
  tools.project.tests.test_spanish_model_variant424
```

The fixture covers all physical slices, both loader alternates, actual command
words, metadata selection, wrapper/ledger fingerprints, raw extents, complete
CFGs, context dataflow, register writes, array bounds, packet fields and gates.
The existing independent ILP32 loader oracle additionally exercises stage
selection and rejects an actual-source mutation. No raw retail payloads or
local proof artifacts are tracked, and the 206 previously registered Spanish
images are preserved.
