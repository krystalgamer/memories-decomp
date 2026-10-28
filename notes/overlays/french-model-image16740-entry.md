# French MODEL16740/16750 ring and beam entry

For #6460, the raw French MODEL.MRG images at sectors 16740 and 16750
now own their loader entries in C. Each entry occupies image `+0x4..0xC44`
(3,136 bytes); the opposite-slot wrapper changes only the entry and three
image-local data symbols. The module header and unclassified suffix remain
unchanged. This does not classify the remaining raw data as C.

The named `gcc_2_8_1_g0_split` profile uses the authoritative
GCC 2.8.1/MASPSX 2.81 pipeline. Both complete original 20,480-byte images
are byte-identical after production linking, with original-sized C symbols:

| Sector | Entry | Complete-image SHA-256 |
|---|---|---|
| 16740 | `func_8013B004` | `43d9b229d9fa13e3ac39315d558f84648ca50a2c1684d08632f7ecacf4c704bc` |
| 16750 | `func_8017B004` | `5cd925b90e3afb997c422cc30a4e940924f600c0aca0824583ca3c7f08304c16` |

## Independently recovered layout and behavior

The 38-byte configuration record holds beam/ring colors, beam dimensions,
ring radius/thickness/height, durations, stagger intervals and phase starts.
State holds its configuration pointer at `+0x0`, 32 beam origins at `+0x4`,
the opposite-slot origin at `+0x104`, 66 ring points at `+0x10C`, current-slot
translation at `+0x31C`, two packed texture words at `+0x324`, elapsed time
at `+0x32C`, and the one-time completion byte at `+0x330`.

Two closed 33-point seams form expanding, fading rings. `RotTransPersN`
projects all 66 points into real stack arrays; 32 FT4 segments use
`AverageZ4` and the combined projection-flag bit `0x20`. Sixteen staggered
mirrored beam pairs grow vertically and fade, using the opposite-slot
origin and `RotAverage4`. An independent white flash covers PAL 320x256.
The update preserves the matrix stack, frame-step timing and
0/4/1/2 completion protocol.

All 784 native instructions were read before source recovery. The trig
calls are `ccos`/`csin`, not `rcos`/`rsin`. Local state/configuration views
come from these accesses; game and SDK declarations come from this
repository's canonical headers. Accepted variant198 C supplied structural
comparison for the real shared inner-row index and color staging, not
types, compiler flags or a substitute match proof.

## Matching evidence

The [attempt ledger](french-model-image16740-entry-attempts.csv) records
all 25 materially distinct native probes and both production image results.
Shared scalar lifetimes, phase-local RGB/counter variables, the common
`i + 33` index, nested completion-phase early returns and separately staged
flash arithmetic are intentional: simplifying them changes original
register allocation or scheduling. No inline assembly, fixed registers,
volatile accesses, artificial dependencies or compiler exception is used.
