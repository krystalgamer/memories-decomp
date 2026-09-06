#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern u8 D_8009B428;
extern short D_8009B098[];
extern u8 D_801AF000[];
extern void func_800434F4(void);
extern void func_80043328(void);
extern void File_RequestAsyncTransfer(int, int, int, int, void *, int, int);
extern void func_800137E4(void);
extern void func_8007E910(int, int);
extern void func_80047AD0(int);
extern void func_80012D84(int);
extern void func_80015780(void);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, int);
extern void func_800428A8(void *, int, int, int, int, int, int, int, int);
extern void func_8004365C(void *, void *);
extern void func_800438B8(int);
extern void func_8007E350(void);
extern void func_801680F4(void);
extern int func_80168160(int);
extern void func_8007AFA4(void);
extern void func_8005B85C(void);
extern void func_80015AD8(void);
extern void func_8002CD8C(void);

void func_80043960(int mode)
{
    register u8 *object;
    register u8 *first;
    D_8009B428 = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F85, 0x22, func_800434F4, 0, 0);
        func_800137E4();
    }
    File_RequestAsyncTransfer(0, 0, 0x1690, 0x36, func_80043328, 0, 0);
    if (mode != 0) {
        int display;
        func_800137E4();
        func_8007E910(0x2C0, 0);
        display = FntOpen(0x10, 0x10, 0x140, 0xF0, 0, 1000);
        SetDumpFnt(display);
        D_8009B098[0] = 0;
        func_80047AD0(2);
        func_80012D84(4);
        func_800137E4();
        return;
    }
    func_80015780();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  (int)D_801AF000);
    *(u16 *)(object + 8) |= 0x28;
    func_8004365C(0, object);
    func_800438B8(4);
    func_8007E910(0x2C0, 0);
    SetDumpFnt(FntOpen(0x10, 0x10, 0x140, 0xF0, 0, 1000));
    D_8009B098[0] = 0;
    func_8007E350();
    first = object;
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    func_8007AFA4();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  (int)D_801AF000);
    *(u16 *)(object + 8) |= 0x28;
    func_8004365C(first, object);
    func_80047AD0(2);
    func_80012D84(4);
    func_8005B85C();
    func_800438B8(0xB4);
    func_80015AD8();
    func_8002CD8C();
}
