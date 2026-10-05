# Spanish MODEL423 nine-row mesh

Independently recover the previously unregistered 1,168-byte renderer at
`+0xE18..+0x12A8` in both MODEL385 images. The pre-recovery census of
5,853 accepted regional C registrations found no accepted normalized
shape. This is new retail-derived decompilation, not a French or other
regional source port.

The first nine-row mesh candidate matched every instruction; an independent
second-slot compilation also matched. Both use the existing
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 / MASPSX 2.81, without forced
registers, padding, allocation controls or new flags. Three local nine-byte
color arrays naturally reproduce the original 336-byte frame.

## Physical images and complete inventory

The exhaustive MODEL archive scan finds exactly these two physical loads:

| MODEL / record | Stage / slot | Header | Sector | Load address |
|---|---|---|---|---|
| 385 / 335 | 7 / 0 | 423 | 92640 | `0x8013B000` |
| 385 / 335 | 8 / 1 | 573 | 92650 | `0x8017B000` |

Both images are 20,480 bytes. The selected command word is 589000;
the record's three words are 589000, 639000 and -2. Full hashes and
physical identities are in the [instance inventory](spanish-model-variant423-instances.csv).

| Function offset | Bytes | Ownership |
|---|---:|---|
| `+0x4..+0xE18` | 3,604 | Assembly entry |
| `+0xE18..+0x12A8` | 1,168 | Matching C mesh |
| `+0x12A8..+0x1CB4` | 2,572 | Assembly |
| `+0x1CB4..+0x2564` | 2,224 | Assembly |
| `+0x2564..+0x2A50` | 1,260 | Matching C [two-sheet renderer](spanish-model-variant423-sheets.md) |
| `+0x2A50..+0x3214` | 1,988 | Assembly; no entry-reachable call observed |
| `+0x3214..+0x37D4` | 1,472 | Assembly |

All fourteen function instances have closed instruction coverage and one
return each. Four C instances cover 4,856 instruction bytes, including the
separately recovered two-sheet renderer; ten assembly instances remain.
Four-byte headers and 6,188-byte suffixes from
`+0x37D4` stay raw. All previously registered modules are preserved.

## Original context, packet and bounded views

The entry calls the mesh at `+0xC7C`, passing `s2` in delay slot `+0xC80`.
The only reaching definition is original `a0` at entry `+0xC`.
Initialization temporarily reuses `s2`, but that path bypasses this runtime
call. The phase-at-least-three gate calls the `+0x3214` helper first;
the mesh does not assume that earlier call leaves state unchanged.

| View | Offset / extent |
|---|---|
| Nine rows of seventeen SVECTORs | `+0..+0x4C8`; row stride `0x88` |
| Nine four-byte colors | `+0x4C8..+0x4EC` |
| One reused GT4 | `+0x1B2C..+0x1B60` |
| Signed halfword origin | `+0x1D58` |
| Three word direction components | `+0x1D60` |
| Frame / step | `+0x1D8C / +0x1D98` |
| Size / intensity / phase | `+0x1DB8 / +0x1DC8 / +0x1E0C` |

The `0x1E10` state is a partial helper view, not an allocation-capacity
claim. Actual resident context pointers and load-bank extents prove it
does not overlap the corresponding loaded banks.

Entry `+0x54` forms `context + 0x1B2C`, saved at stack `+0x94`
by `+0x68`. That is the only direct store to this stack slot before the
`+0x3AC` argument load and `SetPolyGT4` call at `+0x3B0`.
Subsequent texture/CLUT setup targets the same packet. Initialization
also proves nine rows, seventeen points per row and nine color records.
The helper selects this packet at `+0xE68`; its packet register has no
later write until the epilogue. Twelve byte color stores stay inside GT4.

## Rendering and progression

Two `ratan2` calls are retained even though their results are unused.
Even frames add signed `size / 32` to the uniform scale; odd frames use
size unchanged. Rotation is zero and translation uses the signed
halfword origin.

At phase five or later, each of the nine RGB colors is multiplied by
signed intensity, divided by 1,024 with truncation, then narrowed to a
byte. Earlier phases copy the colors unchanged. Eight bands of sixteen
strips connect adjacent rows and adjacent points. The last access is
row eight, point sixteen, inside the proven array. Each quad's first
two corners use the current row color and its last two use the next.

Nonnegative depth and flag permit sorting through the already declared
`func_8005B260` (`src/game/gpu_packets.h`), with sixteen-bit priority and
fourth argument one. Its Spanish resident owner is `0x8004D5B8`.
There is no extra positive-size draw guard.

At phase three or later, size below 4,096 grows by `step * 256`,
clamping on reaching the bound. At phase five, positive intensity falls
by `step * 32`; reaching or crossing zero clamps it and sets phase six.

## Preservation and regressions

Both complete production images match, including every retained assembly
function and raw suffix. Five regressions verify exhaustive physical
identity, all function boundaries and input/final owners, 26 target-compiled
layout constants, original-context provenance, initialized mesh/packet
bounds, all 34 resident bindings, actual loader/context storage owners,
and terminal source/transitive-header fingerprints.

Each selected object has nine external calls to eight distinct addresses,
including two `ratan2` calls, and two local jumps at function-relative
`+0x84 -> +0x90` and `+0x1EC -> +0x240`.

The [attempt ledger](spanish-model-variant423-attempts.csv) records two
independent exact scratch candidates and two complete-image production
terminals. Initial production linking omitted the known `ratan2` alias;
binding it to the independently verified `0x80089928` resolved this
without changing the matched source.
