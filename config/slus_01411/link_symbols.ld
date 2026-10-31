/* Layout symbols the generated section script does not define. */
loaded_image_end = 0x801E0000;
D_80181000 = 0x80181000;

/* Address-based call target retained by unmatched DuelScene_UpdateBattle. */

/* Unmatched func_8002ACA4 still names the primary orbit by address. */

/* Interior byte of the model/graphics state's four-byte A4-A7 owner. */
D_8009AFA6 = D_8009AFA4 + 2;

gText_GlyphUploadScratch = D_801DA000;

gText_aChannels = D_800EB0F8;
gText_Channel1 = D_800EB15C;
gText_Channel3 = D_800EB224;
gText_dwChannel0State = D_800EB12C;

gCampaignMap_bEntryLocation = D_8009B363;
CampaignMap_SetLocation = 0x8016866C;

gDuel_aFieldCardRecords = D_801A7B64;

D_8009AFDC = gModel_dwSecondPassClutBits;
D_8009AFE0 = gModel_dwLitColorCacheBase;

gDebugEffect_pPreviewCard0 = tent_DebugEffectObject0;
gDebugEffect_pPreviewCard1 = tent_DebugEffectObject1;
gDebugEffect_aPreviewCardRecords = D_801A7B80;
gDebugEffect_szOperandsFormat = D_80010074;
gDebugEffect_szSecondOperandMarker = D_80010090;
gDebugEffect_szFirstOperandMarker = D_800100A8;

func_80069E44 = Model_DrawFt3Wireframes;
func_80069F94 = Model_DrawGt3Wireframes;
func_8006A0E8 = Model_DrawFt4Wireframes;
func_8006A268 = Model_DrawGt4Wireframes;
func_8006A3F0 = Model_DrawFt3WindowRecordWireframes;
func_8006A540 = Model_DrawGt3WindowRecordWireframes;
func_8006A694 = Model_DrawFt4WindowRecordWireframes;
func_8006A814 = Model_DrawGt4WindowRecordWireframes;
func_8006A99C = Model_DrawSharedGt3Wireframes;
func_8006AAFC = Model_DrawSharedGt4Wireframes;
func_8006AC88 = Model_DrawSharedGt3WindowRecordWireframes;
func_8006ADE8 = Model_DrawSharedGt4WindowRecordWireframes;

func_800612C0 = Model_DrawFt3Primitives;
func_8006151C = Model_DrawGt3Primitives;
func_800617E0 = Model_DrawFt4Primitives;
func_80061A84 = Model_DrawGt4Primitives;
func_80061DDC = Model_DrawFt3WindowRecordPrimitives;
func_80062058 = Model_DrawGt3WindowRecordPrimitives;
func_8006233C = Model_DrawFt4WindowRecordPrimitives;
func_80062600 = Model_DrawGt4WindowRecordPrimitives;
func_80062978 = Model_DrawTwoSidedFt3Primitives;
func_80062BC0 = Model_DrawTwoSidedGt3Primitives;
func_80062E70 = Model_DrawTwoSidedFt4Primitives;
func_80063100 = Model_DrawTwoSidedGt4Primitives;
func_80063444 = Model_DrawTwoSidedFt3WindowRecordPrimitives;
func_800636AC = Model_DrawTwoSidedGt3WindowRecordPrimitives;
func_8006397C = Model_DrawTwoSidedFt4WindowRecordPrimitives;
func_80063C2C = Model_DrawTwoSidedGt4WindowRecordPrimitives;
func_80063F90 = Model_DrawSemiFt3Primitives;
func_80064248 = Model_DrawSemiGt3Primitives;
func_80064568 = Model_DrawSemiFt4Primitives;
func_80064868 = Model_DrawSemiGt4Primitives;
func_80064C1C = Model_DrawSemiFt3WindowRecordPrimitives;
func_80064EF4 = Model_DrawSemiGt3WindowRecordPrimitives;
func_80065234 = Model_DrawSemiFt4WindowRecordPrimitives;
func_80065554 = Model_DrawSemiGt4WindowRecordPrimitives;
func_80065928 = Model_DrawTwoSidedSemiFt3Primitives;
func_80065BCC = Model_DrawTwoSidedSemiGt3Primitives;
func_80065ED8 = Model_DrawTwoSidedSemiFt4Primitives;
func_800661C4 = Model_DrawTwoSidedSemiGt4Primitives;
func_80066564 = Model_DrawTwoSidedSemiFt3WindowRecordPrimitives;
func_80066828 = Model_DrawTwoSidedSemiGt3WindowRecordPrimitives;
func_80066B54 = Model_DrawTwoSidedSemiFt4WindowRecordPrimitives;
func_80066E60 = Model_DrawTwoSidedSemiGt4WindowRecordPrimitives;

