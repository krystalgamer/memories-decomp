# French MODEL373 entry

The MODEL707 stage7/8 entries `func_8013B004` / `func_8017B004`, at
image offset `0x4`, each own 4,268 bytes with a 272-byte frame.
Both use `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81.
All eight closed functions in each image are now matching C; the seven
accepted helper sources and their ownership are unchanged.

## Independently recovered context

All 1,067 native entry words establish the layout, initialization, calls
and arithmetic. The accepted local helper views corroborate their record
spans. The entry's minimum observed extent is `0x24A0`, not an
allocation-capacity claim. Entry-only extensions remain local.

| Offset | Records |
|---|---|
| `0x0000` | One `0x4EC` nine-row grid |
| `0x04EC` | Five `0x1AC` layers, each with three seventeen-point rows |
| `0x0D48` | Three `0xB4` point groups, with four corners at record `0x80` |
| `0x0F64` | One `0x6C` strip |
| `0x0FD0` | One `0x88` quadrant group |
| `0x1058` | Eight `0x6C` ribbons |
| `0x13B8` | Four `0x90` eight-point rings |
| `0x15F8` | Sixteen `0x7C` rays |
| `0x1DB8` / `0x1DD4` | Shared G3 / G4 |
| `0x1DF8` | Two shared GT4 packets |
| `0x1E60` | Sixteen layer GT4 packets |
| `0x21A0` | Overlapping grid/flat packet union |
| `0x2364` | Point-group FT4 packet |

The packet union is essential: eight GT4s at `0x21A0` overlap an
alternate view containing two FT4s at `0x21D4` and eight FT4 tiles at
`0x2224`. These are not disjoint arrays. The union ends at `0x2364`.

The matrix is at `0x23B0`, target at `0x23D0`, direction at `0x23D8`,
projected deltas at `0x23E8`, and camera direction at `0x23EC`.
Frame, elapsed time, animation frame and step occupy `0x2404..0x2410`;
fade is at `0x2414`, configuration at `0x2418`, and part pointers at
`0x2420`. Phase is at `0x2484`, ambient at `0x2498`, and signed slot
and command at `0x249C` / `0x249E`.

Configuration records have stride `0x28` at raw suffix plus `0x70`:
part bytes at `4..6`, layer start at `0x10`, grid start at `0x14`,
and point-group start at `0x24`.

## Initialization and update

The initial target is `(0,-300,-450)` in slot0 and `(0,-300,450)`
otherwise. Three texture imports at suffix offsets `0`, `0x1C`, `0x38`
use modes `0`, `1`, `1`. Packet page, palette, UV and flag writes retain
their native order, including the overlapping views.

Grid rows use latitudes `0..2048` in steps of256 and seventeen
longitudes in steps of256, with radius and height scaled by128.
The red channel is `(latitude-row)/8`, green is64 and blue is its
complement. Latitude advances in the body after the color expressions.
The grid's angular accumulator shares the real `k` local used by later
corner loops; a separate angular local changes thirteen register words.

Layers retain the unsigned radius expressions and signed fifths.
Point groups retain sixteen ring points, four corners and three signed
thirds. Their scale numerator is evaluated separately as `-n*6144`
before division by3; combining these expressions removes a native
division and changes induction ownership. Vector padding and color code
bytes remain untouched; ribbons and every helper point are not all
initialized by this entry.

Every update copies the opposite slot's target. The part matrix and
direction refresh only below phase4. Projection uses unsigned screen X
and signed screen Y. Layers and grid run when their unsigned start is
at most elapsed and phase is below8; points use only their start test.
At phases6/7 the grid texture is reimported with mode2 and its eight
GT4 headers, page, palette and flags are rebuilt without new UV writes.

These are the only three direct helper calls. The absence of direct
entry calls to strip, ribbon, quad and ray helpers does not establish
global dead code. Animation changes retain two separate frame-step calls.
Ambient is submitted to constant slot2; below phase8 it falls by128 and
clamps to zero, otherwise it becomes `fade << 5`. Phases3..6 return4;
phase7 returns1 and enters8; phase8 advances fade only below64, clamps
at64 and enters9; phase9 returns2.

## Exact evidence and scope

The [attempt ledger](french-model-variant373-entry-attempts.csv) records
all twenty measured candidates and both production image terminals.
The local texture stack view preserves three measured palette halfwords
at `sp+0x4A/0x4C/0x4E`; its preceding `0x3A` untouched bytes do not
identify the original object types. No accesses are added to that prefix.
Actual counter ownership, shared channel expressions, body-owned latitude
update and runtime packet-loop initialization recover the native schedule.
No inline assembly, fixed registers, volatile tricks, fabricated accesses
or arithmetic dependencies, or new compiler profiles are used.

Both complete20KiB images match their independent retail SHA-256 values
in the [instance ledger](french-model-variant373-instances.csv), with
original-sized C `STT_FUNC` entries and all seven helper owners preserved.
This adds8,536 C instruction bytes across the two images.
Raw headers and `0x1110`-byte suffixes retain their existing owners.
The canonical existing `Model_CopySlotU16Values` binding is added at
`0x8005C0B8`; the accepted legacy alias remains unchanged.
