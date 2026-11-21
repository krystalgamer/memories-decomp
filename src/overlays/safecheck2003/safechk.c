#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libetc.h"
#include "../../psyq/libapi.h"
#include "../../psyq/strings.h"
#define PSYQ_EXIT_UNPROTOTYPED
#include "../../psyq/exit.h"
#include "safechk.h"

/* Sony's runtime library safechk 2.0.0.3 as one translation unit, like the
 * library's single object: its .rdata (the SafetySync jump table, the two
 * warning texts and SafetyCheck5's offsets), .text, .data and .bss follow
 * each other in the module in that order. Definition order sets both the
 * .data order and, through the string literals in safechk_messages, the .rdata
 * order, so the message table comes after SafetySync. The 2.0.0.2 object
 * exports only SafetyInit and SafetySync, so the data is static. */

/* 'P' 's', library 28 (safechk), version 2.0.0.3. */
static u8 __ps_libinfo__[8] = { 'P', 's', 28, 0, 0, 0, 0x20, 3 };

/* CD-ROM controller commands, by SafetySendCommand index. */
static SafechkCdCommand safechk_commands[14] = {
    { 0x01, 0, 1, 3 }, /* 0 Getstat */
    { 0x13, 0, 3, 3 }, /* 1 GetTN */
    { 0x14, 1, 3, 3 }, /* 2 GetTD track */
    { 0x02, 3, 1, 3 }, /* 3 Setloc mm, ss, sect */
    { 0x16, 0, 1, 5 }, /* 4 SeekP */
    { 0x0E, 1, 1, 3 }, /* 5 Setmode */
    { 0x0A, 0, 1, 5 }, /* 6 Init */
    { 0x0B, 0, 1, 3 }, /* 7 Mute */
    { 0x03, 0, 1, 3 }, /* 8 Play */
    { 0x19, 1, 1, 3 }, /* 9 Test 04h: read SCEx */
    { 0x19, 1, 2, 3 }, /* 10 Test 05h: SCEx counters */
    { 0x09, 0, 1, 5 }, /* 11 Pause */
    { 0x1E, 0, 1, 5 }, /* 12 ReadTOC */
    { 0x1A, 0, 1, 5 }, /* 13 GetID */
};

/* CD-ROM controller registers: status/index, command, parameter, request. */
static volatile u8 *safechk_cdregs[4] = {
    (volatile u8 *)0x1F801800,
    (volatile u8 *)0x1F801801,
    (volatile u8 *)0x1F801802,
    (volatile u8 *)0x1F801803,
};

/* The explicit zero initializers keep these in .data, where the object has
 * them; GCC 2.7 puts only uninitialized statics in .bss. */
static u8 safechk_params[8] = { 0 };
static u8 safechk_results[8] = { 0 };
/* Controller state 0..19; SafetyInit sets the start state and message set. */
static s32 safechk_state = 1;
/* Sum of the interrupt numbers (INT3 = 3, INT2 = 2, ...) answered for the
 * current command; SafetyPollResult waits until it reaches the command's
 * intsum (3 for one INT3 response, 5 for INT3 then INT2). */
static s32 safechk_intsum = 0;

/* .bss, in declaration order. */
/* VSync(-1) count at the start of the state 9, 14 and 19 delays. */
static s32 safechk_vsync;
/* Last track number (BCD) from GetTN. */
static s32 safechk_last_track;
/* BCD minute and second sent with Setloc: half the start time of track 2,
 * or of the lead-out on a one-track disc. */
static u8 safechk_minute;
static u8 safechk_second;
/* Index into safechk_messages: 0 Shift-JIS, 1 English, 2 (no warning) for
 * an 'E' BIOS. */
static s32 safechk_message_set;

void SafetyInit(void)
{
    switch (*(u8 *)0xBFC7FF52) {
    case 'A':
        safechk_state = 1;
        safechk_message_set = 1;
        break;
    case 'E':
        safechk_state = 0;
        safechk_message_set = 2;
        break;
    case 'J':
        safechk_state = 1;
        safechk_message_set = 0;
        break;
    default:
        safechk_state = 1;
        safechk_message_set = 0;
        break;
    }
}

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
    return &safechk_results[1];
}

