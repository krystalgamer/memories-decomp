# French MODEL103128 four-emitter entry

MODEL.MRG sector 103128 (slot 0) and sector 103138 (slot 1) share this
3,816-byte entry at image offset `0x4`. Each complete 20,480-byte image
matches its original bytes with `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX
2.81. The four-byte loader header and raw suffix beginning at `0xEEC`
remain unchanged. The function names and raw module identities are retained.

| Sector | Entry | Complete-image SHA-256 |
|---|---|---|
| 103128 | `func_8013B004` | `d2c6179efd66e6a17cfa32ad5d2bee5324590ba5fc58e02a7597b2dc5e6ad208` |
| 103138 | `func_8017B004` | `121a41edde9b0e40f2c83c6efec12fc785bb239921464482ba74f3fc7085a486` |

## Measured behavior and layout

The ten-byte configuration selects four model parts and start/end times.
The entry owns four groups of eight projectile positions, velocities and
impact snapshots, plus per-projectile state, size, alpha and RGB. Positions,
velocities and snapshots begin at `0x4`, `0x104` and `0x204`; elapsed/tick
are at `0x304`/`0x308`. Sizes begin at `0x30C`, phase at `0x38C`,
projectile states at `0x38E`, count at `0x3CE`, ambient at `0x3D0`,
page/CLUT at `0x3D4`/`0x3D6`, two texture words at `0x3D8`, alpha at
`0x3E0` and RGB records at `0x400`.

Initialization randomizes small XY offsets, clears velocity and impact
coordinates, and sets alpha 128 and RGB 96/48/48. Updates select camera
presets, override frame step, dim/restore slot ambient, and obtain each
configured part's transform. Projectiles move toward the opposite slot's
Z plane at +/-450 and Y=-350, with random XY spread. Activation increases
the active count every other update, capped at eight.

Each active projectile draws a narrow textured quad. Crossing the target
plane records exactly three coordinate halfwords, not the SVECTOR padding.
Impacts draw four expanding flare quadrants, each with full-color and
quarter-color texture regions; alpha and RGB fade independently.

The footer examines the first emitter's last active projectile. Its final
comparison reads a **word** at `0x38C`, including both phase and the first
projectile state, and compares it with `0x20000`. The local control union
preserves this real overlapping access; it must not become a phase-only
test. No count-zero guard is added. Native G4 header writes and the saved
matrix copy are also preserved even though their results are not consumed.

## Source recovery and evidence

All 954 native instructions per slot were examined. The companion attempt
ledger records 36 distinct native experiments, including unchanged named
profile calibrations, followed by two complete-image production proofs.
Raw aligned difference counts include shifted instructions and register
allocation changes; they are not counts of independent behavioral errors.

Separate positive/negative impact bodies recover native shared tails and
color-index ownership. Real vertical jitter/height temporaries stay inside
the SDK vector expression to retain coordinate lifetimes. Packet view
initialization precedes the slot query.

The final flare recovery computes signed `next_side` and the real first
corner X **before** committing the retained lower-halfword sign. This
preserves the native intermediate copy and saved-register allocation.
The accepted local variant770 flare provided structural ordering evidence,
not this entry's declarations, configuration or compiler settings. Layouts
were recovered independently from these two native images; existing
SDK/game headers supply function declarations.

Both production ELFs own the original 3,816-byte entry in `.unmatched_4`.
No inline assembly, fixed registers, volatile tricks, fabricated accesses
or dependencies, reference headers, retail files, generated assets,
progress refresh or cloud-test changes are included.
