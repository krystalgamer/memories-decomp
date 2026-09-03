# SLUS-01411 Memory Map

## Executable container

| Property | Value |
|---|---|
| File size | `0x1D0800` |
| Header | `0x000000-0x000800` |
| Load address | `0x80010000` |
| Loaded image end | `0x801E0000` |
| Entry point | `0x800129D8` |
| Entry file offset | `0x31D8` |
| Runtime global pointer | `0x8009AF08` |
| Header stack base | `0x801FFFF0` |

The PS-X EXE header declares the complete `0x1D0000`-byte loaded payload as its
text image. The finer regions below are recovered from executable structure and
startup behavior rather than from populated header data/BSS fields.

## Top-level regions

| Region | File range | VRAM range | Size | Classification |
|---|---:|---:|---:|---|
| Header | `0x000000-0x000800` | Not loaded | `0x800` | PS-X EXE header |
| Initial data | `0x000800-0x0031D8` | `0x80010000-0x800129D8` | `0x29D8` | Pointers, strings, constants, jump tables |
| Resident text | `0x0031D8-0x080ED4` | `0x800129D8-0x800906D4` | `0x7DCFC` | MIPS code with small embedded non-function ranges |
| Text padding | `0x080ED4-0x080EE0` | `0x800906D4-0x800906E0` | `0xC` | Zero alignment |
| Initialized data | `0x080EE0-0x08B890` | `0x800906E0-0x8009B090` | `0xA9B0` | Runtime metadata, paths, tables, globals |
| BSS image | `0x08B890-0x0EEF28` | `0x8009B090-0x800FE728` | `0x63698` | Zero bytes explicitly cleared by startup |
| Reserved zero | `0x0EEF28-0x12A800` | `0x800FE728-0x8013A000` | `0x3B8D8` | Zero gap before load slots |
| Overlay slot `0x8013A000` | `0x12A800-0x136800` | `0x8013A000-0x80146000` | `0xC000` | MODEL slot A |
| Overlay slot `0x80146000` | `0x136800-0x158800` | `0x80146000-0x80168000` | `0x22000` | WA shared duel bank |
| Overlay slot `0x80168000` | `0x158800-0x16A800` | `0x80168000-0x8017A000` | `0x12000` | WA screen bank |
| Overlay slot `0x8017A000` | `0x16A800-0x170800` | `0x8017A000-0x80180000` | `0x6000` | MODEL slot B and WA table data |
| Overlay slot `0x80180000` | `0x170800-0x19C800` | `0x80180000-0x801AC000` | `0x2C000` | SU bank |
| Tail data | `0x19C800-0x1D0800` | `0x801AC000-0x801E0000` | `0x34000` | Structured resident tables and encoded data |

`config/slus_01411/image_map.json` records a SHA-256 for every row, and
`make map` verifies continuity, VRAM translation, fill bytes, hashes, the final
resident return instruction, and startup BSS-clear operands.

## Resident text boundaries

The entry point is the first resident instruction. Bytes immediately before it
are strings and pointer tables.

The final resident function returns at `0x800906CC`, with its delay-slot `nop`
at `0x800906D0`. Twelve zero bytes follow before initialized data starts at
`0x800906E0`.

## BSS evidence

The first four entry-point instructions form two `lui`/`addiu` pairs:

- Clear start: `0x8009B090`
- Clear end: `0x800FE728`

The second address requires signed-immediate arithmetic:
`0x80100000 + (int16_t)0xE728 = 0x800FE728`.

The corresponding executable bytes are all zero. An additional zero region
continues to `0x8013A000`, but startup does not include that gap in this clear
loop, so it is classified separately.

## Original linker subregions

A descriptor at file offset `0x80EEC` contains:

```text
800129D8 0007DD08
800906E0 0000A828
8009B4A8 00063280
```

Together with `_gp = 0x8009AF08` and the startup clear loop, this supports the
finer linker layout:

| Region | Address range | Size |
|---|---:|---:|
| Read-only data before resident text | `0x80010000-0x800129D8` | `0x29D8` |
| Resident text and alignment | `0x800129D8-0x800906E0` | `0x7DD08` |
| Ordinary initialized data | `0x800906E0-0x8009AF08` | `0xA828` |
| Small initialized data | `0x8009AF08-0x8009B090` | `0x188` |
| Small BSS | `0x8009B090-0x8009B4A8` | `0x418` |
| Ordinary BSS | `0x8009B4A8-0x800FE728` | `0x63280` |

The PS-X EXE header flattens these into one loaded payload, so the project keeps
the top-level exact-byte image map separately from this inferred original
linker organization.

## Runtime load slots

The region beginning at `0x8013A000` contains fixed high-memory destinations
referenced through a shared table at `0x80010000`. Important boundaries include:

- `0x8013A000`
- `0x8013B000`
- `0x80146000`
- `0x80168000`
- `0x8017A000`
- `0x8017B000`
- `0x80180000`

The original executable places only small identifier words at several slot
bases and otherwise fills the region with zeros. Loaded chunks continue the
identifier sequence, supporting a module-ID interpretation:

| Slot | Resident word | Observed loaded words | Confirmed source |
|---:|---:|---|---|
| `0x8013A000` | `0x07` | Not yet catalogued | `MODEL.MRG` |
| `0x80146000` | `0x17` | `0x18` | `WA_MRG.MRG` |
| `0x80168000` | `0x12` | `0x13`, `0x14`, `0x15`, `0x16` | `WA_MRG.MRG` |
| `0x8017A000` | `0x0C` | Base code from MODEL; data at `+0x1D8` from WA | Shared |
| `0x80180000` | `0x0E` | `0x0F`, `0x10` | `SU.MRG` |

Resident code calls exact functions inside the loaded bytes, including
`0x801462B0` in a WA module, several `0x80168xxx` functions in WA modules,
`0x8017A004` in the MODEL-loaded slot, and multiple `0x80180xxx` functions in
SU modules.

These addresses are therefore runtime-loaded overlay/module destinations, not
resident executable functions. They are shared across MRG files and game
states; they must not be attributed wholesale to WA. See `notes/overlays.md`
for the loader trace and recovered sector layouts.
