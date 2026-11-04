# Alternate overworld table storage

`src/overlays/overworld/alternate_location.c` defines the sixteen
`AlternateLocation` records at `gCampaignMapAlt_aLocationTable`. The existing
`alternate_location.h` is both its owning header and the interface consumed
by `CampaignMap_PickAlternateExit` and `CampaignMap_UpdateAlternateLocation`.
It supplies the bounded extern and `ALTERNATE_LOCATION_COUNT`.

The record types are unchanged: 66 bytes per location, including four
12-byte exits at `+0x12`. The new data-only unit compiles separately into
both overworld images under `gcc_2_8_1_g0`. Neither alternate function body
nor its existing `-O0` profile changes.

## Exact extent and placement

| File range | Runtime range | Owner |
|---|---|---|
| `0x1E54-0x2274` | `0x80169E54-0x8016A274` | Sixteen C records, `16 * 66 = 1056 = 0x420` bytes |
| `0x2274-0x22C8` | `0x8016A274-0x8016A2C8` | C-owned alternate controller state |
| `0x22C8-0x3000` | `0x8016A2C8-0x8016B000` | Remaining generated data blob |

The state prefix is one packed `0x54`-byte record. It includes the two
`AlternateObject` pointers, rebuild-pending byte, transition duration,
location and transition flags used by the matching alternate controller.
Consumer-backed members carry `gCampaignMapAlt_` names (below); the rest keep
offset names. The two following
bytes, `gCampaignMapAlt_LocationPrev` and `D_8016A2C9`, are consumer-backed state too, but stay
with the raw tail because Splat's generated data object preserves this
non-word boundary only when it starts at aligned `0x22C8`.

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
| `0x8016A2B8` | `D_8016A2B8` | `gCampaignMapAlt_MoveState` | The exit picker stores `24` or the exit's step byte before returning a destination |
| `0x8016A2BC` | `D_8016A2BC` | `gCampaignMapAlt_Location` | Indexes the table, compared against 10, overwritten by a picked destination, stored `+ 32` into `D_800C4D82` on entry |
| `0x8016A2BD` | `D_8016A2BD` | `gCampaignMapAlt_bEnterSceneState` | Set to 1 when the picker returns the location with bit `0x8000`; bit 0x80 latches its one-time setup block |
| `0x8016A2C8` | `D_8016A2C8` | `gCampaignMapAlt_LocationPrev` | Receives the current location before each exit pick |

`D_8016A288` (an object whose `0x40` flag toggles on input bit 4) and
`D_8016A2C9` (a toggle that diverts the controller to a raw call) have no
live counterpart and keep their offset names. `notes/semantic-symbol-map.csv`
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
