# Modding Tutorial Evidence

This note records claims recovered from the community modding tutorials
attached to issue #368. The tutorials describe edits from a hex-editor
perspective, so each claim is mapped back to resident code or data before it
is accepted.

Use this note with
[`modding-tutorial-gameplay-patches.md`](modding-tutorial-gameplay-patches.md),
which holds the companion batch of gameplay-focused patch analysis. Both files
preserve offset-level provenance and confidence; they are not a second gameplay
specification. Reconciled game behavior belongs in
[`research/the-game.md`](research/the-game.md), and tentative tutorial claims
must remain qualified when referenced there.

Confidence levels:

- **Confirmed**: the retail bytes, disassembly, and independently recovered
  symbols or behavior agree.
- **High**: the code and tutorial agree, but part of the surrounding semantic
  interpretation is not independently named.
- **Tentative**: the patch effect is plausible, but its target or broader
  behavior still needs corroboration.

## Initial duel life points

**Tutorial:** `Alterar LP - Por Jael Rivera.txt`

The tutorial identifies SLUS file offset `0x7DD0`, where the retail halfword
`40 1F` represents decimal 8000 in little-endian order. It suggests replacing
that halfword with another 16-bit value, for example `28 23` for decimal 9000.

The offset maps to VRAM `0x800175D0`:

```text
0x80010000 + (0x7DD0 - 0x800) = 0x800175D0
```

At that address, `func_800175A0` contains:

```mips
addiu $v0, $zero, 0x1F40
sh    $v0, 0x2($sp)
sh    $v0, 0x0($sp)
```

The function uses these two stack halfwords while initializing the two
`0x20`-byte duel-side records rooted at `D_800E9FF0`. For each side it:

- clears the displayed-life-point field at record offset `+0x12`;
- writes the selected initial value at offsets `+0x14` and `+0x16`;
- clears the remaining per-side display state.

The existing RAM-map-backed symbols identify:

| Record | Displayed LP (`+0x12`) | Authoritative LP (`+0x14`) | Maximum LP (`+0x16`) |
|---|---:|---:|---:|
| Player | `gDuel_wPlayerLifePointDisplay` (`0x800EA002`) | `gDuel_wPlayerLifePoint` (`0x800EA004`) | `0x800EA006` |
| Opponent | `gDuel_wOpponentLifePointDisplay` (`0x800EA022`) | `gDuel_wOpponentLifePoint` (`0x800EA024`) | `0x800EA026` |

Therefore the patch does not directly initialize the displayed LP to 8000.
It initializes both the authoritative and maximum LP, while the displayed
counter begins at zero and is updated afterward. Matching `func_800250C8`
independently establishes the maximum: LP recovery adds to the authoritative
value at `+0x14`, then clamps it to the `+0x16` value. The display counter at
`+0x12` follows the authoritative value separately through
`Duel_UpdateLifePointDisplay`.

The constant is used only when `gDuel_bOpponentID >= 0`, the ordinary duel
path. The negative-opponent-ID path is the 2P Duel setup: menu result `2`
dispatches to `func_8002DC38`, which initializes `D_8009B234` and
`D_8009B236` to 8000 and passes both addresses to the main-menu value editor
at `0x80180FD8`. Its matching input handler at `0x801812B4` lets pad 1 and
pad 2 adjust their respective values to `1` or a multiple of 500 from 500
through 8000. `func_800175A0` then initializes each duel side from those two
selected values, so changing `0x7DD0` does not affect 2P Duel.

**Confidence:**

- **Confirmed** that file offset `0x7DD0` is the 8000 immediate in
  `func_800175A0`.
- **Confirmed** that changing it changes the initial authoritative player and
  opponent LP values on the ordinary duel path.
- **Confirmed** that the adjacent `+0x16` fields are maximum LP values used to
  cap recovery.
- **Confirmed** that 2P Duel instead defaults both sides to 8000, exposes the
  two values to their respective controllers, and uses the selected values as
  each side's initial and maximum LP.

For a value representable by one 16-bit halfword, the tutorial's byte-edit
method preserves the instruction opcode: decimal 9000 is hexadecimal
`0x2328`, so the complete instruction changes from `40 1F 02 24` to
`28 23 02 24`.

## Millennium Eye opponent-card display

**Tutorial:** `Olho do Milenio.txt`

The tutorial identifies SLUS offset `0x8864`, whose retail bytes are
`FF 00 02 24`. The offset maps to `0x80018064`, inside the exact matching C
for `func_80018004`:

```text
0x80010000 + (0x8864 - 0x800) = 0x80018064
```

The instruction is:

```mips
addiu $v0, $zero, 0xFF
```

It supplies the final `field_67` value when the current duel-card display
state byte at `+0x1F` is negative. The matching source first selects the
card's normal display-data index plus one, then overrides that selection with
`0xFF` for the negative mode:

```c
if (D_8009B1C8->field_1F != 0) {
    object->flags |= 0x2000;
    result->field_67 = object->child->field_04 + 1;
    if (D_8009B1C8->field_1F < 0)
        result->field_67 = 0xFF;
}
```

`func_8003AAE4` later passes `field_67` to `func_8003A1EC`, which uses it as
the resource index for image lookup and display-object setup. Replacing the
immediate byte with `0x00` or `0x01` therefore changes the selected opponent
card image resource; it does not alter the duel's underlying card data.

`func_80017DB4` contains the same negative-mode `field_67 = 0xFF` override
for another card-object update path. The tutorial's one-byte edit does not
change that site, so static evidence alone does not prove that every
opponent-card object keeps the replacement index through all later updates.

The tutorial reports that value `0` exposes the opponent's card faces and
value `1` keeps the cards hidden while showing their order numbers. The
static call chain confirms that these values select different image
resources, but the exact appearance of those resource indices has not yet
been independently verified against extracted art or a runtime trace.
`tools/trace/opponent_card_resource_indices.lua` applies both values in one
bounded emulator run, records the resulting display-object field, watches for
later overrides, and restores the retail instruction byte afterward.

**Confidence:**

