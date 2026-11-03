loaded_image_end = 0x801E0000;
gText_bLanguageIndex = D_8009C02B;
Base2_8009B2A8 = gScript_ViewportTargetX;
Base2_8009B2AA = gScript_ViewportTargetY;
gMovie_bStreamEnded = 0x8009BEFF;
gMovie_bFadingOut = 0x8009BF00;
func_80069144 = Color_RgbToHsl;
func_80069358 = Color_HslToRgb;
func_80069620 = Color_TintBgr555Pixel;
func_8006980C = Color_HslToBgr555Pixel;
func_8006986C = Color_TintRgb;
func_80069C40 = Model_MapToBasePrimitiveHandler;
func_80069F6C = Model_RegisterHandlerKey;
GsGetTimInfo = func_80085E84;
GsLinkAnim = func_80089B5C;
GsScanAnim = func_80089AB4;
GsU_02000000 = 0x80089BC4;
GsU_02000001 = 0x80089C54;
func_800612C0 = 0x80060BDC;
func_800617E0 = 0x80060E38;
func_80061DDC = 0x800610DC;
func_80062058 = 0x80061358;
func_8006233C = 0x8006163C;
func_80062600 = 0x80061900;
func_80062978 = 0x80061C78;
func_80062BC0 = 0x80061EC0;
func_80062E70 = 0x80062170;
func_80063100 = 0x80062400;
func_80063444 = 0x80062744;
func_800636AC = 0x800629AC;
func_8006397C = 0x80062C7C;
func_80063C2C = 0x80062F2C;
func_80063F90 = 0x80063290;
func_80064248 = 0x80063548;
func_80064568 = 0x80063868;
func_80064868 = 0x80063B68;
func_80064C1C = 0x80063F1C;
func_80064EF4 = 0x800641F4;
func_80065234 = 0x80064534;
func_80065554 = 0x80064854;
func_80065928 = 0x80064C28;
func_80065BCC = 0x80064ECC;
func_80065ED8 = 0x800651D8;
func_800661C4 = 0x800654C4;
func_80066564 = 0x80065864;
func_80066828 = 0x80065B28;
func_80066B54 = 0x80065E54;
func_80066E60 = 0x80066160;
func_80067220 = 0x80066520;
func_80067354 = 0x80066654;
func_8006759C = 0x8006689C;
func_80067858 = 0x80066B58;
func_80067ABC = 0x80066DBC;
func_80067D94 = 0x80067094;
func_80067FD0 = 0x800672D0;
func_8006825C = 0x8006755C;
func_800684B4 = 0x800677B4;
func_8006875C = 0x80067A5C;
func_80068A00 = 0x80067D00;
func_80068D18 = 0x80068018;
func_80068FD8 = 0x800682D8;
func_8006930C = 0x8006860C;
func_800695A4 = 0x800688A4;
func_8006988C = 0x80068B8C;
func_80069B40 = 0x80068E40;
gFile_wTransferStepFlags_abs = 0x8009C05C;
gDuel_wAiHandStackPending = D_8009B20C;
func_80025034 = DuelEffect_ApplyLifePointRecovery;
func_800251C8 = DuelEffect_ApplyDirectDamage;
func_800252F8 = DuelEffect_ApplyMonsterRemoval;
func_80025568 = DuelEffect_ApplyStopDefense;
func_8002570C = DuelEffect_ApplyBoardDestruction;
func_800258D8 = DuelEffect_ApplyRaigeki;
func_80025B58 = DuelEffect_ApplyDarkPiercingLight;
func_80025C9C = DuelEffect_ApplyStatPenalty;
func_80013A1C = File_TryStartPrimaryTransfer;
func_80013A80 = File_RequestSecondaryRangeTransfer;
func_80013B40 = File_TransferReadyCallback;
func_80013FB8 = File_HandleReadEvent;
func_8001404C = File_HandleSeekEvent;
func_800140C0 = File_HandleCancelPauseEvent;
func_800141AC = File_HandleSetFilterEvent;
func_80014220 = File_HandleRangeReadEvent;
func_800142A8 = File_HandleLogicalPositionEvent;
func_800142F4 = File_ActivateTransfer;
func_800143D0 = File_FinishActiveTransfer;
func_80014474 = File_StepActiveTransfer;
func_80014A48 = SD_ConfigureTransferPhase;
func_80014B58 = File_RequestSecondarySlot;
gDuel_aCardUploadRects = 0x80177EA4;

