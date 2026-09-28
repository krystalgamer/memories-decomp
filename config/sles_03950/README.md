# Italian resident build

Run `make verify-italian-inputs`, `make italian-match`, and
`make italian-inventory` from the repository root. The complete `SLES_039.50`
must have SHA-256
`a01cc55d48df37c6bf9c6e2dfcbb4f429b4f8f2b111e35b44930d56bb12f9b1d`.
The independent [overlay builds](overlays/README.md) cover all six runtime
modules. Retail inputs remain ignored under `game/italy/`.

All **1,140 eligible resident functions / 357,700 bytes** use matching C,
with **124 / 124 overlay function instances / 55,852 bytes** also matching.
The 61 genuinely handwritten functions and 623 Psy-Q/CRT functions keep
their accepted classifications; no eligible game C is reclassified.
The 561 resident source units use the existing named GCC 2.8.1 / MASPSX 2.81
profiles without new flags or duplicated function bodies.

## Independent regional evidence

The legally supplied Italian disc's `SYSTEM.CNF` identifies `SLES_039.50`.
The executable header, load/entry/stack, file size and all image-map hashes
were measured from that executable rather than copied from Spanish metadata.
Both archives are independently hashed by the input and overlay gates.

Comparing every accepted Spanish game-function range found 1,137 identical
functions. The remaining three differ by exactly one language instruction:

| Instruction address | Italian word | Spanish word |
|---|---|---|
| `0x80012AA4` | `0x24020003` | `0x24020004` |
| `0x80030AC0` | `0x24020003` | `0x24020004` |
| `0x80043FDC` | `0x24040003` | `0x24040004` |

Three [thin wrappers](../../src/game/italian/README.md) select
`BUILD_LANGUAGE_INDEX 3`; all other Spanish/shared source paths are reused
unchanged. Their complete grouped objects cover seven functions / 2,464 bytes.
An initial index-2 calibration was rejected by the main-init comparison at
text offset `0x60`; all three retail instructions independently establish 3.
Candidates, rejected probes and exact-link evidence remain local under `tmp/`.

The same 42 C-owned data windows (2,532 bytes including layout padding) are
byte-identical. Split layouts retain explicit owned sections, two-byte
subalignment and odd-length padding. All seven exported initialized-data
symbols remain real section definitions, not absolute aliases. In particular,
the viewport halfwords occupy a four-byte PROGBITS section even though the
compiler names their input section `.sbss`.

`make italian-inventory` requires the whole executable hash and verifies all
matching function addresses and sizes against real linked C definitions.
It regenerates the 1,824-row inventory from those definitions and preserved
fallback assembly. A whole-image hash alone is not treated as proof of C
ownership. Raw data gaps and preserved SDK/handwritten assembly are not
claimed as C.

## CI and reporting

`.github/workflows/italian-overlay-build.yml` verifies both the resident
executable and all six modules using `YGOFM_SLES_03950_URL`,
`YGOFM_ITA_SU_MRG_URL`, and `YGOFM_ITA_WA_MRG_URL`.
The progress generator includes Italian resident and overlay inventories.
Project-wide README/global-usage snapshots are refreshed separately under
#443, not with each matching change.
