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

Ambiguous no-op bodies, conflicting external bindings, and language-specific
control flow remain assembly until separately verified.

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
