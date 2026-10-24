# Regional text data views

These names describe existing PAL language state and Japanese glyph lookup
data. Their native providers are absent from the North American executable;
none is registered as an unqualified resident `global`.

| View | Images | Existing provider | Actual address |
|---|---|---|---|
| `gText_bLanguageIndex` | SLES_039.47 | `D_8009C02B` | `0x8009C02B` |
| `gText_bLanguageIndex` | SLES_039.48 through SLES_039.51 | `D_8009C02B` | `0x8009C44B` |
| `gText_awGlyphNibblePatterns` | SLPM_863.98 | `D_80090E28` | `0x80090CD8` |
| `gText_awGlyphColorMasks` | SLPM_863.98 | `D_80090E48` | `0x80090CF8` |

## PAL language selection

`Main_InitHardware` and `Main_RunBootSequence` initialize/reset the byte to
`BUILD_LANGUAGE_INDEX`. The debug editor reads it, writes the requested
selection and restores the build default when the stored byte reaches five.
The PAL options overlay reads and changes the same state before requesting
the language image.

Independent readers use it for card-art preview positioning, per-language
package sectors, script image IDs, result sprites and name-entry/free-duel
sprite variants. It is therefore the language index, not merely a preview
vertical-layout selector. The generic European build defaults to zero;
French, German, Italian and Spanish wrappers select one through four.
Existing dynamic selection remains intact rather than being replaced with
a compile-time language constant.

The header view expands to the original byte name. Both plain `u8` and
explicit `.data` declaration arms, the wrapper selectors, all arithmetic,
comparisons and stores remain unchanged after preprocessing. There is no
new range guard, enum, native storage definition or NA/JP export.

## Japanese glyph lookup data

`DuelEffect_RasterizeGlyph` reads the channel-selected colour mask and
combines it with each of sixteen nibble patterns while filling a scratchpad
lookup table. Each nibble of the glyph row selects one entry; the existing
shifted copy supplies the shadow.

Both original declarations are **halfword** arrays: `u16[16]` for patterns
and unsized `u16[]` for colour masks. The rasterizer's `u32 w` local promotes
a halfword load; it does not establish a word table. The aliases preserve
these types, bounds, native names and indexing exactly. No colour-table
length, palette meaning or new glyph layout is inferred.

The Japanese-only reader does not establish the role of superficially
similar North American data. The NA numeric-data suffix stays opaque and
address-based.

## Namespace and conservation

The seven registry rows use the validation-only
`regional/<image>/global` kinds documented in
[the semantic naming pass](semantic-naming-pass.md#regional-global-records).
Each canonical export aliases the already-existing provider in that
image's linker metadata. There are no new NA exports or declarations.

Header macros retain original native linkage and preprocessed C tokens.
Registered source/profile contexts, compiler assembly, MASPSX output,
native symbols and the complete NA executable/ELF are compared against the
accepted parent. Regional full-image acceptance remains the responsibility
of the pull request's image validation.
