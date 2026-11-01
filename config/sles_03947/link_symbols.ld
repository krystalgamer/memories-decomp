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
gDuel_bTerrainCodegenAlias = 0x8009C2D9;
gFile_wTransferStepFlags_abs = 0x8009C05C;
gDuel_wAiHandStackPending = D_8009B20C;
gEuropean_D_8009B208 = 0x8009C10A;
gEuropean_D_8009B20C = 0x8009C10E;
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

gText_aChannels = D_800EB0F8;
gText_Channel2View = gText_aChannels + 0xC8;
gText_Channel3View = gText_aChannels + 0x12C;
gText_Channel1 = D_800EB15C;
gText_Channel3 = D_800EB224;
gText_dwChannel0State = D_800EB12C;

gCampaignMap_bEntryLocation = D_8009B363;
CampaignMap_SetLocation = 0x8016866C;

gDuel_aFieldCardRecords = D_801A7B64;

gModel_dwSecondPassClutBits = D_8009BF64;
gModel_dwLitColorCacheBase = D_8009BF68;

gDebugEffect_pPreviewCard0 = tent_DebugEffectObject0;
gDebugEffect_pPreviewCard1 = tent_DebugEffectObject1;
gDebugEffect_aPreviewCardRecords = D_801A7B80;
gDebugEffect_szOperandsFormat = D_80010074;
gDebugEffect_szSecondOperandMarker = D_80010090;
gDebugEffect_szFirstOperandMarker = D_800100A8;

func_8006151C = Model_DrawGt3Primitives;
func_80061A84 = Model_DrawGt4Primitives;

gModel_CameraMove = D_800F2B20;
gModel_wCameraFollowCountdown = D_800F2B22;
gModel_wCameraTargetSlot = D_800F2B4A;
gModel_AnimatedBattleCameraEndpoint = 0x8009BF74;
D_8009BF74 = gModel_AnimatedBattleCameraEndpoint;
gModel_ExodiaOrbitOffset = 0x8009BF84;
D_8009BF84 = gModel_ExodiaOrbitOffset;

gGraphics_FrameStepControl = D_8009AFA4;
gGraphics_nBaseFrameStep = D_8009B0D8;
gGraphics_bSkipDisplaySwap = D_8009B0A8;
gGraphics_pOrderingTable1 = D_800E9D94;
gGraphics_pOrderingTable2 = D_800E9D98;
gGraphics_pOrderingTable3 = D_800E9D9C;

func_800580D4 = Model_BuildViewRelativeCoordinateUnit;
func_80059B90 = Model_GetSlot0OffsetWorldPosition;

func_80050B6C = CardList_BuildRowSetTextBoxes;
gCardList_aRowSets = 0x801A8000;
gCardList_SortIconTable = D_80090DD8;

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
gDuel_bResumeFromAnimatedBattle = D_8009B369;
gDuel_wCampaignSceneOnWin = D_8009B370;
gDuel_wCampaignSceneOnLoss = D_8009B372;
gDuel_wAnimatedBattleBgmCommand = D_8009B374;

gDuel_bSide0ControllerSelector = D_8009B360;
gAi_bGuardianStarChoice = D_800EAE8F;
gAi_bHandFaceDownRequested = tent_AiSelectionRandom;
gAi_bFieldSourceCode = D_800EAE91;
gAi_bFieldTargetCode = D_800EAE92;
AiScript_SetSelectionRandom = AiScript_SetHandFaceDownRequest;
AiScript_ClearSelectionRandom = AiScript_ClearHandFaceDownRequest;
AiScript_LoadSelectionRandom = AiScript_LoadHandFaceDownRequest;

gDuel_bPresentationCursor = D_8009B1B9;
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

gMain_bReturnMode = D_8009B269;
gMain_bActiveModeState = D_8009B26C;
gMain_bRequestedMenuID = D_8009B26D;

gText_PrimaryStringOffsets = D_801B0004;
gText_GlobalStringOffsets = D_801D5804;

gGraphics_aImageTransferRects = D_800E9D70;

gSaveData_aFirstPlayerNameSjis = D_801D160C;
gSaveData_aPairWorkspace = D_801D1200;
gSaveData_aSecondPlayerState = D_801D2200;
gSaveData_aTradeWriteStaging = tent_TradeSaveBuffer;

gDialog_bChoiceControl = D_8009B34C;
gDialog_bCallbackState = D_8009B3C1;

gText_aCurrentPlayerNameGlyphs = D_801B125A;
gText_aFirstPlayerNameGlyphs = D_801B122B;
gText_aSecondPlayerNameGlyphs = D_801B1238;

gFade_bFlags = D_800E9ECE;
gFade_bStep = D_800E9ECF;

PROVIDE(gLibCd_nDeferredStreamInterrupt = D_800F6CD4);
gLibGs_sActiveBufferIndex = gEuropean_D_800FE0CC;
gMovie_bCopyDisplayOnStop = gEuropean_D_8009B061;

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
