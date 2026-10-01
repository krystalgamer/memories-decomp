# Spanish MODEL headers 338/488

Sixteen independently verified images reuse the accepted French symbol-renaming
wrappers and their shared header-321 ribbon/rings/strand bodies without modifications.
All three helpers in each slot compile separately with the named GCC 2.8.1 / MASPSX
2.81 `gcc_2_8_1_g0_split` profile. Regional portability is demonstrated against
the legal Spanish images, not assumed from the source directory.

Models 164, 165, 210, 424 and 609 use stages 7/8; models 34, 443 and 459 use
stages 9/10. The [instance ledger](spanish-model-variant338-instances.csv)
records every compact record, sector, header, command and independent hash.
Slot loads are `0x8013B000` and `0x8017B000`.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xBA0` | 2972 | Unmatched assembly | Yes |
| `0xBA0..0x16D8` | 2872 | Ribbon C | Yes |
| `0x16D8..0x1B90` | 1208 | Rings C | Yes |
| `0x1B90..0x1ED4` | 836 | Strand C | No |
| `0x1ED4..0x270C` | 2104 | Unmatched assembly | Yes |

The strand is retained game code, not a reachable-entry claim or an exclusion.
Its boundary was separately seeded and every instruction in its interval was
checked, as were the other four spans. All have one terminal return and no
unresolved indirect jump. Entry calls only `0xBA0`, `0x16D8` and `0x1ED4`.
This does not establish every possible runtime entry point.

All 327,680 full-image bytes match. The 48 compiler-owned C instances contribute
78,656 instruction bytes; **32 function instances / 81,216 bytes remain
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

## Entry-called ribbon follow-up

The newly accepted local French338 wrapper is a starting candidate, not proof
of Spanish portability. Its existing indexed terminal-point and next-point
forms compile to exactly 2,872 bytes in both Spanish slots with zero differing
words. No C body, header, compiler profile or North American default path changes.
The two new terminal records retain all four previous ledger rows unchanged.

Six fresh compiler objects reproduce all sixteen complete images, including
all 32 retained rings/strand instances. The net addition is sixteen ribbons /
45,952 instruction bytes. Ninety-five freshly target-compiled constants
(380 bytes of read-only data) cover the previous 46 layout checks and 49
ribbon/packet checks. Each image independently passes 143 ribbon instruction
anchors, plus separately rerun retained/context-lifetime anchors; these
overlapping sets are not a claim of that many unique instructions.

Five 932-byte records cover context `0..0x1234`, ending at the rings.
Seventeen-point arrays start at `a=0`, `sa=0x88`, `angle=0xCC`, `b=0x110`,
`sb=0x198`, `width=0x1DC`, `otz=0x2D8`, `flag=0x31C`, `ox=0x360`,
and `oy=0x382`. RGB occupies `0x220..0x222`; count is signed sixteen-bit
at `0x23A`. Entry copies three descriptor color bytes and initializes
count to `-index * 16`. The fourth color byte and opaque intervals do not
gain semantic ownership from these accesses.

The negative-command branch at `0x68` reaches `0x84C` without the
initialization-only reuse of `s3`. Entry preserves that original context
through argument setup at `0x9DC` and the ribbon call at `0x9F4`.
The helper holds context in `s8`; all record-cursor writes, packet
increments/decrements and stack-pointer writes are independently checked.
Its 320-byte frame contains rotation `40..48`, scale `48..64`, matrices
`64..96` and `96..128`, coordinate `128..208`, projection outputs
`208..216`, ordering-table pointer `216..220`, and saves `280..320`.

Two 40-byte FT4 packets cover `0x1E14..0x1E64`; the streamer's separate
packet starts at `0x1E64`. Frame parity and segment index alternate the
packet pointers. Only RGB and eight coordinate halfwords are directly
written, ending at packet byte 35. Drawing requires positive phase
`0x1F68`, and sorting requires both signed depth and flag nonnegative,
using the low sixteen depth bits. The first-segment and terminal-segment
conditions remain separate, including their overwriting coordinate stores.

Count advances by unsigned `(step * 3) >> 1`, clamps at sixteen, and
the first record can advance phase one to two. In phase three, positive
displacement `0x1F4E` decays using unsigned clock `0x1F2C` and descriptor
words `0x2C/0x30`, then clamps to zero. Every actual Spanish command and
52-byte descriptor is checked, including its positive fade denominator.
The two wave clocks advance by `step * 850` and `step << 7` even when
drawing is disabled.

Thirty static calls resolve to eleven distinct actual resident callees.
Fresh ownership checks cover all 35 module bindings, three matching callers,
and both selected context-pointer data owners. Established `RotTransPers`
and `ratan2` aliases replace address labels at the same independently
verified addresses in all sixteen symbol maps and the shared binding file.
The helper's direct view ends at `0x1F6C`; the entry minimum remains
`0x1F7C`. Neither proves allocation capacity or whole-game lifetime isolation.

The accepted cutoff `d9ed7c1d6036cf33b2f17e8ce89d998e307a5032` contains
the prior report but no pending matching branch. This batch brings the
fixed-cutoff Spanish inventory to 894/1,082 C instances / 996,148 bytes
across the same 154 configured images. Entry, streamer and unclassified
suffixes remain explicit; report refreshes are separate.
