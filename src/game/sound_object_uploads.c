#include "../types.h"

typedef struct {
    int image;
    int type;
    short x;
    short y;
    u8 pad_0C[52];
} Packet;

extern u8 *D_8009B458;
extern int D_80011434[];
extern void func_80077450(Packet *);

void func_80049CF8(void)
{
    Packet packet;
    register u8 *initial asm("$2") = D_8009B458;
    register u8 *state asm("$3");
    register int i asm("$16");
    register int offset asm("$17");
    register int *image asm("$18");

    initial[0x500] = 1;
    state = D_8009B458;
    if (*(short *)(state + 0x510) > 0) {
        int count;

        i = 0;
        asm volatile(
            "lui $2,%%hi(D_80011434)\n\t"
            "addiu %0,$2,%%lo(D_80011434)"
            : "=r"(image)
            :
            : "$2"
        );
        offset = i;
        do {
            u8 *entry = state + offset;

            if ((entry[0x183] >> 4) == 0 && entry[0x18D] != 0) {
                register int type asm("$2") = 3;

                packet.type = type;
                packet.image = *image;
                packet.x = 0;
                packet.y = 0;
                func_80077450(&packet);
            }
            image++;
            asm volatile("" : "+r"(image));
            state = D_8009B458;
            count = *(short *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += 40;
        } while (i < count);
    }
    state = D_8009B458;
    *(short *)(state + 0x7E2) = 4;
    state[0x500] = 0;
}

void func_80049DD8(void)
{
    Packet packet;
    register u8 *initial asm("$2") = D_8009B458;
    register u8 *state asm("$3");
    register int i asm("$16");
    register int offset asm("$17");
    register int *image asm("$18");

    initial[0x500] = 1;
    state = D_8009B458;
    if (*(short *)(state + 0x510) > 0) {
        int count;

        i = 0;
        asm volatile(
            "lui $2,%%hi(D_80011434)\n\t"
            "addiu %0,$2,%%lo(D_80011434)"
            : "=r"(image)
            :
            : "$2"
        );
        offset = i;
        do {
            u8 *entry = state + offset;

            if ((entry[0x183] >> 4) == 0 && entry[0x18D] != 0) {
                register int type asm("$2") = 3;

                packet.type = type;
                packet.image = *image;
                packet.x = *(u16 *)(entry + 0x194);
                packet.y = *(u16 *)(entry + 0x196);
                func_80077450(&packet);
            }
            image++;
            asm volatile("" : "+r"(image));
            state = D_8009B458;
            count = *(short *)(state + 0x510);
            asm volatile("" : "+r"(count));
            i++;
            offset += 40;
        } while (i < count);
    }
    state = D_8009B458;
    *(short *)(state + 0x7E2) = 1;
    state[0x500] = 0;
}
