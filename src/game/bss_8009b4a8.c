#include "../types.h"
#include "../external_funcs.h"
#include "ai.h"
#include "build_deck_draw_card_list_page.h"
#include "camera_view.h"
#include "card_list_setup.h"
#include "dialog_choice_state.h"
#include "display_object_brightness.h"
#include "display_parent_links.h"
#include "duel_effect_create_channel.h"
#include "duel_ritual_controller.h"
#include "duel_screen_tables.h"
#include "graphics_frame.h"
#include "graphics_ordering_table_views.h"
#include "input.h"
#include "library_card_grid.h"
#include "main_services.h"
#include "model.h"
#include "model_cubic_coefficients.h"
#include "movie_stream_ranges.h"
#include "resident_bss.h"
#include "../overlays/password/name_entry_state.h"

/* Resident .bss storage from 0x8009B4A8 to 0x800F5D20, previously part of the
   generated zero-filled BSS image (#2602). Unnamed bytes keep address names. */
u32 bss_gGraphics_aFrameBuffers[10416] __asm__("gGraphics_aFrameBuffers") __attribute__((section(".bss"))) = {0};
u32 bss_gGraphics_abPacketBuffers[70000] __asm__("gGraphics_abPacketBuffers") __attribute__((section(".bss"))) = {0};
DISPENV gGraphics_DispEnv __attribute__((section(".bss"))) = {0};
u32 bss_pad_800E9D3C[13] __asm__("D_800E9D3C") __attribute__((section(".bss"))) = {0};
RECT D_800E9D70[2] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800E9D80[4] __asm__("D_800E9D80") __attribute__((section(".bss"))) = {0};
u32 bss_gGraphics_apOrderingTables __asm__("gGraphics_apOrderingTables") __attribute__((section(".bss"))) = 0;
GsOT * D_800E9D94 __attribute__((section(".bss"))) = 0;
GsOT * D_800E9D98 __attribute__((section(".bss"))) = 0;
GsOT * gGraphics_pOrderingTable3 __attribute__((section(".bss"))) = 0;
u32 bss_pad_800E9DA0[4] __asm__("D_800E9DA0") __attribute__((section(".bss"))) = {0};
u32 bss_gMain_apFrameServiceCallbacks[2] __asm__("gMain_apFrameServiceCallbacks") __attribute__((section(".bss"))) = {0};
u32 bss_D_800E9DB8 __asm__("D_800E9DB8") __attribute__((section(".bss"))) = 0;
u32 bss_D_800E9DBC __asm__("D_800E9DBC") __attribute__((section(".bss"))) = 0;
jmp_buf gMain_RecoveryJmpBuf __attribute__((section(".bss"))) = {0};
SpritePrim D_800E9DF0 __attribute__((section(".bss"))) = {0};
u32 bss_pad_800E9E14 __asm__("D_800E9E14") __attribute__((section(".bss"))) = 0;
FileTransferDescriptor gFile_SecondaryTransferDescriptor __attribute__((section(".bss"))) = {0};
u32 bss_gFile_PrimaryTransferDescriptor[12] __asm__("gFile_PrimaryTransferDescriptor") __attribute__((section(".bss"))) = {0};
u32 bss_D_800E9E90[5] __asm__("D_800E9E90") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800E9EA4 __asm__("D_800E9EA4") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800E9EA6 __asm__("D_800E9EA6") __attribute__((section(".bss"))) = 0;
u8 bss_D_800E9EA7 __asm__("D_800E9EA7") __attribute__((section(".bss"))) = 0;
u32 bss_gFile_anLba[3] __asm__("gFile_anLba") __attribute__((section(".bss"))) = {0};
s32 gMovie_dwStreamBaseLba __attribute__((section(".bss"))) = 0;
u32 bss_pad_800E9EB8[2] __asm__("D_800E9EB8") __attribute__((section(".bss"))) = {0};
u32 bss_gFile_nMasterXaLba[2] __asm__("gFile_nMasterXaLba") __attribute__((section(".bss"))) = {0};
u32 bss_gFade_State __asm__("gFade_State") __attribute__((section(".bss"))) = 0;
u16 bss_gFade_abCurrentLevel __asm__("gFade_abCurrentLevel") __attribute__((section(".bss"))) = 0;
u8 bss_D_800E9ECE __asm__("D_800E9ECE") __attribute__((section(".bss"))) = 0;
u8 bss_D_800E9ECF __asm__("D_800E9ECF") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800E9ED0[8] __asm__("D_800E9ED0") __attribute__((section(".bss"))) = {0};
u32 bss_gDisplayObject_apWorkSlots __asm__("gDisplayObject_apWorkSlots") __attribute__((section(".bss"))) = 0;
u32 bss_D_800E9EF4 __asm__("D_800E9EF4") __attribute__((section(".bss"))) = 0;
u32 bss_D_800E9EF8 __asm__("D_800E9EF8") __attribute__((section(".bss"))) = 0;
u32 bss_D_800E9EFC[2] __asm__("D_800E9EFC") __attribute__((section(".bss"))) = {0};
u32 bss_D_800E9F04[3] __asm__("D_800E9F04") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_abSelectionSides[7] __asm__("gDuel_abSelectionSides") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_abSelectionRecord1[7] __asm__("gDuel_abSelectionRecord1") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_abSelectionRecord2[7] __asm__("gDuel_abSelectionRecord2") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_abSelectionRecord3[35] __asm__("gDuel_abSelectionRecord3") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_aSideStates[4] __asm__("gDuel_aSideStates") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800EA000 __asm__("D_800EA000") __attribute__((section(".bss"))) = 0;
u16 bss_gDuel_wPlayerLifePointDisplay __asm__("gDuel_wPlayerLifePointDisplay") __attribute__((section(".bss"))) = 0;
u32 bss_gDuel_wPlayerLifePoint[7] __asm__("gDuel_wPlayerLifePoint") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800EA020 __asm__("D_800EA020") __attribute__((section(".bss"))) = 0;
u16 bss_gDuel_wOpponentLifePointDisplay __asm__("gDuel_wOpponentLifePointDisplay") __attribute__((section(".bss"))) = 0;
u32 bss_gDuel_wOpponentLifePoint[2] __asm__("gDuel_wOpponentLifePoint") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800EA02C __asm__("D_800EA02C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800EA02E __asm__("D_800EA02E") __attribute__((section(".bss"))) = 0;
u8 bss_gDuel_bSide1CardViewMode __asm__("gDuel_bSide1CardViewMode") __attribute__((section(".bss"))) = 0;
DuelHandSlot gDuelHand_aSlots[5 ] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800EA06C __asm__("D_800EA06C") __attribute__((section(".bss"))) = 0;
ScreenPair gDuel_aCardSlotScreenPositions[30 ] __attribute__((section(".bss"))) = {0};
u32 bss_gCardPreview_Resources[12] __asm__("gCardPreview_Resources") __attribute__((section(".bss"))) = {0};
u32 bss_gDuel_wFusionResultCardID[4] __asm__("gDuel_wFusionResultCardID") __attribute__((section(".bss"))) = {0};
u16 gCardPreview_Resource1[22 ] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800EA154[37] __asm__("D_800EA154") __attribute__((section(".bss"))) = {0};
LibraryState gLibrary_State __attribute__((section(".bss"))) = {0};
DuelEffectRequest gDuelEffect_aRequests[8 ] __attribute__((section(".bss"))) = {0};
u8 bss_gAi_Selection __asm__("gAi_Selection") __attribute__((section(".bss"))) = 0;
u8 bss_D_800EAE89 __asm__("D_800EAE89") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800EAE8A __asm__("D_800EAE8A") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800EAE8C __asm__("D_800EAE8C") __attribute__((section(".bss"))) = 0;
u8 bss_D_800EAE8E __asm__("D_800EAE8E") __attribute__((section(".bss"))) = 0;
u8 D_800EAE8F __attribute__((section(".bss"))) = 0;
u8 tent_AiSelectionRandom __attribute__((section(".bss"))) = 0;
u8 D_800EAE91 __attribute__((section(".bss"))) = 0;
u8 D_800EAE92 __attribute__((section(".bss"))) = 0;
u8 bss_pad_800EAE93 __asm__("D_800EAE93") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800EAE94 __asm__("D_800EAE94") __attribute__((section(".bss"))) = 0;
u32 bss_gSceneScript_Slots[16] __asm__("gSceneScript_Slots") __attribute__((section(".bss"))) = {0};
u32 bss_gDebug_szEditorCaretLine[12] __asm__("gDebug_szEditorCaretLine") __attribute__((section(".bss"))) = {0};
u8 tent_DuelEffectOccupancy[240 ] __attribute__((section(".bss"))) = {0};
u32 bss_gText_DecimalDigitGlyphMap[6] __asm__("gText_DecimalDigitGlyphMap") __attribute__((section(".bss"))) = {0};
u32 bss_D_800EB010[58] __asm__("D_800EB010") __attribute__((section(".bss"))) = {0};
u32 bss_D_800EB0F8[13] __asm__("D_800EB0F8") __attribute__((section(".bss"))) = {0};
u32 D_800EB12C __attribute__((section(".bss"))) = 0;
u32 bss_pad_800EB130[11] __asm__("D_800EB130") __attribute__((section(".bss"))) = {0};
u32 bss_D_800EB15C[10] __asm__("D_800EB15C") __attribute__((section(".bss"))) = {0};
DisplayObject * gDisplayObject_apBrightnessPool[4] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800EB194[11] __asm__("D_800EB194") __attribute__((section(".bss"))) = {0};
DuelEffectChannel D_800EB1C0 __attribute__((section(".bss"))) = {0};
u32 bss_D_800EB224[10] __asm__("D_800EB224") __attribute__((section(".bss"))) = {0};
u32 bss_D_800EB24C[15] __asm__("D_800EB24C") __attribute__((section(".bss"))) = {0};
DuelEffectEntry tent_DuelEffectEntries[620 ] __attribute__((section(".bss"))) = {0};
u32 bss_gMain_aAnimatedBattleModels[4] __asm__("gMain_aAnimatedBattleModels") __attribute__((section(".bss"))) = {0};
InputRawPadPacket gInput_abRawPadBuffers[2 ] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800EF6AC __asm__("D_800EF6AC") __attribute__((section(".bss"))) = 0;
u8 gInput_abRepeatTimers[(2  * 16 ) ] __attribute__((section(".bss"))) = {0};
u32 bss_gMemCardWork_Area[4] __asm__("gMemCardWork_Area") __attribute__((section(".bss"))) = {0};
u32 bss_D_800EF6E0[2] __asm__("D_800EF6E0") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800EF6E8 __asm__("D_800EF6E8") __attribute__((section(".bss"))) = 0;
u8 D_800EF6EA __attribute__((section(".bss"))) = 0;
u8 bss_pad_800EF6EB __asm__("D_800EF6EB") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800EF6EC[309] __asm__("D_800EF6EC") __attribute__((section(".bss"))) = {0};
u32 bss_gMemCardDialog_DirectoryBuffer[150] __asm__("gMemCardDialog_DirectoryBuffer") __attribute__((section(".bss"))) = {0};
u32 bss_gMemCardDialog_FileNameBuffer[8] __asm__("gMemCardDialog_FileNameBuffer") __attribute__((section(".bss"))) = {0};
u16 bss_gDisplayObject_awListHeads __asm__("gDisplayObject_awListHeads") __attribute__((section(".bss"))) = 0;
u16 bss_gDisplayObject_wSpriteListHead __asm__("gDisplayObject_wSpriteListHead") __attribute__((section(".bss"))) = 0;
s16 gDisplayObject_wSpriteSheetListHead __attribute__((section(".bss"))) = 0;
u16 bss_D_800EFE3E __asm__("D_800EFE3E") __attribute__((section(".bss"))) = 0;
u16 bss_D_800EFE40 __asm__("D_800EFE40") __attribute__((section(".bss"))) = 0;
u16 bss_D_800EFE42 __asm__("D_800EFE42") __attribute__((section(".bss"))) = 0;
u32 bss_D_800EFE44 __asm__("D_800EFE44") __attribute__((section(".bss"))) = 0;
u32 bss_gDisplayObject_aPool[448] __asm__("gDisplayObject_aPool") __attribute__((section(".bss"))) = {0};
DisplayObject gDisplayObject_aGeneralSlots[
    96  - 16
] __attribute__((section(".bss"))) = {0};
u16 bss_D_800F2848 __asm__("D_800F2848") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F284A __asm__("D_800F284A") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F284C[2] __asm__("D_800F284C") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F2854 __asm__("D_800F2854") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F2856 __asm__("D_800F2856") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F2858[8] __asm__("D_800F2858") __attribute__((section(".bss"))) = {0};
s16 gDisplayObject_awListKeys[7 ] __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F2886 __asm__("D_800F2886") __attribute__((section(".bss"))) = 0;
u32 bss_gMemCard_aDirEntries[150] __asm__("gMemCard_aDirEntries") __attribute__((section(".bss"))) = {0};
u32 bss_gMemCard_aIOEventHandles[4] __asm__("gMemCard_aIOEventHandles") __attribute__((section(".bss"))) = {0};
u32 bss_gMemCard_aHwIOEventHandles[4] __asm__("gMemCard_aHwIOEventHandles") __attribute__((section(".bss"))) = {0};
u32 bss_gMemCard_szRequestPath[8] __asm__("gMemCard_szRequestPath") __attribute__((section(".bss"))) = {0};
u16 bss_D_800F2B20 __asm__("D_800F2B20") __attribute__((section(".bss"))) = 0;
s16 D_800F2B22 __attribute__((section(".bss"))) = 0;
u32 bss_D_800F2B24[3] __asm__("D_800F2B24") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F2B30 __asm__("D_800F2B30") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F2B32 __asm__("D_800F2B32") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F2B34 __asm__("D_800F2B34") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F2B38 __asm__("D_800F2B38") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F2B3A __asm__("D_800F2B3A") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F2B3C __asm__("D_800F2B3C") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F2B40 __asm__("D_800F2B40") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F2B42 __asm__("D_800F2B42") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F2B44 __asm__("D_800F2B44") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F2B48 __asm__("D_800F2B48") __attribute__((section(".bss"))) = 0;
u16 bss_gModel_wCameraTargetSlot __asm__("gModel_wCameraTargetSlot") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F2B4C __asm__("D_800F2B4C") __attribute__((section(".bss"))) = 0;
ModelTintRequest gModel_aTintRequests[10 ] __attribute__((section(".bss"))) = {0};
u32 bss_gModel_aSlots[830] __asm__("gModel_aSlots") __attribute__((section(".bss"))) = {0};
u32 bss_gModel_Slot0MetadataView[30] __asm__("gModel_Slot0MetadataView") __attribute__((section(".bss"))) = {0};
ModelSlotLightEntry gModel_aSlot0FlatLights[3] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800F39E0[4] __asm__("D_800F39E0") __attribute__((section(".bss"))) = {0};
ModelSlotS32Quad gModel_Slot0Scale __attribute__((section(".bss"))) = {0};
u32 bss_pad_800F3A00[4] __asm__("D_800F3A00") __attribute__((section(".bss"))) = {0};
u32 bss_gModel_awSlot0Position[15] __asm__("gModel_awSlot0Position") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F3A4C __asm__("D_800F3A4C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F3A4E __asm__("D_800F3A4E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F3A4F __asm__("D_800F3A4F") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F3A50 __asm__("D_800F3A50") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F3A51 __asm__("D_800F3A51") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F3A52 __asm__("D_800F3A52") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F3A54[2] __asm__("D_800F3A54") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F3A5C __asm__("D_800F3A5C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F3A5E __asm__("D_800F3A5E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F3A5F __asm__("D_800F3A5F") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F3A60[894] __asm__("D_800F3A60") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F4858[5] __asm__("D_800F4858") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F486C __asm__("D_800F486C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F486E __asm__("D_800F486E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F486F __asm__("D_800F486F") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F4870 __asm__("D_800F4870") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F4874 __asm__("D_800F4874") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F4875 __asm__("D_800F4875") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F4876 __asm__("D_800F4876") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F4878 __asm__("D_800F4878") __attribute__((section(".bss"))) = 0;
u16 bss_pad_800F487C __asm__("D_800F487C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F487E __asm__("D_800F487E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F487F __asm__("D_800F487F") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F4880[838] __asm__("D_800F4880") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5598[56] __asm__("D_800F5598") __attribute__((section(".bss"))) = {0};
u32 bss_gModel_awStageSlotModelId[7] __asm__("gModel_awStageSlotModelId") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5694[2] __asm__("D_800F5694") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F569C __asm__("D_800F569C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F569E __asm__("D_800F569E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F569F __asm__("D_800F569F") __attribute__((section(".bss"))) = 0;
u32 bss_gModel_CameraCoordinateUnit __asm__("gModel_CameraCoordinateUnit") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F56A4[19] __asm__("D_800F56A4") __attribute__((section(".bss"))) = {0};
u32 bss_gModel_RView[3] __asm__("gModel_RView") __attribute__((section(".bss"))) = {0};
u32 bss_gModel_ViewReferencePoint[5] __asm__("gModel_ViewReferencePoint") __attribute__((section(".bss"))) = {0};
GsRVIEW2 gModel_SavedRefView __attribute__((section(".bss"))) = {0};
u32 bss_pad_800F5730[8] __asm__("D_800F5730") __attribute__((section(".bss"))) = {0};
u32 bss_gMovie_FileInfo[6] __asm__("gMovie_FileInfo") __attribute__((section(".bss"))) = {0};
SVECTOR gModel_aPoseSnapshot[3 ] __attribute__((section(".bss"))) = {0};
u32 bss_pad_800F5780[2] __asm__("D_800F5780") __attribute__((section(".bss"))) = {0};
Key gModel_aEffectKeys[10] __attribute__((section(".bss"))) = {0};
ModelHandlerRegistryEntry tent_ModelHandlerRegistry[80 ] __attribute__((section(".bss"))) = {0};
s32 gAiScript_aMemory[20 ] __attribute__((section(".bss"))) = {0};
u32 bss_gAiScript_State[37] __asm__("gAiScript_State") __attribute__((section(".bss"))) = {0};
u16 bss_pad_800F5C7C __asm__("D_800F5C7C") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F5C7E __asm__("D_800F5C7E") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F5C7F __asm__("D_800F5C7F") __attribute__((section(".bss"))) = 0;
u16 bss_gAi_wBestDifference __asm__("gAi_wBestDifference") __attribute__((section(".bss"))) = 0;
u8 gAi_bBestAttacker __attribute__((section(".bss"))) = 0;
u8 gAi_bBestTarget __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F5C84 __asm__("D_800F5C84") __attribute__((section(".bss"))) = 0;
u16 bss_D_800F5C88 __asm__("D_800F5C88") __attribute__((section(".bss"))) = 0;
u8 bss_pad_800F5C8A __asm__("D_800F5C8A") __attribute__((section(".bss"))) = 0;
u8 bss_D_800F5C8B __asm__("D_800F5C8B") __attribute__((section(".bss"))) = 0;
u32 bss_pad_800F5C8C[13] __asm__("D_800F5C8C") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5CC0[2] __asm__("D_800F5CC0") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5CC8[10] __asm__("D_800F5CC8") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5CF0 __asm__("D_800F5CF0") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F5CF4[2] __asm__("D_800F5CF4") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5CFC __asm__("D_800F5CFC") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F5D00[2] __asm__("D_800F5D00") __attribute__((section(".bss"))) = {0};
u32 bss_D_800F5D08 __asm__("D_800F5D08") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F5D0C __asm__("D_800F5D0C") __attribute__((section(".bss"))) = 0;
u32 bss_D_800F5D10[4] __asm__("D_800F5D10") __attribute__((section(".bss"))) = {0};
