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

`tools/project/tests/test_save_integrity_layout.py` compiles the actual
records with GCC 2.8.1 and checks all three sizes, first/last mask and
checksum addresses, the `0x400` trade prefix, the `0x628` seal end and the
unchanged `0x5E4` gameplay-state prefix. Full-executable matching verifies
that the typed access spellings retain every retail instruction byte.
