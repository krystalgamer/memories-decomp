# Spanish MODEL config fields named from helper views

This applies the helper-view method of #8822 (its model-config-view-fields note
on that branch) to the Spanish split families. The method is restated here
because that note is not on master yet.

Several helpers read the entry's command record through their own `…Timing`
view, held in a pointer member of the helper's work-area view. Each pairing
below is kept only when all four steps hold:

1. The entry sets `work->config = gSpanishModelVariant<N>_aConfigs + command`.
2. The entry calls the helper with its own work pointer (`point`, the pointer
   it casts to its work struct).
3. The helper casts that pointer (`context`) to its state view.
4. The view's `timing` member sits at the same work offset as the entry's
   `config` member. A probe compiles both headers with the project's GCC 2.8.1
   and compares `offsetof`.

The view's name and type then carry over to the config placeholder at the same
record offset. Every config struct keeps its size, and every existing member
keeps its offset; only unread placeholder bytes were retyped. Every image stays
byte-identical. The views are kept, since each helper still declares only what
it reads. Struct-member renames are recorded here rather than in the semantic
map, which has no field kind.

## `Variant385EntryConfig` (`gSpanishModelVariant385_aConfigs`)

Entry `spanish_model_variant/variant385_entry.c` sets `work->config = gSpanishModelVariant385_aConfigs + command` (line 45); its work struct `Variant385EntryState` has `config` at `+0x1000`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x10` | `unknown_10[4]` → `s32 width` | `ModelVariantStripTiming.width` | `SpanishModelVariant385_DrawStationStrip` (spanish_model_variant/variant385_strip.c; `spanish_model_variant/variant385_entry.c:274`, `Strip385State.timing` at +0x1000) | `extent = state->timing->width * state->radius / 1024;` | high |
| `+0x34` | `unknown_30[0x10]` bytes → `u32 expansion_end` | `ModelVariantStripTiming.expanded` | `SpanishModelVariant385_DrawStationStrip` (spanish_model_variant/variant385_strip.c; `spanish_model_variant/variant385_entry.c:274`, `Strip385State.timing` at +0x1000) | `(state->timing->expanded - state->timing->start);` | high |
| `+0x38` | `unknown_30[0x10]` bytes → `u32 fade_start` | `ModelVariantPulseTiming.fade_start`; `ModelVariantStripTiming.fade` | `SpanishModelVariant385_DrawPulsingQuads` (spanish_model_variant/variant385_pulse.c; `spanish_model_variant/variant385_entry.c:270`, `Pulse385State.timing` at +0x1000); `SpanishModelVariant385_DrawStationStrip` (spanish_model_variant/variant385_strip.c; `spanish_model_variant/variant385_entry.c:274`, `Strip385State.timing` at +0x1000) | `} else if (state->timing->fade_start <= state->time && group->size > 0)`; `} else if (state->timing->fade <= state->time && state->radius > 0)` | high |
| `+0x3C` | `unknown_30[0x10]` bytes → `u32 fade_end` | `ModelVariantPulseTiming.fade_end`; `ModelVariantStripTiming.end` | `SpanishModelVariant385_DrawPulsingQuads` (spanish_model_variant/variant385_pulse.c; `spanish_model_variant/variant385_entry.c:270`, `Pulse385State.timing` at +0x1000); `SpanishModelVariant385_DrawStationStrip` (spanish_model_variant/variant385_strip.c; `spanish_model_variant/variant385_entry.c:274`, `Strip385State.timing` at +0x1000) | `(state->timing->fade_end - state->timing->fade_start);`; `(state->timing->end - state->timing->fade);` | high |

## `Variant388EntryConfig` (`gSpanishModelVariant388_aConfigs`)

Entry `spanish_model_variant/variant388_entry.c` sets `work->config = gSpanishModelVariant388_aConfigs + command` (line 47); its work struct `Variant388EntryState` has `config` at `+0x186C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x14` | `unknown_14[8]` bytes → `u32 grow_end` | `Sheet388Timing.grow_end` | `SpanishModelVariant388_DrawSheets` (spanish_model_variant/variant388_sheets.c; `spanish_model_variant/variant388_entry.c:252`, `Sheet388State.timing` at +0x186C) | `/ (state->timing->grow_end - state->timing->grow_start);` | high |
| `+0x20` | `unknown_20[4]` → `u32 shrink_end` | `Sheet388Timing.shrink_end` | `SpanishModelVariant388_DrawSheets` (spanish_model_variant/variant388_sheets.c; `spanish_model_variant/variant388_entry.c:252`, `Sheet388State.timing` at +0x186C) | `/ (state->timing->shrink_end - state->timing->shrink_start);` | high |

## `Variant417EntryConfig` (`gSpanishModelVariant417_aConfigs`)

Entry `spanish_model_variant/variant417_entry.c` sets `work->config = gSpanishModelVariant417_aConfigs + command` (line 47); its work struct `Variant417EntryState` has `config` at `+0x2138`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0x10]` bytes → `s32 expansion_start` | `Tube417Timing.expansion_start` | `SpanishModelVariant417_DrawTube` (spanish_model_variant/variant417_tube.c; `spanish_model_variant/variant417_entry.c:338`, `Tube417State.timing` at +0x2138) | `if (state->time >= state->timing->expansion_start)` | high |
| `+0x24` | `unknown_20[0x10]` bytes → `s32 expansion_end` | `Tube417Timing.expansion_end` | `SpanishModelVariant417_DrawTube` (spanish_model_variant/variant417_tube.c; `spanish_model_variant/variant417_entry.c:338`, `Tube417State.timing` at +0x2138) | `(state->timing->expansion_end - state->timing->expansion_start);` | high |
| `+0x28` | `unknown_20[0x10]` bytes → `s32 fade_start` | `Tube417Timing.fade_start` | `SpanishModelVariant417_DrawTube` (spanish_model_variant/variant417_tube.c; `spanish_model_variant/variant417_entry.c:338`, `Tube417State.timing` at +0x2138) | `if (state->time >= state->timing->fade_start)` | high |
| `+0x2C` | `unknown_20[0x10]` bytes → `s32 fade_end` | `Tube417Timing.fade_end` | `SpanishModelVariant417_DrawTube` (spanish_model_variant/variant417_tube.c; `spanish_model_variant/variant417_entry.c:338`, `Tube417State.timing` at +0x2138) | `(state->timing->fade_end - state->timing->fade_start);` | high |

