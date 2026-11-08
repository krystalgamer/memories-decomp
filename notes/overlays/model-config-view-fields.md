# MODEL config fields named from helper views

Several MODEL helpers read the entry's command record through their own
`…Timing` view, a pointer member of the helper's work-area view. For each
family below the pointer is proven to be the same record as the entry's
`work->config`, so the view's names carry over to the config struct's
placeholder bytes at the same offsets, with the view's types.

Each trace has four steps:

1. The entry sets `work->config` to `g<Family>_aConfigs + command`.
2. The entry calls the helper with its own work pointer, the same `point`
   or `context` it casts to the work struct.
3. The helper casts that pointer to its state view.
4. The view's `timing` member sits at the same work offset as the entry's
   `config` member; the probe compiles both headers with the project's
   GCC 2.8.1 and compares `offsetof`.

The config struct's size and every other member offset are unchanged.
Only unread placeholder bytes were retyped, and every image stays
byte-identical. The views are kept: each helper still declares only what
it reads. Struct-member renames are recorded here, not in the semantic
map, which has no field kind.

## `ModelVariantSweepConfig` (`gModelVariant413_aConfigs`)

Entry `model_variant/variant413_entry.c` sets `work->config = gModelVariant413_aConfigs + command` (line 69); its work struct `Variant413EntryState` has `config` at `+0x3ADC`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0xC]` bytes → `u32 limit` | `ModelVariantQuadTiming.limit` | `FrenchModelVariant430_DrawSwarm` (french_model_variant/image132118_swarm.c; `model_variant/variant413_entry.c:366`, `Swarm132118State.timing` at +0x3ADC) | `if (state->timing->limit <= state->time)` | high |

## `Variant451EntryConfig` (`gModelVariant451_aConfigs`)

Entry `model_variant/variant451_entry.c` sets `work->config = gModelVariant451_aConfigs + command` (line 69); its work struct `Variant451EntryState` has `config` at `+0x212C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x24` | `unknown_24[4]` bytes → `u32 rate` | `ModelVariantSwarmRateTiming.rate` | `FrenchModelVariant468_DrawSwarm` (french_model_variant/image134622_swarm.c; `model_variant/variant451_entry.c:388`, `Swarm134622State.timing` at +0x212C) | `group->size[j] += state->timing->rate * state->step / 2;` | high |
| `+0x2C` | `unknown_2C[0xC]` bytes → `u32 limit` | `ModelVariantSwarmRateTiming.limit` | `FrenchModelVariant468_DrawSwarm` (french_model_variant/image134622_swarm.c; `model_variant/variant451_entry.c:388`, `Swarm134622State.timing` at +0x212C) | `if (state->timing->limit <= state->time)` | high |

## `Variant380EntryConfig` (`gModelVariant380_aConfigs`)

Entry `model_variant/variant380_entry.c` sets `work->config = gModelVariant380_aConfigs + command` (line 51); its work struct `Variant380EntryState` has `config` at `+0x1B54`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x28` | `unknown_28[4]` bytes → `s32 divisor` | `Sheets397Timing.divisor` | `FrenchModelVariant397_DrawSheets` (french_model_variant/variant397_sheets.c; `model_variant/variant380_entry.c:270`, `Sheets397State.timing` at +0x1B54) | `scale.vx = sheet->size / state->timing->divisor + extra;` | high |
| `+0x34` | `unknown_34[0x10]` bytes → `u32 fade_at` | `Sheets397Timing.fade_at` | `FrenchModelVariant397_DrawSheets` (french_model_variant/variant397_sheets.c; `model_variant/variant380_entry.c:270`, `Sheets397State.timing` at +0x1B54) | `} else if (state->phase == 1 && state->timing->fade_at < state->time)` | high |

## `Variant402EntryConfig` (`gModelVariant402_aConfigs`)