/* Semantic draw-state aliases retain the original native providers. */
gModel_bCurrentDrawMode = D_8009BF6C;
gModel_wDrawTintRedGreen = D_8009BF6E;
gModel_bDrawTintBlue = D_8009BF70;
gModel_aShadowFanSteps = D_800925CC;

gText_Channel2View = gText_aChannels + 0xC8;
gText_Channel3View = gText_aChannels + 0x12C;
gText_Channel1 = D_800EB15C;
gText_Channel3 = D_800EB224;
gText_dwChannel0State = D_800EB12C;

gCampaignMap_bEntryLocation = D_8009B363;
CampaignMap_SetLocation = 0x8016866C;

gDuel_aFieldCardRecords = D_801A7B64;


gDebugEffect_pPreviewCard0 = tent_DebugEffectObject0;
gDebugEffect_pPreviewCard1 = tent_DebugEffectObject1;
gDebugEffect_aPreviewCardRecords = D_801A7B80;
gDebugEffect_szOperandsFormat = D_80010074;
gDebugEffect_szSecondOperandMarker = D_80010090;
gDebugEffect_szFirstOperandMarker = D_800100A8;

func_8006151C = Model_DrawGt3Primitives;
func_80061A84 = Model_DrawGt4Primitives;

gModel_AnimatedBattleCameraEndpoint = 0x8009BF74;
gModel_ExodiaOrbitOffset = 0x8009BF84;

gGraphics_nBaseFrameStep = D_8009B0D8;

func_800580D4 = Model_BuildViewRelativeCoordinateUnit;
func_80059B90 = Model_GetSlot0OffsetWorldPosition;

func_80050B6C = CardList_BuildRowSetTextBoxes;
gCardList_aRowSets = 0x801A8000;

func_800135FC = ViewState_ApplyAlternateOrbit;
gViewState_Current = D_800F2848;

func_8003FFD8 = SD_CdBgmPlay;
gSD_dwSavedBgmCommand = D_8009B404;

func_80019BD0 = Duel_UpdateCardMarkerRemovalTurn;
func_8001ED20 = DisplayObject_UpdateCosineSlideWithRollReset;

func_8002C570 = Library_CheckCardInChest;
gSaveData_Workspace = D_801D0000;

func_8002C400 = CardGrid_GetColumnX;
func_8002C484 = CardGrid_GetRowY;

gDuel_bBattleRemovalSlot0 = D_8009B208;
gDuel_bBattleRemovalSlot1 = D_8009C0B9;

gDuel_bPostDuelMainMode = D_8009B368;
gDuel_wCampaignSceneOnWin = D_8009B370;
gDuel_wCampaignSceneOnLoss = D_8009B372;
gDuel_wAnimatedBattleBgmCommand = D_8009B374;

AiScript_SetSelectionRandom = AiScript_SetHandFaceDownRequest;
AiScript_ClearSelectionRandom = AiScript_ClearHandFaceDownRequest;
AiScript_LoadSelectionRandom = AiScript_LoadHandFaceDownRequest;

gDuel_bPlacementNeedsCardMoveSetup = D_8009B218;
gDuel_bFieldActionAutoCommit = D_8009C0CC;

gDisplayEffect_aMenuRecords = D_800EB010;
gScript_pMenuRecord = D_8009B274;

gFile_dwTransferState = D_8009B0F4;
gFile_dwTransferState_abs = D_8009B0F4;
gFile_dwCancelRequestState = D_8009B134;
gFile_dwCancelRequestState_abs = D_8009B134;