## `Variant438EntryConfig` (`gSpanishModelVariant438_aConfigs`)

Entry `spanish_model_variant/variant438_entry.c` sets `work->config = gSpanishModelVariant438_aConfigs + command` (line 51); its work struct `Variant438EntryState` has `config` at `+0x22B4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x28` | `unknown_20[0x10]` bytes → `u32 fade_start` | `Bands438Timing.fade_start` | `SpanishModelVariant438_DrawAdvancingBand` (spanish_model_variant/variant438_bands.c; `spanish_model_variant/variant438_entry.c:368`, `Bands438State.timing` at +0x22B4) | `if (state->time >= state->timing->fade_start)` | high |
| `+0x2C` | `unknown_20[0x10]` bytes → `u32 fade_end` | `Bands438Timing.fade_end` | `SpanishModelVariant438_DrawAdvancingBand` (spanish_model_variant/variant438_bands.c; `spanish_model_variant/variant438_entry.c:368`, `Bands438State.timing` at +0x22B4) | `(state->timing->fade_end - state->timing->fade_start);` | high |
| `+0x40` | `unknown_40[4]` → `s32 radius` | `Bands438Timing.radius` | `SpanishModelVariant438_DrawAdvancingBand` (spanish_model_variant/variant438_bands.c; `spanish_model_variant/variant438_entry.c:368`, `Bands438State.timing` at +0x22B4) | `radius = state->size * state->timing->radius / 4096;` | high |

## `Variant441EntryConfig` (`gSpanishModelVariant441_aConfigs`)

Entry `spanish_model_variant/variant441_entry.c` sets `work->config = gSpanishModelVariant441_aConfigs + command` (line 48); its work struct `Variant441EntryState` has `config` at `+0x26E4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0xC]` bytes → `s32 expansion_start` | `Tube441Timing.expansion_start` | `SpanishModelVariant441_DrawTubularMesh` (spanish_model_variant/variant441_tube.c; `spanish_model_variant/variant441_entry.c:353`, `Tube441State.timing` at +0x26E4) | `if (state->time >= state->timing->expansion_start)` | high |
| `+0x24` | `unknown_20[0xC]` bytes → `s32 expansion_end` | `Tube441Timing.expansion_end` | `SpanishModelVariant441_DrawTubularMesh` (spanish_model_variant/variant441_tube.c; `spanish_model_variant/variant441_entry.c:353`, `Tube441State.timing` at +0x26E4) | `(state->timing->expansion_end - state->timing->expansion_start);` | high |
| `+0x28` | `unknown_20[0xC]` bytes → `s32 fade_start` | `Tube441Timing.fade_start` | `SpanishModelVariant441_DrawTubularMesh` (spanish_model_variant/variant441_tube.c; `spanish_model_variant/variant441_entry.c:353`, `Tube441State.timing` at +0x26E4) | `if (state->time >= state->timing->fade_start)` | high |

## `Variant456EntryConfig` (`gSpanishModelVariant456_aConfigs`)

