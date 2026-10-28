# French MODEL155016 / MODEL59520 shared entry

For #6460, these four raw French MODEL.MRG images directly reuse the accepted
local `src/overlays/model_variant/variant177_entry.c` and its existing
`variant177_entry_slot1.c` wrapper. No shared C or header is changed or copied.
The established source name is not a new claim about the raw images' header IDs.

| Sector | Load address | Existing source |
| --- | --- | --- |
| 59520 | `0x8013B000` | `variant177_entry.c` |
| 155016 | `0x8013B000` | `variant177_entry.c` |
| 59530 | `0x8017B000` | `variant177_entry_slot1.c` |
| 155026 | `0x8017B000` | `variant177_entry_slot1.c` |

Each entry is 5,372 bytes at load address + 4, with the original 624-byte frame.
The source also owns the real 16-byte scale vector at image offset `0x1500`.
The original module header and raw suffix from `0x1510` remain byte-identical.
All four complete 20,480-byte production images equal the registered retail
bytes and SHA-256 values.

## Evidence and attempts

The strict known-slot byte-normalized donor census found no direct reuse.
A separate opcode/register-shape census supplied a structural lead from the
accepted North American MODEL611/215 registrations of this shared source.
That deliberately lossy comparison was not treated as equivalence: an independent
byte comparison then established that both French slot-zero entries differ
from the donor only at 71 JAL destinations. Slot-one entries additionally have
the expected three LUI and eleven local J slot relocations.

One compiler experiment, at tick 295, compiled the unchanged accepted source
through a temporary include wrapper using `gcc_2_8_1_g0_split`: GCC 2.8.1 and
MASPSX 2.81, without ad hoc flags. French resident bindings recovered every
instruction. The frozen object was independently relinked; slot one was
independently compiled, and all four entries and constant vectors matched.
The temporary wrappers are calibration artifacts, not new production sources.
The related attempt ledger records that calibration and each direct-source
production result.

Production verification uses the original tracked sources, checks the complete
image bytes, preserves each original-sized C function, and verifies the actual
constant-data contribution. Legacy MASPSX emits an unsized local vector label;
Splat places its input `.rodata` in the owning `.unmatched_4` output section.
Ownership is therefore established by the original C object's 16-byte
`.rodata`, the section-defined local label, and the linked
`unmatched_4_RODATA_START` / `END` span, not an invented ELF symbol size or an
absolute override. An initial verifier assumption about that symbol metadata
was corrected without a C/compiler change, and the already verified first
production build was resumed rather than repeated.

Native and complete-image proofs were made on preparation base
`2033767fe028bf1a01e4f3ea06ac2bb6f83cf961`. Retail identities, shared source
inputs, unrelated module registrations and function identities are unchanged.
No later-master combined build is implied by these original-base proofs.
