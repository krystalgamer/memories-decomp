# Spanish resident wrappers

These wrappers include the existing shared C bodies with independently measured
Spanish addresses and dialog geometry. They retain the European unit's named GCC 2.8.1/MASPSX 2.81
profile; no source body, guessed type, or compiler option is imported from an
external reference.

| Wrapper | Spanish entry | Functions | Bytes | Literal specialization |
|---|---|---:|---:|---|
| `duel_update_card_pick_cursor.c` | `0x8002416C` | 1 | 336 | Card ID at `0x8009C5D8`, Y offset at `0x8009C5DD` |
| `save_data_payload.c` | `0x8003D040` | 9 | 1,164 | Campaign scene index at `0x8009C638` |
| `mem_card_io_result_callbacks.c` | `0x800451B8` | 4 | 76 | Result word at `0x800A0000 - 0x3844 = 0x8009C7BC` |
| `sd_arm_busy_callback.c` | `0x80045918` | 1 | 40 | Driver pointer at `0x8009C7E0`, callback at `0x8009C470` |
| `func_80061008.c` | `0x8006B080` | 1 | 240 | Viewport X/Y at `0x8009C4C0` / `0x8009C4C2` |

The literals were recovered from Spanish load/store instruction operands, not
from assuming a fixed regional address delta. These operands are not ELF
relocations, so linker-symbol changes alone cannot match them. `VERSION_EUROPE`
in the viewport wrapper selects the already verified shared implementation;
the Spanish addresses are supplied separately.

Each candidate was compiled and independently linked at its Spanish address,
with every resulting instruction byte compared against the retail executable.
The grouped save-data and callback units retain their complete contiguous
definition order. Integration then passed the complete executable hash and
exact linked-symbol extent checks.

The same batch directly reuses `european/debug_menu_bust_up_entry.c`,
`european/func_8003C224.c`, `func_8004A6D8.c`, and `func_8005C690.c` for another
four functions / 476 bytes. These small units were selected from the remaining
game-function inventory, then independently linked and byte-compared; a
masked-instruction resemblance alone was not accepted.

The address-wrapper batch adds no owned data sections. Probe sources,
relocation bindings, compiled objects, and comparison records stay under `tmp/`.

## Dialog geometry

Seven further wrappers reproduce the wider Spanish dialogs, sharing the same
five base C sources rather than duplicating their bodies:

| Wrapper | Spanish entry | Functions | Text bytes | Owned `.rodata` bytes |
|---|---|---:|---:|---:|
| `duel_effect_create_channel.c` | `0x8003D638` | 1 | 172 | 0 |
| `dialog_transition_8003D614.c` | `0x8003D7DC` | 1 | 312 | 0 |
| `dialog_transition_8003D74C.c` | `0x8003D914` | 1 | 752 | 0 |
| `func_8003DA40.c` | `0x8003DC04` | 1 | 476 | 0 |
| `mem_card_dialog_trade_save.c` | `0x8003F1E4` | 3 | 1,412 | 20 |
| `mem_card_dialog_update.c` | `0x8003F768` | 3 | 788 | 0 |
| `save_data_transfer_runtime.c` | `0x8003FAF8` | 7 | 1,692 | 48 |
| Total | | 17 | 5,604 | 68 |

The Spanish instruction operands independently establish x=8 rather than 32
for sprite placement and slide destinations, and width=304 rather than 256 for
the text boxes. `DuelEffect_CreateChannel` also uses height=48 rather than 64.
The shared `dialog_layout.h` defaults preserve all existing regions; only the
Spanish wrappers override them. The choice byte and load-status byte are
measured at `0x8009C6D0` and `0x8009C435`, respectively. Spotlight centering,
animation increments, flags, and off-screen vertical positions are unchanged.

Each wrapper's complete linked text and any jump table were independently
compared with retail bytes. The tables are supplied by the C objects at
`0x80010444..0x80010458` and `0x8001045C..0x8001048C`; the four-byte gap is
preserved raw. Integration passes the complete Spanish executable hash and
exact function extents. All 27 existing affected source/profile objects retain
their pre-change SHA-256, including the North American, Japanese, and European
variants; the North American complete-executable regression also matches.

The initially ambiguous no-op bodies and conflicting external bindings were
integrated only after separate identity and exact-match verification.

## Effect-preview state window

`debug_effect_screen.c` matches the three contiguous functions at
`0x80022174`, `0x800223B0`, and `0x800226D4` (1,468 text bytes), with the
unchanged `gcc_2_8_1_g8_split` profile and shared function bodies.

All 18 small-data relocations establish these Spanish accesses:

| Role | Address | Window offset |
|---|---|---:|
| Coordinate axis | `0x8009C2C0` | 0 |
| First coordinate | `0x8009C2C4` | 4 |
| Second coordinate | `0x8009C2C5` | 5 |
| Preview page | `0x8009C2C6` | 6 |

The existing regions use coordinate/page offsets 2/4. Applying those offsets
unchanged gave conflicting inferred bases for one symbol. Correcting the
offsets, and accounting for each new object relocation addend when recovering
bindings, produces one consistent Spanish base and exact instruction bytes.

The implementation-only `debug_effect_state.h` supplies the region's bounded
declaration. Spanish declares the seven-byte minimum accessed window; it does
not assert the size of any unaccessed tail or claim ownership of the raw data.
The public function header no longer exports a conflicting six-byte bound to
unrelated translation units. Other regions retain their six-byte declaration
and the North American six-byte initialized definition.

Both incomplete-array declaration probes were rejected by the existing-object
identity gate. The bounded private declaration preserves all five existing
North American, Japanese, and European preview source/profile objects
byte-for-byte, while the Spanish unit passes isolated linking and full-image
matching. Complete North American, Japanese, and European executable
regressions also match. Probe and comparison logs remain local under `tmp/`.

