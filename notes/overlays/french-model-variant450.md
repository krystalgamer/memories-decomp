# French MODEL450 shared PAL helpers

French model 174, stages 9 and 10, contains two complete 20,480-byte images
with headers 450/600 and command 616000. Independent reads and hashes of
both legally obtained archives establish that these complete French images
are byte-identical to their accepted Spanish counterparts.
[The instance table](french-model-variant450-instances.csv) records the
French loader slices and hashes; no Spanish input substitutes for a French
target check.

The matching manifests select the existing accepted Spanish sources
directly, including their existing slot-1 wrappers. No tracked source/header
duplication, regional macro, new compiler profile, or other region's metadata
change is needed. All four French helpers were independently compiled with
`gcc_2_8_1_g0_split` (GCC 2.8.1/MASPSX 2.81):

| Offset | Bytes | Frame | Selected implementation |
| --- | ---: | ---: | --- |
| `0x27C0..0x2D88` | 1,480 | 264 | `spanish_model_variant/variant450_quads.c` |
| `0x2D88..0x310C` | 900 | 288 | `spanish_model_variant/variant450_lines.c` |

The slot-1 files only rename each function. This adds four matching C
instances and 4,760 instruction bytes, not a newly invented algorithm.
[The attempt ledger](french-model-variant450-attempts.csv) records the
independent French compilations and terminal production ownership checks.
The historical Spanish experiments remain in their original ledgers.

## Inventory and unresolved scope

All seven function spans have independently checked, closed contiguous
control-flow graphs: `4..DD8`, `DD8..1794`, `1794..1ECC`, `1ECC..27C0`,
`27C0..2D88`, `2D88..310C`, and `310C..3940`. Only the two selected helpers
are C. The five other functions in each image remain generated assembly.
Closed retained functions are not automatically claimed to be entry-reachable.

The four-byte header and complete `0x3940..0x5000` suffix keep raw owners.
The suffix is 5,824 bytes per image, remains unclassified, and is not
excluded from further research. Registering the two images adds 14
inventoried functions; it does not establish exhaustive French coverage.

The entry calls lines at `0xC34` and quads at `0xC3C`, with `s3` passed
in both delay slots. All 35 distinct external targets resolve to actual
French resident function starts. Canonical SDK names replace duplicate
aliases for `GsSortPoly` and `RotTransPers4`; other unresolved regional
placeholders remain address-based.

## Shared views and explicit limits

The accepted [line recovery](spanish-model-variant450.md) and
[quad recovery](spanish-model-variant450-quads.md) document the source
structure, packet arithmetic, caller assumptions, and isolated execution
evidence. Their source files are shared, not rewritten. Sixty-five
target-compiled constants recheck the two bounded views and SDK layouts.
The line groups, primary records, and fade views have strides 104, 536,
and 160; the seven quad groups have stride 160.

French resident pointers place secondary contexts at `0x80136000` and
`0x80176000`. The line view ends at `+0x42F8`; the quad view ends at
`+0x42FC`. Their prefixes overlap the separate primary-handler loader
windows at `0x8013A000` and `0x8017A000` by **760 and 764 bytes**,
respectively. This measured overlap is not an allocation-size, lifetime,
complete-context, or noninterference proof. No such claim is introduced
by reusing the exact sources.

The quad's depth scaling precedes the nonnegative-depth test, preserving
the retail negative-one rounding case. Its unsigned configuration
arithmetic and zero-duration behavior are unchanged. The lines retain
their four initial angle calls, aliased projection output, conditional
threshold submission, and per-fading-group phase increments.

Complete-image identity and real sized linked C ownership remain the
acceptance gate, rather than donor identity alone. The production check
must cover all 18 C/assembly/raw owners, all other configured French
images, and the French resident. The saved exact French resident ELF also
supports 38 independently verified loader/controller/callee owners.
