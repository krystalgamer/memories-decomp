# French MODEL444 entry

The loader entry at image offset `0x4` is now matching C in both French
MODEL175 stage 9/10 images. Each entry is `0xDB0` bytes (876 instructions),
built with the existing `gcc_2_8_1_g0_split` profile. All five inventoried
functions in each image are now C; the unclassified suffix remains data.

The implementation reuses the accepted sheet and funnel layouts and the
canonical local model/SDK declarations. Its packet and update structure was
compared with the accepted MODEL423 and Spanish MODEL441 entries, but its
declarations, offsets, constants and control flow were recovered from this
French entry. No reference-project types or compiler settings were imported.

## Recovered context and calls

The entry view has a minimum observed extent of `0x1690`, not a proven
allocation capacity. It initializes one tube at `+0`, two sheets at `+0x54C`,
two funnels at `+0x67C`, four coloured spokes at `+0x8AC`, two beams at
`+0xB6C`, and sixteen `0x64`-byte records at `+0xDC4`. The latter records'
colours and `field_48` values are known; their remaining fields and purpose
are deliberately unnamed.

Packets occupy `+0x1404` through `+0x1598`. The matrix is at `+0x15E4`,
the copied target at `+0x1604`, the direction at `+0x160C`, the projected
screen delta at `+0x161C`, and the selected `0x24`-byte configuration at
`+0x164C`. Configuration records start at image data `D_8013D91C + 0xFC`.

The negative-command path calls sheets and funnels only when configuration
word `+0xC` is strictly less than the unsigned animation time at `+0x163C`,
and the tube only when word `+0x10` is strictly less. The existing beam helper is not
called here. There is no second matrix/direction refresh after projection.
`Model_CopySlotPosition` copies all four halfwords; only its first
three coordinates are used in the direction calculation.

Phases 2/3 return 4; phase 4 returns 1 and becomes 5. Phase 5 adds the frame
step to fade unconditionally, clamping at 64 and entering phase 6, which
returns 2. Tint fades by 128 below phase 5 and otherwise follows `fade << 5`.

Counter reuse, temporary declaration order and projection snapshot/store
order affect GCC 2.8.1's allocation and scheduling. In particular,
`-n * 4096` must remain a compiler-derived funnel induction variable:
replacing it with an explicit accumulator swaps stack slots and instructions,
even though the two initial funnel sizes happen to be zero.

## Exact-image acceptance

Both complete `0x5000`-byte images were rebuilt from the tracked layouts,
including all previously accepted helpers, and compared byte-for-byte:

| Image | SHA-256 |
|---|---|
| MODEL175 stage 9 slot 0 | `deed68f2c6d28ce1512f450e9987639700db55846bb6446e1be0523e6aae3b84` |
| MODEL175 stage 10 slot 1 | `521483b87097907cccdb380c693d9623c7128a6fb2a541ff8628b994f616dd45` |

The shared binding file adds canonical SDK aliases at the already identified
French resident addresses; no resident source or compiler profile changes.
The slot-1 wrapper remaps only the entry, its three callees and its data.

The [attempt ledger](french-model-variant444-entry-attempts.csv) records every
measured source change. Fingerprints are source SHA-256 values;
`dependency_fingerprint` is the immediate entry-header SHA-256. Shared
headers are those of accepted base
`a7dc4ba6439d3829256e9a01a6b0a9b0252f4bc2`. Different-word counts compare the
overlapping instruction ranges; a length mismatch is independently rejected.
