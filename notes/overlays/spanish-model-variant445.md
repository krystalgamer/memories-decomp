# Spanish MODEL headers 445 and 595

Twelve distinct Spanish images independently match the unchanged accepted
`variant428_{sheets,webs,spokes,rings,quad}.c` bodies through ten French445
wrappers. An independently refined Spanish band body and its directly
selected slot-one wrapper add twelve further instances. Together these
provide 72 compiler-owned C instances / 84,912 instruction bytes with
named `gcc_2_8_1_g0_split`, GCC 2.8.1 and MASPSX 2.81.
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
| `1034..17A8` | 1908 | bands C | yes |
| `17A8..1C8C` | 1252 | sheets C | yes |
| `1C8C..21F0` | 1380 | webs C | yes |
| `21F0..24F8` | 776 | spokes C | no |
| `24F8..2874` | 892 | rings C | no |
| `2874..2BD8` | 868 | quad C | no |
| `2BD8..3594` | 2492 | generated assembly | yes |

Strict walks cover all instructions and terminal returns in these eight
spans. Bands, sheets and webs are entry-call reachable; the other three C
helpers remain retained without a demonstrated entry-call path. Twenty-four
assembly instances / 79,632 bytes remain untranslated. Real input/final
storage owners preserve twelve four-byte headers and 6,764-byte suffixes,
covering all245,760 image bytes. The81,168 suffix bytes remain unclassified,
not established non-code. The webs match is at `+0x1C8C`, not the distinct
2,492-byte helper at `+0x2BD8`.

## Layouts and real owners

One hundred twenty Spanish instruction anchors verify entry construction,
descriptor selection and band/web record, packet and stack use. Three
416-byte narrow webs occupy `+0x5D0..+0xAB0`; each has two4x6 SVECTOR grids,
color at `+0x180` and scale at `+0x194`. Two152-byte sheets occupy
`+0xC78..+0xDA8`, followed by six144-byte rings, four144-byte spokes and
one144-byte quad at `+0x1348`. A456-byte nine-point band occupies
`+0xAB0..+0xC78`: vector arrays at record offsets0/72/144, projected
coordinates at216/252/288, colors at324/360 and depths at420.
The first sheet's size at `+0xD00` supplies
web/spoke timing.

The band GT4 packet is at `+0x1418`, immediately before the sheet GT4
packet at `+0x144C`; the20-byte `GsGLINE` is at `+0x1514`.
Translations use words `+0x153C/+0x1540/+0x1544` or terminal halfwords
`+0x1548/+0x154A/+0x154C`. The web step and phase words are at
`+0x1588/+0x15BC`. The band projection flags are a separate36-byte local
stack array at SP+200 in a296-byte frame, not a context-record member.
The saved flag pointer at SP+252 and projection output at SP+240 are
separate. Band direction uses halfwords`+0x1560/+0x1562`, radius
`+0x15B0`, path fraction`+0x15B4`, and descriptor timing fields.
All136 target-compiled constants verify accessed local
record fields, vectors, matrices, stored pointers, ordering tables and
packets. The four GCC array labels are size-zero NOTYPE at offsets
0/292/364/452 in a real544-byte read-only section; every value is checked.

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
`0x80087898` and `0x80089928`; `rcos` names its existing verified
`0x800866F8` address. The36-callee address set is unchanged.
Contexts `0x80136000/0x80176000` and their
minimum views do not overlap selected MODEL, primary or secondary loads.
Minimum extent and nonoverlap do not prove allocation capacity or
whole-game lifetime isolation.

## Exactness and scope

The [attempt ledger](spanish-model-variant445-attempts.csv) records twelve
terminal wrapper matches. Complete unmasked links, actual selected
compiler/assembly/raw owners, sized symbols, source fingerprints and
resident/layout evidence are checked independently. Spanish regressions
reuse the accepted source/boundary fixture but read Spanish archives,
resident pointers, descriptors, call targets and instruction anchors.

The [experiment ledger](spanish-model-variant445-experiments.csv) preserves
both materially distinct band candidates. The accepted header428 body had
the correct1,908-byte size but four differing next-column loads. Explicit
`band->sc[j + 1]` and `band->sb[j + 1]` expressions reproduce the required
loads at helper offsets`+0x51C/+0x528/+0x54C/+0x558`. North American
sources and profiles are unchanged.

All132 accepted Spanish module records, including MODEL442, remain intact.
The twelve MODEL445 images give144 configured images,712/1,022 C instances
and668,108 C instruction bytes at this independent accepted baseline.
Pending branches are not stacked and progress snapshots stay separate.
Unknown game code, additional Spanish runtime discovery and the expanded
seven-release campaign remain open.
