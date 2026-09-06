#include "../types.h"

extern u8 D_80010538[];
extern u8 D_800F2B00[];
extern void *gMemCard_aIOEventHandles[];
extern void *D_800F2AF0[];
extern s32 D_8009B430;
extern s16 D_8009B434;
extern u8 D_8009B437;
extern s16 D_8009B44C;
extern volatile s32 gMemCard_nIOResult;
extern int func_800440B4(int, int);
extern void func_80043D48(void **);
extern void func_8008B330(int);
extern void func_8008B340(int);
extern void func_8008B3A0(int);
extern void func_8008F200(void *, void *, int, int);

int func_8004413C(int value)
{
    if (!func_800440B4(value, 2)) {
        return 0;
    }
    func_80043D48(gMemCard_aIOEventHandles);
    func_8008B330(value);
    while (gMemCard_nIOResult < 0) {
    }
    func_80043D48(D_800F2AF0);
    func_8008B3A0(D_8009B437);
    while (gMemCard_nIOResult < 0) {
    }
    func_80043D48(gMemCard_aIOEventHandles);
    func_8008B340(value);
    while (gMemCard_nIOResult < 0) {
    }
    return 1;
}

int func_800441DC(int value, int data, int global_data, int small, int extra)
{
    int result;

    if (func_800440B4(value, 3)) {
        func_8008F200(D_800F2B00, D_80010538, value, data);
        D_8009B44C = small;
        D_8009B430 = global_data;
        D_8009B434 = extra;
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_80044278(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 11)) {
        D_8009B44C = small;
        D_8009B430 = data;
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_800442E4(int value, int data, int global_data, int small, int extra)
{
    int result;

    if (func_800440B4(value, 4)) {
        func_8008F200(D_800F2B00, D_80010538, value, data);
        D_8009B44C = small;
        D_8009B430 = global_data;
        D_8009B434 = extra;
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_80044380(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 12)) {
        D_8009B44C = small;
        D_8009B430 = data;
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int func_800443EC(int value, int data, int small)
{
    int result;

    if (func_800440B4(value, 8)) {
        func_8008F200(D_800F2B00, D_80010538, value, data);
        D_8009B434 = small;
        func_80043D48(gMemCard_aIOEventHandles);
        func_8008B330(value);
        result = 1;
    } else {
        result = 0;
    }
    return result;
}
