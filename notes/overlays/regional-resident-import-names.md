# Regional resident import names in MODEL overlays

Many regional (Japanese and PAL) MODEL overlay symbol files bound a
resident callee under an address-style label: `func_80082D28`, or one
carrying the source family's language, such as `func_french_80082CE8` or
`func_spanish_80089928`.

**What they bind:** each label sits at that image's own resident address,
and the same region's `functions.csv` names that address as a Psy-Q or
game function, for example `GetTPage`, `ratan2` or `Model_SetSlotAmbient`.
The North American overlays bind those same names. The label language
records which family's source the import came from, not the region. For
example, the European English images bind `func_spanish_80082CE8` at
their own `GetTPage` address, `0x80082AE4`.

**No other users:** no C source, test or tool refers to these labels.
They are names only, for generated assembly and linking.

## Rule

For every regional `*model*symbols.txt` / `*model*linker_symbols.txt`
line whose name is `func_[language_]XXXXXXXX` and whose address is in the
resident image:

- **Drop the alias** when the same file already binds the region's name
  for that address.
- **Rename** when the file binds no other name there, but the region's
  `functions.csv` has a named function starting at that address. The
  region's name is used at the same address.
- **Leave it** in every other case: no resident function starts at that
  address, or no resident name exists there.

Addresses, sizes and every other line are unchanged.

## Results

| Shared name | Former labels | Lines | Renamed | Aliases dropped |
|---|---|---|---|---|
| `GetClut` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1253 | 883 | 370 |
| `GetTPage` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1199 | 865 | 334 |
| `GsGetLwUnit` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1226 | 875 | 351 |
| `GsSortGLine` | `func_spanish_…` | 153 | 133 | 20 |
| `GsSortPoly` | `func_spanish_…` | 146 | 120 | 26 |
| `Model_SetSlotAmbient` | `func_french_…` | 42 | 32 | 10 |
| `ReadRotMatrix` | `func_spanish_…` | 184 | 154 | 30 |
| `RotTransPers` | `func_french_…`, `func_spanish_…` | 983 | 745 | 238 |
| `RotTransPers3` | `func_french_…`, `func_spanish_…` | 402 | 335 | 67 |
| `RotTransPers4` | `func_spanish_…` | 1 | 0 | 1 |
| `ScaleMatrix` | `func_spanish_…` | 13 | 8 | 5 |
| `SetPolyF4` | `func_spanish_…` | 49 | 23 | 26 |
| `SetPolyFT4` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1253 | 883 | 370 |
| `SetPolyG3` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1252 | 883 | 369 |
| `SetPolyG4` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1252 | 883 | 369 |
| `SetPolyGT4` | `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1198 | 865 | 333 |
| `SetRotMatrix` | `func_spanish_…` | 184 | 154 | 30 |
| `SetSemiTrans` | `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1198 | 865 | 333 |
| `SetShadeTex` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1199 | 865 | 334 |
| `Square0` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1253 | 883 | 370 |
| `SquareRoot0` | `func_…`, `func_french_…`, `func_german_…`, `func_italian_…`, `func_spanish_…` | 1252 | 883 | 369 |
| `ratan2` | `func_spanish_…` | 168 | 96 | 72 |
| `rcos` | `func_french_…`, `func_spanish_…` | 386 | 295 | 91 |
| `rsin` | `func_spanish_…` | 326 | 245 | 81 |

That is 16,572 lines in 1,253 files: 11,973 renamed and 4,599 redundant
aliases dropped. Twenty further lines were exact same-name, same-address
duplicates already present on master, in eight touched linker files. They
were removed with the duplicate check.

## Left unchanged

- **`GsGetActiveBuff` at `0x800852A8`** (`func_800852A8`,
  `func_spanish_800852A8`): [psyq.md](../psyq.md) keeps these as
  placeholders.
- **Password and Main Menu import aliases:** `D_800EA0E8`, `D_801D4244`,
  `func_80029164` and `func_800291E0` are recorded as preserved legacy
  imports in [card-preview-resource-array.md](../card-preview-resource-array.md).
- **French `D_…_French` literal labels and resident `D_`/`func_` aliases:**
  these are resident-image choices documented in their own notes, for
  example [player-name-glyph-views.md](../player-name-glyph-views.md) and
  [duel-session-controls.md](../duel-session-controls.md).
- **Labels with no named function at their address** in that region's
  resident, such as `func_french_80082D28` in images whose resident places
  `GetClut` elsewhere.
