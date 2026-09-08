#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_layout.h"
#include "display_object_render_table.h"

void func_80040CAC(void);
void func_80040588(void);
void func_80040814(void);
void func_80040BF8(void);
void func_80040DD8(void);
void func_80041068(void);
void func_80040D14(void);

void (*D_80090FB0[DISPLAY_OBJECT_LIST_COUNT])(void) = {
    func_80040CAC,
    func_80040588,
    func_80040814,
    func_80040BF8,
    func_80040DD8,
    func_80041068,
    func_80040D14,
};

GsF_LIGHT D_80090FCC[1] = {
    { 0, 0x1000, 0, 0x60, 0x60, 0x60 },
};

GsF_LIGHT D_80090FDC[1] = {
    { 0, -0x1000, 0, 0x30, 0x30, 0x30 },
};
