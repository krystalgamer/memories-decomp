# French MODEL60348 ring, echo and burst entry

MODEL.MRG sectors 60348, 60358, 168560 and 168570 share one recovered
9,924-byte entry at image offset `0x4`. Each complete 20,480-byte image
matches its original bytes with the named `gcc_2_8_1_g0_split` profile,
GCC 2.8.1 and MASPSX 2.81. The original four-byte loader headers and raw
suffixes beginning at `0x26C8` remain unchanged.

| Sector | Entry | Complete-image SHA-256 |
|---|---|---|
| 60348 | `func_8013B004` | `82ca5c05497f275aaf87437fb08fc453cfffefae38386ed8def948e492d9894a` |
| 60358 | `func_8017B004` | `6645cda4bf17658e74329092f016ef29b8052714e0ed8aba06fed1737bd0c5a1` |
| 168560 | `func_8013B004` | `f4ee0872b6ef2ea9e099e72b52c00c4cc2f7761004c974a40e165c222b637c04` |
| 168570 | `func_8017B004` | `2ff147c5ae39a96a4388e696105b47480b8ef342ad0916baa51b40b2e799b70a` |

## Family and ownership evidence

The two slot-0 entry bodies are byte-identical, as are the two slot-1
bodies. All 25 cross-slot word differences are proven image-local
relocations: eleven internal jumps and fourteen LUI/ADDIU producers.
The proof follows actual branch and delay-slot paths, including three
producers sharing the configuration-table consumer at image offset
`0x164`. Alternate paths kill the high half before use rather than
exposing a non-address constant. All four entries retain the same 91
ordered resident calls to 25 targets and a 360-byte stack frame.

All 2,481 native words were read. Native input snapshots preceded the
four complete baseline builds; 3,388 original source, header, tool and
configuration hashes were checked during candidate work. The original
archive, headers and suffixes were never edited. Raw header numbers
681/748 alone were not treated as source identity evidence.

## Recovered component

The fourteen-byte configuration contains signed start/release ages,
unsigned radius/height and two RGB triples. The 13,004-byte state owns
three packets at offsets 32/72/124, ten vertex arrays at offsets
164/428/692/724/860/996/1028/1060/1196/1332, five flags at 1468, counts at
1473/1474 and two unaligned four-byte tint descriptors at 1475/1479.
The existing `ModelTintColor` declaration preserves their actual
by-value call ABI, including blend-mode bytes.

Eighty-byte elements begin at 1484 (eight rings), 2124 (sixteen echo
rows of eight rings) and 12364 (eight bursts). Each element retains
its active/RGB/UV bytes, mode, age/duration, echo/release halfwords,
coordinate pointer, rotation, scale, position and target.

Commands 0/1/2 start rings from model parts 21/10/7 with their distinct
age thresholds. Commands 0/1 also request short tint ramps and override
frame step. Expanding rings draw 32 textured segments, drop fading echoes
and release camera-facing bursts, then draw sixteen rim segments.
Bursts draw a sprite, two Gouraud shards, and four rotating flashes with
two sixteen-segment cone bands apiece. All projection outputs use the
existing Psy-Q `PSXLONG` declarations.

Positive start delay only decrements the delay: the shared return keeps
the initial result **zero**, not one. Active bursts return four.
Once all elements are inactive and all rings have started, completion
returns one on the first observation and two on subsequent observations.

## Source recovery and integration

Accepted local variant669 supplied related ring/echo/burst source
structure, not a whole-body donor. Its extra streak subsystem, Gouraud
quad, larger element layout and unused frame slot are not present here.
All new local declarations were recovered independently; existing
SDK/game headers supply shared declarations. The shard's radius/3
extent is on Y, and the recovered unsigned-halfword narrowing is retained.

The companion ledger records all three source experiments across four
images and four normal production proofs: sixteen rows. The first
candidate had 310 changed words, concentrated around flag initialization
and delay control flow. The ascending flag loop by itself produced an
honest four-byte overlap rejection. Restoring the native delay/else
lifetime and common return then matched every byte; the compiler reverses
the original ascending five-flag loop.

Both slot-0 images reuse one source; both slot-1 images reuse its existing
macro-wrapper pattern. Each normal production ELF owns exactly one
original-sized C entry. The resident-only integration helper does not
accept overlay inventories, so the tracked overlay manifests, layouts,
inventories and provider map follow the existing regional integration
pattern instead. No generated Splat files were edited.

No inline assembly, source-local externs, fixed registers, volatile tricks,
fabricated accesses or dependencies, unused frame padding, reference
headers, generated-C behavioral tests or local cloud tests are used.

## USA images

The USA images at sectors 60348, 60358, 168560 and 168570 of
`game/DATA/MODEL.MRG` carry the same entry, built from an older debug
revision. It prints the work size after clearing it, and the 24-byte format
string sits right after the code. As a result the entry is 0x26D4 bytes and the
tail data starts 0x28 later than in the French images. The source guards that
call with `MODEL_IMAGE60348_WORK_SIZE_PRINTF`, in the style of
`image9840_entry.c`. `model_variant/image60348_entry_usa.c` and
`_usa_slot1.c` define the string and the shifted tail names, then include
the French file.

These images need GCC 2.8.0 (`gcc_2_8_0_g0_split`, the decompals/old-gcc
`gcc-2.8.0-psx` release):
- **Under 2.8.1**, every opcode matches. The only difference is register
  choice where the two unaligned `ModelTintColor` arguments are packed into
  `$a2`/`$a3`.
- **Under 2.8.0**, the build is byte-identical. The cause is the 2.8.0
  `combine`, which leaves the two packing accumulators with an extra
  recorded death. Local allocation skips them, so global allocation places
  them in `$t0`/`$v1` and the `0xFFFFFF` mask in `$a3`.
- **Nothing else matches.** No source spelling of the call reaches the USA
  registers under 2.8.1. Neither does Sony's 2.8.1 `CC1PSX.EXE`, nor the
  PsyQ 4.0, 4.1, 4.5 and 4.6 compilers.

The resident bindings in `image60348_usa_linker_symbols.txt` are read from
the USA entry's own `jal` targets.

| Sector | Entry | Complete image SHA-256 |
|---|---|---|
| 60348 | `0x8013B004` | `c45f63fc6205ec1ce937edf04ea44aa3efef731bf556b69b7fe095be44abc269` |
| 60358 | `0x8017B004` | `ef03316afd4c7284b450b22cd4d24b94b309a67e22a41cbd542af32dc60d3298` |
| 168560 | `0x8013B004` | `92f46979a8d94c182840fb6eab22ee2d6514562ba82b5903b5f652e574cb663d` |
| 168570 | `0x8017B004` | `1a8df8547e0e95a5176ad8856f445d95cde953af50e2de9440709f6269e04215` |
