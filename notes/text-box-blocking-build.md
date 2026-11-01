# Blocking text builds and glyph scratch

`TextBox_BuildBlocking` (`0x80039A14`) sets the existing build-request bit
`0x800`, calls `TextBox_BuildStep` at least once, and continues until the
existing done bit `0x2000` is set. `TextBox_BuildBlockingWithGlyphScratch`
(`0x80039A60`) preserves the same loop and literal `0xA00`, additionally setting
the legacy glyph-scratch bit `0x200`. Neither helper waits for GPU completion.

The three-function `text_box_runtime.c` object retains its accepted
`TextBox_SetPos` sibling and all seven `gcc_2_8_1_g8` selections. Definition
order and `0xE0/0x4C/0x4C` extents remain unchanged. No flags are cleared
before the first iteration and no timeout, frame advance or new guard is added.

## What the extra bit actually does

NA/Japanese appenders select a `0x88`-byte scratch record per glyph when
`0x200` is set. `gText_GlyphUploadScratch`, formerly `D_801DA000`, keeps the
original incomplete `s16[]` view. The record starts with a RECT prefix;
Japan rasterizes the 16x16 4bpp bitmap at byte `+8`. No array capacity or shared
record struct is inferred.

Japan selects `LoadImage` with the flag and `LoadImage2` without it. The local
retail GPU dispatch table at `0x80094620` identifies slot `+8` as `_addque2`
and slot `+0x20` as `_dws`. `LoadImage` submits that handler through the queue;
`LoadImage2` polls DMA/GPU readiness and invokes the handler directly. Thus the
extra bit is not evidence for a faster/no-wait text-build API, and `LoadImage`
must not be described as the synchronous variant.

The European compact proportional-font appender does not consume `0x200` or
this scratch buffer. Its helper nevertheless retains the same legacy flag
literal and loop. The name identifies the requested legacy option, not a
claim that every region rasterizes or uploads the same way.

## Compatibility and exactness

All existing headers and frozen overlay C remain byte-identical. Canonical
declarations are isolated in `text_box_blocking_build.h`, using the existing
channel tag rather than importing the shared channel typedef: Free Duel has
a private byte-array view that would collide with `duel_effect.h`.

Resident calls use the canonical APIs. Every registered frozen consumer's
actual independent import map exposes the required native canonical binding
plus its legacy function alias. No numeric header or frozen C body is rewritten,
and no historical fingerprint is refreshed.

Scratch is a view of existing storage, not a new allocation. NA and Japan bind
the canonical name directly at `0x801DA000` in their `symbols.txt`; the former
`D_801DA000` provider and its link-symbol aliases are gone. No compiler flags,
source profiles or section positions are introduced. Europe receives no unused
scratch binding.

Complete compiler/MASPSX comparisons cover actual translation units and retain
all instructions, directives and relocations. Frozen outputs compare with zero
substitutions; only resident identifiers change. These author-side probes do
not replace fresh full-image PR checks. No synthetic/generated-C/source-string
test machinery or local cloud/full-image suite is added.
