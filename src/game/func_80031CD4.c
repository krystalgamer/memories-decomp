#include "../types.h"

/* Builds the text box for one card slot of a trade or deck screen. The entry
   is picked out of the list at the scroll offset plus the slot, and the list
   kind at +0x2D47 selects both the box template and a fixed 0x160 shift. */

typedef struct {
    u8 pad_00[4];
    u16 id;
    u8 pad_06[7];
    u8 flags;
    u8 pad_0E[2];
} CardEntry;

typedef struct {
    CardEntry entries[0x2D3];
    u8 pad_2D30[0xC];
    s16 first;
    u8 pad_2D3E[9];
    u8 kind;
} CardList;

typedef struct {
    u8 pad_00[8];
    u16 flags;
} BoxChild;

typedef struct {
    u8 pad_00[0x28];
    BoxChild *child;
    u8 pad_2C[8];
    u16 field34;
    u8 pad_36[4];
    s16 field3A;
    u16 field3C;
    u8 pad_3E[0x16];
    u8 field54;
} Box;

extern s16 gDuel_wSelectedCardID;
extern Box *TextBox_Create(s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);

void func_80031CD4(CardList *list, s32 slot)
{
    Box *box;
    s32 style;

    gDuel_wSelectedCardID = list->entries[list->first + slot].id;
    style = 0;
    if (list->entries[list->first + slot].flags != 0) {
        style = 6;
    }
    box = TextBox_Create(list->kind + 1, style, 0x22, 0x2B, 0x120, 0xB0);
    box->field3A = slot * 22;
    box->child->flags &= 0xFFF7;
    if ((list->entries[list->first + slot].flags & 0x80) != 0) {
        box->field54 = 4;
    }
    if (list->kind != 0) {
        box->field3C += 0x160;
    }
    if (slot != 0) {
        box->field34 |= 0x40;
    }
    func_80039A14(box);
}