func_80067220 = Model_PrepareSharedPrimitiveCaches;
func_80067354 = Model_DrawSharedGt3Primitives;
func_8006759C = Model_DrawSharedGt4Primitives;
func_80067858 = Model_DrawSharedGt3WindowRecordPrimitives;
func_80067ABC = Model_DrawSharedGt4WindowRecordPrimitives;
func_80067D94 = Model_DrawSharedTwoSidedGt3Primitives;
func_80067FD0 = Model_DrawSharedTwoSidedGt4Primitives;
func_8006825C = Model_DrawSharedTwoSidedGt3WindowRecordPrimitives;
func_800684B4 = Model_DrawSharedTwoSidedGt4WindowRecordPrimitives;
func_8006875C = Model_DrawSharedSemiGt3Primitives;
func_80068A00 = Model_DrawSharedSemiGt4Primitives;
func_80068D18 = Model_DrawSharedSemiGt3WindowRecordPrimitives;
func_80068FD8 = Model_DrawSharedSemiGt4WindowRecordPrimitives;
func_8006930C = Model_DrawSharedTwoSidedSemiGt3Primitives;
func_800695A4 = Model_DrawSharedTwoSidedSemiGt4Primitives;
func_8006988C = Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives;
func_80069B40 = Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives;

gModel_CameraMove = D_800F2B20;
gModel_wCameraFollowCountdown = D_800F2B22;
D_800F2B4A = gModel_wCameraTargetSlot;
D_8009AFEC = gModel_AnimatedBattleCameraEndpoint;
D_8009AFFC = gModel_ExodiaOrbitOffset;

D_8009AFA4 = gGraphics_FrameStepControl;
gGraphics_nBaseFrameStep = D_8009B0D8;
D_8009B0A8 = gGraphics_bSkipDisplaySwap;
gGraphics_pOrderingTable1 = D_800E9D94;
gGraphics_pOrderingTable2 = D_800E9D98;
D_800E9D9C = gGraphics_pOrderingTable3;

func_800580D4 = Model_BuildViewRelativeCoordinateUnit;
func_80059B90 = Model_GetSlot0OffsetWorldPosition;

func_80060E70 = CardList_BuildRowSetTextBoxes;
gCardList_aRowSets = D_801A8000;
D_80090DD8 = gCardList_SortIconTable;

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
gDuel_bBattleRemovalSlot1 = D_8009B209;

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
gDuel_bFieldActionAutoCommit = D_8009B21A;

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

gModel_pSlot1PayloadBase = D_80010004;
gModel_AuxiliaryPayloadAddress = D_80010008;
gModel_CreditsModuleAddress = D_80010030;
gModel_pCreditsMusicTrackBuffer = D_80010034;

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

D_8009AF74 = gDisplayObject_awOrderingTableDepthBases;
gFade_bKeepOverlayDepth = D_8009B140;
gFade_bWhiteLatch = D_8009B145;




gMain_bReturnMode = D_8009B269;
gMain_bActiveModeState = D_8009B26C;
gMain_bRequestedMenuID = D_8009B26D;

gText_PrimaryStringOffsets = D_801C0000;
gText_GlobalStringOffsets = D_801D5800;
gText_PrimaryStringAddressAnchor = D_801B0000;

gGraphics_aImageTransferRects = D_800E9D70;
gRuntime_pSharedWorkBase = D_80010000;

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

PROVIDE(gLibCd_nDeferredStreamInterrupt = D_800F5D44);
gLibGs_sActiveBufferIndex = D_800FE0CC;
D_8009B061 = ABSOLUTE(gMovie_bCopyDisplayOnStop);

gDuelEffect_aEntries = tent_DuelEffectEntries;
gDuelEffect_aOccupancyValues = tent_DuelEffectOccupancy;
gDuelEffect_nCompactionRequested = D_8009B330;

PROVIDE(gLibrary_sCapturedReferenceY = D_80181002);
PROVIDE(gLibrary_sSampledReferenceY = D_80181012);

gText_aGlyphLookup = tent_GlyphLookupTable;
gText_aGlyphSearchEntries = tent_GlyphLookupTableEntries;
gText_DecimalDigitSjisKeys = tent_DecimalDigitSjisKeys;

/* Canonical view of the retained native countdown entry point. */
SD_UpdateSecondaryObjectReuseCountdowns = tent_SD_AgeSecondaryObjects;

/* Auxiliary-model upload sector; retain its native provider. */
gModel_abAuxiliaryPaletteSector = D_801DD800;

/* Canonical view of the retained handler-key registry. */
gModel_aHandlerKeyRegistry = tent_ModelHandlerRegistry;

/* Auxiliary slot's retained load-step byte view. */
gModel_bAuxiliaryLoadStep = D_800F5694;
gModel_bAuxiliaryActive = D_800F569F;
gText_DecimalDivisorView = D_80090E0C;
gTextBox_apfnStateCallbacks = D_80090E64;
gDuel_wActionSubstate = D_8009B20C;
gDuel_sEffectWorkCounter = D_8009B20C + 2;
gMain_nServiceResyncCountdown = runtime_gp;
gLibGs_DrawEnvView = D_800FE048;