gFile_TransferBufferBase = D_8009B118;
gFile_abPackageStagingBuffer = D_801DD000;
gModel_abTexturePaletteStaging = D_801DE000;

gDisplay_abResourceBank0 = D_801AF000;
gDisplay_abResourceBank1 = D_801AF800;


gDebug_abEditorCaretColumns = tent_DebugInterfaceFieldB4;
gDebug_bEditorCaretColumn1 = tent_DebugInterfaceFieldB5;
gDebug_bEditorCaretColumn2 = tent_DebugInterfaceFieldB6;
gDebug_bEditorLeadingLineCount = tent_DebugInterfaceFieldB8;
gDebug_abEditorDigitCounts = tent_DebugInterfaceFieldC0;
gDebug_bEditorDigitCount1 = tent_DebugInterfaceFieldC1;
gDebug_bEditorDigitCount2 = tent_DebugInterfaceFieldC2;
gDebug_bEditorRowCount = tent_DebugInterfaceFieldE0;
gDebug_bEditorFlags = tent_DebugInterfaceFlags;
gDebug_pszEditorFormat = tent_DebugInterfaceData;

gText_StagedValues = D_801D5608;
gDuel_DroppedCardTextValue = D_801D56A8;

gDisplayObject_awOrderingTableDepthBases = D_8009AF74;
gFade_bKeepOverlayDepth = D_8009B140;
gFade_bWhiteLatch = D_8009B145;

gMemCardWork_bFrameChecksum = 0x8021007F;


gText_PrimaryStringOffsets = D_801B0004;
gText_GlobalStringOffsets = D_801D5804;

gGraphics_aImageTransferRects = D_8009C418_sles_03947;

gSaveData_aFirstPlayerNameSjis = D_801D160C;
gSaveData_aPairWorkspace = D_801D1200;
gSaveData_aSecondPlayerState = D_801D2200;
gSaveData_aTradeWriteStaging = tent_TradeSaveBuffer;

gDialog_bChoiceControl = D_8009B34C;
gDialog_bCallbackState = D_8009B3C1;

gText_aCurrentPlayerNameGlyphs = D_801B125A;
gText_aFirstPlayerNameGlyphs = D_801B122B;
gText_aSecondPlayerNameGlyphs = D_801B1238;


PROVIDE(gLibCd_nDeferredStreamInterrupt = D_800F6CD4);

gDuelEffect_aEntries = tent_DuelEffectEntries;
gDuelEffect_nCompactionRequested = D_8009B330;

PROVIDE(gLibrary_sCapturedReferenceY = D_80181002);
PROVIDE(gLibrary_sSampledReferenceY = D_80181012);

gText_aGlyphLookup = tent_GlyphLookupTable;

/* Canonical view of the retained native countdown entry point. */

/* Auxiliary-model upload sector; retain its native provider. */
gModel_abAuxiliaryPaletteSector = D_801DD800;

/* Canonical view of the retained handler-key registry. */
gModel_aHandlerKeyRegistry = tent_ModelHandlerRegistry;

/* Auxiliary slot's retained load-step byte view. */
gModel_bAuxiliaryLoadStep = D_800F5694;
gModel_bAuxiliaryActive = D_800F569F;
gText_DecimalDivisorView = D_80090E0C;
gTextBox_apfnStateCallbacks = D_80090E64;
gDuel_wActionSubstate = gEuropean_D_8009B20C;
gDuel_sEffectWorkCounter = D_8009B20C + 2;
gMain_nServiceResyncCountdown = runtime_gp;
gLibGs_DrawEnvView = D_800FE048;
g_SDValue_output_level = g_SDValue;

/* Names inside graphics_frame's own storage (#2602). */
bss_start = D_8009BFE8_sles_03947;
gGraphics_bOrderingTable1Length = gGraphics_abOrderingTableLengths + 1;
gGraphics_bOrderingTable2Length = gGraphics_abOrderingTableLengths + 2;
gGraphics_bOrderingTable3Length = gGraphics_abOrderingTableLengths + 3;
D_8009C02B = D_8009C02B_sles_03947;

