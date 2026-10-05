# Spanish MODEL425 nine-row mesh

Independently recover the previously unregistered 1,164-byte renderer at
`+0xEE4..+0x1370` in both MODEL0 images. The pre-recovery census found no
accepted normalized C shape. This is retail-derived unmatched work, not
a French or other regional source port.

The first candidate and independent second-slot compilation matched all
instructions with `gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81.
No forced registers, allocation controls, padding or new flags were used.
Three nine-byte local color arrays naturally reproduce the 336-byte frame.

## Physical images and ownership

The exhaustive MODEL archive scan finds exactly two physical loads:

| MODEL / record | Stage / slot | Header | Sector | Load address |
|---|---|---|---|---|
| 0 / 0 | 9 / 0 | 425 | 200 | `0x8013B000` |
| 0 / 0 | 10 / 1 | 575 | 210 | `0x8017B000` |

Both images are 20,480 bytes and use command 591000. Their complete hashes
are recorded in the [instance inventory](spanish-model-variant425-instances.csv).

| Function offset | Bytes | Ownership |
|---|---:|---|
| `+0x4..+0xEE4` | 3,808 | Assembly entry |
| `+0xEE4..+0x1370` | 1,164 | Matching C mesh |
| `+0x1370..+0x1DA4` | 2,612 | Assembly |
| `+0x1DA4..+0x26DC` | 2,360 | Assembly |
| `+0x26DC..+0x2BC8` | 1,260 | Assembly |
| `+0x2BC8..+0x3360` | 1,944 | Assembly; no entry-reachable call observed |
| `+0x3360..+0x3908` | 1,448 | Assembly |

All fourteen function instances have closed instruction coverage and one
return each. Two C instances cover 2,328 instruction bytes; twelve assembly
instances remain. Four-byte headers and 5,880-byte suffixes beginning at
`+0x3908` remain raw. Previously registered modules are preserved.

## Original context and initialized storage

The entry calls the mesh at `+0xCE0`, passing `s2` in delay slot `+0xCE4`.
Original `a0` at entry `+0xC` is the only reaching definition. Initialization
reuses this register but bypasses the runtime call. At phase three or
later, the entry invokes `+0x3360` first; no state-preservation assumption
is made across that earlier helper.

| View | Offset / extent |
|---|---|
| Nine rows of seventeen SVECTORs | `+0..+0x4C8`; row stride `0x88` |
| Nine four-byte colors | `+0x4C8..+0x4EC` |
| One reused GT4 | `+0x1528..+0x155C` |
| Signed halfword origin | `+0x1754` |
| Three signed word direction components | `+0x175C` |
| Frame / step | `+0x1788 / +0x1794` |
| Size / intensity / phase | `+0x17B4 / +0x17C4 / +0x180C` |

The `0x1810` state is a partial view, not an allocation-capacity claim.
Actual resident context pointers and bank extents prove non-overlap.

Entry `+0x54` forms `context + 0x1528`, stored at stack `+0x94` by
`+0x68`. This is the only direct write to that slot before `+0x3A4`
loads the argument for `SetPolyGT4` at `+0x3A8`. Texture/CLUT setup
targets this same packet. Geometry initialization proves the nine rows
and seventeen points per row. The helper selects its packet at `+0xF34`;
the register is unchanged until the epilogue, and all twelve color-byte
stores remain inside GT4.

## Rendering and progression

Both unused-result `ratan2` calls are retained. Even frames add signed
`size / 16` to uniform scale; odd frames use size unchanged. Rotation is
zero and translation uses the signed halfword origin.

At phase five or later, nine RGB colors are multiplied by signed intensity,
divided by 1,024 with truncation, then narrowed to bytes. Earlier phases
copy their original colors. Eight bands of sixteen strips connect adjacent
rows and adjacent points. Row eight, point sixteen is the final access,
inside the proven array. First and second corners use the current row
color; third and fourth use the next row.

Nonnegative depth and flag permit `GsSortPoly`, with priority narrowed to
sixteen bits. There is no additional positive-size draw guard.
At phase three or later, size below 4,096 grows by `step * 32` and clamps
on reaching the bound. At phase five, positive intensity falls by
`step * 32`; reaching or crossing zero clamps it and sets phase six.

## Exactness and regression evidence

Both complete production images match, including all assembly and raw
owners. Five regressions verify exhaustive physical identity, all function
boundaries, 26 target-compiled layouts, original context, initialized
packet/mesh bounds, all 33 resident bindings, actual loader/context storage
owners, and terminal source/transitive-header fingerprints.

Each selected object has nine external calls to eight distinct addresses,
including two `ratan2` calls, and two function-relative local jumps:
`+0x84 -> +0x90` and `+0x1EC -> +0x240`.

The [attempt ledger](spanish-model-variant425-attempts.csv) preserves two
independent exact scratch candidates and two complete-image production
terminals.
