# French MODEL headers 475 and 625

Four independent ten-sector French images contain matching sheets, quads and
strand helpers. Models 116/576 use compact records 116/526, stages 7/8 and both
load slots. The [instance inventory](french-model-variant475-instances.csv)
records the physical slices, commands and complete SHA-256 hashes.

Every complete French image is byte-identical to its corresponding accepted
[Spanish header-475 image](spanish-model-variant475.md). The six existing
`src/overlays/spanish_model_variant/variant475_*.c` wrappers are reused without
editing them, their shared NA458 bodies, or their headers. There are no new
French C files or regional conditionals. Independent French compilation uses
`gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81, not the historical NA compiler
registration or the older compiler claim in the common header's comment.

| Image offset | Bytes per image | Owner |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3,732 | Entry assembly |
| `E98` | 2,784 | Ribbon assembly |
| `1978` | 1,212 | Sheet C |
| `1E34` | 940 | Quad C |
| `21E0` | 832 | Strand C |
| `2520` | 2,072 | Streamer assembly |
| `2D38` | 8,904 | Unclassified raw suffix |

The four full-image links account for twelve C owners / 11,936 bytes,
twelve assembly owners / 34,352 bytes, and eight raw owners / 35,632 bytes.
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
preserves that saved register through the quad call at `D10/D14` and sheet
call at `D30/D34`. Strand has no demonstrated direct or normal entry caller:
it remains a matched loaded-code function, not an invented runtime path.
The largest direct entry access requires `3034` bytes, which is a minimum
accessed view, not allocation capacity or a global lifetime proof.

The complete French resident image, final executable symbols, real selected
input definitions and exact full retail function bodies establish 38 resident
owners: all 34 overlay callees plus four loader/initializer/controller owners.
Thirteen distinct imports belong to the three C helpers. `GsSortLine` resolves
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

## Experiments and retained work

The [attempt ledger](french-model-variant475-attempts.csv) retains six initial
exact relocated helper calibrations, four nonexact sibling calibrations, and
six canonical-wrapper terminal records. The sheet / quad / strand frames are
280 / 296 / 264 bytes. Ribbon produces 2,768 bytes / frame 304 rather than
2,784 / 312, with 498 differing words; streamers produce 2,004 / 328 rather
than 2,072 / 336, with 514 differing words. Neither is promoted.

Entry, ribbon and streamer assembly, all raw headers and all 35,616 suffix
bytes are preserved. No suffix is relabeled as padding or unreachable code.
No shared-source, Spanish-registration, README or global-usage data is changed.
