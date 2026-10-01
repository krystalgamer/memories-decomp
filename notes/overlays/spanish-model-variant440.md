# Spanish MODEL headers 440 and 590

Four distinct Spanish images for models262/631 reuse the unchanged
accepted `variant423_{spokes,rings,quad}.c` bodies through the existing
French440 wrappers. Six canonical compiler objects independently provide
12 C instances / 10,176 instruction bytes using named
`gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81. Shared implementations,
canonical local/SDK declarations and profiles are unchanged.

## Loader and boundaries

Models262/631 map to compact records262/581. Stages9/10 select ten-sector
slices200/210 within the 276-sector records and load at
`0x8013B000`/`0x8017B000`. The matched resident controller calls offset4
with context and decoded initial command, then update command-1.
The [instance ledger](spanish-model-variant440-instances.csv) records
each actual Spanish slice, complete hash and request606000.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..F14` | 3856 | generated assembly | yes |
| `F14..16E0` | 1996 | generated assembly | yes |
| `16E0..1BD4` | 1268 | generated assembly | yes |
| `1BD4..2130` | 1372 | generated assembly | yes |
| `2130..2440` | 784 | spokes C | no |
| `2440..27BC` | 892 | rings C | no |
| `27BC..2B20` | 868 | quad C | no |

Strict walks cover every instruction and terminal return in all seven
spans. All three C helpers remain retained code without a demonstrated
direct entry-call path. Sixteen assembly instances / 33,968 bytes remain
untranslated. Real input/final storage owners preserve all four-byte
headers and 9,440-byte suffixes, covering all 81,920 image bytes. The
37,760 suffix bytes remain unclassified, not established non-code.

## Layouts and owners

Entry captures `a0 -> s3 -> s6`. Sixty-nine Spanish entry anchors verify
two 152-byte sheets at `+0x5E8`, six 144-byte rings at `+0x718`, four
144-byte spokes at `+0xA78` and one 144-byte quad at `+0xCB8`.
They include pointer saves/reloads, initialization, counters, bounds
and record advances. The spokes timing input is the first sheet size:
`0x5E8 + 0x88 = 0x670`, not the stale generic source comment's `0x684`.
The line packet is at `+0xE84`, the G4 packet at `+0xD64`, and the
helpers use step/phase fields at `+0xEF8/+0xF24`.

Seventy-three freshly target-compiled constants verify the four local
record types and accessed fields, vectors/matrices, stored coordinate
pointers, `GsOT`, `GsGLINE`, `POLY_G4`, `POLY_GT4` and target
four-byte pointer/integer widths. Verification uses the real 292-byte
`.rodata` extent and every value; its GCC array label is size-zero NOTYPE.

Seven further Spanish anchors verify the descriptor construction at
module `+0x98/+0x9C` and `+0xA8..+0xB8`, eight bytes earlier than in
families418/433. Its base is module `+0x2C1C`, stride48, with the pointer
stored at context `+0xF00`. All four actual requests606000 select
command0, and every 48-byte window fits its preserved suffix owner.
Direct entry accesses establish the minimum context view `+0xF38`
(3,896 bytes), independently of complete-image matching.

Fresh exact Spanish resident proofs verify 36 real callee input/final
owners, three matching initializer/controller/loader owners, and both
context-pointer data owners at `0x80010024/28`. They belong to input
`.data` in `spanish_raw_80010000.o`, even though output `.main` also
contains executable code. Contexts `0x80136000/0x80176000` and their
minimum views do not overlap the selected MODEL, primary or secondary
loads. This does not prove allocation capacity or whole-game lifetime
isolation.

## Exactness and scope

The [attempt ledger](spanish-model-variant440-attempts.csv) records six
terminal wrapper matches. Complete unmasked links, selected compiler,
assembly and raw owners, sized symbols, dependency fingerprints and
resident/layout evidence are checked independently.

Regional regressions reuse the French source and boundary fixture, adding
Spanish fallback-binding, actual descriptor and minimum-context checks.
Accepted module records are preserved and progress snapshots stay
separate. Further Spanish runtime discovery, unknown game code and the
expanded seven-release campaign remain open.
