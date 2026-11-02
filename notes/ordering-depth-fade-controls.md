# Ordering-table depth bases and fade controls

| Former identity | Canonical name | Preserved view |
| --- | --- | --- |
| `D_8009AF74` | `gDisplayObject_awOrderingTableDepthBases` | volatile `u16[4]`, `.sdata` / `.data` |
| `D_8009B140` | `gFade_bKeepOverlayDepth` | `u8`, plain / `.data` |
| `D_8009B145` | `gFade_bWhiteLatch` | `u8`, plain / `.data` |

## Depth reference and signed offsets

`DisplayObject_AcquireSlot` initializes the object's depth from the entry
indexed by `ot_index`. `DisplayObject_SetDepthOffset` keeps the volatile
table pointer and computes/stores `base[index] - signed_offset`.
`DisplayObject_SelectOrderingTable1` and `DisplayObject_SelectOrderingTable3`
select entries 1 and 3 and recompute using the object's existing offset.

The four initialized values remain `0`, `0x20`, `0x800`, `0x20`.
Byte readers retain their original casts to the low byte of entry 1 and
their existing dialog/text/viewer/fade offsets. No unsigned-wrap behavior,
index validation, sign interpretation or ordering-table geometry is changed.
The name identifies reference depths, not a new allocation or an inferred
formula for every table's length.

The original `D_8009AF74_IN_DATA` selector remains: the default declaration
is volatile `u16[4]` in `.sdata`; absolute consumers select `.data`.
Neither the volatile qualifier nor the array bound is normalized away.

## Keep-overlay fade depth

`Fade_DrawOverlay` normally uses depth 4. Only with
`FADE_FLAG_KEEP_OVERLAY` does it consult the depth byte, whose zero value
falls back to `0x3F`. Existing callers derive overrides from table entry 1;
others retain their direct depth values.

This is not an unconditional/default fade depth or a Boolean flag.
The byte, zero sentinel, fallback, narrowing stores and original
`D_8009B140_IN_DATA` addressing selector remain. The existing completion
clear remains at its original point; no new initialization clear is added.

## White transition latch

White color initialization and `Fade_CutToWhite` set the latch.
`Fade_ApplyWhiteLatchToFadeIn` and `Fade_ApplyWhiteLatchToFadeOut` use its
zero/nonzero value to retain the existing white/tinted transition paths.
The existing completion/init clears remain.

The name does not introduce a one-shot reset, normalized Boolean or new
color policy. The `u8` view and `D_8009B145_IN_DATA` selector preserve the
ordinary small-data and image-command absolute forms.

## Initialized ownership and legacy aliases

The NA depth array is an actual initialized C definition, not an external
absolute symbol. The matching ELF identifies it as data at `8009AF74`;
`D_8009AF76` and `D_8009AF7A` are legacy absolute interior aliases.

Its C definition is renamed in the unchanged
`src/game/data_8009af6c.c` unit. The NA linker script therefore supplies
**old `D_8009AF74` = canonical C definition**, not a canonical assignment
that would override/collide with initialized storage. The other images
retain **canonical = original external provider**. Fade bytes retain that
ordinary external-provider direction in every image.

The full `0x1C`-byte NA `.sdata` window keeps its original
`4 + 4 + 8 + 4 + 8` object order, all padding, unrelated format/wildcard
strings and MIDI marker. The initialized table values and eight-byte
extent remain. The unit is not split, and its named profile is unchanged.
Legacy interior aliases keep their original addresses rather than becoming
new definitions or rewritten table expressions.

All 21 logical native roles retain independently established addresses.
Japan binds the depth data `D_8009AF74` at `8009AEBC`; its `8009AF74` is
the AI source-line format `gAiScript_szSourceLineFormat`. Its fade depth
`D_8009B140` is bound at `8009B030`. PAL fade bytes can be adjacent where NA
ones are not; no packed structure or region-independent stride follows.

## Scope and validation

The isolated depth and fade headers mirror their respective old owner
contracts and are included after those owners/selector definitions.
The initialized data unit includes only the depth family.
Old `display_object_helpers.h`, `fade.h`, wrappers, overlays/imports,
profiles, source grouping and every earlier accepted namespace remain.

No existing test module names this family, so all existing tests stay
unchanged. No harness, fixture, probe or testcase is added, and
cloud/regional/overlay/witness suites are not executed locally.
Native compiler conservation does not replace fresh PR-side regional
full-image checks.
