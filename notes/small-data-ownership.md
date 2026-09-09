# Small Data Ownership

Notes on moving `.sdata` out of link-resolved blobs and into the C
translation units that use it. Everything below was measured against the
resident image; the one conversion attempted so far did not match, and the
measurement is recorded rather than the intent.

## What the image looks like today

`config/slus_01411/split.yaml` interleaves two kinds of small-data
subsegment. A `.sdata` entry naming `game/NAME` hands the section to the
object built from that C source. A bare `sdata` entry naming
`initialized_data_ADDR` is a blob Splat disassembles, and every symbol inside
it is resolved by an assignment in `config/slus_01411/c_symbols.ld` rather
than defined anywhere.

Five sources own their small data today. Three of them are data-only
translation units that follow one shape — an explicit section attribute and
an initializer:

```c
u32 gSaveData_dwMaskStateLow __attribute__((section(".sdata"))) = 0x55555555;
```

Three blobs remain, at `0x8009AF08`, `0x8009AF2A` and `0x8009AF44`, plus a
larger one at `0x8009AF6C`.

## The owning unit is predictable, not a guess

Small-data order follows text order. Checking the two owners that appear in
`matching_c.json`:

| Owner | first `.text` | `.sdata` |
|---|---|---|
| `duel_trap_resolution` | `0x8001F0D0` | `0x8009AF24` |
| `duel_card_effects` | `0x800250C8` | `0x8009AF30` |

The blob at `0x8009AF2A` sits between those two, so its owner must be a unit
whose text sits between `0x8001F0D0` and `0x800250C8`.
`debug_effect_screen.c` does, at `0x800220B8`.

That prediction is confirmed independently: `debug_effect_screen.c` is the
**only** consumer of all three names in that blob, `D_8009AF2A`,
`D_8009AF2C` and `D_8009AF2D`.

The build already supports this. `generate_build_config.py` separates
`SMALL_SECTION_TYPES` from `TEXT_CLAIMED_SECTION_TYPES` precisely so a
matched text unit can own its `.sdata` in place, and pointing the subsegment
at `game/debug_effect_screen` produced a correct linker script — the object's
`.sdata` was emitted between `duel_trap_resolution`'s and
`duel_card_effects`', exactly where the blob had been.

So the ordering half of this work is not the obstacle.

## Measured: GCC will not reproduce the interior padding

The four bytes at `0x8009AF2A` are all zero in retail and lay out as:

| Address | Contents |
|---|---|
| `0x8009AF2A` | `D_8009AF2A`, one byte |
| `0x8009AF2B` | unnamed |
| `0x8009AF2C` | `D_8009AF2C[2]`, two bytes |

Writing that as the obvious C, with `aligned(2)` to produce the gap:

```c
u8 D_8009AF2A __attribute__((section(".sdata"))) = 0;
u8 D_8009AF2C[2] __attribute__((section(".sdata"), aligned(2))) = {0, 0};
```

gives an object whose `.sdata` is **six bytes, not four**:

```
.sdata  size=00000006  align=2**2
00000000 g  .sdata  D_8009AF2A
00000004 g  .sdata  D_8009AF2C
```

The section carries four-byte alignment and the pair lands at offset 4, not
offset 2. The extra two bytes push `.initialized_data` past the segment that
follows it, and the link fails outright:

```
section .bss_image_before_viewport VMA [8009b090,8009b145]
overlaps section .initialized_data VMA [800906e0,8009b093]
```

`aligned(2)` does not narrow it, because the constraint is the section
alignment rather than the object's. Any conversion of this blob has to
reproduce a one-byte hole inside a four-byte-aligned small-data section, and
the two-object spelling cannot.

## `D_8009AF2D` has to stay a linker alias regardless

Independently of the alignment problem, one of these names cannot move into C
at all. `D_8009AF2D` is the **second byte of** `D_8009AF2C`, and
`debug_effect_screen.c` uses both spellings:

```c
D_8009AF2C[D_8009AF2A]++;                      /* index can be 1 */
FntPrint(D_80010074, D_8009AF2C[0], D_8009AF2D);
```

This is the same pattern `memory-map.md` records for `D_80185CC8[1]` against
`D_80185CC9`, again inside a single file, so both spellings are faithful to
retail and neither can replace the other. C has no way to define a symbol for
the interior of an array — GCC's `alias` attribute is same-address only — so
naming a byte inside an object is exactly what a linker script is for.

A conversion here would at best be partial: C owning the bytes, with one
assignment left behind in `c_symbols.ld`.

## The other two blobs are blocked for different reasons

`0x8009AF44`, 32 bytes, names four symbols. Two of them, `D_8009AF44` and
`D_8009AF46`, are referenced only from `func_80030998.c`, which is one of the
five sources that are a raw `.word` listing with no C in them at all. Those
symbols therefore have no C spelling to preserve or to move, and the other
two sit far apart with long unnamed runs between them.

`0x8009AF08`, 28 bytes, names no symbols at all in `c_symbols.ld` and has no
C consumer. It does contain a pointer — the word at `+0x10` reads
`0x800E9E60` — so it is a record rather than scratch, but nothing in C
describes it.

## A mislabelled `pad`

`split.yaml` marks `0x8009AF2E` as `pad`, but `D_8009AF2E` is a live variable:
`debug_effect_screen.c` zeroes it, increments it, wraps it at 4 and switches
on it twice. It resolves from `link_symbols.ld` rather than `c_symbols.ld`.

The two bytes are genuinely padding as far as the *blob* is concerned, but the
label reads as "nothing here", and something is.

## Where this leaves the work

The predictable part is ordering: given a blob's address, the owning unit
falls out of text order and can be confirmed against the set of consumers.

The hard part is byte-level layout inside a small-data section. Until there
is a spelling that reproduces a one-byte hole at four-byte section alignment,
the `0x8009AF2A` blob stays as it is. The attempt is recorded here so the
next person starts from the measurement rather than repeating it.