/* Names inside file_transfer_runtime's own storage (#2602). */
D_8009B0F4 = D_8009C040_sles_03947;
gFile_wStepState = D_8009C044_sles_03947;
gFile_FilterParams = D_8009C048_sles_03947;
D_8009B108 = D_8009C04C_sles_03947;
gFile_SecondaryPayloadCompleteCallback = D_8009C050_sles_03947;
D_8009B110 = D_8009C054_sles_03947;
gFile_ReadyDataEventCount = D_8009C058_sles_03947;
gFile_wTransferStepFlags = D_8009C05C_sles_03947;
gFile_wTransferStepFlags_abs = D_8009C05C_sles_03947;
gFile_CommandRetryCount = D_8009C060_sles_03947;
D_8009B134 = D_8009C064_sles_03947;
gFile_RangePauseCallback = D_8009C068_sles_03947;
D_8009B12C = D_8009C06C_sles_03947;
gFile_ServiceCallGate = D_8009C070_sles_03947;
gFile_ServiceReentryLatch = D_8009C074_sles_03947;
gFile_RangeWaitCountdown = D_8009C078_sles_03947;
gFile_RangeReadCallback = D_8009C07C_sles_03947;
D_8009B0FC = D_8009C080_sles_03947;
gFile_ReadySourceCursor = D_8009C084_sles_03947;
gFile_BeforePrimaryRequestCallback = D_8009C088_sles_03947;
gFile_ReadyEventCount = D_8009C08C_sles_03947;
D_8009B118 = D_8009C090_sles_03947;
gFile_TargetLoc = D_8009C094_sles_03947;

/* Names inside fade_runtime's own storage (#2602). */
gDuel_wViewTweenDistanceTarget = D_8009C0AA_sles_03947;

/* Names inside duel_field_card_render's own storage (#2602). */
gDuel_apSwordsEffectObjects = D_8009C0B0_sles_03947;
D_8009B20C = D_8009C0C2_sles_03947;
D_8009B20E = D_8009C0C4_sles_03947;
gDuel_pSide0SaveData = D_8009C0D8_sles_03947;
gDuel_pSide1SaveData = D_8009C0DC_sles_03947;
gDuel_wTwoPlayerCardViewMode = D_8009C0F1_sles_03947;
gDuel_pActionReadyMarker = D_8009C108_sles_03947;
gEuropean_D_8009B208 = D_8009C10A_sles_03947;
gEuropean_D_8009B20C = D_8009C10E_sles_03947;
gDuel_bActiveEffectState = D_8009C1B0_sles_03947;
gDuel_pCardViewerBackground = D_8009C1B4_sles_03947;

/* Names inside duel_effect_dialog_state's own storage (#2602). */
gDuel_pCardViewerCard = D_8009C1C0_sles_03947;
gDuel_pCardViewerTextBox = D_8009C1C8_sles_03947;

/* Names inside duel_effect_object_pool's own storage (#2602). */
gDuel_bEffectRequestStatus = D_8009C1D8_sles_03947;

/* Names inside script_command_busy's own storage (#2602). */
gScript_SharedOperand = D_8009C1F2_sles_03947;

/* Names inside script_op_load_image_scene's own storage (#2602). */
gScript_ImageOperand = D_8009C210_sles_03947;
D_8009B274 = D_8009C214_sles_03947;
gCampaignSceneIndex = D_8009C218_sles_03947;
gScript_WaitCounter = D_8009C220_sles_03947;

/* Names inside debug_menu_editor_entries's own storage (#2602). */
tent_DebugInterfaceFieldC1 = tent_DebugInterfaceFieldC0 + 1;
tent_DebugInterfaceFieldC2 = tent_DebugInterfaceFieldC0 + 2;
tent_DebugInterfaceFieldB5 = tent_DebugInterfaceFieldB4 + 1;
tent_DebugInterfaceFieldB6 = tent_DebugInterfaceFieldB4 + 2;
D_8009B2C6 = gDebug_nLastSoundID + 2;

