# MODEL config fields named from helper views (part 2)

This applies the helper-view method of #8822 (`model-config-view-fields.md`)
to six config structs outside the composite-block splits. A helper reads the
entry's command record through its own `…Timing` view, a pointer member
of the helper's work-area view. Each field below passes four checks:

1. The entry points `work->config` at the command's record in the
   family's config table.
2. The entry calls the helper with its own work pointer, the same `point`
   or `context` it casts to the work struct.
3. The helper casts that pointer to its state view.
4. The view's `timing` member sits at the same work offset as the entry's
   `config` member; the probe compiles both headers with the project's
   GCC 2.8.1 and compares `offsetof`.

The view's names and types replace the config struct's placeholder
members at the same offsets. The struct's size and every other member
offset are unchanged, the views are kept, and every image stays
byte-identical. Struct-member renames are recorded here, not in the
semantic map, which has no field kind.

`EntryConfig455` is left out because its helpers disagree:
`FrenchModelVariant472_DrawBlooms` reads `+0x2C` as `limit`,
`FrenchModelVariant472_DrawMesh` reads it as `fade_start` and
`SpanishModelVariant472_DrawSheets` reads it as `fade_end`.
`EntryConfig443` has no placeholder under any view field.

## `Model386Config`

Entry `model_variant/variant386_entry.c` line 61: `work->config = gModelVariant386_aConfigs + command;`. Its work struct `Model386State` has `config` at `+0x1368`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x2C` | `field_2C` → `s32 divisor` | `Sheets12048Timing.divisor` | `FrenchModelVariant403_DrawSheets` (french_model_variant/image12048_sheets.c; `model_variant/variant386_entry.c:309`, `Sheets12048State.timing` at +0x1368) | `scale.vx = sheet->size / state->timing->divisor + extra;` | high |

## `Model372Config`

Entry `model_variant/variant372_entry.c` line 75: `work->config = gModelVariant372_aConfigs + command;`. Its work struct `Model372State` has `config` at `+0x2C9C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x34` | `field_34` → `u32 fade_begin` | `Sheets389Timing.fade_begin` | `FrenchModelVariant389_DrawSheets` (french_model_variant/variant389_sheets.c; `model_variant/variant372_entry.c:290`, `Sheets389State.timing` at +0x2C9C) | `} else if (state->timing->fade_begin < state->time && sheet->size > 0)` | high |
| `+0x38` | `field_38` → `u32 fade_end` | `Sheets389Timing.fade_end` | `FrenchModelVariant389_DrawSheets` (french_model_variant/variant389_sheets.c; `model_variant/variant372_entry.c:290`, `Sheets389State.timing` at +0x2C9C) | `(state->timing->fade_end - state->timing->fade_begin);` | high |

## `Variant430EntryConfig`

Entry `model_variant/variant430_entry.c` line 76: `work->config = &gModelVariant430_aConfigs[command];`. Its work struct `Variant430EntryState` has `config` at `+0x1FD4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x24` | `unknown_24[4]` bytes → `u32 limit` | `ModelVariantQuadTimingLong.limit` | `FrenchModelVariant447_DrawSwarm` (french_model_variant/variant447_swarm.c; `model_variant/variant430_entry.c:341`, `Swarm447State.timing` at +0x1FD4); `FrenchModelVariant447_DrawSwarm` (french_model_variant/variant447_swarm.c; `model_variant/variant430_entry.c:343`, `Swarm447State.timing` at +0x1FD4) | `if (state->timing->limit <= state->time)` | high |

## `EntryConfig458`

Entry `french_model_variant/variant458_entry.c` line 36: `work->config = (EntryConfig458 *)(gFrenchModelVariant458_D230C + 0xFC) + command;`. Its work struct `EntryState458` has `config` at `+0x1F24`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_1C[0xC]` bytes → `s32 move_start` | `Variant458Timing.move_start` | `FrenchModelVariant458_DrawSheets` (french_model_variant/variant458_sheets.c; `french_model_variant/variant458_entry.c:259`, `Variant458View.timing` at +0x1F24) | `} else if (work->phase == 1 && work->elapsed >= work->timing->move_start)` | high |
| `+0x20` | `unknown_1C[0xC]` bytes → `s32 move_end` | `Variant458Timing.move_end` | `FrenchModelVariant458_DrawSheets` (french_model_variant/variant458_sheets.c; `french_model_variant/variant458_entry.c:259`, `Variant458View.timing` at +0x1F24) | `(work->timing->move_end - work->timing->move_start);` | high |
| `+0x24` | `unknown_1C[0xC]` bytes → `s32 fade_start` | `Variant458Timing.fade_start` | `FrenchModelVariant458_DrawSheets` (french_model_variant/variant458_sheets.c; `french_model_variant/variant458_entry.c:259`, `Variant458View.timing` at +0x1F24) | `} else if (work->phase == 4 && work->timing->fade_start <= work->elapsed)` | high |

## `EntryConfig445Us`

Entry `model_variant/variant445us_entry.c` line 51: `work->config = (EntryConfig445Us *)(gModelVariant445_D2848 + 0xFC) + command;`. Its work struct `EntryState445Us` has `config` at `+0x354C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_13[0x11]` bytes → `s32 first_count` | `Sheets462Timing.first_count` | `FrenchModelVariant462_DrawSheets` (french_model_variant/variant462_sheets.c; `model_variant/variant445us_entry.c:381`, `Sheets462State.timing` at +0x354C) | `count = timing->first_count + 1;` | high |
| `+0x20` | `unknown_13[0x11]` bytes → `s32 paired` | `Sheets462Timing.paired` | `FrenchModelVariant462_DrawSheets` (french_model_variant/variant462_sheets.c; `model_variant/variant445us_entry.c:381`, `Sheets462State.timing` at +0x354C) | `if (timing->paired == 0)` | high |
| `+0x28` | `field_28` → `u32 grow_end` | `Sheets462Timing.grow_end` | `FrenchModelVariant462_DrawSheets` (french_model_variant/variant462_sheets.c; `model_variant/variant445us_entry.c:381`, `Sheets462State.timing` at +0x354C) | `(state->timing->grow_end - state->timing->grow_begin);` | high |

## `Variant366Config`

Entry `model_variant/variant366_entry.c` line 49: `work->config = &gModelVariant366_aConfigs[command];`. Its work struct `Variant366State` has `config` at `+0x1538`. The helper's view names its config pointer `config`, not `timing`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x10` | `unknown_10[4]` bytes → `u32 duration` | `Family383QuadConfig.duration_10` | `FrenchModelVariant383_DrawQuads` (french_model_variant/variant383_quads.c; `model_variant/variant366_entry.c:272`, `Family383QuadView.config` at +0x1538) | `work->scale = (work->elapsed << 12) / work->config->duration_10;` | high |

The view's `count_0C` matches the config's existing `count` at `+0x0C`. The view suffix `_10` only marks the offset, so the config member is `duration`. naming3 found this field.
