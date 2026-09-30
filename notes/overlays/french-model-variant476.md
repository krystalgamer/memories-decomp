# French MODEL headers 476 and 626

Two alternate-1 images for model 712 reuse the accepted header-459 webs
body through two three-line canonical symbol wrappers. Shared C, headers
and the named `gcc_2_8_1_g0_split` profile are unchanged. The authoritative
pipeline is GCC 2.8.1 and MASPSX 2.81.

Both complete canonical and production images match. All 192 preceding
module records are preserved; the full gate reproduces all 194 configured
French images.

## Loader and complete ownership

Model 712 uses compact record 612. Stages 9/10 select ten 2,048-byte
sectors starting at 169,112 and 169,122, loading at
`0x8013B000/0x8017B000`. Entry is at `+4`. The actual command is 642,000,
so initialization receives zero. The
[instance ledger](french-model-variant476-instances.csv) records the
independently read legal slices, actual header words and complete hashes.
Alternate-0 stages 7/8 are different images and are not registered here.

| Offset range | Bytes | Owner | Entry-call reachable |
|---|---:|---|---|
| `0x4..0x135C` | 4,952 | generated assembly | yes |
| `0x135C..0x170C` | 944 | generated assembly | yes |
| `0x170C..0x20BC` | 2,480 | generated assembly | yes |
| `0x20BC..0x247C` | 960 | webs C | no |
| `0x247C..0x2848` | 972 | generated assembly | yes |
| `0x2848..0x2DD4` | 1,420 | generated assembly | yes |
| `0x2DD4..0x3374` | 1,440 | generated assembly | yes |

Strict walks cover all fourteen spans with one terminal return per span.
Entry calls every other function except webs. **Webs is retained code,
not an established entry-call execution path.** Its measured context
compatibility does not prove runtime execution.

Each four-byte header and 7,308-byte suffix has one sized raw owner.
The suffix at `0x3374..0x5000` remains unclassified, not proven non-code.
All six unmatched functions per image remain generated assembly rather
than an opaque raw prefix or a C coverage claim.

## Independently observed layout

French entry captures `a0 -> s3 -> s8`, saves the initial web base at
`sp + 0x8C`, and initializes three 608-byte records at context `0..0x720`.
The next record area's independently formed address is `context + 0x720`.
Each record has two six-by-six `SVECTOR` grids at 0 and `0x120`, color at
`0x240` and scale at `0x254`. Entry's six/six/three bounds, pointer strides,
saved pointer and grid offset are checked by fourteen retail anchors.
Twenty-five freshly target-compiled constants verify this local web
view and the SDK argument types.

The reused helper reads the line packet at `+0x237C`, translation at
`+0x274C/+0x2750/+0x2754`, step at `+0x27B8`, and state at `+0x284C`.
It fades after scale `0x800`, grows toward `0x1000`, and sorts positive
depths without the other families' flag condition. The accepted source
is reused verbatim, not normalized to a sibling's behavior.

The selected 56-byte descriptor is at module `+0x3470`, indexed by
`command % 1000`. Both observed commands select descriptor zero, wholly
inside the single preserved suffix. The legal archive hash, both slices,
command words and target address-forming instructions were checked
independently of the compiler output.

## Context provenance and limitations

The matching resident initializer supplies fixed `0x80136000/0x80176000`
context pointers through slot `field_DEC`; the matching controller and
transfer phase establish secondary dispatch and selected load ranges.
Their three sized C owners agree with a fresh exact resident image.
Direct entry accesses establish a minimum extent of `0x2864` (10,340
bytes). This is not an allocation-capacity declaration or a complete
whole-game lifetime audit.

The selected 96-sector model, two-sector primary and ten-sector secondary
loads do not overlap that minimum context view. Other primary-context
writes and all potential dispatch paths are not inferred. No new backing
allocation is introduced, and the three initialized web records do not
declare the size of the entire context.

## Exactness and preservation

Discovery compiled only the newly accepted header-459 kernel with French
bindings. Header-number similarity was not acceptance evidence. Subsequent
actual canonical links reproduce both 20,480-byte images without masks,
instruction patches or post-link rebasing. The
[attempt ledger](french-model-variant476-attempts.csv) records the two
terminal wrapper fingerprints.

Preflight establishes two sized C owners, twelve explicit fallback
function owners and four raw owners across 40,960 compared bytes.
The C contribution is 1,920 instruction bytes. All 35 distinct resident
callees across the seven functions match a fresh resident with SHA-256
`57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44`.

The new fixture checks loader records, complete hashes, wrappers, all
function spans/calls, raw extents, initialization anchors and selected
descriptor/context separation. Aggregate progress assertions change with
registration; reports remain separate snapshots.
Configured totals become 194 images, 794/1,357 C instances and 726,588
C instruction bytes. These are not exhaustive runtime-coverage totals.

Final acceptance includes a clean exact French resident build, production
ELF checks of both C owners, twelve generated assembly owners and four
raw owners, all 35 callees and three caller owners, and recompilation of
all 25 layout constants. All 102 French MODEL variant and 47
progress/global-usage regressions pass, together with metadata,
basic-type and G32/PSXLONG policy checks.