- **Confirmed** that `0x8864` is the low immediate byte of the negative-mode
  `field_67 = 0xFF` assignment in `func_80018004`.
- **Confirmed** that `field_67` becomes an image resource index and does not
  modify card identity or duel state.
- **Tentative** that resource indices `0` and `1` have exactly the two visual
  meanings reported by the tutorial; runtime or asset-level corroboration is
  still required.

## Dialogue pointer redirects

**Tutorial:** `Alterar Dialogo.txt`

The tutorial uses SLUS file offset `0x1A1C3B` as an example dialogue target
and derives bytes `3B 14` for an `FB 80` redirect. The result is correct, but
the reliable conversion is to remove the PS-X EXE header before encoding the
loaded address:

```text
loaded address = 0x80010000 + (0x1A1C3B - 0x800) = 0x801B143B
stored target  = 0x143B
little endian  = 3B 14
```

`FB 80` belongs to the text stream, not the campaign script opcode table.
The handler at `0x80038BF0` advances the text cursor by
`gDialog_bChoice * 2`, then calls `Text_SetCursorOffset` (`0x80038BA8`).
Exact matching C for `Text_SetCursorOffset` reads the selected 16-bit value
and replaces only the low halfword of the current text pointer:

```c
*p = (*p & 0xFFFF0000) | (v & 0xFFFF);
```

The redirect target must therefore remain in the cursor's current 64 KiB
bank. A real multi-choice block needs one little-endian target halfword per
possible choice. A single target works only when the selected choice index is
known to be zero, so the tutorial's four-byte `FB 80 xx xx` recipe is not a
general unconditional jump.

**Confidence:**

- **Confirmed** that file offset `0x1A1C3B` maps to loaded address
  `0x801B143B` and target bytes `3B 14`.
- **Confirmed** that `FB 80` indexes a table of 16-bit targets by the current
  choice and preserves the pointer's upper 16 bits.
- **Tentative** that inserting only one target after an arbitrary page wait is
  safe; that depends on the active choice state and the source text bank.

## Player name in the duel-result label

**Tutorial:** `Nome no Final.txt`

The tutorial gives runtime address `0x801B0514` and replacement bytes
`FC 5A 12`. That address maps back to SLUS file offset `0x1A0D14`:

```text
file offset = 0x800 + (0x801B0514 - 0x80010000) = 0x1A0D14
```

The retail bytes there begin `24 21 35 FF`. The recovered glyph mapping decodes
`24 21 35` as `YOU`, followed by the `FF` string terminator. This is the
player-side label in the duel-result text bank; the adjacent string contains
the `COM` label.

In this text format, `FC lo hi` inserts another string from the current text
bank. The same `FC 5A 12` sequence is used by the name-confirmation dialog,
where it inserts the string at `0x801B125A`. Matching C in
`SaveData_ApplyRuntimeState` fills that buffer from the saved player name
through `Text_SjisToGlyphCodes`.

Replacing only the three `YOU` glyph bytes with `FC 5A 12` therefore keeps
the existing `FF` terminator and makes the result label render the current
player name. It does not rename the save data or patch a general ending or
credits routine, despite the tutorial's broad title.

**Confidence:**

- **Confirmed** that `0x801B0514` contains the `YOU` glyph sequence and maps
  to SLUS offset `0x1A0D14`.
- **Confirmed** that `FC 5A 12` inserts the runtime player-name string at
  `0x801B125A`.
- **Confirmed** that the patched bytes belong to the player-side duel-result
  label rather than executable code.

## Global card and category name offsets

**Tutorials:**

- `Bloco pointeiros monstros e guardians.txt`
- `Offset Nomes.txt`

The first tutorial identifies SLUS offsets `0x1C6000` and `0x1C6600` as
pointer blocks for card names and for card-type, Guardian Star, and duelist
names. Both offsets belong to one table:

| SLUS offset | Resident address | Table position | Meaning |
|---:|---:|---:|---|
| `0x1C6000` | `0x801D5800` | entry `0x000` / string ID `0x8000` | Start of `gText_aGlobalOffsets` |
| `0x1C6600` | `0x801D5E00` | entry `0x300` / string ID `0x8300` | Start of the card-type labels within the same table |

Exact matching C in `Text_LookupString` indexes this table with
`string_id - 0x8000`, then combines the selected 16-bit offset with the
`0x801D0000` text-bank base. Entry zero points to an empty string. Entries
`0x001`-`0x2D2` correspond to card IDs 1-722, while the 45 entries through
`0x2FF` all point back to that empty string.

The `0x8300` subrange begins with the 24 card-type labels from `Dragon`
through `Equip`. IDs `0x8318`-`0x8321` are the ten Guardian Star names.
After six entries that alias the `Dragon` string, ID `0x8328` is
`Build Deck` and IDs `0x8329`-`0x834F` are the 39 duelist names.

The second tutorial's lone offset, `0x1C92CE`, maps to resident address
`0x801D8ACE`. The table entry for string ID `0x8300` contains offset
`0x8ACE`, and the bytes there decode through `gText_adwGlyphCodeTable` as
`Dragon` followed by the `0xFF` terminator. This is therefore the first
card-type string, not the start of the pointer table or of the 722 card names.
Changing its encoded length without relocating later strings and updating
their offsets would overwrite adjacent text.

**Confidence:**

- **Confirmed** that `0x1C6000` maps to `gText_aGlobalOffsets` and that
  `Text_LookupString` uses it for IDs `0x8000` and above.
- **Confirmed** that `0x1C6600` is entry `0x300` of that same table rather
  than an independent pointer block.
- **Confirmed** that `0x1C92CE` is the encoded `Dragon` label selected by
  string ID `0x8300`.

## Main-menu palette region

**Tutorial:** `Editar Menus.txt`

The tutorial identifies decimal offset `196608` in `SU.MRG` as the palette
used to edit the menus. That is file offset `0x30000`, or logical sector 96.
The independently recovered loader layout places the main-menu executable at
SU sectors `98-114`, beginning at `0x31000`, so the tutorial's offset lies in
the asset area immediately before the code rather than inside the overlay.

