# Spanish MODEL101 entries

Model130, record130, stages7/8 selects two distinct 20KiB images at
sectors36060/36070, loaded at `0x8013B000`/`0x8017B000`. Their header words
are101/231, not the usual150-step pair. The instance ledger records both
Spanish retail hashes; command32004 selects descriptor4.

Both 2,784-byte entry functions at `0x4..0xAE4` select the unchanged
accepted local `variant101_entry.c` and its symbol-only slot wrapper with
`gcc_2_8_1_g0_split` (GCC2.8.1/MASPSX2.81). This adds two inventoried
functions and 5,568 C instruction bytes inside two existing physical images.
The original French implementation's local declarations and source are
unchanged; archive equality was a discovery lead, not the acceptance gate.

The current-master takeover of #7033 promotes the existing raw-image
registrations at sectors36060/36070 rather than adding duplicate canonical
modules. The physical registry remains at3,596 modules. Both complete
shadow images, sized compiler/linked entries, actual16-byte compiler
constants and raw owners were independently proved before promotion.
French already accepts both selections; no source, header, compiler profile
or French registration changes are needed. Shared regression naming/slot
checks use the recorded instance metadata so canonical and physical names
retain the same ownership assertions.

| Image interval | Bytes | Selected owner |
| --- | ---: | --- |
| `0..4` | 4 | Raw header |
| `4..AE4` | 2784 | Compiler-C entry |
| `AE4..AF4` | 16 | C-owned constant vector |
| `AF4..B10` | 28 | Raw image view |
| `B10..5000` | 17648 | Unclassified raw suffix |

The complete production images reproduce both Spanish retail slices.
Regression checks require the selected input compiler object, executable
entry definition and relocated body, actual read-only C constant, and
separately sized non-executable raw owners. Forty-three call relocations
resolve to23 distinct actual Spanish resident starts. Their selected
resident input definitions, linked extents and complete retail bodies are
verified as well; absolute overlay aliases are not used as owner evidence.

GCC emits the constant's real global symbol as `STT_NOTYPE` with size zero.
Its selected input `.rodata` is exactly16 bytes and read-only. The linker
combines that input with the2,784-byte function into a2,800-byte `.entry`
output section whose flags include code and writable storage. The proof
therefore checks the input section extent, explicit linker contribution
and final constant address/bytes, not invented symbol sizes or read-only
output flags.

The inherited target-layout checks and descriptor assertions use the
Spanish archive. Descriptor4 is16 bytes and supplies
`(255,0,64,204,204,204,60,180,30,76,30)`. The bounded context view ends at
`+A60`; it is not an allocation-capacity or whole-animation proof.
The16-byte constant `(4096,4096,4096,0)` is compiler-owned data, not C
instruction coverage. The image view and remaining35,296 suffix bytes stay
in scope and unclassified beyond the measured uses; neither hash equality
nor entry matching proves there is no additional code.
