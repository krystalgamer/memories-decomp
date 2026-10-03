# USA MODEL headers 71 and 201

Fourteen newly registered USA images reuse the accepted
[French header-88 entry](../../src/overlays/french_model_variant/variant88_entry.c)
and its slot-1 wrapper unchanged. Each contributes one exact 2,384-byte C
function under `gcc_2_8_1_g0_split` (GCC 2.8.1, MASPSX 2.81), for **33,376
additional matching C bytes**. These older headers use a slot delta of 130,
not the later MODEL variants' delta of 150 or their GCC 2.7.2 profile.

The [instance ledger](usa-model-variant71-instances.csv) records the USA
slice hashes, compact record indices, stages, load addresses and commands.
Models 290, 295, 501 and 518 use stages 7/8; models 31, 408 and 531 use
stages 9/10. All fourteen complete images are distinct.

## Loader and resident contracts

[`Model_LoadMonsterMerge`](../../src/game/model_load_monster_merge.c)
selects compact 276-sector MODEL records. The stage-7 through stage-10
cases in [the transfer callback](../../src/game/model_texture_transfer.c)
select ten sectors at record offsets 180, 190, 200 and 210. The two
secondary load addresses are `0x8013B000` and `0x8017B000`.
[The controller](../../src/game/model_control.c) calls the entry at load
address plus four, supplying the secondary context and `request % 1000`
on initialization, and `-1` during updates.

The USA records select commands 18000, 18002, 18003, 18004, 18005, 18006
and 18007. The selected 24-byte descriptors start at image offset
`0x970 + (command % 1000) * 24`. Their group/count products are 20, 30,
32, 40, 48 or 60, their periods are positive, and they select at most five
model parts. These accesses fit the donor's measured point and timer spans.
The donor's overlapping texture/completion byte at context `0x140C` and
image/descriptor views at image `0x970` remain unchanged; no extra data
objects or allocation-capacity claims are introduced.

Every image's 23 direct calls resolve to the nineteen named USA resident
function starts in
[the linker map](../../config/slus_01411/overlays/model_variant71_linker_symbols.txt).
The bindings are checked against the USA function inventory, the actual
retail call instructions and the linked ELF symbols. A clean USA resident
build reproduces every callee body and the entire executable, with SHA-256
`84a54ed74f3d0edd6d81380839f7e4ef5bfb21ecea18be9a062bd6bfa5a45c88`.
The implementation uses the existing canonical resident/SDK declarations.

## Ownership and validation

| Image range | Owner | Bytes per image |
|---|---|---:|
| `0..4` | raw header | 4 |
| `4..0x954` | shared entry C | 2,384 |
| `0x954..0x5000` | unclassified raw suffix | 18,092 |

Both canonical compiler objects match all 596 instructions. All fourteen
complete production images match without masks. The ELF checks confirm the
entry's `0x950`-byte `.entry` function and both sized, section-defined raw
owners in every image; the suffix is not an absolute alias or padding claim.
All 253,288 suffix bytes remain unclassified.

Acceptance commands, from the repository root with legal USA inputs:

```sh
make match
make match-overlays
make check-declaration-visibility
make check-metadata check-notes check-g32 check-data-symbols
tools/environments/python/bin/python -m unittest \
    tools.project.tests.test_usa_model_variant71 \
    tools.project.tests.test_model_variant_toolchain
```

The full overlay build verifies 277 images, including all 263 previous
registrations. Ten focused tests check the compiler families, shared source
wiring, raw suffix, resident bindings and actual loader commands/descriptors.
The broader extraction/source suite and these focused tests pass all 60 checks.
These are newly configured images: the previously configured queue of 190
unresolved function instances is unchanged. This addition is not an
exhaustive MODEL or runtime-code census. The project-wide generated progress
snapshot is refreshed separately.

The shared entry header includes the canonical `Model_GetFrameStep`
declaration from `src/game/func_80058E1C.h`. The declaration-visibility
check caught the omitted include when the French donor was registered in
the USA build; adding it preserves all 277 complete USA overlay images.
