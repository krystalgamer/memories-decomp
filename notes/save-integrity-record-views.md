# Save integrity record views

`SaveDataPrimaryIntegrity`, `SaveDataSecondaryIntegrity` and
`SaveDataTertiaryIntegrity` in `src/game/save_data.h` describe the three
sealed regions reached by the matched writers and validator in
`src/game/save_data_payload.c`. They are views of those regions, not a
replacement for the independently named `SaveDataState` gameplay fields.

Offsets below are relative to the persistent-state base, after the save
file's `0x200`-byte header:

| Region | CRC-covered bytes | Mask words | Duplicate CRC halfwords |
|---|---|---|---|
| Primary | `0x000..0x33F` | 15 at `0x340..0x37B` | `0x37C`, `0x37E` |
| Secondary | `0x380..0x3EB` | 4 at `0x3EC..0x3FB` | `0x3FC`, `0x3FE` |
| Tertiary | `0x400..0x603` | 8 at `0x608..0x627` | `0x604`, `0x606` |

The different tertiary ordering is deliberate: its checksums precede its
mask array, whereas the other two arrays precede their checksums. The
record sizes are therefore `0x380`, `0x80` and `0x228`. Assertions cover
each size and every mask/checksum offset. The reserved tail still begins
at `0x628` and is outside these records.

The writers independently establish the layout: each computes CRC-16 over
its payload, writes the same result twice, seeds both mask-state words
from that CRC and walks the mask array backward. The validator repeats
the same three CRC seeds and backward mask scans. The new member spellings
retain the original last-element anchors, descending stores/loads, loop
bounds, halfword store order and generator calls.

The duplicated checksum halfwords are written but are not directly checked
by `SaveData_ValidateIntegrity`; it compares the generated mask words.
That existing behavior is documented rather than changed as part of type
recovery. Signed and unsigned word views also retain their existing use.

The functions keep their byte-buffer interfaces. In particular,
`SaveData_RequestTradeWrite` seals two `0x400`-byte transfer prefixes, while
normal payload construction also writes the tertiary region. A blanket
full-state parameter type would obscure that distinction, so the three
region views are applied only at their established offsets. No larger
allocation, serialization change or new package stride is inferred.

[retired `test_save_integrity_layout.py`](https://github.com/krystalgamer/memories-decomp/blob/4b5667dae10ffb9c72df1a242ee060e654214ae1/tools/project/tests/test_save_integrity_layout.py) compiles the actual
records with GCC 2.8.1 and checks all three sizes, first/last mask and
checksum addresses, the `0x400` trade prefix, the `0x628` seal end and the
unchanged `0x5E4` gameplay-state prefix. Full-executable matching verifies
that the typed access spellings retain every retail instruction byte.

## Persisted VBlank counter

`gSaveData_dwVBlankCounter` (originally `D_8009B0C4`, `0x8009B0C4`)
is the active runtime counter copied into `SaveDataState::vblank_counter`
at state-relative `0x408` and restored from that field. Main initialization
seeds it from the earlier captured frame-counter local, and the VBlank
callback increments it. It is not a frozen snapshot, playtime unit or
wall-clock timestamp; loading a state may replace its current value.

The runtime `s32` and payload `u32` views retain their original bit
behavior without making a new signedness claim. The captured initialization
local, payload-builder local, volatile GP versus plain absolute views,
private `D_8009B0C4_IN_DATA` selector and all save/seal/callback ordering
remain unchanged. Three Japanese wrappers preserve the explicit
`gJapanese_SaveDataVBlankCounter` binding at `0x8009AFB4`. No field offset,
CRC/mask operation, serialization format or table bound changes.
