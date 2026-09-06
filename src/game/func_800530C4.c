#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

typedef struct { int v[8]; } Block;
typedef struct { unsigned char b[8]; } __attribute__((packed)) Packed8;
typedef struct {
    int zero;
    unsigned char pad04[0x14];
    int v18, v1c, v20;
    unsigned char pad24[0x20];
    short r44, r46, r48;
    unsigned char pad4a[2];
    int v4c;
} Object;

extern unsigned short D_8009AF8C, D_8009AF90, D_8009AF8E;
extern unsigned short D_8009B47C, D_8009B478, D_8009B47A;
extern Packed8 D_8009B478_p asm("D_8009B478");
extern Packed8 D_8009B480;
extern Block D_800F56F0, D_800F5710;
extern Object D_800F56A0;
extern void func_800857C0(int), func_80058434(int, int, int, int, int);
extern void func_80088E50(void *, void *);
extern void func_8004E7B0(int), func_80052D2C(int, int, int, int);
extern void func_8005FAE4(void);
extern int func_8005F174(void), func_8005F18C(void);

void func_800530C4(void)
{
    int old = D_8009AF8C;
    D_8009AF90 = 0;
    D_8009AF8E = 0;
    D_8009B47C = 0;
    D_8009B478 = 0;
    D_8009B47A = 0xC00;
    func_800857C0(old);
    D_800F56F0.v[0] = 0;
    D_800F56F0.v[1] = -350;
    D_800F56F0.v[2] = -1200;
    D_800F56F0.v[3] = 0;
    D_800F56F0.v[4] = -350;
    D_800F56F0.v[5] = 0;
    D_800F56F0.v[6] = 0;
    D_800F56F0.v[7] = 0;
    func_80058434(1, 0x80, -85, 1200, 0);
    GsSetRefView2((GsRVIEW2 *)&D_800F56F0);
    {
        Object *o = &D_800F56A0;
        o->r48 = 0;
        o->r46 = 0;
        o->r44 = 0;
        o->v20 = 0;
        o->v1c = 0;
        o->v18 = 0;
        o->v4c = 0;
        func_80088E50(&o->r44, &o->pad04[0]);
        o->zero = 0;
    }
    D_800F5710 = D_800F56F0;
    D_8009B480 = D_8009B478_p;
    func_8004E7B0(1);
    {
        int state = func_8005F174();
        if (state != 1 || func_8005F18C() != state)
            func_80052D2C(0, 0, 0, 0);
    }
    func_8005FAE4();
}