static inline u8 *SafetySync_Params(void)
{
    return safechk_params;
}

/* Runs controller states and returns the last state's result: one state if
 * MODE is nonzero, otherwise states until one returns 0. The repeat is a tail
 * call, which GCC compiles as a jump back to the switch; RET is a parameter so
 * that its initial 0 is set before that jump target. A do/while or for loop
 * gets loop notes instead: loop.c hoists the -1, -2, 1 and 17 compare
 * constants into saved registers, and flow's loop-depth weighting reorders the
 * registers of the state 4 BCD block. */
static inline s32 SafetySync_Run(s32 mode, s32 ret)
{
    s32 t;

    switch (safechk_state) {
    case 1:
        SafetySendCommand(1);
        safechk_state = 2;
        ret = 1;
        break;
    case 2:
        ret = SafetyPollResult(1);
        switch (ret) {
        case 0:
        default:
            ret = 2;
            break;
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = 2;
            break;
        case 1:
            ret = 2;
            safechk_last_track = safechk_results[2];
            SafetySendCommand(6);
            safechk_state = 3;
            break;
        case -1:
            SafetySendCommand(1);
            ret = 2;
            break;
        }
        break;
    case 3:
        ret = SafetyPollResult(6);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_params[0] = (safechk_last_track >= 2) << 1;
            SafetySendCommand(2);
            safechk_state = 4;
        case 0:
        default:
            ret = 3;
            break;
        }
        break;
    case 4:
        ret = SafetyPollResult(2);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            {
                u8 *p = SafetySync_ResultTime();
                s32 mm, ss, tt;

                mm = (p[0] >> 4) * 10 + (p[0] & 0xF);
                ss = (p[1] >> 4) * 10 + (p[1] & 0xF);
                tt = (mm * 60 + ss) >> 1;
                safechk_minute = tt / 60;
                safechk_second = tt % 60;
                safechk_minute = ((safechk_minute / 10) << 4) | (safechk_minute % 10);
                /* Retail takes the units digit from the BCD minute. */
                safechk_second = ((safechk_second / 10) << 4) | (safechk_minute % 10);
            }
            SafetySendCommand(12);
            safechk_state = 5;
        case 0:
        default:
            ret = 7;
            break;
        }
        break;
    case 5:
        ret = SafetyPollResult(12);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            {
                u8 *r = SafetySync_Results();

                if (!(r[0] & 1)) {
                    safechk_state = 1;
                } else if (r[1] & 0x40) {
                    u8 *q = SafetySync_Params();

                    ret = 5;
                    q[0] = safechk_minute;
                    q[1] = safechk_second;
                    q[2] = 0;
                    SafetySendCommand(3);
                    safechk_state = 7;
                }
            }
            break;
        case 1:
            SafetySendCommand(13);
            safechk_state = 6;
        case 0:
        default:
            ret = 5;
            break;
        }
        break;
    case 6:
        ret = SafetyPollResult(13);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            SafetyCheck2();
            break;
        case 1:
            {
                u8 *q = SafetySync_Params();

                q[0] = safechk_minute;
                q[1] = safechk_second;
                q[2] = 0;
            }
            SafetySendCommand(3);
            safechk_state = 7;
        case 0:
        default:
            ret = 6;
            break;
        }
        break;
    case 7:
        ret = SafetyPollResult(3);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_params[0] = ret;
            SafetySendCommand(5);
            safechk_state = 8;
        case 0:
        default:
            ret = 7;
            break;
        }
        break;
    case 8:
        ret = SafetyPollResult(5);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_vsync = VSync(-1);
            safechk_state = 9;
        case 0:
        default:
            ret = 8;
            break;
        }
        break;
    case 9:
        t = VSync(-1);
        if (safechk_vsync + 3 < t) {
            SafetySendCommand(4);
            safechk_state = 10;
        }
        ret = 9;
        break;
    case 10:
        ret = SafetyPollResult(4);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            SafetySendCommand(7);
            safechk_state = 11;
        case 0:
        default:
            ret = 10;
            break;
        }
        break;
    case 11:
        ret = SafetyPollResult(7);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            SafetySendCommand(8);
            safechk_state = 12;
        case 0:
        default:
            ret = 11;
            break;
        }
        break;
    case 12:
        ret = SafetyPollResult(8);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_params[0] = 4;
            SafetySendCommand(9);
            safechk_state = 13;
        case 0:
        default:
            ret = 12;
            break;
        }
        break;
    case 13:
        ret = SafetyPollResult(9);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_vsync = VSync(-1);
            safechk_state = 14;
        case 0:
        default:
            ret = 13;
            break;
        }
        break;
    case 14:
        t = VSync(-1);
        if (safechk_vsync + 200 < t) {
            safechk_params[0] = 5;
            SafetySendCommand(10);
            safechk_state = 15;
        }
        ret = 14;
        break;
    case 15:
        ret = SafetyPollResult(10);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            if (safechk_results[1] != 0) {
                SafetySendCommand(0);
                safechk_state = 16;
            } else {
                safechk_vsync = VSync(-1);
                safechk_state = 19;
            }
        case 0:
        default:
            ret = 15;
            break;
        }
        break;
    case 16:
        ret = SafetyPollResult(0);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
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
        ret = SafetyPollResult(0);
        switch (ret) {
        case -1:
            safechk_state = 1;
            break;
        case 0:
        default:
            ret = 16;
            break;
        case 1:
            safechk_state = ret;
            ret = 16;
            break;
        case -2:
            SafetySendCommand(0);
            ret = 16;
            break;
        }
        break;
    case 18:
        ret = SafetyPollResult(11);
        switch (ret) {
        case -2:
            SafetySendCommand(0);
            safechk_state = 17;
            ret = -1;
            break;
        case -1:
            safechk_state = 1;
            break;
        case 1:
            safechk_state = 0;
        case 0:
        default:
            ret = 18;
            break;
        }
        break;
    case 19:
        t = VSync(-1);
        if (safechk_vsync + 10 < t) {
            SafetySendCommand(11);
            safechk_state = 18;
        }
        ret = 19;
        break;
    case 0:
        ret = 0;
        break;
    }
    if (mode == 0 && ret != 0)
        return SafetySync_Run(mode, ret);
    return ret;
}