## Localized script images

`script_image_objects.c` reuses the European group and the existing
`gcc_2_8_1_g0` profile for four functions / 984 bytes:

| Function | Spanish entry | Bytes |
|---|---|---:|
| `ScriptImage_TransferCallback` | `0x8002DFE8` | 308 |
| `ScriptImage_RequestTransfer` | `0x8002E11C` | 392 |
| `ScriptImage_ReleaseObjects` | `0x8002E2A4` | 84 |
| `func_8002E10C` | `0x8002E2F8` | 200 |

Before the existing BCD index and transfer-table selection, the request remaps
two ids using `D_8009C02B` at `0x8009C44B`:

| Language byte | Input `0x10` | Input `0x11` |
|---|---|---|
| 1 | `0x42` | `0x43` |
| 2 | `0x44` | `0x45` |
| 3 | `0x46` | `0x47` |
| 4 | `0x48` | `0x49` |

Other language values and image ids are unchanged. The remapped id is stored
in the optional owner's `image_id` before the existing three-mode dispatch.
The callback still uploads at `(0x280, 0xD0)` and requests use sector base
`0x29E8`. The shared body selects remapping only with
`SCRIPT_IMAGE_LOCALIZED_IDS`; no function body or language global is duplicated.

The initial remapping probe retained the volatile owner store and emitted
988 bytes instead of 984: GCC repeated the `value >> 4` instruction after
that store. A nonvolatile parameter matched, but would change the function
type seen by unrelated callers. The accepted version keeps the existing
public signature and uses a narrowly gated ordinary store. All four linked
function extents and every instruction byte then match.

The complete group also equals the French retail bytes at the same addresses.
At this batch's baseline, all 23 remaining Spanish targets were still assembly
in the French manifest: 20 had identical bytes. Main initialization and the
language debug entry differed only in language constants; the boot sequence
also differed in register allocation and scheduling.
French metadata is deliberately left to its ongoing matching work.
The three existing North American, Japanese and European script-image objects
remain byte-identical. Full Spanish executable and overlay matching remain
the acceptance gates, not the cross-region byte comparison alone.

## Language-specific units

Four more wrappers select behaviour the Spanish build changes rather than
only its addresses or geometry:

| Wrapper | Spanish entry | Functions | Bytes | Difference |
|---|---|---:|---:|---|
| `main_init.c` | `0x80012A44` | 1 | 404 | stores language index 4 |
| `main_run_boot_sequence.c` | `0x80043D7C` | 3 | 776 | requests and stores language 4 instead of 0 |
| `debug_menu_sound_entry.c` | `0x800309F4` | 3 | 1,284 | the debug language entry wraps to 4 |
| `dialog_choice_cursor.c` | `0x80036EF0` | 2 | 420 | clears R1 from `gInput_dwPreviousHeld` before the tests |
| Total | | 9 | 2,884 | |

The language index is `BUILD_LANGUAGE_INDEX` in `duel_effect_resource_setup.h`,
next to `D_8009C02B`, with the European 0 as its default. The choice cursor's
store is behind `DIALOG_READ_CHOICE_CLEARS_HELD_R1`, and it reaches
`gInput_dwPreviousHeld` through `$at`, so the wrapper also selects that
declaration's `.data` arm. Across all 1,363 source/profile pairs in the
matching manifests, only the three European wrappers that now spell the index
as `BUILD_LANGUAGE_INDEX` preprocess differently, and their compiler assembly
is unchanged. The remaining result-outro difference is covered below.

## Localized result-outro completion

`duel_result_orbit_sprites.c` selects `DUEL_RESULT_LOCALIZED_SPRITES` while
reusing the European wrapper and shared runtime. The complete grouped object
uses `gcc_2_8_1_g8_split` and covers:

| Function | Spanish entry | Bytes |
|---|---|---:|
| `DuelResult_UpdateOrbitSprite` | `0x80020CB0` | 412 |
| `func_80020EE8` | `0x80020E4C` | 100 |
| `DuelScene_UpdateResultOutro` | `0x80020EB0` | 1,672 |
| `Duel_ShowResultPage` | `0x80021538` | 220 |
| Total | | 2,404 |

The language byte at `0x8009C44B` selects one of four opponent/no-opponent
table pairs. Each table has two winner rows of ten four-byte entries:

| Language | Opponent table | No-opponent table |
|---|---|---|
| 1 | `0x80091B30` | `0x80091B80` |
| 2 | `0x80091BD0` | `0x80091C20` |
| 3 | `0x80091C70` | `0x80091CC0` |
| 4 | `0x80091D10` | `0x80091D60` |

The spawn loop visits ten entries. A zero `x` skips the entry entirely;
otherwise it records `i + 1` at `0x8009C2BC` before clearing the slot and
testing `kind`. The later retarget loop reads that dynamic count. The
fixed-language builds constrain the selector to 1..4; the missing default arm
is preserved from the binary. Table and count declarations remain private,
and no initialized data ownership is claimed for these raw regions.

The first link exposed one missing existing BGM-symbol binding:
`tent_DuelResultBgmId` is the halfword at `0x8009C50C`, independently measured
from GP+`0x274` accesses around the BGM calls. Adding that binding produced an
exact complete group immediately. A gated production form then retained the
same bytes and all five existing affected source/profile objects byte-for-byte.

Both the private integration probe and the clean production Spanish executable
match in full, with all four definitions verified as real section symbols.
The same group equals the French, Italian and German retail bytes. Together
with the original six matched overlays, this completed those inventories, not
an exhaustive runtime census. The newly identified duel-effect bank and boot
module are separate runtime work; the bank still has 48 provisional assembly
functions after its first 37 shared C matches.
