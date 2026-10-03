# French MODEL headers 393 and 543

The 22 independent ten-sector images in
[the instance ledger](french-model-variant393-instances.csv) use header 393
at `0x8013B000` and header 543 at `0x8017B000`. The entry, ribbon and rings
helpers are matching C. The complete images retain their four-byte headers,
assembly streamer functions and 11,344-byte raw suffixes.

| Offset | Bytes | Treatment |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3,544 | Matching entry C |
| `0xDDC` | 2,344 | Matching ribbon C |
| `0x1704` | 1,160 | Matching rings C |
| `0x1B8C` | 2,084 | Assembly streamers |
| `0x23B0` | 11,344 | Raw, unclassified suffix |

## Source and ownership

Two French wrappers rename `func_8013C6D8` in the unchanged accepted
`src/overlays/model_variant/variant376_rings.c` body to `func_8013C704` and
`func_8017C704`. The unchanged local headers supply the accessed views.
The authoritative new-work profile is `gcc_2_8_1_g0_split`, GCC 2.8.1 with
MASPSX 2.81; the older NA376 registration's historical compiler is not
used. Both slots reproduce all 1,160 bytes and the 264-byte frame.

The independent French entry uses `variant393_entry.c` and a complete
slot-one wrapper, including all three helper names and the suffix symbol.
It reuses the accepted French337 record, ring and streamer views after
independent offset verification, not an assumed descriptor or state layout.
Both entries reproduce 3,544 bytes with a 208-byte frame under the same
named GCC 2.8.1 / MASPSX 2.81 profile.

The complete-image proof accounts for 66 real C owners / 155,056 bytes,
22 preserved assembly owners / 45,848 bytes, and 44 raw owners /
249,656 bytes. Those categories cover all 450,560 image bytes without
masked comparisons or patched instructions. The selected C object must
define the sized function in the actual link, not merely reside nearby.
Every four-function image has a fresh delay-slot-aware closed CFG, all
words reached, one return and no unresolved or indirect transfer.
The entry directly calls all three helpers.

[The attempt ledger](french-model-variant393-attempts.csv) retains the
original nonexact sibling calibrations in both slots. The unchanged NA376
ribbon produced 2,320 bytes / frame 312 against 2,344 / 320, with 328
unequal words. Streamers produced 2,024 / 328 against 2,084 / 328, with
515 unequal words. Those rejected candidates are not promoted.

The ribbon now reuses `variant376_ribbon.c` through two French wrappers.
One measured `VERSION_FRENCH` branch follows the existing French337 ribbon
pattern: at the final point, use the current index `k` and previous index
`k - 1` instead of literal 16 and 15. Both spellings address the same
seventeen-point arrays, but their old-GCC induction lifetimes differ.
This recovers all 2,344 bytes and the 320-byte frame in both slots.
The non-French branch retains its original source exactly; affected NA
overlay images are independently matched without changing their existing
registrations or compiler profiles.

The ribbon's `0x3A4` view is unchanged: spine / projection / angle at
`0` / `0x88` / `0xCC`, displaced spine / projection / width at
`0x110` / `0x198` / `0x1DC`, color at `0x220`, flags / depth at
`0x2D8` / `0x31C`, and two seventeen-halfword offset arrays at
`0x360` / `0x382`. Target compilation checks the complete record and
all these fields. Actual descriptor counts are nonzero in all 22 observed
images; this does not establish safety for arbitrary commands or writers.

Twelve paired entry experiments are preserved in that ledger. The first
3,544-byte candidate differed only in the ordering of the counter reset
and two projection loads. Moving the reset earlier lost first-iteration
zero folding; type, mask and ordinary loop spellings did not recover the
retail sequence. An explicit zero-count guard followed by a do-loop,
retaining the sampled descriptor through the mode and vertex reads and
reloading it after the counter increment, matches both slots without
artificial stores, dependencies or assembly. The two terminal records
cover canonical compiler inputs and all 22 complete images.

## Loader, selector and accessed views

The compact MODEL record formula is independently checked against the
resident `Model_LoadMonsterMerge` implementation. The selected record
uses sectors `record * 276 + 180 + slot * 10` for stages 7/8 or
`record * 276 + 200 + slot * 10` for stages 9/10. The corresponding
signed command at metadata sector 275, offset `0x110` or `0x114`, is
`559000 + selector`. Actual selectors span 0 through 7.

The exact French initializer `0x8004FC2C`, controller `0x80058B4C` and
transfer callback `0x80059EF4` establish the stored secondary contexts,
slot-specific code destinations and `request % 1000` dispatch to image
offset 4. Retail pointer storage, complete linked resident bodies and
their actual input definitions were checked, not inferred from relocated
overlay addresses. Including the merge loader and all overlay callees,
the proof checks 38 actual resident owners. In particular, `0x8005C4D8`
is `func_800593D0`, not an interior alias of a nearby matrix routine.

Entry offsets `0xC` / `0x14` capture the original context. Rings begin
at context `0x3A4`; entry increments at `0x80C` / `0x828` and bound at
`0x820` establish two 152-byte records, ending at `0x4D4`. The direct
call at image `0xC34` passes the same context in its `0xC38` delay slot.
The helper accesses the GT4 at `0xC38`, matrix at `0xD1C`, target vector
at `0xD3C`, frame / elapsed / step at `0xDA0` / `0xDA4` / `0xDAC`,
configuration pointer at `0xDB4`, single-ring flag at `0xDC8`, and state
at `0xDF8`. Target compilation confirms these local views and SDK sizes.

**Descriptor stride is 68 bytes, not 56.** Entry instructions at
`0x8C..0x94` compute `(selector * 16 + selector) * 4`, indexing the
table at image `0x24AC`. `ModelVariant376Config` is only the helper's
56-byte accessed prefix: start / end at `0x24` / `0x28`, fade times at
`0x30` / `0x34`. The entry additionally reads through descriptor `0x40`.
No array of 56-byte records is declared. The entry's independently
recovered descriptor is `0x44` bytes. Its state extends through the slot
and command halfwords at `0xE08` / `0xE0A`, requiring a minimum `0xE0C`
bytes, not an allocation bound. The renderer's `sizeof == 0xDFC` remains
a partial view. Neither view asserts complete capacity or global lifetime.

The entry has one 932-byte record, two 152-byte rings at `0x3A4`, and two
888-byte streamers at `0x4D4`. Matrix / target / three sampled positions
are at `0xD1C` / `0xD3C` / `0xD44`; the part counter is at `0xDC8` and
phase at `0xDF8`. Eighty target-compiled entry layout constants cover
these views, including canonical signed `PSXLONG` projection outputs.
The 64 bytes preceding stack projection inputs remain unrecovered
locals, not invented matrices.

Fresh metadata and descriptor reads across all 22 images observe
mode/count pairs `(0, 2)` fourteen times, `(1, 2)` six times and `(0, 3)`
twice. Translation triples are `(0, -64, 32)`, `(0, 0, 0)`, `(0, 0, 80)`
and `(0, 16, -32)`. This is the observed selector domain, not proof of
arbitrary-command safety or complete writer ownership.

Initialization samples vertices only for mode 1; updates sample them for
every nonzero mode. The slot-zero matrix X translation adds the descriptor
offset during initialization but subtracts it during per-part updates.
Both asymmetries are preserved, as are two separate frame-step getter
calls. Negative commands retain the original context for all three helpers.

The raw suffix is not declared padding, unreachable code or a completely
classified data structure. No global-usage or progress snapshot is
regenerated as part of this matching change.