s32 SafetySync(s32 mode)
{
    return SafetySync_Run(mode, 0);
}

/* Polls the controller for command N's responses: 0 while none has arrived
 * or their interrupt numbers sum to less than the command's intsum, 1 when
 * done (results in safechk_results), -1 on an INT5 error and -2 when the
 * status reports the shell open. Command 10's results carry no status byte,
 * so it skips that test. */
s32 SafetyPollResult(s32 n)
{
    s32 intsum;
    s32 stat;
    s32 i;
    s32 t;
    u8 c;

    intsum = safechk_commands[n].intsum;
    *safechk_cdregs[0] = 1;
    /* reorg predicts a jump out of a loop as not taken, so the return 0
     * branch fills its delay slot from the fall-through, as retail does. */
    do {
        t = *safechk_cdregs[3] & 7;
        if (t != (*safechk_cdregs[3] & 7)) {
            return 0;
        }
    } while (0);
    stat = t;
    c = stat;
    if (c != 0) {
        safechk_intsum += c;
        *safechk_cdregs[0] = 1;
        *safechk_cdregs[3] = 7;
        for (i = 0; i < 4; i++) {
            *(volatile s32 *)0 = i;
        }
        if (safechk_intsum < intsum) {
            return 0;
        }
        safechk_intsum = 0;
        if (stat == 5) {
            volatile u8 *src = safechk_cdregs[1];

            safechk_results[0] = *src;
            safechk_results[1] = *src;
            *safechk_cdregs[0] = 1;
            *safechk_cdregs[2] = 0x1F;
            if (safechk_results[0] & 0x10) {
                return -2;
            }
            return -1;
        }
        for (t = 0; t < safechk_commands[n].nresults; t++) {
            safechk_results[t] = *safechk_cdregs[1];
        }
        *safechk_cdregs[0] = 1;
        *safechk_cdregs[2] = 0x1F;
        if (n == 10) {
            return 1;
        }
        if (safechk_results[0] & 0x10) {
            return -2;
        }
        return 1;
    }
    return 0;
}

