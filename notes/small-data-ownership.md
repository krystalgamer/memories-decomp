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

No game-owned small-data blob remains. The last one, at `0x8009AF2A`, is now
the six-byte `gDebugEffect_abPreviewState` array in
`debug_effect_screen.c`. The former `0x8009AF08` watchdog word is owned by
`main_services.c`, and the former `0x8009AF6C` blob is fully split among
addressed data owners through `ai_script_source_line_format.c`.

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

## One packed object reproduces the interior padding

The four bytes at `0x8009AF2A` are all zero in retail and lay out as:

| Address | Contents |
|---|---|
| `0x8009AF2A` | `D_8009AF2A`, one byte |
| `0x8009AF2B` | unnamed |
| `0x8009AF2C` | `D_8009AF2C[2]`, two bytes |

Writing that as separate C objects, with `aligned(2)` to produce the gap:

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
alignment rather than the object's. The successful spelling avoids a second
object entirely:

```c
u8 gDebugEffect_abPreviewState[6]
    __attribute__((section(".sdata"))) = {0};
```

Byte 0 is the axis selector, bytes 2 and 3 are the two coordinates, and byte 4
is the preview page. Bytes 1 and 5 are explicit unused storage. The one array
has no interior object alignment to satisfy, so GCC emits a six-byte `.sdata`
section beginning exactly at `0x8009AF2A`.

## Overlapping labels do not require overlapping C objects

`D_8009AF2D` was the **second byte of** `D_8009AF2C`, and
`debug_effect_screen.c` used both spellings:

```c
D_8009AF2C[D_8009AF2A]++;                      /* index can be 1 */
FntPrint(D_80010074, D_8009AF2C[0], D_8009AF2D);
```

Both spellings were faithful views, but they were not required ABI identities:
this file was their only consumer. Replacing them together with
`gDebugEffect_abPreviewState[2]` and `[3]` preserves the same `%gp_rel`
displacements without asking C to define an interior alias. The same
translation unit also replaces `D_8009AF2E` with element 4, which proves the
two bytes formerly marked as `pad` contain one live byte followed by one
unused byte.

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
state, and the trailing model-scene fields.

Independently addressed scalar fields retain their symbols and explicit
`.sdata` placement. The four bytes at `0x8009AFA4..0x8009AFA7` now have one
real `u8 D_8009AFA4[4]` owner: the frame-step override, an unclassified byte,
the byte named `D_8009AFA6`, and the active-slot index used by matching
`func_800507D0`. The latter is not padding. `link_symbols.ld` preserves
`D_8009AFA6 = D_8009AFA4 + 2` as an interior identity, not a second allocation.
The halfwords at A8/AA remain separate and outside this four-byte extent.
The resulting object still has:

```text
.sdata  size=00000024  align=2**2
00000000 R_MIPS_32 D_80091008
```

Every public symbol lands at its retail offset, and the linked payload matches
the original 36 bytes.

Existing scalar compiler views are deliberately retained. `graphics_frame.c` needs
absolute, non-volatile declarations for `D_8009AFA2`-`D_8009AFA4`, while
`func_80058E1C` needs the volatile small-data view of `D_8009AFA3`.
`MODEL_GRAPHICS_STATE_SCENE_BYTES` selects the bounded four-byte view for
the owner and scene consumer; it cannot be combined with the absolute frame
view. Scalar readers and writers still address the first byte.
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
large `0x80091958-0x8009AF08` range as the only generated `.data` blob after
the mapped tables and file-name records. Those exact adjacent split
boundaries make it `0x95B0`, or 38,320 bytes; shorter label-span totals omit
3,596 unnamed bytes inside the same Psy-Q SDK data subsegment.

## Where this leaves the work

The resident game-owned initialized and small-data windows are now C-owned.
The only remaining ordinary initialized-data blob begins at `0x80091958`;
the ownership census in `build.md` attributes its 274 referenced labels
exclusively to Psy-Q SDK functions, so it is vendor data rather than unfinished
game data.
