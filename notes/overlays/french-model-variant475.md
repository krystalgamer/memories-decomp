# French MODEL headers 475 and 625

Four independent ten-sector French images contain matching entries, ribbons, sheets,
quads, strand and retained streamer helpers. Models 116/576 use compact records 116/526, stages 7/8 and both
load slots. The [instance inventory](french-model-variant475-instances.csv)
records the physical slices, commands and complete SHA-256 hashes.

Every complete French image is byte-identical to its corresponding accepted
[Spanish header-475 image](spanish-model-variant475.md). The six existing
`src/overlays/spanish_model_variant/variant475_*.c` wrappers are reused without
editing them, their shared NA458 bodies, or their headers. The accepted Spanish
475 ribbon body and its slot-one wrapper are also reused unchanged. The entry
has a locally recovered French body, entry-only header and slot-one wrapper;
no accepted Spanish entry body was available. Retained streamers have a local
French body and slot-one wrapper using the unchanged `Variant458Streamer` header. There are no regional conditionals.
Independent French compilation uses
`gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81, not the historical NA compiler
registration or the older compiler claim in the common header's comment.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3,732 | Entry C |
| `E98` | 2,784 | Ribbon C |
| `1978` | 1,212 | Sheet C |
| `1E34` | 940 | Quad C |
| `21E0` | 832 | Strand C |
| `2520` | 2,072 | Retained streamer C |
| `2D38` | 8,904 | Unclassified raw suffix |

The four full-image links account for twenty-four C owners / 46,288 bytes,
no assembly owners, and eight raw owners / 35,632 bytes.
Selected compiler objects must define the sized functions in the real link;
candidate location, masked words and favorable alternative offsets are not
acceptance criteria.

## Independent French evidence

All 24 conservative function walks cover their exact spans, with one return
and no unresolved or indirect transfer. Four physical loader slices and actual
metadata commands are checked independently. Each request is `641000`, selecting
record zero of the 56-byte descriptor table at image `2E34`; its six timing
words are 20, 80, 160, 280, 460 and 560.

Seventy-two target-compiled layout constants and 79 French retail anchors
support the unchanged local declarations. Sixteen 156-byte sheets occupy
`1904..22C4`; six 132-byte strands occupy `22C4..25DC`. The quad view spans
`0..1E4`, with four eight-point arrays, RGB at `140`, levels at `154` and hidden
words at `174`. `ModelVariantSheet` is a 152-byte prefix read by the quad helper,
not the 156-byte sheet array stride. GT4, FT4 and GsLINE views respectively span
`2CB8..2CEC`, `2D98..2DC0` and `2DC0..2DD0`.

The entry captures the original context at `C/14`. Its negative-command path
preserves that saved register through the ribbon call at `CD4/CD8`,
quad call at `D10/D14` and sheet
call at `D30/D34`. Strand has no demonstrated direct or normal entry caller:
it remains a matched loaded-code function, not an invented runtime path.
The largest direct entry access requires `3034` bytes, which is a minimum
accessed view, not allocation capacity or a global lifetime proof.

The complete French resident image, final executable symbols, real selected
input definitions and exact full retail function bodies establish 38 resident
owners: all 34 overlay callees plus four loader/initializer/controller owners.
Thirteen distinct imports belong to the original three C helpers. `GsSortLine` resolves
to the actual SDK function at `80083F38`; an initial calibration link failed
explicitly on this missing alias, then resumed separately using the already
accepted French binding. Both stored context pointers and code destinations
are checked against retail storage.

The inherited descriptor, packet-constructor and frame-getter regressions read
the French inputs, not the Spanish files. The getter contains two reads; it is
not an unconditional clamp. No safe quad-count bound, stable getter read,
complete writer audit or strand normal-call contract is inferred from source
reuse. The accepted Spanish behavioral investigation supplies additional
context, but its host-oracle case counts are not presented as new French runs.

## Ribbon reuse

The accepted Spanish endpoint uses `k` / `k - 1` under `k == 12`, retaining
the secondary `ribbon + 0x30` pointer. Independent French compilation reproduces
both 2,784-byte bodies with 312-byte frames under the unchanged named profile.
Four complete images link with these real compiler objects; no instruction
output or shared source is patched.

Forty-nine fresh target constants and 112 retail anchors per French image
confirm the accessed views and initialization instructions. Eight 740-byte
records occupy `1E4..1904`; arrays `a/sa/angle/b/sb/width` start at
`0/68/9C/D0/138/16C`, RGB at `1A0`, count/state/length at `1C0/1C8/1CC`,
and depth/flag/x-offset/y-offset at `248/27C/2B0/2CA`. The two 40-byte FT4s
occupy `2D20..2D70`; their observed code is `2F`, not the quad packet's `2E`.
The actual descriptor count is eight in all four images.

All eleven ribbon imports resolve to actual selected French resident input
definitions and complete byte-identical retail functions. `RotTransPers` at
`80087868` is a distinct 44-byte SDK owner. Each ribbon object has 35 resolved
relocations: 27 imported calls and eight local jumps. The latter are checked
against their real `.text` section, opcode and in-body target, not treated as
external callees. Four fresh closed CFGs and the original-context call
at `CD4/CD8` independently establish the loaded function and its entry path.

The inherited conditional record-bound regression covers byte-sized steps
0..255, without assuming a stable getter or clamp to six. It still assumes
the observed initialization, descriptor count and absence of external record
corruption. No new French host-oracle or initializer-emulation case counts,
whole-frame spill proof, complete writer audit or allocation/lifetime proof
are claimed.

## Experiments and retained work

The [attempt ledger](french-model-variant475-attempts.csv) retains six initial
exact relocated helper calibrations, four nonexact sibling calibrations, and
six canonical-wrapper terminal records, followed by two exact canonical ribbon
calibrations and two ribbon terminal records. All sixteen earlier rows are
preserved verbatim. The sheet / quad / strand frames are 280 / 296 / 264 bytes.
The initial NA458 ribbon calibration produced 2,768 bytes / frame 304 rather than
2,784 / 312, with 498 differing words; streamers produce 2,004 / 328 rather
than 2,072 / 336, with 514 differing words. The initial ribbon source is not
promoted; only the independently proved accepted Spanish body is selected.
Subsequent scratch-only streamer probes remain nonexact and are not promoted.

Six distinct paired entry probes follow those twenty rows, ending with
two exact-text records, then two canonical terminal records. The first two
produce 3,728 bytes / frame 216 with 798 differing words. Separate position
induction restores 3,732 / 224 but leaves 744 differences. A distinct quad
phase produces 3,712 / 224 with 365 differences. Recovering shared
orbital/ribbon phase lifetimes, a local streamer point counter and the
retail initialization order matches initialization but leaves 3,720 bytes
and 297 differences. Explicit opposite-slot call branches and packed signed
projection words recover both complete 3,732-byte bodies with frame 224.
The five rejected sources remain scratch-only; no forced registers, synthetic
stores, assembly patches or altered compiler flags are used.

## Entry reconstruction

The resident controller `func_800559D4`, selected at French `80058B4C`,
calls the slot's loaded code at base plus four with `field_DEC` as the original
context. Initialization receives the nonnegative request modulo 1,000;
updates receive `-1`. Fresh reads of the two models' metadata give `641000`,
so these four observed instances select descriptor zero at `2E34`. This
does not establish a safe domain for arbitrary externally supplied commands.
The source retains the direct 56-byte descriptor indexing and eight unsigned
part selectors.

The target-compiled entry views extend the already accepted helper prefixes
without changing them. One 484-byte quad record starts at zero; eight
740-byte ribbons at `1E4`; sixteen 156-byte sheets at `1904`; two 820-byte
streamers at `25DC`. The matrix starts at `2DE4`, target at `2E04`, and three
eight-element VECTOR arrays at `2E0C`, `2E8C`, and `2F0C`. Array strides,
packet offsets, descriptor fields, scalar state and signed packed projection
storage are checked with the authoritative target compiler. The local header
only describes the minimum observed `3034`-byte view, not an allocation.

Initialization preserves all repeated packet constructors and per-point
position calculations. A separate quad angle and shared orbital/ribbon angle
recover ordinary scalar lifetimes. The 64 bytes preceding the projection
inputs remain unrecovered local storage; their original types are not claimed.
`RotTransPers` writes canonical signed packed `PSXLONG` words; x uses the low
unsigned half and y uses an arithmetic high-half extraction. Opposite-slot
selection retains the original branches. The two independent
`Model_GetFrameStep()` calls, unsigned timing comparisons, helper calls with
the original context, fade transitions and return codes remain unchanged.

Both canonical entry objects are linked alongside every previous C owner
and the preserved streamer/raw owners in four complete byte-identical images.
The slot wrapper renames the entry, all three local callees and the raw suffix.
French resident bindings, previous helper sources and all 279 configured
image declarations remain unchanged. No host-emulator case count, complete
writer audit or whole-context lifetime proof is claimed.

Streamer assembly, all raw headers and all 35,616 suffix
bytes are preserved. No suffix is relabeled as padding or unreachable code.
No previously accepted shared-source, Spanish-registration, README or
global-usage data is changed.


## Retained streamer recovery

Both `0x2520..0x2D38` bodies reproduce all 2,072 bytes with 336-byte
frames. Four independent complete scratch links preserve the twenty
accepted C owners / 38,000 bytes and add four streamers / 8,288 bytes.
All 32 actual inputs, twenty-four closed function boundaries, twenty
target-compiled layout constants and 34 resident callee owners are checked.
Streamers, like strand, have no direct entry-call path; loading and
initializing their records does not establish that either renderer executes.

The unchanged local `Variant458Streamer` describes two `0x334`-byte
records at context `0x25DC..0x2C44`. Seventeen-point arrays `a/sa/angle/b/sb`
start at `0/0x88/0xCC/0x110/0x198`; widths, colors, depths and signed
screen offsets are at `0x1DC/0x264/0x2AC/0x2F0/0x312`. Projection outputs
`p` and `flag` are stack words at `0xD0/0xD4`, not record fields.
The helper accesses packet bytes at `0x2D20` through the canonical
`POLY_G4` view. This does not replace the ribbon's overlapping `POLY_FT4`
view or assert exclusive storage ownership, allocation capacity or lifetime.

Radius is 160; projected width caps at two when length exceeds 512.
Only strictly positive depth below `0x800` reaches sorting; the temporary
projection flag is not a draw gate. In state four, positive length is
reduced by `step << 4`; exhausted length clamps to zero and advances state
to five. Seventeen new French native instruction anchors preserve cursor
strides, stack outputs, depth gates and state behavior.

The fresh clean control reproduces 2,072 bytes / frame 336 / 124 differing
words. Literal endpoint arguments, indexed result fields, genuine
endpoint/nonendpoint offset scopes and a real predecessor cursor remove
the structural differences, leaving 39 words exchanging `s4/s5`.
Initializing the actual twist phase before the point/wave loop controls
recovers the native lifetimes and makes both bodies exact. No forced
registers, artificial conditions or stores, fake dependencies, inline
assembly, source-local extern declarations or compiler changes are introduced.

All 34 earlier ledger rows remain verbatim. Twenty-six appended rows
preserve nine historical and three new paired experiments plus two
terminal fingerprints. Historical frozen source, wrapper, object and
binary receipts are reverified against the fresh slot targets; their
verification tick is recorded without guessing original experiment ticks
from a shared script filename. Rejected sources remain scratch-only.

Each 8,904-byte suffix stays unclassified. Inventory completion for these
six functions does not establish exhaustive French runtime coverage.
Production integration and clean acceptance are recorded separately.

## Streamer production acceptance

The clean French resident and all 323 configured complete overlay images
match their retail inputs. The four MODEL475/625 production links retain
32 actual defining inputs: twenty-four C owners / 46,288 bytes, no assembly
owners, and eight raw header/suffix owners / 35,632 bytes. Independent
relinks reproduce the production ELFs and complete images; selected C
input text matches the frozen exact objects. All twenty previously accepted
C owners / 38,000 bytes remain intact, and all 34 resident callee defining
objects and complete bodies are reverified.

All 27 focused regressions pass without skips. Metadata, primitive-type,
attempt-ledger, matching-source, notes and G32 policy checks pass. The
configured French totals are 1,753 matching C instances / 2,455,684
instruction bytes out of 1,975 inventoried functions across 323 images.
These totals do not establish exhaustive runtime coverage: strand and
streamers still have no demonstrated direct entry-call path, and every
8,904-byte suffix remains unclassified.
