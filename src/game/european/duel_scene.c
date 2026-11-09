#include "../../types.h"

/* SLES-03947 build of src/game/duel_scene.c: the values that differ in
 * this release, collected from the former per-unit wrappers. */

/* duel_field_card_render */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). Only Duel_RenderFieldCard3D
 * reads them. */
#define DUEL_FIELD_CARD_CLUT_Y 0xE1
#define DUEL_FIELD_CARD_CLUT_X16 0x28

/* duel_draw_field_cards */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_CARDS_CLUT_1 0x3829
#define DUEL_FIELD_CARDS_CLUT_3 0x3868

/* duel_card_and_status_draw */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_FRAME_SCREEN_HEIGHT 0x100
#define DUEL_SPRITE_CLUT_CXCY 0xE10280

/* duel_load_package_stage */
#define VERSION_EUROPE

/* duel_init_model_scene */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x65800

/* duel_load_terrain_package */
#define DUEL_TERRAIN_LOAD_FIRST_SECTOR 0x1B88
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT 0xF0
/* value * 0xF0 */
#define DUEL_TERRAIN_LOAD_OFFSET(value) (((value) * 15) * 16)

/* duel_init_scene */
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16) + 0x1B88)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xF0
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_INIT_RESOURCE_CLUT_X 0x280
#define DUEL_INIT_RESOURCE_CLUT_Y 0xDF

/* duel_phase_entry */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PHASE_PANEL_TARGET_Y 0xBE
#define DUEL_PHASE_FIELD_40_BASE 0x360
#define DUEL_PHASE_HAND_CARD_Y 0x2A2

/* duel_draw_resolution */
#define DUEL_DRAW_RESOLUTION_CARD_Y 0xA2
#define DUEL_CARD_READBACK_HEIGHT 0xD4

/* duel_create_card_effect_overlay */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_EFFECT_OVERLAY_HEIGHT 0xD4

/* duel_scene_update_card_use */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_USE_OBJECT_Y 0xE

/* duel_scene_card_placement */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_CARD_PLACEMENT_SCREEN_HEIGHT 256

/* duel_scene_card_move */
#define DUEL_CARD_OFFSCREEN_Y -0x100

#define DUEL_HAND_STACK_INNER_Y 210
#define DUEL_HAND_STACK_COUNT_FIELD_5D 0xB0
#define DUEL_HAND_STACK_COUNT_CX 0x2D0
#define DUEL_SELECTION_PANEL_TARGET_Y 0x84

/* duel_scene_hand_actions */
#define DUEL_HAND_ACTIONS_LOG_INVALID_AI_SELECTION 0

/* duel_scene_field_actions */
#define DUEL_FIELD_ACTIONS_INITIAL_PANEL_TARGET_Y 0xBE
#define DUEL_FIELD_ACTIONS_TARGETING_PANEL_TARGET_Y 0x84

/* duel_battle_stats */

/* duel_scene_battle */
#define DUEL_BATTLE_EFFECT_REQUEST_Y_OFFSET 0x6A
#define DUEL_BATTLE_STAT_DISPLAY_Y 0xE

/* duel_result_runtime */
/* One 0x22-sector outro package per text language, from 0x2290; the
   duelist data from 0x2218, as for the scene initializer above. */
#define DUEL_RESULT_OUTRO_START_SECTOR (0x2290 + gText_bLanguageIndex * 0x22)
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x2218
#define DUEL_RESULT_PAGE_TEXT_Y 0x38
#define DUEL_RESULT_PAGE_TEXT_HEIGHT 0xF0

/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_STARCHIP_Y 210
#define DUEL_RESULT_RANK_Y 40

/* debug_effect_screen */
#define DEBUG_EFFECT_PAGE1_FIELD_32 0x0E

/* duel_field_display_objects */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_SCREEN_HEIGHT 0x100

/* duel_update_card_pick_cursor */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PICK_PANEL_TARGET_Y 0x84
/* The four language builds (src/game/spanish/duel_scene.c) supply their own. */
#ifndef DUEL_VIEWER_CARD_ID_ADDRESS
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009C1B8
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009C1BD
#endif

/* duel_scene_update */
/* Values that differ in the European release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_QUIT_BOX_FLAGS 0x40
#define DUEL_QUIT_BOX_HEIGHT 0x30

/* The US block defines these two together with the height; they are the US
 * values. */
#define DUEL_QUIT_BOX_X 0x78
#define DUEL_QUIT_BOX_WIDTH 0x50

/* duel_action_lock */
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((((terrain) * 15) * 16) + 0x1C58)

/* duel_field_effects */
#define DUEL_RITUAL_CARD_START_Y -42
#define DUEL_RITUAL_CARD_REST_Y 14
#define DUEL_RITUAL_SCREEN_HEIGHT 256

#include "../duel_scene.c"
