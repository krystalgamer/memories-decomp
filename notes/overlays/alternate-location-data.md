# Alternate overworld table storage

`src/overlays/overworld/alternate_location.c` defines the sixteen
`AlternateLocation` records at `gCampaignMapAlt_aLocationTable`. The existing
`alternate_location.h` is both its owning header and the interface consumed
by `CampaignMap_PickAlternateExit` and `CampaignMap_UpdateAlternateLocation`.
It supplies the bounded extern and `ALTERNATE_LOCATION_COUNT`.

The record types are unchanged: 66 bytes per location, including four
12-byte exits at `+0x12`. The new data-only unit compiles separately into
both overworld images under `gcc_2_8_1_g0`, in North America, Japan and
PAL alike. Japan's bytes are North America's; PAL's differ in one byte,
location 9's only exit `pad2[2]` (`0xC0` NTSC, `0xD0` PAL, the live table's
sprite `y`), which the PAL entries select with `VERSION_EUROPE`. The table leaves
both alternate function bodies unchanged.

## Exact extent and placement

| File range | Runtime range | Owner |
|---|---|---|
| `0x1E54-0x2274` | `0x80169E54-0x8016A274` | Sixteen C records, `16 * 66 = 1056 = 0x420` bytes |
| `0x2274-0x22CA` | `0x8016A274-0x8016A2CA` | Alternate controller state, common storage declared by `alternate_location_tick.c` (PAL: same names in another order, to `0x22CC`) |
| `0x22CA-0x3000` | `0x8016A2CA-0x8016B000` | Remaining raw bytes (PAL from `0x22CC`) |

The state is zero-initialized common storage, like the live state (see
[`live-state-data.md`](live-state-data.md)). `alternate_location_tick.c`
declares its eight consumer-backed objects, the two `DisplayObject`
pointers, transition flags, move state, location, enter-scene byte,
`gCampaignMapAlt_LocationPrev` and `D_8016A2C9`, as tentative definitions
and builds with `gcc_2_8_1_o0_g0_split_comm`;
`CampaignMap_PickAlternateExit` reads two of them through `extern`. Each
layout's `alternate_location_tick_commons` subsegment places them from its
symbol file, so the unconsumed words and padding between them need no C.
In North America and Japan the block ends on the halfword after
`D_8016A2C9`, at `0x8016A2CA`, where the variants' bytes begin.

Both verified module slices have SHA-256
`0259a2516466fd98262c69a1f4da03d22fec71b9f4e125adea7925fe35220c12`.
The standalone compiled data section and each final module reproduce every
byte of that slice. A clean rebuild distinguishes the selected C object from
stale generated assembly.

## Alternate state names

Each name follows the alternate controller's own reads and writes, which
mirror `CampaignMap_UpdateLocation` and `CampaignMap_PickExit` statement for
statement. They are distinct `gCampaignMapAlt_` symbols, not aliases of the
live `gCampaignMap_` globals, and the raw callees stay unchanged. The US
`overworld_symbols.txt` (also read by Japan) and each European
`overworld_symbols.txt` bind them at their own addresses.

| US address | Former name | Name | Evidence |
|---|---|---|---|
| `0x80169E54` | `D_80169E54` | `gCampaignMapAlt_aLocationTable` | Sixteen 66-byte `AlternateLocation` records indexed by the current location |
| `0x8016A278` | `D_8016A278` | `gCampaignMapAlt_pLocationMarker` | Created for locations `>= 10`, receives the record's `a`/`b` pair, released and cleared below 10 |
| `0x8016A29C` | `D_8016A29C` | `gCampaignMapAlt_bTransitionFlags` | Set after a new location is picked; while set the controller waits on the raw transition call, then clears it and rebuilds |
| `0x8016A288` | `D_8016A288` | `gCampaignMapAlt_pSpotlight` | Its `f8` bit `0x40` toggles on input bit 4; same state offset and `0x40` flag as `gCampaignMap_pSpotlight` (below) |
| `0x8016A2B8` | `D_8016A2B8` | `gCampaignMapAlt_MoveState` | The exit picker stores `24` or the exit's step byte before returning a destination |
| `0x8016A2BC` | `D_8016A2BC` | `gCampaignMapAlt_Location` | Indexes the table, compared against 10, overwritten by a picked destination, stored `+ 32` into `D_800C4D82` on entry |
| `0x8016A2BD` | `D_8016A2BD` | `gCampaignMapAlt_bEnterSceneState` | Set to 1 when the picker returns the location with bit `0x8000`; bit 0x80 latches its one-time setup block |
| `0x8016A2C8` | `D_8016A2C8` | `gCampaignMapAlt_LocationPrev` | Receives the current location before each exit pick |