Entry `model_variant/variant402_entry.c` sets `work->config = gModelVariant402_aConfigs + command` (line 51); its work struct `Variant402EntryState` has `config` at `+0x22D0`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_13[0x0D]` bytes → `u16 spin` | `Swarm419Timing.spin` | `FrenchModelVariant419_DrawSwarm` (french_model_variant/variant419_swarm.c; `model_variant/variant402_entry.c:332`, `Swarm419State.timing` at +0x22D0) | `spin = state->timing->spin << 11;` | high |
| `+0x24` | `unknown_24[4]` bytes → `u32 limit` | `ModelVariantQuadTimingLong.limit` | `FrenchModelVariant419_DrawQuads` (french_model_variant/variant419_quads.c; `model_variant/variant402_entry.c:335`, `Quads419State.timing` at +0x22D0); `FrenchModelVariant419_DrawSwarm` (french_model_variant/variant419_swarm.c; `model_variant/variant402_entry.c:332`, `Swarm419State.timing` at +0x22D0) | `if (state->timing->limit <= state->time && state->phase >= 3)`; `if (state->timing->limit <= state->time)` | high |

## `ModelVariantSpiralConfig` (`gModelVariant409_aConfigs`)

Entry `model_variant/variant409_entry.c` sets `work->config = gModelVariant409_aConfigs + command` (line 41); its work struct `EntryState409` has `config` at `+0x2110`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `field_1C` → `u32 ribbon_start` | `ModelVariantRibbonGrowTiming.grow_start` | `FrenchModelVariant426_DrawRibbon` (french_model_variant/variant426_ribbon.c; `model_variant/variant409_entry.c:311`, `Ribbon426Work.timing` at +0x2110) | `if (work->amp < 0x400 && work->timing->grow_start <= work->clock)` | high |

The `ModelVariantRibbonGrowTiming` view calls this word `grow_start`. The config uses `ribbon_start`, the name given to the same ribbon-growth start in `EntryConfig425` (#8816), because the ribbon progress rises from it to `travel_start`.

## `Variant411UsEntryConfig` (`gModelVariant411_aConfigs`)

Entry `model_variant/variant411us_entry.c` sets `work->config = gModelVariant411_aConfigs + command` (line 68); its work struct `Variant411UsEntryState` has `config` at `+0x2750`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0x10]` bytes → `u32 limit` | `ModelVariantQuadTiming.limit` | `FrenchModelVariant428_DrawQuads` (french_model_variant/variant428_quads.c; `model_variant/variant411us_entry.c:373`, `Quads428State.timing` at +0x2750); `FrenchModelVariant428_DrawSwarm` (french_model_variant/variant428_swarm.c; `model_variant/variant411us_entry.c:370`, `Swarm428State.timing` at +0x2750) | `if (state->timing->limit <= state->time && state->phase >= 3)`; `if (state->timing->limit <= state->time)` | high |

## `Variant412EntryConfig` (`gModelVariant412_aConfigs`)

