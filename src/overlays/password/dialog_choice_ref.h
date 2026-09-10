#ifndef MEMORIES_DECOMP_PASSWORD_DIALOG_CHOICE_REF_H
#define MEMORIES_DECOMP_PASSWORD_DIALOG_CHOICE_REF_H

#include "../../types.h"

/* The resident dialog choice index, as this overlay has to spell it.
 *
 * shop.c tests it against 0 after a prompt and name_entry_runtime.c clears it
 * before raising one and tests it afterwards, so both read it as the option
 * the player settled on.
 *
 * The address-based name is kept on purpose. 0x8009B34D is the byte the
 * resident tree declares as gDialog_bChoice -- symbols.txt has
 * `gDialog_bChoice = 0x8009B34D`, and src/game/dialog_choice.h declares it
 * `extern s8 gDialog_bChoice` after working out that eleven declarations in
 * four spellings reduce to one small-data arm. An overlay cannot say that
 * name: overlay links resolve resident data through splat's generated
 * undefined_syms_auto.txt, and password's emits
 *
 *     D_8009B34D = 0x8009B34D;
 *
 * so gDialog_bChoice is not visible here and using it would be an undefined
 * reference. src/overlays/main_menu/card_tables.h records the same situation
 * for gDuel_adwCardStats, and #3326 hit its function-side form with
 * TextBox_Create.
 *
 * The s8 spelling matches the resident small-data arm; both files already
 * agreed on it. */
extern s8 D_8009B34D;

#endif
