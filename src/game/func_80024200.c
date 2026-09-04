#include "../types.h"

typedef struct {
    u8 pad[0x30];
    void *field_30;
    u16 flags;
} Window;

extern u16 D_8009B162;
extern u8 D_8009B164;
extern u16 D_8009B16C;
extern u8 D_8009B174;
extern u16 D_8009B23A;
extern volatile u8 D_8009B260 __attribute__((section(".data")));
extern s8 D_8009B34D[9];
extern Window D_800EB224;
extern void (*D_80090998[])(void);
extern void func_800235C0(void), func_8002C6C8(void);
extern int func_80026B34(void), DuelEffect_UpdateState(void);
extern void SD_SEPlayFull(int);
extern Window *TextBox_CreateFlagged(int, int, int, int, int, int, int);
extern void func_80039794(void), TextBox_Destroy(Window *);

void func_80024200(void)
{
    u8 value;
    Window *window;
    if (D_8009B162 != 0)
        func_800235C0();
    func_8002C6C8();
    value = D_8009B260;
    if (value & 0x80) {
        if (value & 1)
            return;
        D_8009B260 = value & 0x7F;
    }
    if (func_80026B34() != 0 || DuelEffect_UpdateState() != 0)
        return;
    value = D_8009B164;
    if (value != 0) {
        if (!(value & 0x80)) {
            D_8009B164 = value | 0x80;
            SD_SEPlayFull(0x30);
            window = TextBox_CreateFlagged(3, 0x22, 0x78, 0x58, 0x50, 0x24, 0x20);
            do {
                func_80039794();
            } while (window->field_30 == 0);
        } else {
            register Window *cleanup __asm__("$4");
            func_80039794();
            __asm__(
                "lui $2,%%hi(D_800EB224)\n\t"
                "addiu %0,$2,%%lo(D_800EB224)"
                : "=r"(cleanup));
            if (cleanup->flags & 0x2000) {
                TextBox_Destroy(cleanup);
                D_8009B164 = 0;
                if (D_8009B34D[0] != 0)
                    D_8009B16C |= 0x2000;
            }
        }
    } else {
        register void (**callbacks)(void) __asm__("$3");
        register u16 index __asm__("$2");
        __asm__("lui %0,%%hi(D_80090998)" : "=r"(callbacks));
        index = D_8009B23A;
        __asm__("addiu %0,%0,%%lo(D_80090998)" : "+r"(callbacks));
        callbacks[index & 0xF]();
        if (!(D_8009B23A & 0x8000))
            D_8009B174 = 0;
    }
}
