# French MODEL headers 393 and 543

The 22 independent ten-sector images in
[the instance ledger](french-model-variant393-instances.csv) use header 393
at `0x8013B000` and header 543 at `0x8017B000`. Only the rings helper is
matching C. The complete images retain their four-byte headers, three
assembly functions and 11,344-byte raw suffixes.

| Offset | Bytes | Treatment |
| --- | ---: | --- |
| `0` | 4 | Raw header |
| `4` | 3,544 | Assembly entry |
| `0xDDC` | 2,344 | Assembly ribbon |
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

The complete-image proof accounts for 22 real C owners / 25,520 bytes,
66 preserved assembly owners / 175,384 bytes, and 44 raw owners /
249,656 bytes. Those categories cover all 450,560 image bytes without
masked comparisons or patched instructions. The selected C object must
define the sized function in the actual link, not merely reside nearby.
Every four-function image has a fresh delay-slot-aware closed CFG, all
words reached, one return and no unresolved or indirect transfer.
The entry directly calls all three helpers.

[The attempt ledger](french-model-variant393-attempts.csv) also retains the
two nonexact sibling calibrations in both slots. The unchanged NA376
ribbon produces 2,320 bytes / frame 312 against 2,344 / 320, with 328
unequal words. Streamers produce 2,024 / 328 against 2,084 / 328, with
515 unequal words. Neither result is promoted.

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
No array of 56-byte records is declared. Likewise, the state view's
`sizeof == 0xDFC` is not an allocation bound: the entry accesses a slot
halfword at `0xE08`, requiring at least `0xE0A` direct bytes. Neither
partial view asserts complete capacity or global lifetime.

The raw suffix is not declared padding, unreachable code or a completely
classified data structure. No global-usage or progress snapshot is
regenerated as part of this matching change.
