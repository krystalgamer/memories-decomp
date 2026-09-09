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

## Overlapping symbols: the ordering-table pointers

`c_symbols.ld` gives three consecutive words their own names:

```text
D_800E9D90 = 0x800E9D90;
D_800E9D94 = 0x800E9D94;
D_800E9D98 = 0x800E9D98;
```

They are not three objects. `graphics_frame.c` declares the first as
`u32 *D_800E9D90[4]`, and the overlays index it, so `D_800E9D90[1]` *is*
`D_800E9D94` and `D_800E9D90[2]` *is* `D_800E9D98`. Both spellings are in the
tree at once:

| Reached as | Where | Declared |
| --- | --- | --- |
| `D_800E9D90[1]` | `frontend.c` and two more | `GsOT *D_800E9D90[]` |
| `D_800E9D94` | `trade_screen_helpers.c` | `GsOT *D_800E9D94` |
| `D_800E9D90[2]` | `value_setup.c`, `trade_offers.c` | `GsOT *D_800E9D90[]` |
| `D_800E9D98` | `display_projection.c` | `void *D_800E9D98[]` |

The element names are not decompiler noise. Retail materializes them itself:
`func_8004CB0C` loads the third word as `lui %hi(D_800E9D98)` /
`lw %lo(D_800E9D98)`, not as a displacement off `D_800E9D90`. A file that
reaches the word by its own name and a file that reaches it as an element of
the array are both reproducing what retail did, which is why eleven files
declare this storage six different ways and none of them is simply wrong.

`fade_draw_overlay.c` is the case that shows the overlap can be wider than one
word: it declares `s32 D_800E9D94[4]` and reads `[0]`, a view that nominally
spans `D_800E9D98` and beyond.

Two consequences for the data work:

- Do not "unify" these declarations. The array view and the element names are
  different addressing forms of one object, and the build depends on which one
  each translation unit uses.
- A symbol-size heuristic based on the gap to the next symbol will understate
  this array badly: it reports four bytes because the next *name* is four bytes
  away, while the object is at least sixteen. Any tool that infers sizes that
  way needs to treat consecutive `D_` names as possible interior elements.

## Overlapping symbols reached by offset from another symbol

The section above finds overlaps by reading the symbol table. A second kind is
invisible that way, because the source never writes the name of the object it
is using. `func_8001BAF0.c` reached the deck records like this:

```c
base = (u8 *)gDuel_aActiveCards;
deck = base - 0x31E0;
```

`symbols.txt` puts `gDuel_aActiveCards` at `0x801AB000` and
`gDuel_aDeckCardRecords` at `0x801A7E20`, which differ by exactly `0x31E0`. So
the file was walking a named array by negative offset from a different named
array, and then defining its own struct for records that already had a shared
type. Grepping for the destination's name finds nothing.

Subtracting addresses is therefore a cheap identity test that no name search
can do. Two more instances are confirmed:

- `duel_rewards.c` builds `gDuel_awPlayerDeck + 0x5BC` in `Duel_AwardCard`,
  and `0x801D07BC - 0x801D0200` is exactly `0x5BC`, so that destination is
  `gDuel_awRecentCardDrops`.
- `sound_output_state.c` reads `state + 0x40` where `state` is `g_SDValue`.
  `g_SDValue` is at `0x8009B45C` and `D_8009B49C` is `0x40` later, so that
  name lies **inside** the `g_SDValue` record rather than beside it.

The last one is a true overlap in the sense the section above uses: one region
carrying two names. The other two are a file declining to name what it is
using, which is a different problem with the same tell.

### What a scan for this has to handle

Two failure modes are worth recording, because both produced confident and
wrong answers before the third attempt worked.

**Value arithmetic looks like address arithmetic.** Matching `SYMBOL + CONST`
against the symbol table returns nine hits in this tree and every one is
spurious. `D_8009B066 = (D_8009B066 + 1) & 1;` increments a counter, and it
"lands on" `D_8009B067` only because consecutive symbols are one byte apart.
Restricting to symbols declared as arrays, whose name decays to an address,
removes all nine.

**The address is usually copied to a local first.** With that restriction the
same scan returns *zero* — including for `func_8001BAF0.c`, the case the scan
was written for, because `gDuel_aActiveCards` and `0x31E0` are on separate
lines. A zero there measured the reach of the scan rather than the state of the
tree. One hop of alias tracking, recording locals assigned an array symbol's
address, brings back the founding case and the three above.

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
states; they must not be attributed wholesale to WA. See
`notes/overlays/runtime-loader.md` for the loader trace and recovered sector
layouts.

### Duel card storage inside the SU bank

Matching duel C establishes three contiguous runtime ranges near the end of
the SU slot:

| Address range | Size | Exact organization |
|---|---:|---|
| `0x8018C2D8-0x801A7AD8` | `0x1B800` | 80 per-deck-slot card-data blocks of `0x580` bytes |
| `0x801A7AD8-0x801A7E20` | `0x348` | 30 field/card records of `0x1C` bytes |
| `0x801A7E20-0x801A8000` | `0x1E0` | 80 `DuelDeckCardRecord` entries of 6 bytes, player then opponent |

`Duel_PopulateCombinedDeckData` fills the first range in combined-deck order,
copying one selected `DUEL_CARD_DATA_BLOCK_SIZE` (`0x580`) block for each of
the 80 slots and assigning that slot's `data_block_index`. The block itself
has an exact GPU-transfer split in `Duel_SetupCardRecord`: a `20 x 32`-word
image occupies the first `0x500` bytes, and a `64 x 1`-word palette occupies
the final `0x80` bytes.

The same setup function stores a pointer to the selected 6-byte deck record in
the field record's `data` word, then multiplies its `data_block_index` by
`0x580` to select the image and palette block. This establishes a three-level
field-record to deck-record to card-data relationship without assigning one
structure type to the whole SU allocation.
