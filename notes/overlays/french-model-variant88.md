# French MODEL headers 88 and 218

Fourteen distinct images for models 31, 290, 295, 408, 501, 518 and 531
now have an exact 2,384-byte entry using the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
The slot-1 wrapper only renames the entry and its opaque suffix symbol.
No compiler profile, shared SDK header or other regional implementation
changes.

## Loader and ownership

The [instance ledger](french-model-variant88-instances.csv) records all
physical slices, compact indices, stages, commands and complete hashes.
Models 290/295/501/518 use stages 7/8; models 31/408/531 use stages 9/10.
The established loader selects ten sectors at compact-record offsets
180/190 or 200/210, loading at `0x8013B000` or `0x8017B000`.
The matched controller supplies `request % 1000` for initialization and
`-1` for later updates, with the same fixed secondary-context pointer.
Three actual linked resident caller owners and their retail bytes were
checked independently.

| Offset range | Bytes | Physical owner |
|---|---:|---|
| `0..4` | 4 | raw header |
| `4..0x954` | 2384 | entry C |
| `0x954..0x5000` | 18092 | one unclassified raw suffix |

Both canonical slot objects were independently linked into all fourteen
complete images. Every unmasked byte matches, with sized C and raw
owners verified in the resulting ELFs. Nineteen actual resident callee
bodies and all 23 call relocations per slot were checked, not merely
symbol names or candidate locations.

## Measured views and lifetimes

Twenty-five target-compiled constants establish the local 24-byte
descriptor, context offsets and existing SDK layouts. The context has
its descriptor pointer at zero, a point span at `4..0x1004`, a timer span
at `0x1004..0x1404`, frame at `0x1404`, and texture words at
`0x1408/0x140C`. The completion byte intentionally overlaps the second
texture word at `0x140C`. These are accessed views, not a claim about
total allocation capacity or initialized padding.

Actual requests `18000/18002/18003/18004/18005/18006/18007` select the
corresponding descriptor indices at `0x970 + index * 24`.
Selected group/count products are 20, 30, 32, 40, 48 or 60; periods are
positive and at most five part indices are selected. Their point and
timer accesses fit the measured offset spans. The minimum context extent
`0x1410` is separate from the selected model, primary module and overlay
loads; no new backing object or whole-game noninterference claim is made.

The two 28-byte image views begin at `0x954` and `0x970`. The latter
overlaps descriptor storage. Canonical C therefore declares only the
opaque suffix and obtains typed views at offsets zero and `0x1C`;
it does not invent separate overlapping data objects or additional
resource ownership.

## Exactness evidence and remaining scope

The [attempt ledger](french-model-variant88-attempts.csv) preserves 43
historical/current experiments and two terminal canonical matches.
Fresh controls reproduce five terminal differences for the flat literal
form and one difference for the zero-flag increment form. A retained
1,028-byte suffix fragment provided a new nested positive-completion
branch structure; applying that structure produces the exact entry,
including the literal result instruction at image offset `0x910`.
The single-owner canonical views preserve the complete match.

Historical receipts that omitted their profile retain an empty profile
cell rather than a guessed flag set. Their differing-word counts are
recomputed over the full stored linked text and target lengths. The
current experiments and terminal rows explicitly name the authoritative
profile.

The retained fragment has different context offsets and unresolved
backward flow; it is structural evidence, not a complete recovered
function or proof of compiler, build date, regional origin or execution.
Four additional tiny return signatures in two suffixes also remain
unattributed. None is registered, promoted or declared harmless padding.
All 253,288 suffix bytes in this family remain in the unresolved scope.

This independent branch starts at accepted `eba0780d3c2d054a324a62f4f03fd77df317543b`
and preserves all 293 prior French registrations. It adds fourteen C
instances and 33,376 C instruction bytes. Expected configured totals are
1,619/1,897 C instances, 2,193,764 C instruction bytes and 307 images;
these are configured counts, not exhaustive runtime completion.
Progress snapshots remain separate.

Production acceptance reproduces all 307 complete French overlays and
the clean French resident. Actual production ELFs verify the fourteen
new C owners, twenty-eight raw owners and resident callees/callers;
the canonical header also reproduces all 25 target layout constants.
The seven new regressions and 57 source/extraction/toolchain checks pass.
The broader French suite passes 366 tests with one unrelated optional
MODEL400 native-pointer check skipped because host clang is unavailable.
Basic types, external attempts, G32 and matching-source contracts pass.
