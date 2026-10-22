#ifndef MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H
#define MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/display_object.h"

/* Value-setup screen state. D_801845BC holds the two live selections and
   toggle result; gMainMenu_pValueSetupToggle points at the caller's toggle; gMainMenu_aValueSetupEntries holds
   the two values and their output pointers. The remaining symbols own the
   screen's display objects. */
extern u8 D_801845BC[];
extern u8 *G32 gMainMenu_pValueSetupToggle;
extern ValueSetupEntry gMainMenu_aValueSetupEntries[2];
extern DisplayObject *G32 D_801845A0;
extern DisplayObject *G32 D_801845A4;
extern DisplayObject *G32 gMainMenu_apValueSetupCursors[];
extern DisplayObject *G32 gMainMenu_pValueSetupMainCursor;
extern u8 D_801845BE;

void MainMenu_StartValueSetup(u16 *first, u16 *second, u8 *toggle);
s32 MainMenu_UpdateValueSetup(void);
void MainMenu_FinishValueSetup(void);
void MainMenu_StartValueWidgetTween(s32 index, s32 value);
void MainMenu_UpdateValueWidgetTween(DisplayObject *o);
void MainMenu_DrawValueSetup(void);
s32 MainMenu_CountDecimalDigits(s32 value);

#endif