In the retail archive, all six `0x200`-byte chunks from `0x30000` through
`0x30BFF` contain nonzero 16-bit values consistent with PlayStation colour
data. The following `0x400` bytes at `0x30C00-0x30FFF` are zero padding before
the executable begins. The complete candidate region hashes to:

```text
SHA-256: 5b59103a270882b261ff9c13ba68060a90b3b01f9b5475da50af11dc5908ba19
```

**Confidence:**

- **Confirmed** that the tutorial's decimal offset is `SU.MRG+0x30000` and
  that this is sector 96, directly before the main-menu code at sector 98.
- **High** that the nonzero `0xC00`-byte region is main-menu palette data,
  based on the tutorial identification, archive placement, and 16-bit value
  shape.
- **Tentative** on individual palette boundaries and which menu elements use
  each range; those still require GPU-upload or draw-call evidence.

## WA menu background and symbol palettes

**Tutorial:** `Paleta Inicial.txt`

The tutorial identifies two image starts and two palette starts in
`WA_MRG.MRG`:

| Resource named by tutorial | WA offset | Package-relative offset |
|---|---:|---:|
| Menu background image | `0xFD3800` | `+0x00000` |
| Menu symbol image | `0xFE3800` | `+0x10000` |
| Background palette, 256 colours | `0xFEB800` | `+0x18000` |
| Symbol palette, 16 colours | `0xFEBA00` | `+0x18200` |

The resident loader in `func_8002F630` requests 50 sectors beginning at WA
sector `0x1FA7`. This is exactly the package
`0xFD3800-0xFEC800`:

```text
0x1FA7 * 0x800 = 0xFD3800
0x32   * 0x800 = 0x19000
```

Its exact matching callback, `func_8002F4C0`, divides the package at
`+0x18000`, the tutorial's first palette offset. The callback then uploads a
`256 x 2` rectangle of 16-bit colour values to VRAM `(0, 244)`. The
`0x200`-byte 256-colour palette therefore fills the first row, and the
16-colour palette at `+0x200` begins the second row at VRAM `(0, 245)`.

The two image starts likewise divide the front `0x18000` bytes into a
`0x10000`-byte first range and a `0x8000`-byte second range. Both ranges and
both palette spans contain retail data rather than padding:

```text
WA[0xFD3800:0xFE3800]  SHA-256 48c6a860dc1da811fc739de4a248e0fbe9ca92e78f6580d273a72609cc5e0b6f
WA[0xFE3800:0xFEB800]  SHA-256 58da7dcc3c46e6dcf3cc61854ad04c7f64c3a5509ce77fba9411c809e296ed75
WA[0xFEB800:0xFEBA00]  SHA-256 59a8f715e25f09a875747f647b700aefa0d78d759512400179406bf42449c2a7
WA[0xFEBA00:0xFEBA20]  SHA-256 6c98c1e36e44f6b060ef7c9fd751cb651dc3aea83c362fbfa1a627e10894c25a
```

**Confidence:**

- **Confirmed** that all four offsets belong to the fixed 50-sector package
  loaded from WA sector `0x1FA7`.
- **Confirmed** that the two palette offsets map to the first full row and
  the first 16 entries of the second row in the callback's VRAM upload.
- **High** that the two front ranges are respectively the menu background
  and symbol images; their boundaries and loader are exact, while the visual
  labels come from the tutorial.

## Card cursor and fusion-number graphics

**Tutorials:**

- `Apontador Cartas.txt`
- `Numeral Carta Fusao.txt`

The resident loaders place every offset from these tutorials inside one of two
known `WA_MRG.MRG` screen packages:

| Resource named by tutorial | WA offset | Package-relative offset | Loader region |
|---|---:|---:|---|
| Shared card pointer image | `0xB54000` | `+0xC000` from `0xB48000` | Boot UI image payload |
| Fusion-card numeral image | `0xB55C00` | `+0xDC00` from `0xB48000` | Boot UI image payload |
| Shared card-pointer palette | `0xB60840` | `+0x840` in the palette block at `0xB60000` | Boot UI CLUT upload |
| Fusion-card numeral palette | `0xB608A0` | `+0x8A0` in the palette block at `0xB60000` | Boot UI CLUT upload |
| Password card image | `0xF97800` | `+0x00000` from `0xF97800` | Password-screen image payload |
| Password cursor image | `0xFAB800` | `+0x14000` from `0xF97800` | Password-screen image payload |
| Password frame image | `0xFAF800` | `+0x18000` from `0xF97800` | Password-screen image payload |
| Password frame palette | `0xFB7A00` | `+0x200` in the palette block at `0xFB7800` | Password-screen CLUT upload |
| Password card/cursor palette | `0xFB8800` | `+0x1000` in the palette block at `0xFB7800` | Password-screen CLUT upload |

`func_80043960` requests the 54-sector boot UI package at WA sector `0x1690`,
which is byte range `0xB48000-0xB63000`. Its callback transfers an initial
`0x18000`-byte graphics region followed by the `0x1000`-byte block beginning
at `0xB60000`; that second block is uploaded to VRAM rectangle
`(512, 248, 256, 8)`. The two listed boot palettes therefore map to VRAM
coordinates `(544, 252)` and `(592, 252)`, respectively.

Likewise, `func_8003BEB8` requests the 86-sector password package at sector
`0x1F2F`, byte range `0xF97800-0xFC2800`. Its callback consumes a
`0x20000`-byte image region followed by a `0x2000`-byte block uploaded to
VRAM rectangle `(256, 240, 256, 16)`. The listed frame palette is one row into
that block at `(256, 241)`, while the shared card/cursor palette begins at
`(256, 248)`.

The package placement, sizes, and VRAM coordinates are confirmed by the
resident request and callback code. The individual visual labels come from
the tutorials and are high-confidence asset identifications; no image decoder
is needed to establish that the offsets belong to the stated image and CLUT
transfer regions.

## Boot UI colour ramps

