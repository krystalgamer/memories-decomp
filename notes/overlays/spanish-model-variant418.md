# Spanish MODEL headers 418 and 568

Two independently verified MODEL410 images reuse the unchanged accepted
`variant401_{spokes,rings,quad}.c` implementations through the existing
French `variant418_*` wrappers. The independently verified
`variant418_rays*` wrappers reuse the accepted local `variant433_rays.c`
with its measured MODEL418 tail offset and flag grid.
The accepted standalone `variant418_sheet*` sources independently match both
Spanish sheet helpers, preserving the `G32` guest timing-pointer storage.
Ten compiler-owned C instances cover 10,408
instruction bytes under the named `gcc_2_8_1_g0_split` profile, using GCC
2.8.1 and MASPSX 2.81. Source family401 is provenance, not Spanish identity.
No shared implementation, declaration, compiler profile or SDK type changed.

## Loader and boundaries

Model410 maps to compact record360. Stages9/10 select sectors200/210 of
the 276-sector record, loading ten sectors at `0x8013B000`/`0x8017B000`.
The [instance ledger](spanish-model-variant418-instances.csv) records the
actual slices, independent complete hashes and observed request584002.
The matched resident controller calls module offset4 with its context and
the initial command, then update command-1.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..FE0` | 4060 | generated assembly | yes |
| `FE0..1760` | 1920 | generated assembly | yes |
| `1760..1C9C` | 1340 | sheets C | yes |
| `1C9C..2204` | 1384 | generated assembly | yes |
| `2204..272C` | 1320 | rays C | yes |
| `272C..2A3C` | 784 | spokes C | no |
| `2A3C..2DB8` | 892 | rings C | no |
| `2DB8..311C` | 868 | quad C | no |

Strict control-flow walks cover every instruction in all eight spans.
The sheet and ray helpers are directly entry-called; spokes, rings and quad remain
module-local code without demonstrated entry-call paths.
Six assembly instances (14,728 bytes) remain untranslated. Both
four-byte headers and 7,908-byte suffixes have real storage owners; the
suffixes remain unclassified, not established non-code.

## Layouts and ownership

Entry preserves the context through `a0 -> s3 -> s6`. Forty-three actual
Spanish instruction anchors establish two 152-byte sheets at `+0x11DC`,
six 144-byte rings at `+0x130C`, four 144-byte spokes at `+0x166C` and
one 144-byte quad at `+0x18AC`, including advances and initialization
bounds. The spokes timing input is the first sheet's size at
`0x11DC + 0x88 = 0x1264`. Rings use color offset128, spokes offset132.

Ninety-two freshly target-compiled constants verify these local record
types, their accessed fields, and canonical SDK layouts for `SVECTOR`,
`VECTOR`, `MATRIX`, `GsCOORDINATE2`, `GsOT`, `GsGLINE`, `POLY_G4` and
`POLY_GT4`, including target pointer and integer widths. The constant
array is a size-zero NOTYPE label in a complete 292-byte `.rodata`
section in the original proof. The extended 368-byte `.rodata` owner
retains all 73 values and adds sixteen ray-record and three flag-array
values; section extent, every array label and all values are checked.

Seven additional Spanish anchors establish descriptor base`+0x3218`,
stride48 and the saved pointer at context`+0x1AF4`. The actual request
selects command2, a 48-byte window at image`+0x3278`, within the suffix
owner. Direct entry accesses require at least `0x1B2C` context bytes.

Fresh exact Spanish resident proofs establish all 36 real callee input
and final owners, three matching initializer/controller/loader owners,
and both context-pointer storage owners at `0x80010024/28` in
`spanish_raw_80010000.o` input `.data`. That input data resides in mixed
executable output `.main`; output flags do not erase its input ownership.
The pointers select `0x80136000/0x80176000`. Their minimum accessed views
are disjoint from the selected 96-sector MODEL, two-sector primary and
ten-sector secondary loads. This is not an allocation-capacity or
whole-game lifetime-isolation claim.

## Exact matching and limits

The [attempt ledger](spanish-model-variant418-attempts.csv) records all ten
unchanged canonical source matches. All three retained helpers were
freshly recompiled after detecting changed shared-header fingerprints;
historical objects were not relabeled with new source hashes.
Actual links reproduce both complete
20,480-byte images without masking, selecting sized compiler functions,
explicit assembly fallbacks and real header/suffix storage. Dependency
fingerprints, actual resident symbols and selected input sections are
checked separately from image hashes.

The regional regression fixture reuses the French boundary/source tests
and adds Spanish fallback-binding completeness, descriptor selection and
minimum-context checks. All previously accepted Spanish registrations
remain unchanged. This branch adds two C instances / 2,680 bytes, yielding
812/1,082 C instances and 824,124 bytes across 154 configured images at its
accepted cutoff, preserving accepted MODEL418 and MODEL433 rays. Pending
web work is not included or stacked.
Maintainer acceptance is tracked separately. Progress snapshots stay separate. Further game-owned
code, unknown suffixes and exhaustive runtime coverage across all seven
releases remain open.

## Entry-called rays

Sixteen 168-byte records occupy context `[0,0xA80)`: nine eight-byte points
at offset0, nine four-byte color lanes at72 and nine signed-word progress
values at124. Ranges `[108,124)` and `[160,168)` remain opaque.
Initialization uses progress `-256*j`, red/green `(-64 - 24*j) & 255`,
blue255 and the measured record/element strides.

The negative-command update path calls rays at `+0xE78`. The earlier
timer branch rejoins at `+0xE4C`, before the phase gate. Positive phase
first calls the unmatched strip helper, then reloads phase; rays require
the resulting signed phase `>= 2`. Nonpositive phase skips the strip
and ray calls. Initialization jumps past rendering. These are local
gate facts, not a claim that every entry path reaches this section.

The fixed 20-byte line packet occupies `+0x1A78..+0x1A8C`; projection
words are at packet offsets4/8 and colors at12..17. Eight segments use
adjacent point pairs, each supplied twice to `RotTransPers4`.
The 928-byte frame separates coordinate `[128,208)`, the 576-byte
`PSXLONG flag[16][9]` grid `[208,784)`, three nine-byte color arrays
starting784/800/816, projection output `[832,836)`, original context
`[836,840)` and ordering-table pointer `[840,844)`. Saved registers begin888.
The grid address is `sp + 208 + 36*i + 4*j`; sixteen rays and eight
segments use 128 distinct words, ending at780 without reaching the colors.

Retail reads stack word864 at `+0x22C4`, before the first possible writes
at `+0x23B0/+0x23B8`, and stores it into unused scale words48/52/56.
No stack load reads that scale storage. The unchanged source preserves
this behavior rather than inventing an initialization. A further 122
ray-related instruction checks per image establish the gate, frame,
packet, live context, grid address and record advancement. The direct
ray context minimum is `+0x1B2A`, below the entry minimum `+0x1B2C`.

All thirteen static call sites resolve to nine independently checked real
resident functions. `rcos` and `ratan2` now name the verified existing
160-byte and 372-byte SDK owners at `0x800866F8` and `0x80089928`.
All 36 binding addresses and SDK classifications are unchanged; none
of the ten compiler objects references the replaced address-based
alias names. Shared ray regressions exercise both releases' actual
archives, with additional Spanish packet/context/flag-grid bounds checks.

## Entry-called sheets

The negative-command update path calls sheets at `+0xE3C`, before the
unmatched web helper, passing the unchanged context. The unsigned timer
at `+0x1AE4` must reach the selected descriptor's start30; its end is110.
The branch at `+0xE34` otherwise rejoins at `+0xE4C`, skipping both calls.
Initialization jumps past rendering. This is a local reachability gate,
not a claim that every update invokes sheets.

Two 152-byte records occupy `[0x11DC,0x130C)`, with four four-point arrays
at offsets0/32/64/96, outer color128, inner color132 and size136. The
suffix `[140,152)` remains opaque. Translation uses the positive part of
word `+0x1F4` for the first sheet and `+0x1D4` for the second, in the
geometry record `[0xF60,0x11DC)`. Each axis adds its delta times that
amount divided by1024, preserving signed truncation. Odd frames add the
second sheet's signed `size/8` bias to its scale.

After drawing, the first sheet resets to2048, or zero when signed phase
is at least3. At phase0 the second sheet uses the unsigned descriptor-relative
timing quotient, clamping4096 and advancing to phase1 when reached.
The emitted division and zero-denominator trap remain unchanged.
Nonzero phases below2 select4096; phase2 grows by frame-step shifted12
and caps32768; phase3 shrinks by frame-step shifted8, clamps zero and
advances to phase4. Guards and later phases retain their original behavior.

Each sheet draws four quads through the same fixed 52-byte `POLY_GT4`
packet `[0x19B0,0x19E4)`. Projection outputs use offsets8/20/32/44.
The first three vertices use the inner color and the fourth the outer
color. Signed `otz*8/10` and projection-flag checks guard sorting.
The 264-byte frame separates coordinate `[128,208)`, projection output
`[208,212)`, flag `[212,216)` and ordering-table pointer `[216,220)`;
saved registers begin224. The context, geometry-record, sheet, size and
packet pointer register writes are checked over the complete helper.

The fresh proof retains all92 target constants and checks213 retained/sheet
instruction anchors per image. Ten static calls resolve to nine real
resident helpers, within the freshly verified set of36 module bindings.
The sheet's direct context minimum is `+0x1B1C`, below the entry's
`+0x1B2C`; neither is an allocation or whole-game lifetime claim.
