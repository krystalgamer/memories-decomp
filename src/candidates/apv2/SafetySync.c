#include "../../types.h"
#include "../../psyq/libetc.h"

extern s32 D_80169474;
extern u8 safechk_params[];
extern u8 safechk_results[];
/* D_8016946D and D_8016946E are safechk_results[1..2]; indexing the array
 * instead changes the object's relocation addends. */
extern u8 D_8016946D[];
extern u8 D_8016946E;
extern s32 D_801694C4;
extern s32 D_801694C8;
extern u8 D_801694CC;
extern u8 D_801694CD;

void func_80168C6C(s32);
s32 func_80168A98(s32);
void SafetyCheck2(void);

/* States that touch several bytes of one buffer take its address from one
 * of these accessors. Each integrated call ends in a label, so when
 * SafetySync_Run is integrated the inliner does not substitute the constant
 * address into every access, and cse keeps it in one register as in retail. */
static inline u8 *SafetySync_Results(void)
{
    return safechk_results;
}

static inline u8 *SafetySync_ResultTime(void)
{
    return D_8016946D;
}

static inline u8 *SafetySync_Params(void)
{
    return safechk_params;
}

/* Runs controller states and returns the last state's result: one state if
 * ARG is nonzero, otherwise states until one returns 0. The repeat is a tail
 * call, which GCC compiles as a jump back to the switch; RET is a parameter so
 * that its initial 0 is set before that jump target. A do/while or for loop
 * gets loop notes instead: loop.c hoists the -1, -2, 1 and 17 compare
 * constants into saved registers, and flow's loop-depth weighting reorders the
 * registers of the state 4 BCD block. */
static inline s32 SafetySync_Run(s32 arg, s32 ret)
{
    s32 t;

    switch (D_80169474) {
    case 1:
        func_80168C6C(1);
        D_80169474 = 2;
        ret = 1;
        break;
    case 2:
        ret = func_80168A98(1);
        switch (ret) {
        case 0:
        default:
            ret = 2;
            break;
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = 2;
            break;
        case 1:
            ret = 2;
            D_801694C8 = D_8016946E;
            func_80168C6C(6);
            D_80169474 = 3;
            break;
        case -1:
            func_80168C6C(1);
            ret = 2;
            break;
        }
        break;
    case 3:
        ret = func_80168A98(6);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            safechk_params[0] = (D_801694C8 >= 2) << 1;
            func_80168C6C(2);
            D_80169474 = 4;
        case 0:
        default:
            ret = 3;
            break;
        }
        break;
    case 4:
        ret = func_80168A98(2);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            {
                u8 *p = SafetySync_ResultTime();
                s32 mm, ss, tt;

                mm = (p[0] >> 4) * 10 + (p[0] & 0xF);
                ss = (p[1] >> 4) * 10 + (p[1] & 0xF);
                tt = (mm * 60 + ss) >> 1;
                D_801694CC = tt / 60;
                D_801694CD = tt % 60;
                D_801694CC = ((D_801694CC / 10) << 4) | (D_801694CC % 10);
                D_801694CD = ((D_801694CD / 10) << 4) | (D_801694CC % 10);
            }
            func_80168C6C(12);
            D_80169474 = 5;
        case 0:
        default:
            ret = 7;
            break;
        }
        break;
    case 5:
        ret = func_80168A98(12);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            {
                u8 *r = SafetySync_Results();

                if (!(r[0] & 1)) {
                    D_80169474 = 1;
                } else if (r[1] & 0x40) {
                    u8 *q = SafetySync_Params();

                    ret = 5;
                    q[0] = D_801694CC;
                    q[1] = D_801694CD;
                    q[2] = 0;
                    func_80168C6C(3);
                    D_80169474 = 7;
                }
            }
            break;
        case 1:
            func_80168C6C(13);
            D_80169474 = 6;
        case 0:
        default:
            ret = 5;
            break;
        }
        break;
    case 6:
        ret = func_80168A98(13);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            SafetyCheck2();
            break;
        case 1:
            {
                u8 *q = SafetySync_Params();

                q[0] = D_801694CC;
                q[1] = D_801694CD;
                q[2] = 0;
            }
            func_80168C6C(3);
            D_80169474 = 7;
        case 0:
        default:
            ret = 6;
            break;
        }
        break;
    case 7:
        ret = func_80168A98(3);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            safechk_params[0] = ret;
            func_80168C6C(5);
            D_80169474 = 8;
        case 0:
        default:
            ret = 7;
            break;
        }
        break;
    case 8:
        ret = func_80168A98(5);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            D_801694C4 = VSync(-1);
            D_80169474 = 9;
        case 0:
        default:
            ret = 8;
            break;
        }
        break;
    case 9:
        t = VSync(-1);
        if (D_801694C4 + 3 < t) {
            func_80168C6C(4);
            D_80169474 = 10;
        }
        ret = 9;
        break;
    case 10:
        ret = func_80168A98(4);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            func_80168C6C(7);
            D_80169474 = 11;
        case 0:
        default:
            ret = 10;
            break;
        }
        break;
    case 11:
        ret = func_80168A98(7);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            func_80168C6C(8);
            D_80169474 = 12;
        case 0:
        default:
            ret = 11;
            break;
        }
        break;
    case 12:
        ret = func_80168A98(8);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            safechk_params[0] = 4;
            func_80168C6C(9);
            D_80169474 = 13;
        case 0:
        default:
            ret = 12;
            break;
        }
        break;
    case 13:
        ret = func_80168A98(9);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            D_801694C4 = VSync(-1);
            D_80169474 = 14;
        case 0:
        default:
            ret = 13;
            break;
        }
        break;
    case 14:
        t = VSync(-1);
        if (D_801694C4 + 200 < t) {
            safechk_params[0] = 5;
            func_80168C6C(10);
            D_80169474 = 15;
        }
        ret = 14;
        break;
    case 15:
        ret = func_80168A98(10);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            if (D_8016946D[0] != 0) {
                func_80168C6C(0);
                D_80169474 = 16;
            } else {
                D_801694C4 = VSync(-1);
                D_80169474 = 19;
            }
        case 0:
        default:
            ret = 15;
            break;
        }
        break;
    case 16:
        ret = func_80168A98(0);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            SafetyCheck2();
        case 0:
        default:
            ret = 16;
            break;
        }
        break;
    case 17:
        ret = func_80168A98(0);
        switch (ret) {
        case -1:
            D_80169474 = 1;
            break;
        case 0:
        default:
            ret = 16;
            break;
        case 1:
            D_80169474 = ret;
            ret = 16;
            break;
        case -2:
            func_80168C6C(0);
            ret = 16;
            break;
        }
        break;
    case 18:
        ret = func_80168A98(11);
        switch (ret) {
        case -2:
            func_80168C6C(0);
            D_80169474 = 17;
            ret = -1;
            break;
        case -1:
            D_80169474 = 1;
            break;
        case 1:
            D_80169474 = 0;
        case 0:
        default:
            ret = 18;
            break;
        }
        break;
    case 19:
        t = VSync(-1);
        if (D_801694C4 + 10 < t) {
            func_80168C6C(11);
            D_80169474 = 18;
        }
        ret = 19;
        break;
    case 0:
        ret = 0;
        break;
    }
    if (arg == 0 && ret != 0)
        return SafetySync_Run(arg, ret);
    return ret;
}

s32 SafetySync(s32 arg)
{
    return SafetySync_Run(arg, 0);
}