**Tutorial:** `Cores Para Add e Como Add Cor Roxa.docx`

The tutorial identifies seven colour ramps at `WA_MRG.MRG+0xB61002`, spaced
by `0x20` bytes:

| Zero-based row | Tutorial label | Row start | First nontransparent colour |
|---:|---|---:|---:|
| 0 | White | `0xB61000` | `0xB61002` |
| 1 | Yellow | `0xB61020` | `0xB61022` |
| 2 | Blue | `0xB61040` | `0xB61042` |
| 3 | Green | `0xB61060` | `0xB61062` |
| 4 | Grey | `0xB61080` | `0xB61082` |
| 5 | Orange | `0xB610A0` | `0xB610A2` |
| 6 | Red | `0xB610C0` | `0xB610C2` |
| 7 | Empty | `0xB610E0` | `0xB610E2` |

Each row is one 16-colour PlayStation CLUT. The tutorial starts every listed
ramp at `+2` because the first halfword is the transparent colour entry. Its
seven "original game" byte sequences match the remaining 15 retail entries
exactly. The eighth row is entirely zero, corroborating the warning that
selecting the unused eighth colour slot makes affected text or symbols
disappear unless a complete replacement palette is supplied.

The loader independently confirms the region. `func_80043960` includes
`WA_MRG.MRG+0xB61000` in its 54-sector boot UI request, and
`func_80043328` transfers an `0x800`-byte tail beginning there before
uploading a `(640, 232, 16, 8)` rectangle. That upload consumes exactly the
first `0x100` bytes: eight rows times sixteen 16-bit colours. Bytes
`0xB61100-0xB617FF` are zero padding in the retail archive.

```text
SHA-256 of WA_MRG.MRG[0xB61000:0xB61100]:
93c8426458da03801fd7c8762b86d0d06e322525468f03c39e6a4306b345bf26
```

**Confidence:** Confirmed for the row boundaries, retail values, empty eighth
row, loader range, and VRAM upload geometry. The tutorial's suggested blue,
purple, red, orange, yellow, and green replacements are custom palettes rather
than retail data.

## Attribute icon images and palettes

**Tutorials:**

- `Offset e Paleta Atributos - Đỗ Thành Đạt.txt`
- `Detalhes cartas.txt`

The tutorial lists ten copies of the attribute icons: one in each of the seven
terrain records and one in each Library, Password, and Build Deck package.
Every image start is at package-relative offset `+0x1C000`:

| Context | Image offset | First palette offset |
|---|---:|---:|
| Normal | `0xB7F000` | `0xB84E00` |
| Forest | `0xBF4800` | `0xBFA600` |
| Wasteland | `0xC6A000` | `0xC6FE00` |
| Mountain | `0xCDF800` | `0xCE5600` |
| Meadow | `0xD55000` | `0xD5AE00` |
| Umi | `0xDCA800` | `0xDD0600` |
| Yami | `0xE40000` | `0xE45E00` |
| Library | `0xF02800` | `0xF08600` |
| Password | `0xFB3800` | `0xFB9600` |
| Build Deck | `0x10E0800` | `0x10EA600` |

`Detalhes cartas.txt` repeats these same ten image offsets under the label
"ATK & DEF Number" and, for each terrain package, also gives the address
`+0x200` from the listed start. Directly decoding the retail bytes as 4-bpp
graphics shows a row of nine distinct 32x16 emblems across the `0x900`-byte
region, consistent with the attribute labels below rather than a decimal
digit sheet. The `+0x200` addresses land inside that same atlas, at the start
of its second 32x16 tile; they are not separate assets.

The stated `32x16` dimensions and 16-color palettes imply `0x100` bytes per
4-bpp icon and `0x20` bytes per palette. The nine icon images therefore occupy
the first `0x900` bytes at each image offset, and the nine listed palettes
occupy `0x120` bytes at `0x20` strides from each first-palette offset.

All ten `0x900`-byte icon sequences are byte-identical:

```text
SHA-256: f02d8280cb698decff9234ce576ea2aadf0ed5355ba64a08f5116ba22844ae8f
```

All ten `0x120`-byte palette sequences are also byte-identical:

```text
SHA-256: 5997302376ade7d6bdba06dba04cb397598d37e23045556f28049bfa615e4b9c
```

The first seven offsets follow the known `0x75800` terrain-record stride.
Library and Password use the same `+0x1C000` image position in their fixed
packages. Build Deck also uses `+0x1C000`, but its palette sequence is
`+0x25E00` from the package start rather than `+0x21E00`. The tutorial's nine
labels—Light, Dark, Earth, Water, Fire, Wind, Spell/Equip, Trap, and
"Light 2"—are preserved as visual observations; the duplicate bytes and
archive placement are confirmed directly from the retail `WA_MRG.MRG`.

## Small and large card-frame palettes

**Tutorials:**

- `Cards.txt`
- `PALETAS E OFFSET PARA CARTAS.xlsx`

The tutorial's `0x200`-byte spacing identifies palette rows, not separate
card images. In each duel-terrain package, resident loader callback
`Duel_LoadPackageStage` reads the `0x2000`-byte block at package offset
`+0x20000` and uploads it as a `256 x 16` rectangle of 16-bit colours at
VRAM `(256, 240)`. Each `0x200`-byte step is therefore one complete
256-colour row.

The tutorial's twelve card-frame rows have this common package-relative
layout:

| VRAM row | Package offset | Card display | Card kind |
|---:|---:|---|---|
| 241 | `+0x20200` | Small | Normal |
| 242 | `+0x20400` | Small | Magic |
| 243 | `+0x20600` | Small | Trap |
| 244 | `+0x20800` | Small | Ritual |
| 245 | `+0x20A00` | Small | Fusion |
| 246 | `+0x20C00` | Small | Effect |
| 248 | `+0x21000` | Large | Normal |
| 249 | `+0x21200` | Large | Magic |
| 250 | `+0x21400` | Large | Trap |
| 251 | `+0x21600` | Large | Ritual |
| 252 | `+0x21800` | Large | Fusion |
| 253 | `+0x21A00` | Large | Effect |

