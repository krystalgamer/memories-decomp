# Spanish MODEL headers 445 and 595

Twelve distinct Spanish images independently match the unchanged accepted
`variant428_{sheets,webs,spokes,rings,quad}.c` bodies through ten French445
wrappers. These provide 60 compiler-owned C instances / 62,016 instruction
bytes with named `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81.
Canonical local/SDK declarations and compiler profiles are unchanged.

## Loader and boundaries

Models187/596 use stages7/8; models239/361/368/478 use stages9/10.
Their compact records are187/546/239/311/318/428 respectively. Ten-sector
slices at record-relative sectors180/190 or200/210 load at
`0x8013B000`/`0x8017B000`. The matching resident controller calls offset4
with context and decoded initial command, then update command-1.
The [instance ledger](spanish-model-variant445-instances.csv) records each
actual Spanish slice, complete hash and request.

| Offset range | Bytes | Owner | Direct-entry reachable |
|---|---:|---|---|
| `4..1034` | 4144 | generated assembly | yes |
| `1034..17A8` | 1908 | generated assembly | yes |
| `17A8..1C8C` | 1252 | sheets C | yes |
| `1C8C..21F0` | 1380 | webs C | yes |
| `21F0..24F8` | 776 | spokes C | no |
| `24F8..2874` | 892 | rings C | no |
| `2874..2BD8` | 868 | quad C | no |
| `2BD8..3594` | 2492 | generated assembly | yes |

Strict walks cover all instructions and terminal returns in these eight
spans. Sheets and webs are entry-call reachable; the other three C helpers
remain retained without a demonstrated entry-call path. Thirty-six
assembly instances / 102,528 bytes remain untranslated. Real input/final
storage owners preserve twelve four-byte headers and 6,764-byte suffixes,
covering all245,760 image bytes. The81,168 suffix bytes remain unclassified,
not established non-code. The webs match is at `+0x1C8C`, not the distinct
2,492-byte helper at `+0x2BD8`.

## Layouts and real owners

Seventy-four Spanish instruction anchors verify entry construction,
descriptor selection and the web helper's record/packet use. Three
416-byte narrow webs occupy `+0x5D0..+0xAB0`; each has two4x6 SVECTOR grids,
color at `+0x180` and scale at `+0x194`. Two152-byte sheets occupy
`+0xC78..+0xDA8`, followed by six144-byte rings, four144-byte spokes and
one144-byte quad at `+0x1348`. The first sheet's size at `+0xD00` supplies
web/spoke timing.

The sheet GT4 packet is at `+0x144C`; the20-byte `GsGLINE` is at `+0x1514`.
Translations use words `+0x153C/+0x1540/+0x1544` or terminal halfwords
`+0x1548/+0x154A/+0x154C`. The web step and phase words are at
`+0x1588/+0x15BC`. All113 target-compiled constants verify accessed local
record fields, vectors, matrices, stored pointers, ordering tables and
packets. The three GCC array labels are size-zero NOTYPE at offsets
0/292/364 in a real452-byte read-only section; every value is checked.

Actual requests611000..611004 select52-byte descriptors from module
`+0x3690`, with the pointer stored at context `+0x1590`. Models187/596
select command0; models239/361/368/478 select4/3/1/2. Every selected
descriptor lies inside its preserved suffix owner. Direct entry accesses
establish minimum context `+0x15D0` (5,584 bytes), ending with the halfword
at `+0x15CE`.

Fresh Spanish resident proofs verify36 real callee input/final owners,
three matching initializer/controller/loader owners, and both context
pointer data owners at `0x80010024/28`. They belong to input `.data` in
`spanish_raw_80010000.o`, although linked `.main` also contains executable
code. `RotTransPers3` and `ratan2` retain verified resident addresses
`0x80087898` and `0x80089928`. Contexts `0x80136000/0x80176000` and their
minimum views do not overlap selected MODEL, primary or secondary loads.
Minimum extent and nonoverlap do not prove allocation capacity or
whole-game lifetime isolation.

## Exactness and scope

The [attempt ledger](spanish-model-variant445-attempts.csv) records ten
terminal wrapper matches. Complete unmasked links, actual selected
compiler/assembly/raw owners, sized symbols, source fingerprints and
resident/layout evidence are checked independently. Spanish regressions
reuse the accepted source/boundary fixture but read Spanish archives,
resident pointers, descriptors, call targets and instruction anchors.

Accepted module records remain intact and progress snapshots stay separate.
Unknown game code, additional Spanish runtime discovery and the expanded
seven-release campaign remain open.
