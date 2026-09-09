#ifndef MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H

#include "../../types.h"

typedef void (*MainMenuEntryEffectUpdate)(u8 *object);

/* Frontend screen state. None of these is defined in C, and all four sources
 * that use them already include this header, so they were being re-declared
 * by hand for no reason other than the absence of a line here.
 *
 * What the uses show, without renaming anything:
 *
 *   gMain_bMenuID          The selected entry, assigned as `menu % 11`, so
 *                          the wheel has eleven positions.
 *   gMain_apMenuEntries    One object pointer per entry, cleared to 0 for
 *                          the slots that are not live.
 *   D_80184596             The transition mode, taken straight from
 *                          MainMenu_StartFrontendEntryTransition's argument.
 *   D_80184599             Set to 1 when that transition starts, and tested
 *                          as the "transition running" flag.
 *   D_80184597             A shade level, only ever 0 or 0x80.
 *   D_80184595             A flag, cleared on init and again once handled.
 *   D_8018459A..D_8018459D Four more flags with the same shape as each other:
 *                          tested non-zero, acted on, then cleared.
 *
 * D_80184598 sits inside that run and is deliberately not here: frontend_init
 * spells it u8 and frontend_update spells it s8. Moving a declaration its two
 * declarers disagree about would spread the wrong one, so it stays local
 * until the sign is settled from evidence.
 */
extern u8 gMain_bMenuID;
extern u8 *gMain_apMenuEntries[];
extern u8 D_80184595;
extern u8 D_80184596;
extern u8 D_80184597;
extern u8 D_80184599;
extern u8 D_8018459A;
extern u8 D_8018459B;
extern u8 D_8018459C;
extern u8 D_8018459D;

void MainMenu_StartFrontendEntryTransition(s32 mode);
void MainMenu_DrawFrontendBackground(void);
void MainMenu_SpawnFrontendEntryAfterimage(u8 *source);
void MainMenu_UpdateFrontendEntryAfterimage(u8 *object);

#endif