Applying those relative offsets to the Normal, Forest, Wasteland, Mountain,
Meadow, Umi, Yami, and Library package starts reproduces every distinct
absolute offset in the tutorial. Their complete `+0x20000-+0x21FFF`
palette blocks are byte-identical:

```text
SHA-256: e7a7296c5a77ad4bc89577db20e50c05d7c6f8e2faa2625b587b6fa37f8c3463
```

The entries numbered 4 and 5 in `Cards.txt` repeat the Forest and Wasteland
offsets exactly; they are duplicate tutorial rows, not additional archive
copies.

The password-screen package starts at `0xF97800`. Its exact callback
`func_8003BD14` uploads the same `0x2000`-byte palette rectangle, and the
tutorial lists its six large-card rows at
`0xFB8800-0xFB93FF`. Five rows are byte-identical to the terrain and Library
copies. The large-Magic row differs only in its final `0x20` bytes, where the
password package uses sixteen `0x0001` entries instead of the shared row's
final sixteen colours.

The workbook independently lists the same ten package starts and absolute
large-card palette offsets. For the seven terrain packages plus Library and
Password, its six rows are at package offsets `+0x21000`, `+0x21200`,
`+0x21400`, `+0x21600`, `+0x21800`, and `+0x21A00`; the Build Deck copies are
shifted to `+0x25000` through `+0x25A00`. Its category headings differ from
`Cards.txt`: it labels `+0x21200` as Equip and repeats every `+0x21800` address
under both Magic and "purple Fusion." The workbook therefore corroborates the
row locations, but not six distinct category assignments.

**Confidence:**

- **Confirmed** that the listed offsets are 256-colour rows in the
  `+0x20000` palette block, based on both exact loader callbacks and their
  VRAM upload geometry.
- **Confirmed** that all eight full terrain/Library blocks are identical and
  that tutorial entries 4 and 5 are duplicates.
- **Confirmed** that the workbook's Magic and "purple Fusion" columns contain
  the same ten offsets.
- **High** that the row labels describe small and large card-frame variants;
  those visual roles come from the tutorial, while their card-kind ordering
  and palette boundaries are exact.

## Duel-results image and palette package

**Tutorial:** `Results - Paletas.docx`

The tutorial places the results-screen graphics at `WA_MRG.MRG+0xED5800`
and a second image range at `+0xEDD800`. Resident function `func_80020F4C`
requests 34 sectors beginning at WA sector `0x1DAB`, exactly covering
`0xED5800-0xEE6800`, during end-of-duel setup:

```text
0x1DAB * 0x800 = 0xED5800
0x22   * 0x800 = 0x11000
```

Its exact matching callback, `func_80020BE4`, assigns the front `0x10000`
bytes to the image-transfer phase. The two tutorial image offsets are
therefore the starts of the package's two `0x8000`-byte halves. The tutorial
associates the first half with the rank, statistics, card, and general
results backgrounds, and the second with the `YOU` win/lose graphics.

The callback next reads the `0x800` bytes beginning at `0xEE5800` and uploads
them as a `256 x 4` rectangle of 16-bit colours at VRAM `(0, 248)`. Every
tutorial palette offset is within this block and aligned to one
`0x20`-byte, 16-colour CLUT:

| Visual role from tutorial | WA palette offset(s) |
|---|---|
| Results background | `0xEE5800` |
| Rank background variants | `0xEE5820`, `0xEE5840`, `0xEE5880-0xEE59A0` at `0x20` strides |
| Statistics background | `0xEE59C0` |
| Results cards | `0xEE59E0` |
| `YOU`, 1P win | `0xEE5A00`; alternate at `0xEE5B00` |
| `YOU`, 1P lose | `0xEE5A20`; alternate at `0xEE5B20` |

The complete uploaded palette block hashes to:

```text
SHA-256: bc1bbeb55e28cef036ec35e90d232b070df8221d5d114b4116a9743498fe7776
```

The tutorial calls the two later `YOU` palettes repeats. They preserve the
same 15-bit BGR values, but they are not byte-identical: every nonzero entry
in the later copy has bit `0x8000` set. They are therefore alternate CLUTs
with the PlayStation STP bit enabled, not redundant duplicate data.

**Confidence:**

- **Confirmed** that the image and palette offsets belong to the
  34-sector end-of-duel results package.
- **Confirmed** that each listed palette is a 16-colour slot in the
  `256 x 4` VRAM upload.
- **Confirmed** that the later win/lose palettes differ only by the STP bit
  on their nonzero colours.
- **High** for the tutorial's visual labels within the two image halves; the
  package boundaries and palette locations are exact, but the image atlas has
  not been independently segmented.

## Skip the opening Heishin text segment

**Tutorial:** `Remover Heishin.txt`

The tutorial replaces three bytes at SLUS offset `0x1A1A7A` with
`FD 1C 13`. The file offset maps to loaded address `0x801B127A`, the start of
global text string 1350. Its retail bytes begin `F7 05 00`; the patch replaces
that first control with an `FD` text-stream jump.

The `FD` dispatch-table entry is `Text_SetCursorOffset` (`0x80038BA8`). Its
exact matching C reads a little-endian 16-bit target and replaces only the low
halfword of the current text pointer. The patched target is therefore:

```text
FD 1C 13 -> low halfword 0x131C -> 0x801B131C
```

The destination remains in the same `0x801Bxxxx` text bank and begins with
valid text controls (`FE F7 0A ...`). Relative to the byte after the inserted
three-byte jump, the patch skips `0x9F` bytes of the original text stream.

This is not a change to the campaign bytecode interpreter, opponent data, or
the sound driver. Any skipped Heishin, music, or title presentation is encoded
inside the bypassed text-stream region.

A controlled in-memory replay applied the three bytes before starting a new
game. The ordinary dim-room sequence—Heishin walking, finding the treasure,
and laughing—did not play. After name entry, the patched flow showed a black
screen with an empty text box while the end-of-scene music played, faded
through a main-menu-like image, and continued into the Simon Muran
interaction. The script confirmed that it applied the RAM patch; although its
cursor-jump breakpoint did not produce the intended detailed log, the human
observation establishes the visible skip and that the campaign remained
playable through the next scene.

