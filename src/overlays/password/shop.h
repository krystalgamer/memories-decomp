#ifndef MEMORIES_DECOMP_PASSWORD_SHOP_H
#define MEMORIES_DECOMP_PASSWORD_SHOP_H

#include "../../types.h"

/* Known prefix of the preview's control object, not its full allocation. */
typedef struct {
    u8 pad0[0x8];
    u16 flags;
    u8 pad0A[0x17];
    u8 phase;
    u8 pad22[0x10];
    u16 y;
} PasswordCardPreviewView;

typedef void (*PasswordCursorUpdate)(u8 *object);

extern PasswordCardPreviewView *D_8016D4D8;

void Password_InitShopScreen(void);
void Password_UpdateShopScreen(void);
void Password_UpdateDigitCursor(u8 *object);
void Password_UpdateDigitCursorDecoration(u8 *object);
/* Retain caller setup; the selected card comes from cached slot 0. */
void Password_RecreateCardPreview(s32 ignored);

#endif