Entry `model_variant/variant412_entry.c` sets `work->config = gModelVariant412_aConfigs + command` (line 68); its work struct `Variant412EntryState` has `config` at `+0x1E60`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0x10]` bytes → `u32 limit` | `ModelVariantQuadTiming.limit` | `FrenchModelVariant429_DrawQuads` (french_model_variant/variant429_quads.c; `model_variant/variant412_entry.c:375`, `Quads429State.timing` at +0x1E60); `FrenchModelVariant429_DrawSwarm` (french_model_variant/variant429_swarm.c; `model_variant/variant412_entry.c:372`, `Swarm429State.timing` at +0x1E60) | `if (state->timing->limit <= state->time && state->phase >= 3)`; `if (state->timing->limit <= state->time)` | high |

## `Variant432EntryConfig` (`gModelVariant432_aConfigs`)

Entry `model_variant/variant432_entry.c` sets `work->config = gModelVariant432_aConfigs + command` (line 43); its work struct `Variant432EntryState` has `config` at `+0x1BF4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x18` | `unknown_18[0xC]` bytes → `s32 move_start` | `Variant449Timing.move_start` | `FrenchModelVariant449_DrawSheet` (french_model_variant/variant449_sheet.c; `model_variant/variant432_entry.c:287`, `Variant449View.timing` at +0x1BF4) | `} else if (work->phase == 1 && work->elapsed >= work->timing->move_start)` | high |
| `+0x1C` | `unknown_18[0xC]` bytes → `s32 move_end` | `Variant449Timing.move_end` | `FrenchModelVariant449_DrawSheet` (french_model_variant/variant449_sheet.c; `model_variant/variant432_entry.c:287`, `Variant449View.timing` at +0x1BF4) | `(work->timing->move_end - work->timing->move_start);` | high |
| `+0x20` | `unknown_18[0xC]` bytes → `s32 fade_start` | `Variant449Timing.fade_start` | `FrenchModelVariant449_DrawSheet` (french_model_variant/variant449_sheet.c; `model_variant/variant432_entry.c:287`, `Variant449View.timing` at +0x1BF4) | `} else if (work->phase == 3 && work->timing->fade_start <= work->elapsed)` | high |

## `Variant452EntryConfig` (`gFrenchModelVariant452_aConfigs`)

Entry `french_model_variant/variant452_entry.c` sets `work->config = gFrenchModelVariant452_aConfigs + command` (line 55); its work struct `Variant452EntryState` has `config` at `+0x3374`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0x18]` bytes → `u32 scale_end` | `Variant452SheetTiming.scale_end` | `FrenchModelVariant452_DrawSheets` (french_model_variant/variant452_sheets.c; `french_model_variant/variant452_entry.c:332`, `Variant452SheetView.timing` at +0x3374) | `/ (work->timing->scale_end - work->timing->scale_start);` | high |
| `+0x2C` | `unknown_20[0x18]` bytes → `u32 fade_start` | `Variant452SheetTiming.fade_start` | `FrenchModelVariant452_DrawSheets` (french_model_variant/variant452_sheets.c; `french_model_variant/variant452_entry.c:332`, `Variant452SheetView.timing` at +0x3374) | `} else if (work->elapsed >= work->timing->fade_start)` | high |
| `+0x30` | `unknown_20[0x18]` bytes → `u32 fade_end` | `Variant452SheetTiming.fade_end` | `FrenchModelVariant452_DrawSheets` (french_model_variant/variant452_sheets.c; `french_model_variant/variant452_entry.c:332`, `Variant452SheetView.timing` at +0x3374) | `/ (work->timing->fade_end - work->timing->fade_start);` | high |

## `Variant438EntryConfig` (`gModelVariant438_aConfigs`)

Entry `model_variant/variant438_entry.c` sets `work->config = gModelVariant438_aConfigs + command` (line 74); its work struct `Variant438EntryState` has `config` at `+0x3754`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x24` | `unknown_24[4]` bytes → `u32 rate` | `ModelVariantSwarmRateTiming.rate` | `FrenchModelVariant455_DrawSwarm` (french_model_variant/variant455_swarm.c; `model_variant/variant438_entry.c:386`, `Swarm455State.timing` at +0x3754) | `group->size[j] += state->timing->rate * state->step / 2;` | high |
| `+0x2C` | `unknown_2C[0xC]` bytes → `u32 limit` | `ModelVariantSwarmRateTiming.limit` | `FrenchModelVariant455_DrawSwarm` (french_model_variant/variant455_swarm.c; `model_variant/variant438_entry.c:386`, `Swarm455State.timing` at +0x3754) | `if (state->timing->limit <= state->time)` | high |

## `Variant446EntryConfig` (`gModelVariant446_aConfigs`)

Entry `model_variant/variant446_entry.c` sets `work->config = gModelVariant446_aConfigs + command` (line 77); its work struct `Variant446EntryState` has `config` at `+0x4A64`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_15[0x13]` bytes → `s32 first_count` | `PairedSheets463Timing.first_count` | `FrenchModelVariant463_DrawPairedSheets` (french_model_variant/variant463_paired_sheets.c; `model_variant/variant446_entry.c:489`, `PairedSheets463State.timing` at +0x4A64); `FrenchModelVariant463_DrawSheets` (french_model_variant/variant463_sheets.c; `model_variant/variant446_entry.c:491`, `Sheets463State.timing` at +0x4A64) | `count = timing->first_count;` | high |
| `+0x24` | `unknown_15[0x13]` bytes → `s32 paired` | `PairedSheets463Timing.paired` | `FrenchModelVariant463_DrawPairedSheets` (french_model_variant/variant463_paired_sheets.c; `model_variant/variant446_entry.c:489`, `PairedSheets463State.timing` at +0x4A64); `FrenchModelVariant463_DrawSheets` (french_model_variant/variant463_sheets.c; `model_variant/variant446_entry.c:491`, `Sheets463State.timing` at +0x4A64) | `if (timing->paired == 0)` | high |
| `+0x2C` | `unknown_2C[4]` bytes → `s32 mode` | `PairedSheets463Timing.mode` | `FrenchModelVariant463_DrawPairedSheets` (french_model_variant/variant463_paired_sheets.c; `model_variant/variant446_entry.c:489`, `PairedSheets463State.timing` at +0x4A64) | `if (timing->mode == 1)` | high |
| `+0x34` | `unknown_34[4]` bytes → `u32 grow_end` | `PairedSheets463Timing.grow_end` | `FrenchModelVariant463_DrawPairedSheets` (french_model_variant/variant463_paired_sheets.c; `model_variant/variant446_entry.c:489`, `PairedSheets463State.timing` at +0x4A64); `FrenchModelVariant463_DrawSheets` (french_model_variant/variant463_sheets.c; `model_variant/variant446_entry.c:491`, `Sheets463State.timing` at +0x4A64) | `(state->timing->grow_end - state->timing->grow_begin);` | high |