**Confidence:**

- **Confirmed** that `0x1A1A7A` maps to `0x801B127A` and begins with retail
  bytes `F7 05 00`.
- **Confirmed** that `FD 1C 13` redirects the text cursor to `0x801B131C`
  while preserving its current 64 KiB bank.
- **Confirmed** by controlled replay that this removes the opening Heishin
  segment and continues through the transition into the Simon Muran scene.

## Campaign duel records and music selection

**Tutorials:**

- `Trocar Musica.txt`
- `Heishin Campanha.txt`
- `Offset Personagens - Por Jean Cabral.docx`
- `Offset Personagens - Por Jean Cabral 1.2.docx`

The tutorials identify nine-byte records embedded in the global text bank.
The Heishin record appears verbatim at SLUS offsets `0x1A5964` and
`0x1A5C93`:

```text
F8 0D 08 74 75 06 30 72 FF
```

`F8` dispatches through the 27-entry extended text-control table, where
subcommand `0x0D` resolves to exact matching C function
`Text_StartCampaignDuel`.
That function consumes the record in this order:

| Bytes | Meaning | Heishin value |
|---|---|---:|
| `F8 0D` | Extended text-control command and subcommand | `0x0D` |
| next byte | Opponent ID | `8` |
| next byte | Post-duel continuation selected for result index `0` | `0x74` |
| next byte | Post-duel continuation selected for result index `1` | `0x75` |
| next byte | Duel terrain | `6` (Dark) |
| next two bytes | Sound command, little endian | `0x7230` |
| `FF` | End of the containing text string | |

After storing those fields, the handler selects duel mode. `Main_RunDuel`
later indexes the two continuation values with `D_8009B362 * 2`. The
tutorial identifies index `0` as the win path and index `1` as the loss path;
the code independently confirms that they are separate post-duel
continuations.

The explicit `0x7230` value is later passed to `func_8003FF08`, the resident
sound-command wrapper. It must be edited as one little-endian halfword, not
as two independent music bytes. The tutorial's labels for the `0x71D0`-
`0x72C0` values are behavior reports; static code confirms the command values
but not every audible track name.

The revised character-offset tutorial labels the observed sound commands as:

| Command | Tutorial label |
|---:|---|
| `0x71D0` | Preliminary match |
| `0x71E0` | Finals match |
| `0x71F0` | Seto Kaiba |
| `0x7200` | Egyptian duel |
| `0x7210` | Mage duel |
| `0x7220` | High Mage duel |
| `0x7230` | Heishin |
| `0x7240` | Priest Seto |
| `0x7250`, `0x7260` | DarkNite / Nitemare |
| `0x7270` | Free Duel |
| `0x7280` | 3D battle |
| `0x7290` | Seto Kaiba 3D battle |
| `0x72A0` | Egyptian 3D duel |
| `0x72B0` | DarkNite 3D battle |
| `0x72C0` | Free Duel select |

These labels remain tutorial observations rather than static audio
identifications.

`Text_StartCampaignDuel` also chooses a second sound value from the opponent ID:
`0x7280` for IDs `9`-`16`, `0x7290` for ID `17`, `0x72B0` for ID `38`, and
`0x72A0` otherwise. Changing the record's explicit sound command therefore
does not replace every opponent-dependent sound selection made for the duel.

The campaign tutorial changes the first Heishin record from continuation pair
`74 75` to `75 61`. This leaves the opponent, terrain, and sound command
unchanged and redirects only the two post-duel paths. Because the same retail
record occurs twice, editing `0x1A5964` does not globally modify every
Heishin encounter.

The two character-offset documents also provide one record for every opponent
ID from `1` through `38`. Comparing all 38 listed byte sequences against the
untouched North American executable found 37 exact matches. The exception is
Bandit Keith: the tutorial lists:

```text
F8 0D 0C 7D 7C 00 D0 71 FF
```

but the retail record at SLUS offset `0x1A6A5C` is:

```text
F8 0D 0C 7C 7D 00 D0 71 FF
```

The opponent, terrain, and sound command are unchanged; only the two
post-duel continuation bytes are reversed in the tutorial.

The records are not all unique search anchors. Exact retail duplicates occur
for Heishin and Teana 2nd, three times each for Ocean, Forest, Mountain,
Desert, and Meadow Mage, and five times for Labyrinth Mage. The tutorial's
`0x1A1A8F` is a starting point for searching the dialogue bank, not the first
duel record itself; Simon Muran's record begins at `0x1A1FA6`.

**Confidence:**

- **Confirmed** that `F8 0D` dispatches to `Text_StartCampaignDuel` and that
  the record fields have the layout shown above.
- **Confirmed** that `30 72` is the little-endian sound command `0x7230` and
  that the two Heishin records occur at the listed SLUS offsets.
- **Confirmed** that 37 of the 38 character-tutorial records occur verbatim
  in the untouched SLUS, and that Bandit Keith's retail continuation pair is
  `7C 7D`, not `7D 7C`.
- **High** that `0x74` and `0x75` are respectively the win and loss
  continuations; the selection mechanism is exact, while the outcome labels
  come from the tutorial's observed behavior.
- **Tentative** for music names that have not been corroborated by an audio
  trace.

## Pharaoh Atem final-duel payload

**Tutorial:** `Pharaoh Atem no Ultimo Duelo.txt`

The tutorial writes a `0x642`-byte payload at SLUS offset `0x1B01BE`. That
range maps to `0x801BF9BE-0x801BFFFF` and ends exactly at file offset
`0x1B0800`, where the `0x801C0000` global string-offset table begins. The
payload therefore does not overwrite the offset table, but its destination is
not empty padding: the retail range contains 339 nonzero bytes through
`0x1B034D`. Applying the tutorial replaces existing tail text data and clears
the rest of the bank.

