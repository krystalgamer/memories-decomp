# French MODEL headers 475 and 625

Four independent ten-sector French images contain matching ribbons, sheets,
quads and strand helpers. Models 116/576 use compact records 116/526, stages 7/8 and both
load slots. The [instance inventory](french-model-variant475-instances.csv)
records the physical slices, commands and complete SHA-256 hashes.

Every complete French image is byte-identical to its corresponding accepted
[Spanish header-475 image](spanish-model-variant475.md). The six existing
`src/overlays/spanish_model_variant/variant475_*.c` wrappers are reused without
editing them, their shared NA458 bodies, or their headers. The accepted Spanish
475 ribbon body and its slot-one wrapper are also reused unchanged. There are no new
French C files or regional conditionals. Independent French compilation uses
`gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81, not the historical NA compiler
registration or the older compiler claim in the common header's comment.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3,732 | Entry assembly |
| `E98` | 2,784 | Ribbon C |
| `1978` | 1,212 | Sheet C |
| `1E34` | 940 | Quad C |
| `21E0` | 832 | Strand C |
| `2520` | 2,072 | Streamer assembly |
| `2D38` | 8,904 | Unclassified raw suffix |

The four full-image links account for sixteen C owners / 23,072 bytes,
eight assembly owners / 23,216 bytes, and eight raw owners / 35,632 bytes.
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

Entry and streamer assembly, all raw headers and all 35,616 suffix
bytes are preserved. No suffix is relabeled as padding or unreachable code.
No shared-source, Spanish-registration, README or global-usage data is changed.
