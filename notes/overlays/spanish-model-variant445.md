# Spanish MODEL headers 445 and 595

Twelve distinct Spanish images independently match the unchanged accepted
`variant428_{sheets,webs,spokes,rings,quad,spiral}.c` bodies through twelve French445
wrappers. An independently refined Spanish band body and its directly
selected slot-one wrapper add twelve further instances. Together these
provide 84 compiler-owned C instances / 114,816 instruction bytes with
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
| `2BD8..3594` | 2492 | spiral C | yes |

Strict walks cover all instructions and terminal returns in these eight
spans. Bands, sheets, webs and spiral are entry-call reachable; the other three C
helpers remain retained without a demonstrated entry-call path. Twelve
assembly instances / 49,728 bytes remain untranslated. Real input/final
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

## Spiral geometry, colors and narrowed growth

The unchanged accepted spiral wrappers reproduce all twelve 2,492-byte
Spanish spans. All72 prior compiler-C objects remain identical, including
the refined Spanish band body. The new helper adds twelve C instances /
29,904 bytes without a source, header, declaration or compiler-profile edit.

Twelve124-byte arms occupy context`0..0x5D0`, ending exactly at the narrow
web array. Two vertices at16, screens at32, angles at40, displaced vertices
at48, displaced screens at64 and widths at72 precede two four-byte color
slots each at80/88, signed depths at100 and halfword offsets at108/112.
Unknown bytes, vertex padding and the fourth byte of each color slot remain
opaque and untouched by this helper.

Entry's saved context at stack`+0x80` supplies the first arm. Its initializer
advances by124 bytes twelve times. Point0 RGB comes from selected descriptor
bytes0..2 for the color row at88 and4..6 for the row at80; point1 RGB is
zero. Independent pointer/register checks distinguish the fixed arm+4 pointer
from the incrementing point pointer and preserve their full initializer ranges.
All five actual requests611000..611004 use the measured52-byte descriptor
stride. Unsigned time at`+0x1580` reaching descriptor field`+0x30`
gates the spiral call at module`+0xEB4`, with original context in its
delay slot. This is separate from the sheet/web gate at descriptor`+0x1C`.

Geometry uses signed halfword size`+0x15A8` divided by2 for radius;
the separate halfword`+0x15B0` supplies displacement divided by512 or
multiplied by24 then divided by4096. Entry initializes size and sweep to0,
and`+0x15B0` to4096. Angular spacing divides the shifted arm index by12;
the helper reads but does not advance sweep`+0x15A4`.
Parity at`+0x157C` selects scale4096 or4608. Translation reads
`+0x153C/+0x1540/+0x1544`.

The400-byte frame has bounded argument/local windows: arguments16..40,
rotation40..48, scale48..64, matrix64..96, local-screen matrix96..128,
coordinate128..208, `flags[12][2]`208..304, projection result304..308
and the separate single-point flag308..312. The flag array's eight-byte
rows are distinct from124-byte arms. This classifies the named windows,
not every spill or whole-game lifetime.

Two four-point calls per arm preserve duplicated point/output pairs, followed
by single-point calls whose separate flag does not control visibility.
Two mirrored quads reuse the52-byte GT4 at`+0x1418..+0x144C`,
initialized by the real `SetPolyGT4`. Both submissions use point0 signed
depth/primary flag, accept zero, and narrow sorting depth to16 bits.
Each corner copies its corresponding RGB triplet; command, UV fields,
packet tails and other context bytes are preserved.

If signed size is below1024, growth adds frame step`+0x1588` times64.
The result is stored/narrowed to a signed halfword **before** comparing
against1024 and clamping. Size already1024 does not advance, even for
negative steps. Entry's two separate frame-step getter calls and the
second return's stored update remain unchanged.

Forty-five target-compiled constants,107 layout/caller anchors and31 scalar
checks per image establish these contracts. Eleven actual helper resident
definitions plus the packet setter have input-object, selected-link,
final-symbol and complete retail-body evidence. `RotTransPers` adds the
canonical name at`0x80087868` while retaining `func_french_80087868`,
which the remaining entry assembly still uses. The37 names retain36
distinct resident addresses.

An ILP32 oracle passes589,824 cases and28,311,552 projection calls:
all signed sweep values and every signed size for eight frame-step edges,
including full-word wrapping and halfword narrowing before clamp. It compares
all6,144 context/guard bytes and every52-byte submitted packet.
Fourteen compiled mutations are rejected. Deterministic SDK mocks check
argument and state contracts, not GPU/GTE emulation or retail execution.
Required views remain within the observed entry minimum`+0x15D0`;
no allocation-capacity or exclusive-lifetime claim is made.

## Exactness and scope

The [attempt ledger](spanish-model-variant445-attempts.csv) records fourteen
terminal wrapper matches. Complete unmasked links, actual selected
compiler/assembly/raw owners, sized symbols, source fingerprints and
resident/layout evidence are checked independently. Spanish regressions
reuse the accepted source/boundary fixture but read Spanish archives,
resident pointers, descriptors, call targets and instruction anchors.
Spiral regressions also exercise the shared storage fixture against Spanish
images while explicitly preserving the entry's legacy binding alias.

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