At payload offset `+0x94` (SLUS `0x1B0252`) is this campaign duel record:

```text
F8 0D 27 02 61 00 40 72 FF
```

Using the verified `F8 0D` layout, it selects opponent ID `0x27` (decimal
`39`), post-duel continuation IDs `0x02` and `0x61`, Normal terrain, and sound
command `0x7240`. Existing game research identifies opponent slot 39 as Duel
Master K. The payload does not add a new opponent identity or alter that
opponent's deck, model, portrait, or name; presenting the slot as Pharaoh Atem
depends on other data changes.

The large zero-filled tail is significant. The payload is sized to the
remaining text-bank space rather than just the encoded dialogue and duel
record, so it should be treated as a full replacement of that region, not as
a short insertion that can be combined blindly with other text mods.

**Confidence:**

- **Confirmed** that the payload exactly fills `0x801BF9BE-0x801BFFFF`
  without crossing into the string-offset table.
- **Confirmed** that its embedded duel record launches opponent slot 39 with
  Normal terrain and sound command `0x7240`.
- **Confirmed** that the destination overwrites existing nonzero retail text
  data.
- **Tentative** that the complete patch produces the tutorial's claimed
  Pharaoh Atem presentation without additional modified assets or opponent
  data.

## Attack-trigger trap thresholds

**Tutorials:**

- `Efeito trap.txt`
- `Offset de Traps e Magicas - Por Wladmir Ghost.txt`

Both tutorials identify the same six consecutive bytes beginning at SLUS
offset `0x8B724`. These are the initialized
`gDuel_abTrapAttackThresholds` table rather than instruction immediates and
map to resident addresses `0x8009AF24`-`0x8009AF29`:

| SLUS offset | Resident address | Retail byte | ATK threshold | Trap card |
|---:|---:|---:|---:|---|
| `0x8B724` | `0x8009AF24` | `5` | `500` | House of Adhesive Tape (`681`) |
| `0x8B725` | `0x8009AF25` | `10` | `1000` | Eatgaboon (`682`) |
| `0x8B726` | `0x8009AF26` | `15` | `1500` | Bear Trap (`683`) |
| `0x8B727` | `0x8009AF27` | `20` | `2000` | Invisible Wire (`684`) |
| `0x8B728` | `0x8009AF28` | `30` | `3000` | Acid Trap Hole (`685`) |
| `0x8B729` | `0x8009AF29` | `255` | `25500` | Widespread Ruin (`686`) |

The trap-selection path in `func_8001F0D0` scans indices `5` through `0`.
For each available trap it loads the corresponding byte from
`gDuel_abTrapAttackThresholds`, multiplies it by 100 using shifts and
additions, and compares that product with the attacking monster's calculated
ATK. The comparison accepts equal values, matching the documented "ATK less
than or equal to" effects.

The descending scan does not give the broadest trap priority. Every available
trap whose threshold still covers the attack replaces the current selection,
and the loop stops once it reaches an available threshold that is too low.
With the retail table's increasing thresholds, the result is the
lowest-threshold available trap that can destroy the attacker; Widespread
Ruin wins only when no narrower set trap qualifies.

That early stop also matters when editing the table. Non-monotonic values can
make a lower-index trap unreachable whenever an available higher-index trap
with a smaller threshold terminates the scan first. Keeping thresholds
nondecreasing from card IDs `681` through `686` preserves the retail
selection model.

After selecting an index, the function adds `0x2A9` (decimal `681`) to
produce the trap card ID. This independently fixes the table order to the
first six trap cards. The retail value `255` gives Widespread Ruin an
effective `25500` threshold, which is why it behaves as the unrestricted
attack-trigger trap under normal duel stats.

Each edit changes a table byte directly. For example, a desired threshold of
`5000` uses decimal byte value `50`, encoded as hexadecimal `0x32`; writing
decimal `50` as hexadecimal `0x50` would instead select `80 * 100 = 8000`.
Values above `25500` cannot be represented by this one-byte table.

**Confidence:**

- **Confirmed** that `0x8B724`-`0x8B729` are the six threshold bytes consumed
  by `func_8001F0D0`.
- **Confirmed** that each value is multiplied by 100 and compared inclusively
  with calculated ATK.
- **Confirmed** that indices `0`-`5` map to card IDs `681`-`686` in the trap
  order shown above.
- **Confirmed** that, with the retail table order, simultaneous qualifying
  traps are prioritized from the lowest sufficient threshold upward.

## Guardian Star bonus magnitude

**Tutorial:** `Guardian Star V2 - Por Rafael Ferreira.txt`

The tutorial identifies three little-endian `500` immediates in the SLUS:

| SLUS offset | VRAM | Retail bytes | Instruction role |
|---:|---:|---|---|
| `0x10770` | `0x8001FF70` | `F4 01 42 28` | Test whether the displayed modifier is below `500` |
| `0x10784` | `0x8001FF84` | `F4 01 02 24` | Clamp the displayed modifier to `500` |
| `0x1D3D0` | `0x8002CBD0` | `F4 01 02 24` | Return the positive Guardian Star matchup modifier |

The first two sites are in `func_8001F55C`. That path advances a signed
display value by `16` per update, clears its active flag once the value
reaches the limit, clamps it to `500`, and writes the result to two display
fields. Both immediates must change together to preserve the same stop and
clamp value.

The third site is the positive return in the exact matching
`Duel_CalcGuardianStarMatchup`. Guardian Star IDs `1`-`6` form one advantage
cycle and IDs `7`-`10` form another. Adjacent values in one direction return
`500`, adjacent values in the opposite direction return `-500`, and all
other pairs return zero.

The tutorial changes only the positive `500` return. The negative immediate
is a separate instruction at SLUS offset `0x1D3E4`, with retail bytes
`0C FE 02 24` for `-500`. A patch that changes the three listed positive
values but leaves `0x1D3E4` untouched makes favorable and unfavorable
matchups asymmetric.

The tutorial's replacement table correctly treats the value as a 16-bit
little-endian integer: for example, decimal `1000` is hexadecimal `0x03E8`
and is written as bytes `E8 03`.