Entry `spanish_model_variant/variant456_entry.c` sets `work->config = gSpanishModelVariant456_aConfigs + command` (line 47); its work struct `Variant456EntryState` has `config` at `+0x2330`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_1C[8]` bytes → `u32 fade_start` | `Timing456.fade_start` | `SpanishModelVariant456_DrawSheets` (spanish_model_variant/variant456_sheets.c; `spanish_model_variant/variant456_entry.c:335`, `Sheets456State.timing` at +0x2330) | `if (state->time >= state->timing->fade_start)` | high |
| `+0x20` | `unknown_1C[8]` bytes → `u32 fade_end` | `Timing456.fade_end` | `SpanishModelVariant456_DrawSheets` (spanish_model_variant/variant456_sheets.c; `spanish_model_variant/variant456_entry.c:335`, `Sheets456State.timing` at +0x2330) | `/ (state->timing->fade_end - state->timing->fade_start);` | high |

## `Variant459EntryConfig` (`gSpanishModelVariant459_aConfigs`)

Entry `spanish_model_variant/variant459_entry.c` sets `work->config = gSpanishModelVariant459_aConfigs + command` (line 46); its work struct `Variant459EntryState` has `config` at `+0x309C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x28` | `unknown_28[4]` bytes → `u16 limit` | `Timing459.limit` | `SpanishModelVariant459_DrawPairedSheets` (spanish_model_variant/variant459_sheets.c; `spanish_model_variant/variant459_entry.c:363`, `Sheet459State.timing` at +0x309C) | `if (sheet->size < state->timing->limit)` | high |
| `+0x38` | `unknown_38[0x10]` bytes → `u32 end` | `Timing459.grow_end`; `Lines459Timing.cycle_end` | `SpanishModelVariant459_DrawPairedSheets` (spanish_model_variant/variant459_sheets.c; `spanish_model_variant/variant459_entry.c:363`, `Sheet459State.timing` at +0x309C); `SpanishModelVariant459_DrawLines` (spanish_model_variant/variant459_lines.c; `spanish_model_variant/variant459_entry.c:369`, `Lines459State.timing` at +0x309C) | `/ (state->timing->grow_end - state->timing->grow_start);`; `(state->timing->cycle_end - state->timing->cycle_start) - 4096;` | high |

## `Variant479EntryConfig` (`gSpanishModelVariant479_aConfigs`)

Entry `spanish_model_variant/variant479_entry.c` sets `work->config = gSpanishModelVariant479_aConfigs + command` (line 39); its work struct `Variant479EntryState` has `config` at `+0x4178`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_1C[0x10]` bytes → `u32 grow_end` | `ModelVariantSheetTiming.grow_end` | `SpanishModelVariant479_DrawSheets` (spanish_model_variant/variant479_sheets.c; `spanish_model_variant/variant479_entry.c:321`, `Sheets479State.timing` at +0x4178) | `(state->timing->grow_end - state->timing->grow_start);` | high |
| `+0x24` | `unknown_1C[0x10]` bytes → `u32 fade_start` | `ModelVariantSheetTiming.shrink_start` | `SpanishModelVariant479_DrawSheets` (spanish_model_variant/variant479_sheets.c; `spanish_model_variant/variant479_entry.c:321`, `Sheets479State.timing` at +0x4178) | `if (state->time >= state->timing->shrink_start && sheet->size > 0)` | high |
| `+0x28` | `unknown_1C[0x10]` bytes → `u32 fade_end` | `ModelVariantSheetTiming.shrink_end` | `SpanishModelVariant479_DrawSheets` (spanish_model_variant/variant479_sheets.c; `spanish_model_variant/variant479_entry.c:321`, `Sheets479State.timing` at +0x4178) | `(state->timing->shrink_end - state->timing->shrink_start);` | high |

## Names chosen and skipped

- **Names.** The config uses the view's name, except for these cases:
  - Header 385 `+0x34` takes `expansion_end` (the strip view's `expanded`),
    the name the 411, 417 and 441 configs use for the same expansion window.
  - Header 459 `+0x38` takes `end`. The sheet view calls it `grow_end` and the
    line view `cycle_end`; both pair it with the existing `start` at `+0x34`.
  - Header 479 `+0x24` takes `fade_start` (the view's `fade_begin`), reusing
    the existing name.
- **Conflicting views, skipped.**
  - 385 `+0x30`: the pulse view's `end`, the strip view's `start` and the line
    view's `duration`.
  - 438 `+0x20`: the sheet and line views' `grow_end`/`cycle_end`, but the band
    view's `grow_start`. 438 `+0x24` (the band view's `grow_end`) is skipped
    with it.
- **Already named, kept.**
  - Bytes that already have a config name keep it: 385/388/459 `count`, 388
    `sheet_start`/`track_end`, 417 `size`, 438/459/479 `start`, 441 `end`, and
    456 `sheets_start`/`lines_start`/`phase_start`.
  - 411's views read only members that are already named.
- **Out of scope.** Header 405 is in #8819.
