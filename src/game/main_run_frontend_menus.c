#include "../types.h"
#include "../psyq/rand.h"

extern unsigned char D_8009B26C;
extern unsigned char D_8009B268;
extern unsigned char D_8009B269;
extern unsigned char D_8009B26D;
extern unsigned char D_800E9DC0[];
extern void func_8005B85C(void);
extern void func_800137E4(void);
extern void func_80039E9C(void);
extern void func_8018001C(int, int);
extern void func_80015A00(void);
extern void func_8003FF34(void);
extern void Fade_WaitOut(void);
extern void func_80180DD0(void);
extern void func_8002D458(int);
extern int func_80180390(void);
extern void func_8003BBF8(void);
extern void func_801683EC(void);
extern int NameEntry_PollCompletion(void);
extern void func_8003BEB8(void);
extern void func_8016A080(void);
extern void func_8016A37C(void);
extern void func_8003C2B4(void);
extern void Options_Init(void);
extern int Options_Update(void);
extern void func_8003C498(void);
extern void func_8003C950(void);
extern int func_8003CA5C(void);
extern void func_8008FB8C(void *, int);

void Main_RunMenu(void){unsigned char f=D_8009B26C;int r;if((f&0x40)==0){D_8009B26C=f|0x40;func_8005B85C();func_800137E4();func_80039E9C();func_8018001C(D_8009B268,D_8009B26D);func_80015A00();}rand();r=func_80180390();if(r>=0){func_8003FF34();Fade_WaitOut();func_80180DD0();func_8002D458(r);D_8009B269=8;}}

void Main_RunNameEntry(void)
{
    unsigned char flags = D_8009B26C;
    unsigned char value;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003BBF8();
        func_801683EC();
    }
    if (NameEntry_PollCompletion()) {
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}

void Main_RunPasswordMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003BEB8();
        func_8016A080();
    }
    func_8016A37C();
}

void Main_RunOptionsMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C2B4();
        Options_Init();
        func_80015A00();
    }
    if (Options_Update() == 0) {
        unsigned char value;

        func_8003FF34();
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}

void Main_RunGameOver(void){unsigned char f=D_8009B26C;if((f&0x40)==0){D_8009B26C=f|0x40;func_8003C498();func_8003C950();}if(func_8003CA5C()==0){unsigned char v;func_8003FF34();Fade_WaitOut();v=D_8009B269;__asm__ volatile("nop");D_8009B26C=v;if(v){D_8009B268=1;D_8009B26D=0;D_8009B26C=8;func_8008FB8C(D_800E9DC0,1);}}}

void func_8002D7C4(void)
{
}
