# Spanish MODEL385 Gouraud lines

The game-owned helper at image `+0xF5C..+0x1478` is independently recovered
as C: 1,308 instruction bytes in each of eight physical loads. The two slot
compilations use the authoritative `gcc_2_8_1_g0_split` profile (GCC 2.8.1 /
MASPSX 2.81). Every complete 20,480-byte image matches its retail hash.

This is not a regional C port. Before integration, the regional matching
manifests contained 5,961 C entries; fourteen same-size bodies were compared
and none had this helper's normalized instruction shape. Types and behavior
were recovered from local retail instructions and the established local SDK
declarations. Similar rendering algorithms do not imply identical context,
submission gates, or instruction ownership.

## Physical images and inventory

`spanish-model-variant385-instances.csv` records all eight hashes and physical
sectors. Scanning every model record's stages 7 through 10 finds precisely
these MODEL385/535 loads:

| Model | Record | Stages | Sectors | Command | Start time | Duration |
| --- | --- | --- | --- | --- | --- | --- |
| 170 | 170 | 7 / 8 | 47100 / 47110 | 551003 | 0 | 58 |
| 406 | 356 | 9 / 10 | 98456 / 98466 | 551006 | 20 | 68 |
| 407 | 357 | 9 / 10 | 98732 / 98742 | 551004 | 0 | 56 |
| 513 | 463 | 9 / 10 | 127988 / 127998 | 551005 | 0 | 48 |

Even stages use slot one at `0x8017B000`, with header 535; odd stages use
slot zero at `0x8013B000`, with header 385. Model numbers are not record
indices after the archive's discontinuity. No physical image is represented
as a duplicate-sector shortcut.

The six closed, contiguous functions occupy:

| Range | Ownership |
| --- | --- |
| `+0x4..+0xF5C` | Entry, retained generated ASM |
| `+0xF5C..+0x1478` | Gouraud lines, matching C |
| `+0x1478..+0x1B00` | [Three-station strip](spanish-model-variant385-strip.md), matching C |
| `+0x1B00..+0x21B8` | Retained generated ASM |
| `+0x21B8..+0x26B4` | [Pulsing quads](spanish-model-variant385-pulse.md), matching C |
| `+0x26B4..+0x2EB4` | [Rings](spanish-model-variant385-rings.md), matching C |

Each entry directly calls all five helpers. CFG traversal covers every word
of each function, with one return and no indirect calls. The four-byte header
and `+0x2EB4..+0x5000` tail remain raw data, not executable padding. Integration
of the line checkpoint added eight matching instances (10,464 instruction
bytes) and inventoried forty-eight functions. The subsequent strip checkpoint
adds another eight C instances, leaving thirty-two ASM instances.
The pulsing-quad checkpoint adds eight more C instances (10,208 bytes),
bringing the family to twenty-four C instances and twenty-four ASM instances.

## Context, initialization, and actual call gate

The original `a0` context is saved in `s2` at entry `+0xC`; reaching-definition
analysis, including branch delay slots and the entry's initialization loops,
finds only that definition at the helper call `+0xDF4`. Its delay slot passes
`s2` to `a0`. The observed entry call requires unsigned time at least the
descriptor's `+0x2C` start and signed phase at most zero. The helper retains
its phase-one and later-phase paths; this call site does not establish those
paths as reachable.

The descriptor table is at image `+0x2FB0`, with 76-byte records indexed by
the actual command modulo 1000. Entry `+0x90..+0xBC` builds and stores this
pointer at context `+0x1000`. Each selected record is inside the raw tail;
its start and duration are checked independently of the C source.

Three groups begin at context zero and have stride `0x1A0`. Each contains
two banks of four rows of six eight-byte `SVECTOR` endpoints, followed by
color at `+0x180`, signed size at `+0x194`, and completion at `+0x198`.
Initialization traverses those same dimensions and strides, assigns RGB
128/128/128, clears completion, and staggers size by group index.

The 20-byte `GsGLINE` lies at context `+0xF84..+0xF98`. Origin is `+0xFAC`,
target is `+0xFB8`, the two-component direction view is `+0xFC4`, projected
coordinates are `+0xFD0`, and the other direction vector is `+0xFD4`.
Time, step, timing pointer, and phase are at `+0xFF0`, `+0xFF8`, `+0x1000`,
and `+0x1030`. The private `State385` is a `0x1034`-byte helper view, not a
claim about the complete allocation.

The helper retains four `ratan2` calls, constructs an identity rotation with
uniform scale, and selects origin or target translation by phase. For each
line, `RotTransPers4` receives repeated endpoint pairs and repeated packet
output pointers. Color ownership switches between endpoints at phase two.
Submission requires **nonnegative depth and nonnegative GTE flag**, then
narrows depth to `u16`. This is not the positive-depth/no-flag rule used by
some related renderers.

Phase-zero timing uses an unsigned quotient, normalizes by subtracting 4096,
then uses the signed intermediate in the group-relative subtraction.
Phase one restaggers sizes. Later phases advance by `step * 256`, wrapping
at 8192 or clamping and marking completion when phase is at least five.
No extra divide-by-zero fallback or semantic simplification was introduced.

## Match and ownership evidence

The attempt ledger retains all four source experiments and eight terminal
whole-image records. The initial reconstruction produced 1,312 bytes with
275 positional differences. A shared fade intermediate and explicit channel
initialization recovered 1,308 bytes with one differing timing instruction.
Separating signed normalized progress recovered every instruction. The
slot-one wrapper was then independently compiled and linked.

Thirty target-compiled layout constants cover SDK sizes, group bounds,
context fields, packet outputs, and descriptor duration. Repeated header
inclusion is checked. Packet-base and context-register lifetimes are
validated against retail; only the attribute and RGB fields are directly
stored, while projection writes the two four-byte coordinate pairs.

The focused tests verify every selected code/data input against the final
linker script and final symbols, reconstruct all selected C relocations,
and compare all eight complete binaries. The helper has eleven calls to
eight resident callees and ten local jumps. The family binding file covers
all thirty-six resident addresses used by the six functions. Resident
callee, loader, slot-pointer, and context-range ownership checks use the
clean Spanish resident ELF and original executable bytes.