/* Acknowledges pending interrupts, masks them, and sends command N with its
 * parameters from safechk_params. */
void SafetySendCommand(s32 n)
{
    s32 i;
    s32 k;

    *safechk_cdregs[0] = 1;
    *safechk_cdregs[3] = 7;
    for (i = 0; i < 4; i++) {
        *(volatile s32 *)0 = i;
    }
    k = n * 4;
    *safechk_cdregs[0] = 1;
    *safechk_cdregs[2] = 0x18;
    *safechk_cdregs[0] = 0;
    i = 0;
    if ((&safechk_commands[0].nparams)[k] != 0) {
        do {
            *safechk_cdregs[2] = safechk_params[i];
            i++;
        } while (i < (&safechk_commands[0].nparams)[k]);
    }
    *safechk_cdregs[0] = 0;
    *safechk_cdregs[1] = safechk_commands[n].cmd;
}

/* Warning text and layout per message set: 0 Shift-JIS, drawn from the
 * BIOS kanji ROM ("forced termination; the console may have been
 * modified"), and 1 English. SafetyInit selects set 2 only for an 'E' BIOS,
 * whose check starts in the finished state 0 and never draws. */
static SafechkMessage safechk_messages[2] = {
    { 2, 10, 16, 3, 80, 92,
      "\213\255\220\247\217I\227\271\202\265\202\334\202\265\202\275\201B\n"
      "\226{\221\314\202\252\211\374\221\242\202\263\202\352\202\304\202\242\202\351\n"
      "\202\250\202\273\202\352\202\252\202\240\202\350\202\334\202\267\201B" },
    { 1, 31, 8, 3, 36, 92,
      "     SOFTWARE TERMINATED\n"
      "CONSOLE MAY HAVE BEEN MODIFIED\n"
      "     CALL 1-888-780-7690" },
};

/* Frame outline offsets; the .data padding after them rounds the section to
 * sixteen bytes. */
static u8 safechk_coords[17][2] = {
    { 0, 0 }, { 6, 5 }, { 12, 10 }, { 18, 15 }, { 24, 20 }, { 30, 25 }, { 35, 30 }, { 40, 34 },
    { 45, 38 }, { 49, 41 }, { 53, 44 }, { 56, 47 }, { 59, 49 }, { 61, 51 }, { 62, 52 }, { 63, 53 },
    { 64, 54 }
};

void SafetyCheck2(void)
{
    DRAWENV draw;
    DISPENV disp;
    DR_ENV drenv;
    u32 prim[3];
    SafechkRect rect;
    s32 i;
    s32 color;
    s16 x;
    s16 y;

    ResetGraph(1);
    StopCallback();
    ResetGraph(5);
    *(u16 *)0x1F801DAA = 0;
    SetDefDrawEnv(&draw, 0, 0, 320, 240);
    SetDefDispEnv(&disp, 0, 0, 320, 240);
    draw.isbg = 1;
    SetDrawEnv(&drenv, &draw);
    DrawPrim(&drenv);
    PutDispEnv(&disp);
    prim[0] = 0x02000000;
    prim[1] = 0xE6000002;
    prim[2] = 0;
    DrawPrim(prim);
    color = 0xFFFF;
    rect.w = safechk_messages[safechk_message_set].width;
    rect.h = 1;
    for (i = 0; i < 2; i++) {
        x = safechk_messages[safechk_message_set].x;
        y = safechk_messages[safechk_message_set].y;
        rect.x = x + i;
        rect.y = y + i;
        SafetyCheck3(safechk_messages[safechk_message_set].text, &rect, color);
        color = 0x8000;
    }
    SafetyCheck5();
    SetDispMask(1);
    exit();
}

