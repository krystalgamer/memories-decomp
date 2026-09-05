#include "../types.h"

/* Creates 3 objects via func_800400AC(func_8004002C(),
   kind) and configures each: obj1 gets an 8-arg func_800428A8 setup
   plus a func_800428EC(obj1, -5), then sets the options state and output type
   from gSD_bOutputType (clearing the output type back to 0 when its sign bit
   is set) and calls func_8003C4E0(0); obj2 gets an
   8-arg func_800404CC setup and is stashed in D_8009B388;
   obj3 gets a 10-arg func_80040510 setup. Finally Options_UpdateLayout is called
   with the selection flag (set to 0 earlier), D_8009B380 is set to obj3,
   and func_8003FF08(0x7350) runs last.

   gSD_bOutputType needs an oversized array extern to force absolute (lui+lbu)
   addressing instead of gp-relative -- see project memory on far globals
   needing this trick. The single load of it is cached in a local and
   reused for both the output-type store and the `< 0` sign test, matching
   how gcc schedules the real target (interleaved with obj1->f8's
   load-early/store-late around the intervening `ori`). */

struct Obj {
    char pad0[0x8];
    u16 f8;
    char pad1[0x30 - 0xA];
    u16 f30;
    u16 f32;
};

extern s8 gSD_bOutputType[16];
extern u8 gOptions_bState;
extern s8 gOptions_bOutputType;
extern struct Obj *D_8009B380;
extern s8 gOptions_bSelection;
extern struct Obj *D_8009B388;
extern u8 D_801AF000[];

extern s32 func_8004002C(void);
extern struct Obj *func_800400AC(s32 slot, s32 kind);
extern void func_800428A8(struct Obj *obj, s32 a1, s32 a2, s32 a3,
                                   s32 a5, s32 a6, s32 a7, s32 a8, u8 *a9);
extern void func_800428EC(struct Obj *obj, s32 a1);
extern void func_8003C4E0(s32 a0);
extern void func_800404CC(struct Obj *obj, s32 a1, s32 a2, s32 a3,
                                         s32 a5, s32 a6, s32 a7, s32 a8);
extern void func_80040510(struct Obj *obj, s32 a1, s32 a2, s32 a3, s32 a5,
                           s32 a6, s32 a7, s32 a8, s32 a9, s32 a10);
extern void Options_UpdateLayout(s32 arg0);
extern void func_8003FF08(s32 a0);

void Options_Init(void) {
    register struct Obj *obj asm("s1");
    s32 s0;
    s32 s2;

    obj = func_800400AC(func_8004002C(), 2);
    s2 = 0x10;
    func_800428A8(obj, 0, 0, 0, 0, 0, s2, 0x100, D_801AF000);
    func_800428EC(obj, -5);
    gOptions_bState = 1;
    {
        s8 flag408 = gSD_bOutputType[0];
        obj->f8 |= 0x28;
        gOptions_bOutputType = flag408;
        if (flag408 < 0) {
            gOptions_bOutputType = 0;
        }
    }
    gOptions_bSelection = 0;
    func_8003C4E0(0);

    obj = func_800400AC(func_8004002C(), 2);
    s0 = 0xB;
    func_800404CC(obj, 0x18, 0x48, 3, 4, 0, s0, 0x20C);
    D_8009B388 = obj;
    obj->f8 |= 0x28;

    obj = func_800400AC(func_8004002C(), 1);
    func_80040510(obj, 0x68, 0x48, 0x10, s2, 0x50, 0x80, s0, 0x210, 0xFC);

    {
        s32 flag = gOptions_bSelection;
        D_8009B380 = obj;
        Options_UpdateLayout(flag);
    }
    func_8003FF08(0x7350);
}
