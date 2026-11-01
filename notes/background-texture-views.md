# Auxiliary activity and packed backdrop texture views

The VRAM-copy animation and ordinary backdrop renderer use the auxiliary
model slot and selected background record. These names describe three
existing views, without new rendering or slot-lifecycle logic:

| Original | Canonical view | Unchanged representation |
|---|---|---|
| `D_800F569F` | `gModel_bAuxiliaryActive` | incomplete `u8[]`, read at `[0]` |
| `ModelBackgroundRecord.field_A8` | `texture_height_flags` | `u16`, `+0xA8` |
| `ModelBackgroundRecord.field_AA` | `texture_u_page` | `u16`, `+0xAA` |

## Activity is an interior byte, not a new object

The slot extent and active member are already asserted as `0xE20` and
`+0xE1F`. Every actual provider satisfies
`active = slots + 2 * 0xE20 + 0xE1F`, or `slots + 0x2A5F`:

| Image | Slots base | Existing activity provider |
|---|---|---|
| North America | `0x800F2C40` | `0x800F569F` |
| Japan | `0x800F04E0` | `0x800F2F3F` |
| United Kingdom | `0x800F3D70` | `0x800F67CF` |
| France | `0x800F4178` | `0x800F6BD7` |
| Germany | `0x800F4178` | `0x800F6BD7` |
| Italy | `0x800F4178` | `0x800F6BD7` |
| Spain | `0x800F4178` | `0x800F6BD7` |

The French `D_800F569F_French` label at the North American value is not the
actual `D_800F569F` provider. It is not used to qualify the alias.

Aggregate reset/load paths write slot 2's `is_active`; the background
renderer reads it through the aggregate. `Model_StepBackgroundVramAnimation`
retains the separate native-label read and its nonzero gate before choosing
the existing selector-1/selector-4 VRAM-copy paths.

The canonical C declaration has no assembler label and the old
incomplete-array declaration is gone from the owning header.
Its former central declaration is retired rather than duplicated.
Each image exports the
canonical alias through its existing linker-symbol mechanism. This neither
allocates another byte nor folds the native relocation into a slot-base
offset. Raw byte values and partial reset/activation behavior are unchanged.

## Packed height and texture flags

The renderer uses `texture_height_flags & 0x1FFF` for vertical clipping and
texture edges. Bits 13 and 14 select U scaling and contribute texture-page
depth bits. The attribute path tests **all three** high bits with `0xE000`;
bit 15 is not discarded or given a guessed meaning.

The word is not a plain height, a new two-bit enum, or proof of an allocation
extent. All masks, shifts, signed pitch views, truncations, clipping order
and the existing texture-depth input contract remain unchanged.

## Packed U and page

The low six bits of `texture_u_page`, shifted according to the packed depth,
produce the U base. The low ten bits masked by `0x3FF` and shifted by six
contribute texture-page X bits. The raw `u16` stays intact; high ignored
bits are not asserted to be zero or sanitized.

The existing `texture_v_page` union keeps its separate byte and halfword
reads. Record/coordinate reads before the activity guard, sprite setup,
CLUT fields, width handling, floor bands and submission order are untouched.

## Conservation

The native activity declaration/relocation and all original object symbols
remain unchanged; only one canonical absolute alias is added per final image.
Consequently the executable can remain byte-identical without claiming that
the final ELF symbol table is byte-identical.

Existing background-renderer and ownership witnesses retain their complete
bytes, including raw record offsets, depth/page/channel inputs, clipping
cases, mutation controls and compiler flags. No new harness, fixture,
guard, source group, SDK declaration or compiler profile is introduced.