/* Names inside sorted_entry_relink's own storage (#2602). */
gModel_dwFogDrawFlags = D_8009C26C_sles_03947;
gSortedEntry_pCursor = D_8009C270_sles_03947;
gSortedEntry_nCount = D_8009C274_sles_03947;
gSortedEntry_pBase = D_8009C278_sles_03947;
gSortedEntry_nSortedCount = D_8009C27C_sles_03947;

/* Names inside input_pads's own storage (#2602). */

/* Names inside mem_card_dialog's own storage (#2602). */
gSaveDataSequence = D_8009C31C_sles_03947;
D_8009B3BC = D_8009C32C_sles_03947;
D_8009B3DE = D_8009C34A_sles_03947;

/* Names inside sound_frontend's own storage (#2602). */

/* Names inside display_object_core's own storage (#2602). */

/* Names inside mem_card_driver's own storage (#2602). */

/* Names inside sound_output_transition's own storage (#2602). */

/* Names inside model_scene_controllers's own storage (#2602). */
gModel_ImageCopyRectY = gModel_ImageCopyRect + 2;
gModel_ImageCopyRectW = gModel_ImageCopyRect + 4;
gModel_ImageCopyRectH = gModel_ImageCopyRect + 6;
gModel_ViewSnapshot = D_8009C3F0_sles_03947;
gModel_wViewYawSnapshot = D_8009C3F2_sles_03947;
gModel_wViewPitchSnapshot = D_8009C3F4_sles_03947;

/* Names inside graphics_frame's own storage (#2602). */
gGraphics_apOrderingTables = D_8009C418_sles_03947 + 32;
gMain_RecoveryJmpBuf = D_8009C418_sles_03947 + 48;
gMain_apFrameServiceCallbacks = D_8009C418_sles_03947 + 96;
gGraphics_DispEnv = D_8009C418_sles_03947 + 112;
gGraphics_abPacketBuffers = D_8009C418_sles_03947 + 184;
gGraphics_aFrameBuffers = D_8009C418_sles_03947 + 280184;

/* Names inside file_transfer_runtime's own storage (#2602). */
gFile_anLba = D_800EAD78_sles_03947;
gMovie_dwStreamBaseLba = D_800EAD78_sles_03947 + 12;
gFile_nMasterXaLba = D_800EAD78_sles_03947 + 24;
gFile_PrimaryTransferDescriptor = D_800EAD78_sles_03947 + 32;
gFile_SecondaryTransferDescriptor = D_800EAD78_sles_03947 + 104;

/* Names inside fade_runtime's own storage (#2602). */
gFade_State = D_800EAE28_sles_03947;
gFade_abCurrentLevel = D_800EAE28_sles_03947 + 4;

/* Names inside duel_field_card_render's own storage (#2602). */
gDuel_aSideStates = D_800EAE58_sles_03947;
gDuel_bSide1CardViewMode = D_800EAE58_sles_03947 + 63;
gDuelHand_aSlots = D_800EAE58_sles_03947 + 64;
gDuel_aCardSlotScreenPositions = D_800EAE58_sles_03947 + 128;
gDisplayObject_apWorkSlots = D_800EAE58_sles_03947 + 248;
D_800E9EF4 = D_800EAE58_sles_03947 + 252;
gDuel_abSelectionSides = D_800EAE58_sles_03947 + 280;
gDuel_abSelectionRecord1 = D_800EAE58_sles_03947 + 308;
gDuel_abSelectionRecord2 = D_800EAE58_sles_03947 + 336;
gDuel_abSelectionRecord3 = D_800EAE58_sles_03947 + 364;
gCardPreview_Resources = D_800EAE58_sles_03947 + 504;
gCardPreview_Resource1 = D_800EAE58_sles_03947 + 568;

/* Names inside duel_effect_process_entries's own storage (#2602). */
tent_DuelEffectEntries = D_800EBF70_sles_03947;
D_800F0004 = D_800EBF70_sles_03947 + 16532;
D_800F0008 = D_800EBF70_sles_03947 + 16536;
D_800F0030 = D_800EBF70_sles_03947 + 16576;