void SafetyCheck3(u8 *str, SafechkRect *rect, s32 color)
{
    s32 len;
    s32 x;
    u8 *start;
    u8 *glyph;
    u32 range;
    s32 hi;
    u8 c;

    len = strlen(str);
    start = str;
    x = rect->x;
    for (; str < start + len; str++) {
        c = *str;
        if (c == '\n') {
            rect->x = x;
            rect->y += 18;
        } else if (safechk_message_set == 0) {
            hi = *str++ << 8;
            glyph = (u8 *)Krom2RawAdd(*str | hi);
            SafetyCheck4(rect, glyph, color, 7);
            rect->x += rect->w + 1;
        } else if (c == ' ') {
            rect->x += 8;
        } else {
            glyph = (u8 *)0xBFC7F8DE + (*str - 0x21) * 15;
            range = SafetyGetGlyphRange(glyph);
            rect->w = range - (range >> 16) + 1;
            SafetyCheck4(rect, glyph, color, range);
            rect->x += rect->w + 2;
        }
    }
}

void SafetyCheck4(SafechkRect *rect, u8 *bits, s32 color, u32 range)
{
    SafechkGlyphPrim prim;
    s32 x;
    s32 y;
    s32 row;
    s32 i;
    s32 b;
    u16 lo;
    u16 hi;
    u16 *p;
    u16 zero;

    lo = range;
    hi = range >> 16;
    x = rect->x;
    y = rect->y;
    prim.tag = (((rect->w + 1) >> 1) + 3) << 24;
    prim.code = 0xA0000000;
    prim.wh = *(u32 *)&rect->w;
    for (row = 0; row < 15; row++) {
        p = prim.data;
        i = 0;
        if (safechk_messages[safechk_message_set].count != 0) {
            do {
                for (b = lo; b >= hi; b--) {
                    zero = *bits >> 8;
                    *p++ = (((*bits >> b) & 1) == 0) ? zero : color;
                }
                i++;
                bits++;
            } while (i < safechk_messages[safechk_message_set].count);
        }
        for (i = 0; i < 2; i++) {
            prim.xy = *(u32 *)rect;
            DrawPrim(&prim);
            rect->x++;
        }
        rect->x = x;
        rect->y++;
    }
    rect->y = y;
}

/* Returns the lowest (high half) and highest (low half) bit set in any row of
 * a 15-row BIOS font glyph; SafetyCheck3 derives the glyph width from it. */
u32 SafetyGetGlyphRange(u8 *p)
{
    u16 max;
    u16 min;
    s32 i;
    s32 j;
    s32 first;

    max = 0;
    min = 7;
    for (i = 14; i >= 0; i--, p++) {
        first = 1;
        for (j = 7; j >= 0; j--) {
            if ((*p >> j) & 1) {
                if (first) {
                    if (max < j) {
                        max = j;
                    }
                    if (j < min) {
                        min = j;
                    }
                    first = 0;
                } else if (j < min) {
                    min = j;
                }
            }
        }
    }
    return (min << 16) | max;
}

void SafetyCheck5(void)
{
    SafechkFramePoly poly;
    s32 i;
    s32 j;
    s32 k;
    s32 idx;
    s32 n;
    s8 *d;

    poly.tag = 0x05000000;
    poly.code = 0x280000FF;
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 4; j++) {
            SafechkFramePoly *pp = &poly;
            s8 dir[8] = { 1, 1, -1, 1, 1, -1, -1, -1 };

            for (k = 0; k < 4; k++) {
                d = &dir[j * 2];
                n = (k + 2) >> 2;
                idx = i + (k & 1);
                pp->xy[k] = ((d[1] * safechk_coords[16 - idx][n] + 120) << 16)
                           | (d[0] * safechk_coords[idx][n] + 160);
            }
            DrawPrim(&poly);
        }
    }
    poly.xy[0] = 0x00500070;
    poly.xy[1] = 0x00480078;
    poly.xy[2] = 0x00A800C8;
    poly.xy[3] = 0x00A000D0;
    DrawPrim(&poly);
}
