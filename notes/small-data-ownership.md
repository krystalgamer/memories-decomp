# Small Data Ownership

Notes on moving `.sdata` out of link-resolved blobs and into the C
translation units that use it. Everything below was measured against the
resident image; successful and blocked conversions are both recorded so
later work starts from measured compiler behavior.

## What the image looks like today

`config/slus_01411/split.yaml` interleaves two kinds of small-data
subsegment. A `.sdata` entry naming `game/NAME` hands the section to the
object built from that C source. A bare `sdata` entry naming
`initialized_data_ADDR` is a blob Splat disassembles, and every symbol inside
it is resolved by an assignment in `config/slus_01411/c_symbols.ld` rather
than defined anywhere.

Several sources own their small data today. Data-only translation units
generally follow one shape — an explicit section attribute and an initializer:

```c
u32 gSaveData_dwMaskStateLow __attribute__((section(".sdata"))) = 0x55555555;
```

One small blob remains, at `0x8009AF2A`. The former `0x8009AF08` watchdog
word is now owned by `main_services.c`, and the former
`0x8009AF6C` blob is split around three C-owned interior ranges: a 28-byte
head at `0x8009AF6C`, model/graphics state at `0x8009AF88`, model primitive
templates at `0x8009AFAC`, model handler state at `0x8009AFE4`, and a 40-byte
tail at `0x8009B058`.

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

## `0x8009AF44` is C-owned with scalar halfword packing

After the following format and layout ranges moved into their own C objects,
the remaining extracted range was 16 bytes:

```text
00 05 01 00 00 00 01 00 0f 00 f0 00 00 0f 00 f0
```

It has public symbols at `D_8009AF44`, `D_8009AF46` and `D_8009AF4C`,
corresponding to offsets `0`, `2` and `8`. A natural C spelling using one
scalar and two arrays did not reproduce that layout: GCC 2.8.1 emitted
`.align 2` before each array, producing offsets `0`, `4` and `12` and a
20-byte section. Packed wrapper types and reduced variable-alignment
attributes produced the same assembly.

The exact spelling is eight ordered `u16` scalars. GCC emits `.align 1` for
each scalar, so the three public boundary names land at the retail offsets
and the unnamed continuation values remain local to
`frontend_debug_constants.c`. The resulting `.sdata` section is exactly 16
bytes with two-byte alignment, and the complete executable matches.

`D_8009AF44` and `D_8009AF46` remain relocation targets in `func_80030998`,
which is generated assembly again; `D_8009AF4C` remains the start of the eight-byte mask block
read by unmatched `func_80030294`.

## `0x8009AF88` is one C-owned model/graphics state block

The 36 bytes from `D_8009AF88` through the unnamed continuation at
`0x8009AFAB` are now emitted by `model_graphics_state.c`. They combine the
active model-record pointer, model view/scene state, graphics buffer/frame
state, and six trailing bytes reached by unmatched model code.

The exact layout needs separate scalar objects rather than a struct because
assembly names eighteen interior addresses independently. Explicit `.sdata`
attributes keep zero-valued objects out of `.sbss`; private continuation
scalars preserve the unnamed bytes. The resulting object has:

```text
.sdata  size=00000024  align=2**2
00000000 R_MIPS_32 D_80091008
```

Every public symbol lands at its retail offset, and the linked payload matches
the original 36 bytes.

Two compiler views are deliberately retained. `graphics_frame.c` needs
absolute, non-volatile declarations for `D_8009AFA2`-`D_8009AFA4`, while
`func_80058E1C` needs the volatile small-data view of `D_8009AFA3`.
`func_8004E7B0` also keeps tentative common definitions of `D_8009AF88`,
`D_8009AF8E`, and `D_8009AF90`: changing them to extern shortens its text by
four bytes. The data-only unit remains the strong definition, so those commons
allocate no storage.

## `0x8009AFE4` is one C-owned model-handler state block

The 116 bytes from `D_8009AFE4` through the end of `D_8009B050` are emitted by
`model_handler_state.c`. The prefix contains five byte/halfword state labels,
including a private halfword continuation that preserves the unnamed bytes at
`0x8009AFEA`. Two word pairs and two mutable words follow. The rest of the
block is eleven fixed-size diagnostic strings and formatting fragments used by
the adjacent unmatched model handlers.

An isolated GCC 2.8.1 probe reproduced the entire prefix byte-for-byte:

```text
.sdata  size=00000074  align=2**2
no relocations
```

All twenty public symbols land at their extracted offsets. Splitting the
original blob at `D_8009B058` leaves the unrelated 40-byte tail in generated
assembly rather than assigning it to the model-handler owner.

## The runtime watchdog word

The four-byte extracted blob at `0x8009AF08` is `runtime_gp`, the watchdog
counter read, decremented, and reset to `0x3C` only by `main_services.c`.
That unit now defines the word in `.sdata`, preserving its `%gp_rel` accesses
and its position before `main_frame`'s small data. The generated blob is no
longer needed.

The ordinary initialized-data region has the same ownership model. Its former
36-byte leading blob at `0x800906E0` is now `psyq/startup_data.c`, leaving the
large `0x80091958` range as the only generated `.data` blob after the mapped
tables and file-name records.

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