## `ModelVariantSweepConfig` (`gModelVariant454_aConfigs`)

Entry `model_variant/variant454_entry.c` sets `work->config = gModelVariant454_aConfigs + command` (line 73); its work struct `Variant454EntryState` has `config` at `+0x23F8`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0xC]` bytes → `u32 limit` | `ModelVariantQuadTiming.limit` | `FrenchModelVariant471_DrawQuadGroup` (french_model_variant/variant471_quad_pairs.c; `model_variant/variant454_entry.c:412`, `QuadPairState471.timing` at +0x23F8); `FrenchModelVariant471_DrawQuads` (french_model_variant/variant471_quads.c; `model_variant/variant454_entry.c:415`, `Quads471State.timing` at +0x23F8) | `if (state->timing->limit <= state->time)`; `if (state->timing->limit <= state->time && state->phase >= 3)` | high |

## `EntryConfig436` (`gFrenchModelVariant436_aConfigs`)

Entry `french_model_variant/variant436_entry.c` sets `work->config = gFrenchModelVariant436_aConfigs + command` (line 37); its work struct `EntryState436` has `config` at `+0x15F4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x18` | `unknown_18[0x18]` bytes → `s32 phase2_end` | `Timing436.phase2_end` | `SpanishModelVariant436_DrawSheets` (spanish_model_variant/variant436_sheets.c; `french_model_variant/variant436_entry.c:288`, `Sheet436State.timing` at +0x15F4) | `/ (state->timing->phase2_end - state->timing->grow_end);` | high |
| `+0x1C` | `unknown_18[0x18]` bytes → `s32 phase3_end` | `Timing436.phase3_end` | `SpanishModelVariant436_DrawSheets` (spanish_model_variant/variant436_sheets.c; `french_model_variant/variant436_entry.c:288`, `Sheet436State.timing` at +0x15F4) | `/ (state->timing->phase3_end - state->timing->phase2_end);` | high |
| `+0x20` | `unknown_18[0x18]` bytes → `s32 travel_end` | `Timing436.travel_end` | `SpanishModelVariant436_DrawSheets` (spanish_model_variant/variant436_sheets.c; `french_model_variant/variant436_entry.c:288`, `Sheet436State.timing` at +0x15F4) | `/ (state->timing->travel_end - state->timing->phase3_end);` | high |

## `Variant437EntryConfig` (`gFrenchModelVariant437_aConfigs`)

Entry `french_model_variant/variant437_entry.c` sets `work->config = gFrenchModelVariant437_aConfigs + command` (line 37); its work struct `Variant437EntryState` has `config` at `+0x15F4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_18[0x18]` bytes → `u32 move_start` | `Timing437.move_start` | `SpanishModelVariant437_DrawSheet` (spanish_model_variant/variant437_sheet.c; `french_model_variant/variant437_entry.c:283`, `Sheet437State.timing` at +0x15F4) | `(state->timing->move_start - state->timing->grow_end);` | high |
| `+0x20` | `unknown_18[0x18]` bytes → `u32 move_end` | `Timing437.move_end` | `SpanishModelVariant437_DrawSheet` (spanish_model_variant/variant437_sheet.c; `french_model_variant/variant437_entry.c:283`, `Sheet437State.timing` at +0x15F4) | `(state->timing->move_end - state->timing->move_start);` | high |

## `ModelVariantGrowConfig` (`gFrenchModelVariant461_aConfigs`)

Entry `french_model_variant/variant461_entry.c` sets `work->config = gFrenchModelVariant461_aConfigs + command` (line 44); its work struct `EntryState461` has `config` at `+0x336C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_13[0x11]` bytes → `s32 first_count` | `ModelVariantPairedSheetTiming.first_count` | `SpanishModelVariant461_DrawSheets` (spanish_model_variant/variant461_sheets.c; `french_model_variant/variant461_entry.c:380`, `Sheets461State.timing` at +0x336C) | `count = timing->first_count + 1;` | high |
| `+0x20` | `unknown_13[0x11]` bytes → `s32 paired` | `ModelVariantPairedSheetTiming.paired` | `SpanishModelVariant461_DrawSheets` (spanish_model_variant/variant461_sheets.c; `french_model_variant/variant461_entry.c:380`, `Sheets461State.timing` at +0x336C) | `if (timing->paired == 0)` | high |

## `Variant464EntryConfig` (`gFrenchModelVariant464_aConfigs`)

Entry `french_model_variant/variant464_entry.c` sets `work->config = gFrenchModelVariant464_aConfigs + command` (line 47); its work struct `Variant464EntryState` has `config` at `+0x1C2C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x20` | `unknown_20[0x10]` bytes → `u32 grow_end` | `Sheets464Timing.grow_end` | `SpanishModelVariant464_DrawSheets` (spanish_model_variant/variant464_sheets.c; `french_model_variant/variant464_entry.c:373`, `Sheets464State.timing` at +0x1C2C) | `(state->timing->grow_end - state->timing->grow_begin);` | high |
| `+0x28` | `unknown_20[0x10]` bytes → `u32 fade_begin` | `Sheets464Timing.fade_begin` | `SpanishModelVariant464_DrawSheets` (spanish_model_variant/variant464_sheets.c; `french_model_variant/variant464_entry.c:373`, `Sheets464State.timing` at +0x1C2C) | `} else if (state->time > state->timing->fade_begin && sheet->size > 0)` | high |
| `+0x2C` | `unknown_20[0x10]` bytes → `u32 fade_end` | `Sheets464Timing.fade_end` | `SpanishModelVariant464_DrawSheets` (spanish_model_variant/variant464_sheets.c; `french_model_variant/variant464_entry.c:373`, `Sheets464State.timing` at +0x1C2C) | `(state->timing->fade_end - state->timing->fade_begin);` | high |

## `EntryConfig473` (`gFrenchModelVariant473_aConfigs`)

Entry `french_model_variant/variant473_entry.c` sets `work->config = gFrenchModelVariant473_aConfigs + command` (line 38); its work struct `EntryState473` has `config` at `+0x3BF0`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_1C[0x10]` bytes → `u32 grow_end` | `ModelVariantSheetTiming.grow_end` | `SpanishModelVariant473_DrawSheets` (spanish_model_variant/variant473_sheets.c; `french_model_variant/variant473_entry.c:337`, `Sheets473State.timing` at +0x3BF0) | `(state->timing->grow_end - state->timing->grow_start);` | high |
| `+0x24` | `unknown_1C[0x10]` bytes → `u32 fade_begin` | `ModelVariantSheetTiming.shrink_start` | `SpanishModelVariant473_DrawSheets` (spanish_model_variant/variant473_sheets.c; `french_model_variant/variant473_entry.c:337`, `Sheets473State.timing` at +0x3BF0) | `if (state->time >= state->timing->shrink_start && sheet->size > 0)` | high |
| `+0x28` | `unknown_1C[0x10]` bytes → `u32 fade_end` | `ModelVariantSheetTiming.shrink_end` | `SpanishModelVariant473_DrawSheets` (spanish_model_variant/variant473_sheets.c; `french_model_variant/variant473_entry.c:337`, `Sheets473State.timing` at +0x3BF0) | `(state->timing->shrink_end - state->timing->shrink_start);` | high |
