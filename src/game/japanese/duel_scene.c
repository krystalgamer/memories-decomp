#include "../../types.h"

/* SLPM-86398 build of src/game/duel_scene.c: the values and storage
 * that differ in this release, collected from the former per-unit wrappers. */

/* duel_card_and_status_draw */
#define VERSION_JAPAN

/* duel_load_package_stage */
/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_PACKAGE_STAGE7_SECTORS 48

/* duel_init_model_scene */
#define DUEL_MODEL_SCENE_BUFFER_SIZE 0x65000

/* duel_load_terrain_package */
#define DUEL_TERRAIN_LOAD_FIRST_SECTOR 0x16B5
#define DUEL_TERRAIN_LOAD_SECTOR_COUNT 0xEF
/* value * 0xEF */
#define DUEL_TERRAIN_LOAD_OFFSET(value) (((value) * 15) * 16 - (value))

/* duel_init_scene */
#define DUEL_INIT_TERRAIN_SECTOR(value) ((((value) * 15) * 16 - (value)) + 0x16B5)
#define DUEL_INIT_TERRAIN_SECTOR_COUNT 0xEF
#define DUEL_INIT_DUELIST_DATA_FIRST_SECTOR 0x1D3E

/* duel_scene_card_move */
#define DUEL_CARD_STAGING_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)

/* duel_scene_field_actions */
/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_FIELD_ACTIONS_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)
#define DUEL_FIELD_ACTIONS_CANCEL_BUTTON PAD_BUTTON_CROSS

/* duel_update_card_use_confirmation */
/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define FUNC_80020988_CANCEL_BUTTON PAD_BUTTON_CROSS

/* duel_result_runtime */
#define DUEL_RESULT_OUTRO_START_SECTOR 0x1DB6
#define DUEL_RESULT_DUELIST_DATA_FIRST_SECTOR 0x1D3E

/* Values that differ in the Japanese release; the US source names each
 * with an #ifndef default (jp_promote.REGIONAL). */
#define DUEL_RESULT_REWARDS_CONFIRM_BUTTON PAD_BUTTON_CIRCLE

/* debug_effect_screen */
#define DEBUG_EFFECT_FAST_STEP_BUTTON PAD_BUTTON_CIRCLE
#define DEBUG_EFFECT_FAST_MOVE_BUTTON PAD_BUTTON_CROSS
#define DEBUG_EFFECT_PLACE_BUTTON PAD_BUTTON_CIRCLE

/* duel_update_card_pick_cursor */
#define DUEL_VIEWER_CARD_ID_ADDRESS 0x8009B136
#define DUEL_VIEWER_Y_OFFSET_ADDRESS 0x8009B13B

/* duel_scene_update */
#define DUEL_QUIT_BOX_X 0x70
#define DUEL_QUIT_BOX_WIDTH 0x60
#define DUEL_QUIT_BOX_HEIGHT 0x20

/* duel_action_lock */
#define DUEL_TERRAIN_EFFECT_SECTOR(terrain) \
    ((((terrain) * 15) * 16 - (terrain)) + 0x1784)

/* duel_field_effects */
#define DUEL_EFFECT_RITUAL_CONFIRM_MASK (PAD_BUTTON_CIRCLE | PAD_BUTTON_SQUARE)

#include "../duel_scene.c"
