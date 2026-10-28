# French MODEL168264 ray-shell entry

Tracked for #6460. French MODEL.MRG header 146 supplies the same component in
raw images 168264/168274 at slot addresses `0x8013B004`/`0x8017B004`.
Each entry owns its original `0xF9C` bytes (999 instruction words), using
`gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81. Both complete 20,480-byte
images reproduce their retail bytes and hashes; headers and raw suffixes
remain unchanged.

## Recovered component

The local header records a 70-byte configuration and 924-byte state, recovered
from native accesses. Configuration contains ray/particle colors, sixteen
part identifiers, radius/width controls, timing fields and sixteen signed
start times with a negative sentinel. State retains sixteen origins,
64 randomized particle positions, 64 randomized ray scales, camera phase,
two uploaded texture handles, elapsed time, notification index and tint gate.

Each active origin draws two 64-ray Gouraud fans, then a later pass projects
the randomized shell as 2x2 filled boxes. The second fan divides its doubled
random scale by three before applying age/duration. Camera and animation
progression, tint requests and one-at-a-time completion notifications remain
in native order.

The first negative start skips both rendering passes. An index-based sentinel
guard and first-pass do/while recover that control flow exactly. Halfword RGB
temporaries and a cached blue grayscale quotient recover native division,
packet-store and register scheduling. Accepted local
`src/overlays/model_variant/variant118_entry.c` provided related fan-structure
evidence; this component's layouts, trigonometry, scaling and timings were
independently recovered from its own image. No external reference types or
compiler flags were imported. Header numbers are not cross-region identity:
the USA header-146 component is different.

## Native details retained

- The initial active-slot query and both texture uploads remain, although
  their results are unused by later rendering.
- The delay path queries frame step again.
- Tint descriptors initialize RGB only, preserving native uninitialized
  blend-mode bytes and canonical by-value calls.
- Fan projection writes its third result to an otherwise unused FT4 packet;
  the G3 endpoint remains the separately projected origin. Each fan triangle
  is submitted twice, with the second submission replacing its middle point.
- Only the particle transform multiplies by the light matrix.
- The camera phases, animation seeks, notification returns and original
  sentinel scans are preserved rather than normalized.

## Evidence

`french-model-image168264-entry-attempts.csv` records all fifteen compiled
experiments from checkpoints 285/286 and both production-image proofs.
Fourteen experiments reached the first exact native body; the fifteenth
only cleaned indentation and removed an unused declaration. Source/header
fingerprints, named profiles, byte counts and precise mismatch reasons are
retained for each attempt.

Both relocated native bodies match independently. Production ELF symbols
retain one original-sized C owner per image, and complete image comparisons
verify that no header, raw suffix or unrelated module was rewritten.
