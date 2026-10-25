#ifndef MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H
#define MEMORIES_DECOMP_MAIN_MENU_VALUE_SETUP_H

#include "../../types.h"
#include "../../ygo_types.h"
#include "../../game/display_object.h"

#define MAIN_MENU_VALUE_SETUP_CHOICE_LEFT 0
#define MAIN_MENU_VALUE_SETUP_CHOICE_RIGHT 1
#define MAIN_MENU_VALUE_SETUP_CHOICE_VALUE 2

/* Choices [0/1] belong to the two players; [2] is their shared toggle choice.
   The separate scalar view aliases [2], without merging the C identities. */
extern u8 gMainMenu_abValueSetupChoices[] __asm__("D_801845BC");
extern u8 gMainMenu_bValueSetupToggleChoice __asm__("D_801845BE");

/* gMainMenu_pValueSetupToggle points at the caller's toggle; gMainMenu_aValueSetupEntries holds
   the two values and their output pointers. The remaining symbols own the
   screen's display objects. Native choice declarations remain compatible. */
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