The named alternate state sits at a uniform `+0xCB0` from the live state in
both orderings: US `gCampaignMap_pLocationMarker`/`gCampaignMap_pSpotlight`
at `0x801695C8`/`0x801695D8` against `0x8016A278`/`0x8016A288`, and Europe at
`0x801695F0`/`0x801695FC` against `0x8016A2A0`/`0x8016A2AC`; the transition
flags, move state, location, enter-scene and previous-location bytes keep the
same displacement in both regions. `gCampaignMapAlt_pSpotlight` is therefore
the alternate copy of the spotlight slot, and its only alternate use, toggling
bit `0x40` of `f8`, is the same bit `CampaignMap_SetLocation` clears and
`CampaignMap_UpdateLocationTransition` sets on `gCampaignMap_pSpotlight`. The
alternate controller never creates that object, so whether it is valid at
runtime remains unproved.

`D_8016A2C9` (a toggle that diverts the controller to a raw call) is the
`+0xCB0` copy of the live `D_80169619`, but that live byte is only cleared by
`CampaignMap_SetLocation` and the raw diverted call is unresolved, so both
keep their offset names. The unconsumed alternate words and padding
(`D_8016A274`, `D_8016A27C`, `D_8016A28C`, `D_8016A29D`, `D_8016A29E`,
`D_8016A2A0`, `D_8016A2BE`, `D_8016A2C0`) also keep offset names in the
North American map, as labels of the commons block; several span more
than one live counterpart. No C declares them. `notes/semantic-symbol-map.csv`
records each name once per overworld image.

## Equal bytes do not identify one object

These alternate tables also equal the live tables at `0x801691A8` byte for
byte. That is four equal slices across the two module images, but **two
different storage addresses in each module**. The live and alternate
functions use different globals and have different unresolved-call
constraints. No table is aliased to the other or combined into one object.

The initializer therefore follows `AlternateLocation` and `AlternateExit`,
not the live `MapLocation` interpretation. The alternate readers constrain
the gate, input mask, destination, move value, confirm byte and unsigned
coordinate copies. Bytes they do not interpret remain explicit byte arrays
or scalar padding fields. They are not replaced with the live family's
camera or sprite names merely because the stored bits agree.

In particular, `pad17` is `0x80` in records 10-15, and the other padded
ranges include nonzero bytes. Every byte is initialized explicitly rather
than treating unobserved storage as zero or free space. High-bit flag
values and the unused-exit destination sentinel `16` are preserved.

## Symbols and unchanged raw calls

The old generated blob had interior labels `D_80169E60` and `D_80169E62`,
at the first record's `a` and `b` fields. No tracked C/header names either
label directly. The C consumers continue to address those fields through
`gCampaignMapAlt_aLocationTable[index]`.

After mapping, `gCampaignMapAlt_aLocationTable` is section-defined data at `0x80169E54`.
Splat's generated absolute alias for `D_80169E62` still has its original
address; the unreferenced generated `D_80169E60` storage label is no longer
emitted. There is no linker assignment overriding the C table definition.
No interior function symbol was found in this table's baseline ELF range.

This does **not** resolve the alternate controller's separate call into the
live table at `func_80169230`. That target and the other raw callees remain
unchanged, as documented in
[`campaign-map-records.md`](campaign-map-records.md). Moving table storage
and the adjacent state prefix into C is not evidence for relabeling those
calls as live entrypoints.