/* Names inside text_box_runtime's own storage (#2602). */
gText_aChannels = D_800F0430_sles_03947;
D_800EB12C = D_800F0430_sles_03947 + 52;
D_800EB15C = D_800F0430_sles_03947 + 100;
gDisplayObject_apBrightnessPool = D_800F0430_sles_03947 + 140;
D_800EB224 = D_800F0430_sles_03947 + 300;
gMain_aAnimatedBattleModels = D_800F0430_sles_03947 + 400;

/* Names inside input_pads's own storage (#2602). */
gInput_abRepeatTimers = D_800F05D0_sles_03947;
gInput_abRawPadBuffers = D_800F05D0_sles_03947 + 32;

/* Names inside mem_card_dialog's own storage (#2602). */
gMemCardDialog_DirectoryBuffer = D_800F0638_sles_03947;
gMemCardDialog_FileNameBuffer = D_800F0638_sles_03947 + 600;
gMemCardWork_Area = D_800F0638_sles_03947 + 632;
gMemCardWork_bRootBlocks = D_800F0638_sles_03947 + 648;
D_800EF6EA = D_800F0638_sles_03947 + 658;

/* Names inside display_object_core's own storage (#2602). */
gDisplayObject_awListHeads = D_800F0DA0_sles_03947;
gDisplayObject_wSpriteListHead = D_800F0DA0_sles_03947 + 2;
gDisplayObject_wSpriteSheetListHead = D_800F0DA0_sles_03947 + 4;
D_800EFE3E = D_800F0DA0_sles_03947 + 6;
D_800EFE40 = D_800F0DA0_sles_03947 + 8;
D_800EFE42 = D_800F0DA0_sles_03947 + 10;
D_800EFE44 = D_800F0DA0_sles_03947 + 12;
gDisplayObject_awListKeys = D_800F0DA0_sles_03947 + 16;
D_800F2848 = D_800F0DA0_sles_03947 + 32;
D_800F284A = D_800F0DA0_sles_03947 + 34;
D_800F2856 = D_800F0DA0_sles_03947 + 46;
gDisplayObject_aPool = D_800F0DA0_sles_03947 + 80;
gDisplayObject_aGeneralSlots = D_800F0DA0_sles_03947 + 1872;

/* Names inside mem_card_driver's own storage (#2602). */
gMemCard_aIOEventHandles = D_800F37F0_sles_03947;
gMemCard_aHwIOEventHandles = D_800F37F0_sles_03947 + 16;
gMemCard_szRequestPath = D_800F37F0_sles_03947 + 32;
gMemCard_aDirEntries = D_800F37F0_sles_03947 + 64;
gAi_bHandFaceDownRequested = D_800EAE88 + 8;
D_8009C0CC = D_8009C0CC_sles_03947;
D_800F5694 = D_800F3A10 + 7300;
D_8009C0A8 = D_8009C0A8_sles_03947;
D_8009C0C0 = D_8009C0C0_sles_03947;
D_8009C148 = D_8009C148_sles_03947;
D_8009C408 = D_8009C408_sles_03947;
gAiScript_State = D_800F5BE8;
gAi_Selection = D_800EAE88;
gAi_wBestDifference = D_800F5BE8 + 152;
gModel_CameraCoordinateUnit = D_800F3A10 + 7312;
gModel_RView = D_800F3A10 + 7392;
gModel_SavedRefView = D_800F3A10 + 7424;
gModel_Slot0MetadataView = D_800F3938;
gModel_Slot0Scale = D_800F39B0 + 64;
gModel_ViewReferencePoint = D_800F3A10 + 7404;
gModel_aEffectKeys = D_800F5768 + 32;
gModel_aPoseSnapshot = D_800F5768;
gModel_aSlot0FlatLights = D_800F39B0;
gModel_aSlots = D_800F2C40;
gModel_awSlot0Position = D_800F3A10;
gModel_awStageSlotModelId = D_800F3A10 + 7272;
gSceneScript_Slots = D_800EAE88 + 16;