**Confidence:**

- **Confirmed** that the first two offsets are the display stop/clamp pair.
- **Confirmed** that `0x1D3D0` and `0x1D3E4` are the positive and negative
  matchup returns.
- **Confirmed** that changing only the tutorial's three offsets leaves the
  negative matchup modifier at `-500`.

## Spellbinding Circle and Shadow Spell reductions

**Tutorial:** `Shadow Spell e Spellbinding Circle.txt`

The tutorial replaces two paired signed immediates in `func_80025D30`:

| Spell | Role | SLUS offset | Retail bytes/value | Tutorial bytes/value |
|---|---|---:|---|---|
| Spellbinding Circle | Active-card modifier | `0x16660` | `0C FE` / `-500` | `44 FD` / `-700` |
| Spellbinding Circle | Effect-record modifier | `0x1666C` | `0C FE` / `-500` | `44 FD` / `-700` |
| Shadow Spell | Active-card modifier | `0x16680` | `18 FC` / `-1000` | `F1 D8` / `-9999` |
| Shadow Spell | Effect-record modifier | `0x16688` | `18 FC` / `-1000` | `F1 D8` / `-9999` |

The function selects the Spellbinding Circle branch when the current effect
card ID is `0x15D` (decimal `349`). It subtracts `500` from the target
`DuelCardRecord.stat_modifier` at offset `+0x12`, then stores the same
`-500` value in the paired effect record. The alternate branch performs the
same two writes with `-1000`, matching Shadow Spell's stronger two-level
reduction documented by the card behavior research.

The first value in each pair changes the duel state used by card-stat
calculation. The second keeps the associated effect/display record aligned
with that reduction. Editing only one offset can therefore make the applied
modifier and its effect state disagree.

The tutorial values are signed 16-bit little-endian integers:

```text
-700  = 0xFD44 -> 44 FD
-9999 = 0xD8F1 -> F1 D8
```

Both replacements fit in the signed halfword written by the retail code.

**Confidence:**

- **Confirmed** that the retail pairs are `-500` and `-1000`.
- **Confirmed** that the first write updates the active card's
  `stat_modifier`.
- **High** that the second write is the visual/effect-state counterpart; its
  record is populated from the card's display object immediately before the
  paired modifier write, but its complete layout remains unnamed.

## Force Exodia wins to S-TEC

**Tutorial:** `Exodia Sempre S-Tec - Por Wladmir Ghost.txt`

The tutorial changes the same signed end-reason value at three code sites:

| SLUS offset | VRAM | Retail instruction | Role |
|---:|---:|---|---|
| `0x8BD0` | `0x800183D0` | `addiu $v0, $zero, 40` | Recognize the Exodia end reason in the summon sequence |
| `0x9BDC` | `0x800193DC` | `addiu $v1, $zero, 40` | Write the winner's Exodia end-reason byte |
| `0x11DF4` | `0x800215F4` | `addiu $v0, $zero, 40` | Recognize Exodia while building the duel result/rank display |

Retail uses end reason `+40` for Exodia. The writer stores only its low byte,
and both readers load that field as a signed byte. Replacing each immediate's
little-endian bytes `28 00` with `81 FF` changes the shared value to `-127`
while keeping all three producer/consumer sites synchronized.

`Duel_CalcRankScore` starts each score at `50`, adds the signed end-reason
byte, then adds the ten statistic-table adjustments. Existing exhaustive
rank research gives a retail maximum of `+139`; because an Exodia reason of
`+40` contributes to that maximum, the other terms can contribute at most
`49`:

```text
patched maximum = 50 - 127 + 49 = -28
```

A score of `9` or lower maps to S-TEC, so the patched Exodia result remains
S-TEC regardless of the other tracked duel statistics. Updating the two
comparison sites as well as the writer preserves the Exodia-specific summon
and result-display branches despite changing the score contribution.

**Confidence:**

- **Confirmed** that all three retail immediates are the same signed
  end-reason value.
- **Confirmed** that `81 FF` encodes `-127` and is stored/read as a signed
  byte.
- **Confirmed** from the score bounds that the patched value always produces
  S-TEC.
- **High** that both comparison sites preserve the complete Exodia-specific
  presentation path; their branches are clear in resident assembly, while
  the surrounding state machines remain only partially named.

## Library 3D-card crash workaround

**Tutorial:** `Tutorial Bug do Library e Bug 3D.txt`

The tutorial writes `5C 02 3C` beginning at SLUS offset `0x1B5ED`. The byte
immediately before that range is zero, so the complete instruction at
`0x1B5EC` changes from:

```text
00 00 00 00    nop
00 5C 02 3C    lui $v0, 0x5C00
```

That instruction maps to `0x8002ADEC` in `func_8002ACA4`, which is reached
from the Library menu update path. Retail code has just loaded the selected
card's packed word from `gDuel_adwCardStats[card_id - 1]`. It then extracts
bits `26`-`30` and calls `Model_LoadMonsterMerge` only when the value is less
than `CARD_TYPE_MAGIC` (`20`):

```text
type = (gDuel_adwCardStats[card_id - 1] >> 26) & 0x1F
if (type < 20)
    Model_LoadMonsterMerge(...)
```

The inserted `lui` overwrites the loaded word with `0x5C000000`. Its type
field is `23`, the `CARD_TYPE_EQUIP` value, so the comparison fails and the
monster-model load is skipped. The patch therefore avoids the reported crash
by suppressing this 3D setup path; it does not repair the card-name pointer
described in the first half of the tutorial. It also applies at the shared
decision point rather than only to the malformed card.

**Confidence:**

- **Confirmed** that the three-byte edit completes the instruction
  `lui $v0, 0x5C00` at `0x8002ADEC`.
- **Confirmed** that the replacement produces type value `23` and bypasses
  the `Model_LoadMonsterMerge` call.
- **High** that bypassing this call is the complete reason the tutorial's
  reported Library crash disappears; the failure itself has not been
  reproduced with the external card editor.
