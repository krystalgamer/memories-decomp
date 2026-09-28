# Spanish resident build

`make spanish-match` validates the Spanish inputs and image map, cleans the
split/build output, compiles the configured C units, links the entire
`SLES_039.51`, and requires SHA-256
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.
`make spanish-inventory` additionally regenerates `functions.csv` from the
matching manifest, exact linked C symbol extents, and generated fallback
assembly. Runtime modules have their own [overlay build](overlays/README.md).

The initial resident integration reuses 492 existing European/shared C units:
976 functions and 259,032 bytes, without copying or editing any C source or
adding compiler flags. Their named GCC 2.8.1/MASPSX 2.81 profiles are unchanged.
The ordered per-function ranges in `matching_c.json` completely cover each
grouped object's text. Data-owning units are intentionally excluded from this
batch rather than replacing their C definitions with absolute aliases.

## Evidence and exclusions

Each selected object's European linked instructions were searched against
Spanish resident text while masking only its ELF relocation fields. Exactly
one full-unit match was required; unmatched or ambiguous units remained
generated assembly. The resulting 1,291 external symbol bindings were recovered
from the Spanish instruction operands, accounting for paired HI16/LO16
relocations and the measured GP change from `0x8009BE84` to `0x8009C298`.
Object-relative local references are resolved by the linker.

The first full link identified two necessary corrections, not source changes:
HI16/LO16 pairs must recover the whole address rather than assume a small
regional delta; and auto-generated in-image and external symbols both need
the `spanish_` namespace to avoid collisions with old address-based C names.
After those corrections, the complete executable matches, including all raw
regions and generated fallback assembly.

`debug_effect_screen.c` remains assembly: its references to
`gDebugEffect_abPreviewState` imply conflicting Spanish addresses, so a single
linker alias cannot reproduce the body. No approximate match is claimed.
Candidate shape scans, relocation evidence and failed-link logs remain local
under `tmp/`.

All 61 game-owned handwritten assembly functions were independently compared
against the classified European inventory: equal sizes and complete
opcode/register shapes, with no European game C function sharing those
shapes. Each inventory row records its European counterpart. They remain
assembly and are excluded from C-target progress. The embedded SDK getter at
`0x8005C018..0x8005C028`, startup, and the library range
`0x80073C4C..0x800918DC` are likewise not C candidates.

The image-map boundaries are measured independently of the C selection:
entry point `0x800128CC`, final resident return at `0x800918D4`, and startup
BSS clearing of `0x8009C408..0x800FFC30`. The initial data and remainder of the
executable are preserved as binary regions; this does not count them as
decompiled C data or replace runtime-overlay validation.

The Spanish CI workflow checks both resident and runtime-overlay matches.
Progress uses `functions.csv` and the matching manifests, validates all
ownership/range relationships, and retains separate overlay counts. Snapshot
refreshes remain independent work under #443.
