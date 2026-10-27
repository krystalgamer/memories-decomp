# French MODEL412 entry

The loader entry at image offset `0x4` is matching C in both French
MODEL10 stage 7/8 images (header words 412/562). Each entry is `0xB94`
bytes, or 741 instructions, using the existing `gcc_2_8_1_g0_split`
profile. All six inventoried functions in each image are now C. The
separately owned header and unclassified suffix are unchanged.

The implementation reuses the accepted local MODEL402 record, ring, band
and projection declarations and MODEL412 trail layout. Its offsets,
constants and control flow were recovered from this French entry; no
reference-project types or compiler settings were imported.

## Context and initialization

The entry view has a minimum observed extent of `0x1B7C`, not a proven
allocation capacity. It initializes one trail at `+0`, one `0x58`-byte
record at `+0x1268`, two rings at `+0x12C0`, and two bands at `+0x16A0`.
Each of the trail's six rows has 31 columns, but initialization touches
only columns 0 through 29. The entry does not initialize the retained
ribbon region at `+0x13E0`.

Packets occupy `+0x1904` through `+0x1A64`. The matrix is at `+0x1AB0`,
the copied target at `+0x1AD0`, the direction at `+0x1AD8`, and the
projected screen delta at `+0x1AE8`. The configuration pointer at
`+0x1B24` selects a 32-byte record from `D_8013D7A8 + 0xFC`. That record
supplies one root-part byte, six further part bytes, a part-count
halfword and three unsigned timing words.

The band packet at `+0x19E0` is constructed twice at the same address.
Its first initialization reads an unwritten page halfword at `sp+0xC0`
and an unwritten CLUT halfword at `sp+0x4C`, then the second construction
overwrites it with the known band texture. The C preserves these original
reads and the unused matrix locals that position them; initializing the
locals or removing the first construction would not reproduce retail.
No claim of defined portable C behavior is made for those reads.

## Update and completion

The negative-command path captures the current animation frame before
copying the opposite slot's position. It calls `GsGetLw`, not
`GsGetLwUnit`. After projection, it repeats the same root-part matrix and
direction refresh until the configuration part count is reached; it does
not select successive entries from the six-part array.

Only the trail helper at image offset `0xB98` is called directly, once
unsigned elapsed time reaches configuration word `+0x14`. The retained
strip, ribbons, rings and bands have no direct call from this entry;
their global reachability remains unresolved.

When configuration duration `+0x1C` is reached in phase 0, the entry
returns 4 without changing phase. Phase 1 returns 1 and becomes 2.
Phase 2 advances fade toward 64, then clamps it and becomes 3. A separate
final phase-3 test returns 2, including on the call that reaches the
clamp. Tint decays by 128 below phase 2 and otherwise follows `fade << 5`.

Counter reuse, temporary declarations and packed-texture result lifetimes
affect GCC 2.8.1's allocation and scheduling. The record and trail outer
loops share `n`; all four texture-pack calls assign `packed`. Explicit
opposite-slot call branches produce the original common call sequence,
whereas a Boolean slot argument replaces its branch with `sltiu`.

## Complete-image acceptance

Both complete `0x5000`-byte images were rebuilt from their tracked layouts,
including all five previously accepted helpers, and compared byte-for-byte:

| Image | SHA-256 |
|---|---|
| MODEL10 stage 7 slot 0 | `7bb600d385b45a92b8ad2acee1f2c66a40284b10321df874427cec229ac91717` |
| MODEL10 stage 8 slot 1 | `0c5223daeb6f668aec4052f3ee4fdd46656bef093babd55379a0850b38a238e5` |

The binding file adds six canonical SDK aliases at already identified
French resident addresses. No resident source or compiler profile changes.
The slot-1 wrapper remaps only the entry, its trail callee and its data.

The [attempt ledger](french-model-variant412-entry-attempts.csv) records
all four measured native candidates and the two promoted complete-image
results. Source and immediate entry-header fingerprints are SHA-256;
shared dependencies are those of accepted base
`af3626df0acb60f64ca1a8f39b04a18159a9473c`. Different-word counts compare
overlapping instruction ranges; a length mismatch is independently rejected.
