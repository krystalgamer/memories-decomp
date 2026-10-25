#include "../types.h"
#include "display_object_config.h"
#include "script_command_busy.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_core.h"
#include "display_object_layout.h"
#include "fade.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "file_transfer_state.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "sound_output.h"
#include "../unmatched.h"
#include "display_resource_banks.h"
#include "scene_script.h"
#include "script_op_duel_result.h"
#include "duel_result_load_package_stage.h"
#include "campaign_scene_package.h"
#ifdef VERSION_EUROPE
#include "duel_effect_resource_setup.h"
#endif

/* The menu assets' first sector in WA.MRG. A regional build defines its own. */
#ifndef SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR
#define SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR FILE_WA_MENU_ASSETS_START_SECTOR
#endif

/* Duel result screen setup. Reads the two-byte result code from the script
   stream, queues the result sector through File_RequestAsyncTransfer with
   DuelResult_LoadPackageStage as the completion callback, and on the first pass (once the
   transfer and fade flags are clear) builds the screen: the banner object
   from gDisplay_abResourceBank0, the two 0x140-wide panels, and the white-on-black fill
   object kept in gScript_TransitionObject, then starts the music. On later passes it
   waits for the fade, releases the fill object and the panel list, and
   clears the phase flag. */
void Script_OpDuelResult(void) {
    u8 *p;
    DisplayObject *o;
    DisplayObject *q;
    u16 flags;
    s32 four;
    s32 color;

    if (ScriptCommand_MarkStarted() == 0) {
        p = gScript_StreamCursor;
        gScript_StreamCursor = p + 2;
        gScript_SharedOperand = p[0] | (p[1] << 8);
        File_RequestAsyncTransfer(
            0, 0,
            SCRIPT_DUEL_RESULT_MENU_ASSETS_START_SECTOR,
            FILE_WA_MENU_ASSETS_SECTOR_COUNT,
            DuelResult_LoadPackageStage, 0, 0
        );
        File_WaitForTransfers();
    }
    flags = gScript_wCommandWord;
    if ((flags & 0x4000) == 0) {
        if (((gFile_dwTransferState_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             gFile_dwCancelRequestState_abs) != 0) {
            return;
        }
        gScript_wCommandWord = flags | 0x4000;
        if (gCampaign_PrimaryDisplayObject != 0) {
            gCampaign_PrimaryDisplayObject->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        ScriptImage_ReleaseObjects((ScriptImageObjectSet *)gSceneScript_Slots);
#ifdef VERSION_EUROPE
        /* The European banner takes gText_bLanguageIndex as its sixth argument, one
           0x140 by 0x100 panel stands in for the US two, and the fill quad
           is initialised in mode 1 with corners at y 0, 0x40 and 0x100. */
        four = 4;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(o, 0, 0, 0, 0, gText_bLanguageIndex, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(o, 1);
        o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots[0].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0, 0, 0x140, 0x100, 0, 0, 0x19, 0x280, 0xD5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[1].object_address = (s32)o;
        gSceneScript_Slots[2].object_address = 0;
        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(q, 1);
        q->field_30.word = 0x140;
        q->field_38.word = 0x400000;
        q->position.word = 0;
        q->field_40.word = 0x400140;
        q->field_48.word = 0x1000000;
        q->field_50.word = 0x1000140;
        q->field_2C.word = 0xFFFFFF;
        q->field_34.word = 0xFFFFFF;
        q->field_4C = 0xFFFFFF;
        q->field_54 = (void *)0xFFFFFF;
        gScript_TransitionObject = q;
        q->attribute |= (GsALON | GsATWO);
#else
        four = 4;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 2);
        DisplayObject_ConfigureSpriteAtPositionWithResource(o, 0, 0, 0, 0, 0, 0x17, four, gDisplay_abResourceBank0);
        DisplayObject_SetDepthOffset(o, 1);
        o->flags |= DISPLAY_OBJECT_FLAG_TEXTURE_CELL_OFFSET |
                    DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        gSceneScript_Slots[0].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0, 0, 0x140, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[1].object_address = (s32)o;
        o = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), 1);
        DisplayObject_ConfigureScreenSprite(o, 0x100, 0, 0x40, 0xF0, 0, 0, 0x19, 0, 0xF5);
        DisplayObject_SetDepthOffset(o, -1);
        gSceneScript_Slots[2].object_address = (s32)o;
        q = DisplayObject_AcquireSlot(DisplayObject_FindFreeGeneralSlot(), four);
        DisplayObject_InitializeGouraudQuad(q, 0);
        color = 0xF00140;
        q->field_30.word = 0x140;
        q->field_38.word = 0xF00000;
        q->field_3C.word = 0xFFFFFF;
        q->field_44.word = 0xFFFFFF;
        q->position.word = 0;
        q->field_40.word = color;
        gScript_TransitionObject = q;
        q->attribute |= (GsALON | GsATWO);
#endif
        Fade_StartIn();
        gFade_State.step = four;
        Fade_Wait();
    } else {
        if (SD_GetStatusFlags() & 0x80) {
            return;
        }
        Fade_WaitOut();
        if (gCampaign_PrimaryDisplayObject != 0) {
            gCampaign_PrimaryDisplayObject->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        DisplayObject_ReleaseIfPresent(gScript_TransitionObject);
        ScriptImage_ReleaseObjects((ScriptImageObjectSet *)gSceneScript_Slots);
        gScript_wCommandWord = 0;
    }
}
