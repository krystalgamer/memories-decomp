# Data Crystal research guide

The Data Crystal pages for Forbidden Memories contain useful community labels,
enumerations, and disassembly. They should be treated as an index for local
verification, not as authoritative build metadata. This note records how the
three supplied pages map onto the NTSC-U repository.

## Sources

- [Notes](https://datacrystal.tcrf.net/wiki/Yu-Gi-Oh!_Forbidden_Memories/Notes)
- [Text table](https://datacrystal.tcrf.net/wiki/Yu-Gi-Oh!_Forbidden_Memories/TBL)
- [ROM map](https://datacrystal.tcrf.net/wiki/Yu-Gi-Oh!_Forbidden_Memories/ROM_map)

All three pages identify the NTSC-U `SLUS-01411` release. Their contents are
community-maintained and may combine static analysis, runtime experiments, and
labels whose evidence is no longer available.

## Address coordinate warning

The ROM-map page says its addresses come from an extracted bin, but entries
such as `0x12B50` are the low 24 bits of runtime address `0x80012B50`, not the
byte offset of that code in `SLUS_014.11`. The local executable maps:

```text
runtime address = executable file offset + 0x8000F800
```

For example:

| Data Crystal coordinate | Local runtime address | `SLUS_014.11` file offset |
|---:|---:|---:|
| `0x12B50` | `0x80012B50` (`Main_Init`) | `0x3350` |
| `0x12C44` | `0x80012C44` (`srand` call) | `0x3444` |
| `0x8E590` | `0x8008E590` (Psy-Q `rand`) | `0x7ED90` |

Do not apply these coordinates directly to a raw disc image, an archive, or
the executable file. Resolve the storage format first:

- Resident code/data: use the executable image map.
- `WA_MRG.MRG`, `MODEL.MRG`, or `SU.MRG`: use archive sector and member
  boundaries.
- Runtime overlays: identify the loaded module because several modules reuse
  the same `0x801xxxxx` address.

The ROM-map disassembly is still valuable as a historical annotation source.
For example, its `0x12C44` entry correctly identifies the
`srand(0x55555555)` call in `Main_Init`.

## Enumerations from the Notes page

The Notes page provides labels for several integer domains. These are good
candidates for enums once their consumers are identified:

### Main modes

| ID | Community label |
|---:|---|
| `0x00` | New Game |
| `0x01` | Load |
| `0x02` | 2P Duel |
| `0x03` | Trade |
| `0x04` | Option |
| `0x05` | Campaign |
| `0x06` | Free Duel |
| `0x07` | Build Deck |
| `0x08` | Library |
| `0x09` | Password |
| `0x0A` | Save |
| `0x0B` | Debug Menu |

These labels broadly agree with the locally recovered `Main_Run*` dispatcher
functions. The IDs should be confirmed at the dispatch table before defining a
shared enum.

### Terrain

| ID | Terrain |
|---:|---|
| `0x00` | Normal |
| `0x01` | Forest |
| `0x02` | Wasteland |
| `0x03` | Mountain |
| `0x04` | Meadow |
| `0x05` | Sea |
| `0x06` | Dark |

### Card type

IDs `0x00` through `0x13` cover monster types from Dragon through Plant.
`0x14`, `0x15`, `0x16`, and `0x17` are Magic, Trap, Ritual, and Equip. These
values can help type card-record fields, but local table layout and consumers
must establish the field width and ownership.

### Music

The page labels tracks `0x00` through `0x38`, including title/menu music,
character and location themes, duel tiers, win/lose cues, and campaign
cutscenes. Use these labels to annotate arguments to the sound frontend only
after distinguishing music IDs from sound-effect IDs.

### Scenes

The scene list associates sparse IDs with visible text or story events. Useful
anchors include:

| ID | Community label |
|---:|---|
| `0x002` | Player name entry |
| `0x040`-`0x045` | Post-duel result text |
| `0x0BC`-`0x0E0` | Memory-card prompts and results |
| `0x500` | Introduction |
| `0x543` | Heishin first rematch |
| `0x560` | Pegasus duel |
| `0x5C8` | Heishin duel |
| `0x5D4`-`0x5D7` | Nitemare duel through ending |

Some entries are explicitly marked unknown, glitchy, or softlocking. A label
describes observed content, not necessarily a unique game state or callable
scene module.

## What the TBL page represents

The TBL page maps compact display glyph indices to characters. It is not a
Shift-JIS byte table despite the game also using Shift-JIS for dynamic text.
The nonalphabetical indices make this clear: uppercase `A` is glyph `0x18`,
`B` is `0x2D`, lowercase `a` is `0x03`, and digit `0` is `0x38`.

This agrees with the runtime text model:

- `gText_adwGlyphCodeTable` at `0x801D9000` maps glyph indices to Shift-JIS
  codes.
- `Text_SjisToGlyphCodes` converts a Shift-JIS string, such as the player name,
  into the game's glyph-code stream.
- Stored message data can therefore use compact glyph indices while dynamic
  input remains Shift-JIS.

The TBL page contains suspicious duplicate character labels, including two
entries each for `<` and `>`, and `0x55 = a`. Preserve the numeric value when
testing those entries; do not normalize or silently correct the community
table.

## Verification workflow

1. Record the Data Crystal URL and exact claimed value.
2. Convert its coordinate into the correct local address space.
3. Locate the resident function, archive member, or overlay module.
4. Confirm the value through local callers, table accesses, strings, or a
   runtime trace.
5. Add a semantic name only when the evidence meets
   `notes/naming-conventions.md`.

Large copied tables should not become a second source of truth in the
repository. Prefer a small confirmed enum or symbol change tied to its actual
consumer, and keep Data Crystal attribution in the semantic evidence record.

