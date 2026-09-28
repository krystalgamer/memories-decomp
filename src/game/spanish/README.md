# Spanish resident wrappers

These wrappers include the existing shared C bodies with independently measured
Spanish constants. They retain the European unit's named GCC 2.8.1/MASPSX 2.81
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

The batch adds no owned data sections. Functions with differing UI geometry,
ambiguous no-op bodies, conflicting external bindings, or language-specific
control flow remain assembly until separately verified. Probe sources,
relocation bindings, compiled objects, and comparison records stay under `tmp/`.
