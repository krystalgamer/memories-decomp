# Spanish MODEL headers 415 and 565

Ten independently checked Spanish secondary images for models 102, 282,
288, 642 and 645 at stages 9/10 contain three exact C helpers under the
named `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
Sheets and webs reuse unchanged accepted French wrappers and header-398
bodies. Bands uses two directly selected Spanish sources with verified
indexed next-column expressions. Accepted North American compiler
selection is not a rule for this campaign.

## Images and complete ownership

The [instance ledger](spanish-model-variant415-instances.csv) records
compact records, commands, sectors and all ten distinct complete hashes.
Each ten-sector image loads at `0x8013B000/0x8017B000`, with entry at `+4`.
Its archive sector is `record * 276 + 180 + (stage - 7) * 10`.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0x12B4` | 4,784 | generated assembly | yes |
| `0x12B4..0x18A4` | 1,520 | generated assembly | yes |
| `0x18A4..0x2024` | 1,920 | bands C | yes |
| `0x2024..0x2508` | 1,252 | sheets C | yes |
| `0x2508..0x2A34` | 1,324 | webs C | no |
| `0x2A34..0x2FB8` | 1,412 | generated assembly | yes |

All 204,800 image bytes match without masks or instruction patches.
Thirty selected, sized compiler owners contribute 44,960 C bytes.
Thirty assembly owners retain 77,160 bytes. Headers and each 8,264-byte
suffix have real raw owners; the 82,640 suffix bytes remain unclassified.
Webs is retained game code without a claimed direct entry-call path.

## Independently recovered records and storage

The 106 target-compiled C/SDK constants and 74 Spanish instruction anchors
per image check record fields, initialization, strides, helper accesses,
stack storage and descriptor formation.

Three 416-byte narrow webs occupy context `0..0x4E0`, with 4-by-6
`SVECTOR` grids at `0/0xC0`, color `0x180`, scale `0x194` and done `0x198`.
One 456-byte band then occupies `0x4E0..0x6A8`. Its nine-point vector
arrays start at `0/72/144`, screen words at `216/252/288`, colors at
`324/360` and nine depth words at `420`. Two 152-byte sheets occupy
`0x6A8..0x7D8`.

Unlike MODEL414, the band's nine projection flags are a local 36-byte
array at stack `+200` in a 296-byte frame, not fields appended to the
context band. The saved flag pointer, row/column arithmetic, projection
argument and zeroing instructions independently establish this ownership.
The projection interpolation output at stack `+240` is separate.

Bands uses the packet at context `0x1854`, phase at `0x19EC`, radius
halfword at `0x1A18` and path/state words at `0x1A1C/0x1A30`.
Actual commands `581000/581001/581004` select 48-byte descriptors at
`module + 0x30B4 + (command % 1000) * 48`, inside the suffix owner.
The descriptor pointer is stored at context `0x1A00`.

All 36 real resident callee input/linked owners, three matching
initializer/controller/loader owners and both raw context-pointer owners
were checked against the fresh exact Spanish executable. Its SHA-256 is
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.
Input `.data` in `spanish_raw_80010000.o` supplies `0x80010024/28`,
pointing to `0x80136000/0x80176000`. The mixed executable output section
does not replace proof of the actual data input owner.

Direct entry accesses establish a minimum context extent of `0x1A44`,
separate from the selected model, auxiliary and secondary loads.
Neither this view nor the helper stack evidence proves complete context
allocation capacity or whole-game lifetime isolation.

## Refinement and integration

The [experiment ledger](spanish-model-variant415-experiments.csv) preserves
the initial 1,920-byte GCC 2.8.1 result with four different loads.
Using `band->sc[j + 1]` and `band->sb[j + 1]` instead of the shifted
`col` pointer's index-one expressions reproduces the addressing at helper
offsets `0x528/0x534/0x558/0x564`. The
[terminal ledger](spanish-model-variant415-attempts.csv) records all six
selected sources. Original shared bodies, declarations and profiles are
unchanged. Candidate sources, recursive hashes, compiler objects and
complete image/layout/owner evidence remain in local scratch storage.

The independent accepted-master baseline preserves all 132 Spanish
registrations, including accepted MODEL442. Adding ten images yields
142 images, 670/986 C instances and 628,156 C instruction bytes.
Pending MODEL414 bands, MODEL445, MODEL440 webs and MODEL433 webs
are not stacked. Regional fixture selection
preserves French defaults; no report regeneration is part of this change.
Unknown spans and the wider seven-release runtime campaign remain open.
