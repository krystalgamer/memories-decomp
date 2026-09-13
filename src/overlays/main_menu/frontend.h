#ifndef MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H
#define MEMORIES_DECOMP_MAIN_MENU_FRONTEND_H

#include "../../types.h"

typedef void (*MainMenuEntryEffectUpdate)(u8 *object);

/* Frontend screen state, all of it owned by the front-end functions in
 * frontend.c and frontend_background.c and by MainMenu_UpdateFrontendMenu
 * (now a build-integrated candidate, src/candidates/main_menu/func_80180390.c). None
 * of these is defined in C.
 *
 * What the uses show, without renaming anything:
 *
 *   gMain_bMenuID          The selected entry, assigned as `menu % 11`, so
 *                          the wheel has eleven positions.
 *   gMain_apMenuEntries    One object pointer per entry, cleared to 0 for
 *                          the slots that are not live.
 *   D_80184558             The three standing objects: background, title and
 *   D_8018455C             prompt. Made by MainMenu_InitFrontendMenu and
 *   D_80184560             released by MainMenu_DestroyFrontendMenu.
 *   D_80184596             The transition mode, taken straight from
 *                          MainMenu_StartFrontendEntryTransition's argument.
 *   D_80184598             The fade direction, and the reason it is signed:
 *                          MainMenu_UpdateFrontendMenu assigns it 1 and -1,
 *                          tests `< 0` to decide which end of the fade it is
 *                          at, and shifts it left three into D_80184597 as a
 *                          step. See below.
 *   D_80184599             Set to 1 when that transition starts, and tested
 *                          as the "transition running" flag.
 *   D_80184597             A shade level, only ever 0 or 0x80.
 *   D_80184595             A flag, cleared on init and again once handled.
 *   D_8018459A..D_8018459D Four more flags with the same shape as each other:
 *                          tested non-zero, acted on, then cleared.
 *
 * D_80184598 used to sit outside this list because frontend_init.c spelled it
 * u8 and frontend_update.c spelled it s8, and a declaration its two declarers
 * disagree about spreads the wrong one. Coalescing those two sources into
 * frontend.c forced the question and the evidence settles it: the only writes
 * are 0, 1 and -1, and the only read that cares about sign is `< 0`. The u8
 * spelling was harmless rather than right, because the file that used it only
 * ever wrote 0.
 *
 * D_80184558/5C/60 are the three singleton display objects allocated by
 * MainMenu_InitFrontendMenu. Their matching and candidate consumers use the
 * shared DisplayObject fields rather than deriving the layout again.
 */
extern struct DisplayObject *D_80184558;
extern struct DisplayObject *D_8018455C;
extern struct DisplayObject *D_80184560;
extern s8 D_80184598;
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
