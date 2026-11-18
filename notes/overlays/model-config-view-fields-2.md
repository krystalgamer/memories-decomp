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

`EntryConfig455`'s `+0x28..+0x30` are a chain of times, which the helpers
read in adjacent pairs under different names. The sheets and spiral fade over
`+0x28..+0x2C`, the blooms stop at `+0x2C`, and the mesh fades over
`+0x2C..+0x30`. When those helpers moved onto `EntryState455`, the words
became `u32 fade_start`, `fade_end` and `mesh_fade_end`. This also dropped
the spiral's two `(u32)` casts.
`EntryConfig443`'s `+0x48`/`+0x4C` placeholder sat under the 443 mesh
view's `grow_start`/`grow_end`. When `SpanishModelVariant443_DrawMesh` was
moved onto `EntryState443`, these became `mesh_start`/`mesh_end`, the window
over which the mesh's `progress` grows. The 443 sheets view used `grow_start`/`grow_end`
for `+0x38`/`+0x3C`, the config's `start`/`web_end`. When that helper was
moved onto the record, its `+0x50`/`+0x54` fade window became
`fade_start`/`fade_end`. The words from `+0x38` to `+0x54` form a chain of times, and
helpers read adjacent pairs as windows under different names. The ribbons
read `spread_start..spread_end` (`+0x40..+0x44`), the sheets'
spread reads `spread_end..mesh_start` (`+0x44..+0x48`), and the mesh reads
`mesh_start..mesh_end`.

## `Model386Config`

Entry `model_variant/e569_entry.c` line 61: `work->config = gModelVariant386_aConfigs + command;`. Its work struct `Model386State` has `config` at `+0x1368`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x2C` | `field_2C` → `s32 divisor` | `Sheets12048Timing.divisor` | `FrenchModelVariant403_DrawSheets` (model_variant/image12048_sheets.c; `model_variant/e569_entry.c:309`, `Sheets12048State.timing` at +0x1368) | `scale.vx = sheet->size / state->timing->divisor + extra;` | high |

## `Model372Config`

Entry `model_variant/e338_entry.c` line 75: `work->config = gModelVariant372_aConfigs + command;`. Its work struct `Model372State` has `config` at `+0x2C9C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x34` | `field_34` → `u32 fade_begin` | `Sheets389Timing.fade_begin` | `FrenchModelVariant389_DrawSheets` (model_variant/e338_sheets.c; `model_variant/e338_entry.c:290`, `Sheets389State.timing` at +0x2C9C) | `} else if (state->timing->fade_begin < state->time && sheet->size > 0)` | high |
| `+0x38` | `field_38` → `u32 fade_end` | `Sheets389Timing.fade_end` | `FrenchModelVariant389_DrawSheets` (model_variant/e338_sheets.c; `model_variant/e338_entry.c:290`, `Sheets389State.timing` at +0x2C9C) | `(state->timing->fade_end - state->timing->fade_begin);` | high |

## `Variant430EntryConfig`

Entry `model_variant/h430_entry.c` line 76: `work->config = &gModelVariant430_aConfigs[command];`. Its work struct `Variant430EntryState` has `config` at `+0x1FD4`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x24` | `unknown_24[4]` bytes → `u32 limit` | `ModelVariantQuadTimingLong.limit` | `FrenchModelVariant447_DrawSwarm` (model_variant/h430_swarm.c; `model_variant/h430_entry.c:341`, `Swarm447State.timing` at +0x1FD4); `FrenchModelVariant447_DrawSwarm` (model_variant/h430_swarm.c; `model_variant/h430_entry.c:343`, `Swarm447State.timing` at +0x1FD4) | `if (state->timing->limit <= state->time)` | high |

## `EntryConfig458`

Entry `model_variant/h441_entry_pal.c` line 36: `work->config = (EntryConfig458 *)(gFrenchModelVariant458_D230C + 0xFC) + command;`. Its work struct `EntryState458` has `config` at `+0x1F24`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_1C[0xC]` bytes → `s32 move_start` | `EntryConfig458.move_start` (was `Variant458Timing`) | `FrenchModelVariant458_DrawSheets` (model_variant/h441_sheets.c; `model_variant/h441_entry_pal.c:259`, `EntryState458.config` at +0x1F24) | `} else if (work->phase == 1 && work->elapsed >= work->config->move_start)` | high |
| `+0x20` | `unknown_1C[0xC]` bytes → `s32 move_end` | `EntryConfig458.move_end` (was `Variant458Timing`) | `FrenchModelVariant458_DrawSheets` (model_variant/h441_sheets.c; `model_variant/h441_entry_pal.c:259`, `EntryState458.config` at +0x1F24) | `(work->config->move_end - work->config->move_start);` | high |
| `+0x24` | `unknown_1C[0xC]` bytes → `s32 fade_start` | `EntryConfig458.fade_start` (was `Variant458Timing`) | `FrenchModelVariant458_DrawSheets` (model_variant/h441_sheets.c; `model_variant/h441_entry_pal.c:259`, `EntryState458.config` at +0x1F24) | `} else if (work->phase == 4 && work->config->fade_start <= work->elapsed)` | high |

## `ModelVariantGrowConfig`

Entry `model_variant/h445_entry.c` line 51: `work->config = (ModelVariantGrowConfig *)(gModelVariant445_D2848 + 0xFC) + command;`. Its work struct `EntryState445Us` has `config` at `+0x354C`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x1C` | `unknown_13[0x11]` bytes → `s32 first_count` | `Sheets462Timing.first_count` | `FrenchModelVariant462_DrawSheets` (model_variant/h445_sheets.c; `model_variant/h445_entry.c:381`, `Sheets462State.timing` at +0x354C) | `count = timing->first_count + 1;` | high |
| `+0x20` | `unknown_13[0x11]` bytes → `s32 paired` | `Sheets462Timing.paired` | `FrenchModelVariant462_DrawSheets` (model_variant/h445_sheets.c; `model_variant/h445_entry.c:381`, `Sheets462State.timing` at +0x354C) | `if (timing->paired == 0)` | high |
| `+0x28` | `field_28` → `u32 grow_end` | `Sheets462Timing.grow_end` | `FrenchModelVariant462_DrawSheets` (model_variant/h445_sheets.c; `model_variant/h445_entry.c:381`, `Sheets462State.timing` at +0x354C) | `(state->timing->grow_end - state->timing->grow_begin);` | high |

## `Variant366Config`

Entry `model_variant/e549_entry.c` line 49: `work->config = &gModelVariant366_aConfigs[command];`. Its work struct `Variant366State` has `config` at `+0x1538`. The helper's view names its config pointer `config`, not `timing`.

| Offset | Old → new | View field | Helper (call) | Helper reads | Confidence |
|---|---|---|---|---|---|
| `+0x10` | `unknown_10[4]` bytes → `u32 duration` | `Variant366Config.duration` (the 383 helpers now read `Variant366State` directly) | `FrenchModelVariant383_DrawQuads` (model_variant/e549_quads.c; `model_variant/e549_entry.c:272`, `Variant366State.config` at +0x1538) | `work->scale = (work->clock.frame << 12) / work->config->duration;` | high |

The view's `count_0C` matches the config's existing `count` at `+0x0C`. The view suffix `_10` only marks the offset, so the config member is `duration`. naming3 found this field.
