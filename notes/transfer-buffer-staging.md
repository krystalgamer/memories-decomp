# Transfer buffer address and package staging

These three existing identities connect transfer descriptor windows,
package uploads and model metadata/palette staging. They describe existing
address/view roles, not new storage or independent allocations.

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009B118` | `gFile_TransferBufferBase` | stored address: `s32` / `.data` / `u8 *G32` `.data` |
| `D_801DD000` | `gFile_abPackageStagingBuffer` | incomplete `u8[]` |
| `D_801DE000` | `gModel_abTexturePaletteStaging` | incomplete `u8[]`, interior palette view where established |

## Stored transfer-buffer address

`File_InitTransferState` is the only C writer and stores its existing
signed-word argument. `File_SetPositionTable` supplies the address of
`gLibrary_aCardArtRecord`. The word holding that address is separate from
the arena it points to; the rename does not retype the initializer or
change that API.

Descriptor producers and `SD_ConfigureTransferPhase` use the stored address
as a base or select the next sector with `+ FILE_SECTOR_SIZE`. This is not
a two-sector allocation claim: boot callbacks also access
`base + 2 * FILE_SECTOR_SIZE`, and preview uploads use offsets `0x2640`,
`0x2840` and further image offsets.

The new declaration header retains the original guard precedence:
`D_8009B118_IS_POINTER_IN_DATA` selects `u8 *G32` with `.data`;
otherwise `D_8009B118_IN_DATA` selects signed `s32` with `.data`;
otherwise the declaration remains plain signed `s32`. No scalar-to-pointer
conversion, new cast, sign change or common declaration normalization is
introduced.

The canonical buffer header is included after the original `unmatched.h`
declaration owner, so late pointer guards (notably the preview uploader's)
have already been selected. Actual compiler-expanded legacy/canonical
declarations agree in all 60 source/profile contexts. A linked-byte match
alone is not used to justify a different declaration view.

## Shared package staging, not image-only storage

`MainMenu_LoadPackageStage` first stages two sectors at the shared array,
then passes its base to `LoadImage2`. Model package callbacks similarly
upload texture/CLUT rectangles from the same view and reuse it in later
phases.

The role is broader than images. `Model_LoadMonsterMergeStage` stage 16
copies `sound_entries` and the existing CF8 metadata block from that same
base. An image-only name would omit an actual consumer, so the canonical
name is package staging. The metadata fields and their meanings are not
renamed here.

## Model palette interior view

In `Model_LoadMonsterMergeStage`, stage 3 advances a local source by
`2 * FILE_SECTOR_SIZE` after the first staged image payload, then sets up
the following transfer at that address. NA automatic bindings and four
tracked PAL configurations establish that this is the
`gModel_abTexturePaletteStaging` interior view at staging base `+0x1000`.

The non-alternate stage 4 consumes that view through `LoadImage2` with the
existing `256 x 1` palette rectangle and model palette coordinates.
The alternate route instead reads the staging base in stage 6; that
different expression is not rewritten to the palette name.

The producer's symbol-plus-offset expression stays as written. Neither
alias receives a sized array or a new extent assertion. These views can
overlap each other and the transfer pointer's default arena; the names
do not claim exclusive ownership or disjoint regions.

## Native providers and declaration surfaces

The address-holding word retains its original logical provider in all
seven images: NA `8009B118`, Japan `8009B008`, English PAL `8009C090`,
other PAL `8009C4B0`.

Both arena views are bound directly at `0x801DD000` / `0x801DE000` in
every region's `symbols.txt` or `link_symbols.ld`. The former
`D_801DD000` / `D_801DE000` providers and their alias lines are gone.
English PAL and Japan, which used to inherit the addresses from their
automatic symbols, now carry the same numeric bindings; all seven resident
images still match byte-for-byte.

The scalar/pointer declaration family and the incomplete arena declarations
live in separate isolated headers. This lets the existing model-texture
witness keep its independently declared pointer fixture while importing
only the renamed arena declarations. `file_transfer.h` declares the same
two arrays under their canonical names.

## Preservation and controls

The component preserves accepted transfer/cancellation names and the
MODEL459 integration. All old headers, regional wrappers, overlays/imports,
source grouping, profiles, descriptor arithmetic, flags and callback order
remain unchanged.

Three existing root controls receive only literal namespace substitutions.
The package-stage witness additionally imports the canonical buffer
declaration under its existing pointer guard; the model-texture witness
imports only the staging declarations. Whole-file inverse reconstruction
removes only these includes and reverses names. Existing cases, assertions,
mutations, generators, fixture layouts and compiler/setup logic remain.
The original pointer-selection guard names are unchanged; they are not
renamed along with variable identifiers.

French and Spanish Options controls keep their old overlay-import names
because their sources are unchanged. No harness, fixture, probe or testcase
is added, and cloud/regional/overlay/witness suites are not run locally.
Native-profile conservation does not replace fresh PR-side full regional
image checks.
