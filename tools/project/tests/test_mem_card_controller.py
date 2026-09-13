"""Legacy card-controller ILP32 witnesses with safe fixed-address buffers.

Opaque request stubs test controller behavior, not card hardware. In particular
the loaded-entry stub explicitly returns its index: it does not validate the
legacy native void implementation's return-register pass-through. The Linux
bootstrap uses MAP_FIXED_NOREPLACE, never a mapping that overwrites host memory.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/func_8003DC1C.c"
START = """
.text
.globl _start
_start:
    mov (%esp), %eax
    lea 4(%esp), %edx
    and $-16, %esp
    sub $8, %esp
    push %edx
    push %eax
    call main
    mov %eax, %ebx
    mov $1, %eax
    int $0x80
.globl map_page
map_page:
    push %ebx
    push %esi
    push %edi
    push %ebp
    mov 20(%esp), %ebx
    mov $192, %eax
    mov $4096, %ecx
    mov $3, %edx
    mov $0x100022, %esi
    mov $-1, %edi
    xor %ebp, %ebp
    int $0x80
    pop %ebp
    pop %edi
    pop %esi
    pop %ebx
    ret
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
void func_8003DC1C(void);
u32 map_page(u32 address);
#define U16(p, n) (*(u16 *)((u8 *)(p) + (n)))
#define U32(p, n) (*(u32 *)((u8 *)(p) + (n)))
#define CHECK(c, n) do { if (!(c)) return (n); } while (0)

MemCardWorkArea D_800EF6D0;
u16 D_8009B3CC;
u8 D_8009B3DD, D_8009B3CF, D_8009B3D5, D_8009B3C1, D_8009B3C7;
s32 D_8009B3C8, D_8009B3E4;
void (*D_80090F88[MEM_CARD_WORK_CALLBACK_COUNT])(MemCardWorkRoot *, MemCardWorkSlot *);
char D_8009AF6C[] = "entry";
char D_80010378[] = "free";
char D_80010398[] = "make";
char D_800103A4[] = "fail";
s32 gMemCard_nDirEntries, gMemCard_nFreeBlocks;
static struct DIRENTRY directory[3];
struct DIRENTRY *gMemCard_pDirEntries = directory;
u8 gMemCard_szSaveFileName[24];
u8 gSaveData_aHeaderTemplate[512];
DuelEffectChannel D_800EB0F8[DUEL_EFFECT_CHANNEL_COUNT];
s8 gDialog_bChoice;

static MemCardWorkRoot *root;
static MemCardWorkSlot *slot;
static u8 transfer[0x20000];
static s32 error, count, sync_status, find_result, mutation, executions;
typedef struct { s32 kind, a, b, c, d, e; } Event;
static Event events[40];

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}
static void event(s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e)
{
    if (count >= 40) { error = 1; return; }
    events[count].kind = kind; events[count].a = a; events[count].b = b;
    events[count].c = c; events[count].d = d; events[count].e = e;
    count++;
}
int FntPrint(char *format, s32 value)
{
    if (format == D_8009AF6C)
        event(1, (s32)((struct DIRENTRY *)value - directory), value, 0, 0, 0);
    else if (format == D_80010378) event(2, value, 0, 0, 0, 0);
    else error = 2;
    return 0;
}
int printf(const char *format, ...)
{
    if (format == D_80010398) event(3, 0, 0, 0, 0, 0);
    else if (format == D_800103A4) event(4, 0, 0, 0, 0, 0);
    else error = 3;
    return 0;
}
s32 func_80044838(s32 mode, s32 *request, s32 *result)
{
    event(5, mode, 0, 0, 0, 0);
    if (request != &D_8009B3E4 || result != &D_8009B3C8) error = 4;
    if (mutation == 1) { *request = 8; *result = 0; }
    return sync_status;
}
static void stage(MemCardWorkRoot *r, MemCardWorkSlot *s)
{
    event(6, D_8009B3C1 & 15, 0, 0, 0, 0);
    if (r != root || s != slot) error = 5;
    D_8009B3C1 = 0;
    D_8009B3CF = 0;
}
void func_80039794(void)
{
    event(7, 0, 0, 0, 0, 0);
    if (mutation == 2) {
        U32(&D_800EB0F8[root->text_index], 0x34) = 0x2000;
        root->text_index ^= 1;
    }
}
DuelEffectChannel *DuelEffect_CreateChannel(s32 message, s32 wait)
{
    event(8, message, wait, 0, 0, 0);
    return &D_800EB0F8[0];
}
void MemCard_Init(long value) { event(9, value, 0, 0, 0, 0); }
void MemCard_InitIOEvents(void) { event(10, 0, 0, 0, 0, 0); }
s32 MemCard_ReqLoadDirectory(s32 channel) { event(11, channel, 0, 0, 0, 0); return 1; }
s32 MemCard_FindLoadedEntry(u8 *name)
{
    event(12, name == gMemCard_szSaveFileName, 0, 0, 0, 0);
    if (mutation == 3) D_8009B3CC |= 2;
    return find_result;
}
s32 MemCard_ReqReadSector(s32 channel, s32 buffer, s32 sector)
{
    event(13, channel, buffer, sector, 0, 0); return 1;
}
s32 MemCard_ReqWriteSector(s32 channel, s32 buffer, s32 sector)
{
    event(14, channel, buffer, sector, D_8021007F, 0); return 1;
}
s32 MemCard_ReqReadFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size)
{
    event(15, channel, name == (s32)gMemCard_szSaveFileName, buffer, offset, size);
    return 1;
}
s32 MemCard_ReqWriteFile(s32 channel, s32 name, s32 buffer, s32 offset, s32 size)
{
    event(16, channel, name == (s32)gMemCard_szSaveFileName, buffer, offset, size);
    return 1;
}
s32 MemCard_ReqCreateFile(s32 channel, s32 name, s32 blocks)
{
    event(17, channel, name == (s32)gMemCard_szSaveFileName, blocks, 0, 0);
    return 1;
}
void Util_CopyWords(u8 *destination, u8 *source, u32 length)
{
    u32 i;
    event(18, (s32)destination, source == gSaveData_aHeaderTemplate, length, 0, 0);
    if (destination != (u8 *)0x80300000 || source != gSaveData_aHeaderTemplate ||
        length != 512) { error = 6; return; }
    for (i = 0; i < length; i++) destination[i] = source[i];
}
void Util_FillMemory(u8 *destination, s32 value, u32 length)
{
    u32 i;
    event(19, (s32)destination, value, length, 0, 0);
    if (destination != (u8 *)0x80300200 || value != 0x86 ||
        length != 2048) { error = 7; return; }
    for (i = 0; i < length; i++) destination[i] = value;
}

static void setup(s32 mode, s32 selected)
{
    s32 i;
    clear(&D_800EF6D0, sizeof(D_800EF6D0));
    clear(events, sizeof(events)); clear(directory, sizeof(directory));
    clear(D_800EB0F8, sizeof(D_800EB0F8));
    root = &D_800EF6D0.root;
    slot = &D_800EF6D0.slots[selected];
    D_8009B3DD = selected; D_8009B3CF = mode;
    D_8009B3CC = 0; D_8009B3D5 = 7; D_8009B3C1 = 0; D_8009B3C7 = 0;
    D_8009B3E4 = 0; D_8009B3C8 = 0;
    gDialog_bChoice = 0;
    gMemCard_nDirEntries = 0; gMemCard_nFreeBlocks = 0x123;
    gMemCard_pDirEntries = directory;
    root->blocks = 2; root->remaining = 1000; root->chunk = 400;
    root->offset = 65530; root->text_index = 1;
    slot->cursor = transfer + 0x10000; slot->free_blocks = 0xFE; slot->entry_index = 0xCC;
    slot->pad_0E[0] = 0xA5;
    for (i = 0; i < MEM_CARD_WORK_CALLBACK_COUNT; i++) D_80090F88[i] = stage;
    for (i = 0; i < 128; i++) ((u8 *)0x80210000)[i] = i ^ 0x5A;
    ((u8 *)0x80210000)[127] = 0xD3;
    ((u8 *)0x80210000)[128] = 0xA6;
    for (i = 0; i < 512; i++) gSaveData_aHeaderTemplate[i] = i ^ 0x73;
    clear((u8 *)0x80300000, 4096);
    ((u8 *)0x80300000)[0xA00] = 0xC7;
    directory[1].head = 129;
    count = error = mutation = 0; sync_status = -1; find_result = 1;
}
static void run(void) { executions++; func_8003DC1C(); }
static s32 valid(void)
{
    return !error && slot->pad_0E[0] == 0xA5 &&
           ((u8 *)0x80210000)[128] == 0xA6 &&
           ((u8 *)0x80300000)[0xA00] == 0xC7;
}
static s32 message(s32 id)
{
    return count && events[count - 1].kind == 8 &&
           events[count - 1].a == id && events[count - 1].b == 1 &&
           (D_8009B3CC & 0x400);
}

static s32 directory_debug(void)
{
    static const s32 sizes[] = {-1,0,1,3};
    s32 selected, i, n;
    for (selected = 0; selected < 2; selected++) for (i = 0; i < 4; i++) {
        setup(14, selected); D_8009B3CC = 0x2000; gMemCard_nDirEntries = sizes[i];
        run(); n = sizes[i] < 0 ? 0 : sizes[i];
        CHECK(valid() && count == n + 1 && events[n].kind == 2 && events[n].a == 254, 10);
        while (n--) CHECK(events[n].kind == 1 && events[n].a == n &&
                          events[n].b == (s32)((u8 *)directory + 40 * n), 11);
    }
    return 0;
}

static s32 sync_gates(void)
{
    static const u16 flags[] = {0,0x1000,0x4000,0x5000};
    static const s32 statuses[] = {-1,0,1,2};
    s32 f, s;
    for (f = 0; f < 4; f++) for (s = 0; s < 4; s++) {
        setup(14, 0); D_8009B3CC = flags[f]; sync_status = statuses[s];
        D_8009B3E4 = 8; run();
        CHECK(valid() && count == (f >= 2) &&
              D_8009B3D5 == (f == 3 && s == 2 ? 0 : 7), 20);
        if (count) CHECK(events[0].kind == 5 && events[0].a == 1, 21);
    }
    return 0;
}

static s32 directory_completion(void)
{
    static const s32 results[] = {0,3,4,1,-1,9};
    static const u16 flags[] = {0x7000,0x7800,0x7002,0x7000,0x7000,0x7000};
    static const u8 outcome[] = {0,0,1,2,2,2};
    s32 i, selected;
    for (selected = 0; selected < 2; selected++) for (i = 0; i < 6; i++) {
        setup(14, selected); D_8009B3CC = 0x5000; sync_status = 1;
        D_8009B3E4 = 2; D_8009B3C8 = results[i]; run();
        CHECK(valid() && count == 1 && D_8009B3CC == flags[i] &&
              D_8009B3D5 == outcome[i] && slot->free_blocks == (i < 2 ? 0x23 : 0xFE), 30);
    }
    return 0;
}

static s32 transfer_completion(void)
{
    static const u16 remaining[] = {1000,100,100,0,0x8000,0xFFFF,1,500,1};
    static const s16 chunks[] = {400,100,200,1,1,-1,32767,300,-1};
    static const s16 next[] = {600,0,-100,-1,32767,0,-32766,200,2};
    static const s16 final_chunks[] = {400,100,200,1,1,-1,32767,200,-1};
    static const u16 offsets[] = {394,65530,65530,65530,65531,65530,65530,294,65529};
    s32 request, i, fail;
    for (request = 3; request <= 4; request++)
    for (i = 0; i < 9; i++) for (fail = 0; fail < 2; fail++) {
        setup(14, 1); D_8009B3CC = 0x5000; sync_status = 1;
        D_8009B3E4 = request; D_8009B3C8 = fail;
        root->remaining = remaining[i]; root->chunk = chunks[i];
        run();
        CHECK(valid() && count == 1 && root->remaining == (fail ? (s16)remaining[i] : next[i]), 40);
        CHECK(root->chunk == (fail ? chunks[i] : final_chunks[i]) &&
              root->offset == (fail ? 65530 : offsets[i]), 41);
        CHECK(slot->cursor == transfer + 0x10000 +
              (!fail && next[i] > 0 ? chunks[i] : 0) &&
              D_8009B3D5 == (fail ? 1 : next[i] > 0 ? 7 : 0), 42);
    }
    return 0;
}

static s32 simple_completion(void)
{
    static const s32 requests[] = {0,1,5,6,7,8,9,10,11,12,13};
    static const s32 results[] = {0,1,-1};
    s32 r, v, handled;
    for (r = 0; r < 11; r++) for (v = 0; v < 3; v++) {
        setup(14, 0); D_8009B3CC = 0x5000; sync_status = 1;
        D_8009B3E4 = requests[r]; D_8009B3C8 = results[v]; run();
        handled = r == 5 || r == 8 || r == 9;
        CHECK(valid() && count == 1 && D_8009B3D5 == (handled ? v != 0 : 7), 50);
    }
    return 0;
}

static s32 stage_and_dialog(void)
{
    static const u32 flags[] = {0,0x2000,0x2008,8,0xFFFFFFFF,0x12000};
    s32 selected, index, i;
    for (selected = 0; selected < 2; selected++)
    for (index = 1; index <= 4; index += 3) {
        setup(0, selected); D_8009B3C1 = 0x80 | index;
        run();
        CHECK(valid() && count == 1 && events[0].kind == 6 && events[0].a == index &&
              !D_8009B3C1 && !D_8009B3CF, 60);
    }
    for (index = 0; index < 2; index++) for (i = 0; i < 6; i++) {
        setup(0, 0); root->text_index = index; D_8009B3CC = 0x404;
        U32(&D_800EB0F8[index], 0x34) = flags[i]; run();
        CHECK(valid() && count == 1 && events[0].kind == 7 && !D_8009B3CF &&
              D_8009B3CC == (i == 1 || i == 5 ? 4 : 0x404), 61);
    }
    return 0;
}

static s32 startup_states(void)
{
    s32 choice, option, outcome;
    setup(0, 0); run();
    CHECK(valid() && !count && D_8009B3CF == 0x80 && D_8009B3C1 == 1, 70);
    for (choice = 0; choice < 2; choice++) {
        setup(0x80, 0); gDialog_bChoice = choice ? -1 : 0; run();
        if (choice) CHECK(valid() && !count && D_8009B3C1 == 2 && D_8009B3CF == 0x80, 71);
        else CHECK(valid() && count == 3 && events[0].kind == 9 && events[0].a == 1 &&
                   events[1].kind == 10 && events[2].kind == 11 && !events[2].a &&
                   D_8009B3CF == 0x81 && D_8009B3CC == 0x5000, 72);
    }
    setup(1, 0); run();
    CHECK(valid() && count == 1 && events[0].kind == 11 && D_8009B3CF == 0x81 &&
          D_8009B3CC == 0x1000, 73);
    setup(0x81, 0); run();
    CHECK(valid() && !count && D_8009B3CF == 4, 74);
    for (option = 0; option < 2; option++) for (outcome = 0; outcome < 3; outcome++) {
        setup(4, 0); D_8009B3CC = 0x3000; D_8009B3C7 = option;
        D_8009B3D5 = outcome; run();
        CHECK(valid() && !(D_8009B3CC & 0x1000), 75);
        if (outcome == 2) CHECK(message(0xD2) && D_8009B3CF == 4, 76);
        else CHECK(D_8009B3CF == (option ? 7 : 5) && count == 1, 77);
    }
    return 0;
}

static s32 sector_read_and_write(void)
{
    static const s32 heads[] = {-2147483647-1,-129,-128,-127,-65,-64,-63,-1,0,1,63,64,65,2147483647};
    static const s32 sectors[] = {-33554432,-2,-2,-1,-1,-1,0,0,0,0,0,1,1,33554431};
    s32 i, write;
    for (write = 0; write < 2; write++) for (i = 0; i < 14; i++) {
        setup(write ? 8 : 5, 0); directory[1].head = heads[i]; run();
        CHECK(valid() && count == 2 && events[0].kind == 12 && events[0].a == 1 &&
              events[1].kind == (write ? 14 : 13) && !events[1].a &&
              (u32)events[1].b == 0x80210000 && events[1].c == sectors[i], 80);
        CHECK(D_8009B3CF == (write ? 0x88 : 0x85), 81);
        if (write) CHECK(D_8021007A == 255 && D_8021007B == 255 &&
                          D_8021007C == 255 && D_8021007D == 255 &&
                          D_8021007E == 1 && D_8021007F == 0 &&
                          ((u8 *)0x80210000)[127] == 0, 82);
    }
    return 0;
}

static s32 file_reads(void)
{
    s32 mode, missing, flag, outcome;
    for (mode = 5; mode <= 6; mode++)
    for (missing = 0; missing < 2; missing++) for (flag = 0; flag < 2; flag++) {
        setup(mode, 1); find_result = missing ? -1 : 1; D_8009B3CC = flag ? 2 : 0;
        run(); CHECK(valid(), 90);
        if (missing || flag) CHECK(message(0xD3) && count == 2, 91);
        else if (mode == 6) CHECK(count == 3 && events[1].kind == 8 &&
             events[1].a == 0x80D5 && !events[1].b && events[2].kind == 15 &&
             !events[2].a && events[2].b == 1 && (u32)events[2].c == 0x80200000 &&
             events[2].d == 512 && events[2].e == 7680 &&
             slot->entry_index == 1 && D_8009B3CC == 0x1000, 92);
    }
    setup(6, 1); find_result = 257; run();
    CHECK(valid() && slot->entry_index == 1, 93);
    for (mode = 5; mode <= 6; mode++) for (outcome = 0; outcome < 3; outcome++) {
        setup(mode | 0x80, 0); D_8009B3D5 = outcome; run();
        CHECK(valid(), 94);
        if (mode == 5 && !outcome) CHECK(D_8009B3CF == 0x86 && count == 3, 95);
        else CHECK(message(mode == 5 || outcome == 1 ? 0xD8 : 0xD7), 96);
    }
    return 0;
}

static s32 write_file_and_fallthrough(void)
{
    s32 flag, missing, outcome, i;
    for (flag = 0; flag < 2; flag++) for (missing = 0; missing < 2; missing++) {
        setup(7, 0); D_8009B3CC = flag ? 2 : 0; find_result = missing ? -1 : 1;
        run(); CHECK(valid(), 100);
        if (flag) CHECK(D_8009B3CF == 9 && !count, 101);
        else if (missing) CHECK(D_8009B3CF == 10 && count == 1, 102);
        else {
            CHECK(count == 5 && events[0].kind == 12 && events[1].kind == 8 &&
                  events[1].a == 0x80D6 && !events[1].b &&
                  events[2].kind == 18 && events[3].kind == 19 &&
                  events[4].kind == 16 && !events[4].a && events[4].b == 1 &&
                  (u32)events[4].c == 0x80300000 && events[4].d == 0 && events[4].e == 2560 &&
                  D_8009B3CC == 0x1000 && D_8009B3CF == 0x87, 103);
            for (i = 0; i < 512; i++) CHECK(((u8 *)0x80300000)[i] == gSaveData_aHeaderTemplate[i], 104);
            for (i = 512; i < 2560; i++) CHECK(((u8 *)0x80300000)[i] == 0x86, 105);
        }
    }
    for (outcome = 0; outcome < 3; outcome++) {
        setup(0x87, 0); D_8009B3D5 = outcome; run();
        CHECK(valid() && count == 3 && events[0].kind == 8 &&
              events[0].a == (outcome == 1 ? 0xD9 : 0xD7) && events[0].b == 1 &&
              events[1].kind == 12 && events[2].kind == 14 &&
              D_8009B3CF == 0x88 && D_8009B3CC == 0x400 && D_8021007F == 0, 106);
        setup(0x88, 0); D_8009B3D5 = outcome; run();
        CHECK(valid() && (outcome ? message(0xD9) : count == 0), 107);
    }
    return 0;
}

static s32 create_and_diagnostics(void)
{
    static const u8 free_blocks[] = {0,1,2,255};
    s32 mode, f, outcome;
    for (mode = 9; mode <= 10; mode++) for (f = 0; f < 4; f++) {
        setup(mode, 1); slot->free_blocks = free_blocks[f]; run();
        CHECK(valid() && D_8009B3CF == (mode | 0x80) && count == (f >= 2), 110);
        if (count) CHECK(events[0].kind == 17 && !events[0].a &&
                         events[0].b == 1 && events[0].c == 2, 111);
    }
    for (mode = 9; mode <= 10; mode++) for (outcome = 0; outcome < 3; outcome++) {
        setup(mode | 0x80, 0); D_8009B3D5 = outcome; run();
        CHECK(valid() && count == (outcome ? 1 : 2) && events[count - 1].kind == 4, 112);
        if (!outcome) CHECK(events[0].kind == 3, 113);
    }
    return 0;
}

static s32 opaque_and_unknown(void)
{
    static const u8 modes[] = {2,3,11,12,13,14,15,0xFF};
    s32 i;
    setup(14, 0); D_8009B3CC = 0x5000; sync_status = 1; mutation = 1; run();
    CHECK(valid() && D_8009B3D5 == 0 && count == 1, 120);
    setup(0, 0); D_8009B3CC = 0x404; mutation = 2; run();
    CHECK(valid() && D_8009B3CC == 4 && root->text_index == 0 && !D_8009B3CF, 121);
    setup(5, 0); mutation = 3; run();
    CHECK(valid() && message(0xD3), 122);
    for (i = 0; i < 8; i++) {
        setup(modes[i], 0); run();
        CHECK(valid() && count == 0 && D_8009B3CF == modes[i], 123);
    }
    return 0;
}

int main(int argc, char **argv)
{
    static const s32 cases[] = {8,16,12,36,33,16,11,28,15,10,14,11};
    s32 scenario, result;
    if (sizeof(void *) != 4 || sizeof(long) != 4 || sizeof(struct DIRENTRY) != 40 ||
        sizeof(MemCardWorkRoot) != 0x1C || sizeof(MemCardWorkSlot) != 0x268 ||
        sizeof(MemCardWorkArea) != 0x4EC || argc != 2) return 130;
    if (map_page(0x80210000) != 0x80210000 || map_page(0x80300000) != 0x80300000) return 131;
    if ((u32)&D_8021007A != 0x8021007A || (u32)&D_8021007F != 0x8021007F) return 132;
    scenario = argv[1][0] - '0';
    if (argv[1][1]) scenario = scenario * 10 + argv[1][1] - '0';
    switch (scenario) {
    case 0: result = directory_debug(); break;
    case 1: result = sync_gates(); break;
    case 2: result = directory_completion(); break;
    case 3: result = transfer_completion(); break;
    case 4: result = simple_completion(); break;
    case 5: result = stage_and_dialog(); break;
    case 6: result = startup_states(); break;
    case 7: result = sector_read_and_write(); break;
    case 8: result = file_reads(); break;
    case 9: result = write_file_and_fallthrough(); break;
    case 10: result = create_and_diagnostics(); break;
    case 11: result = opaque_and_unknown(); break;
    default: return 133;
    }
    if (result) return result;
    return executions == cases[scenario] ? 0 : 134;
}
"""


def normalized(text: str, directory: Path) -> str:
    return re.sub(
        r'^#include "([^"]+)"',
        lambda match: '#include "' + str((directory / match[1]).resolve()) + '"',
        text, flags=re.MULTILINE,
    )


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class MemCardControllerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="mem-card-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        cls.directory = Path(temporary.name)
        cls.witness = cls.directory / "witness.c"
        prefix = SOURCE.read_text().split("void func_8003DC1C(void)", 1)[0]
        cls.witness.write_text(normalized(prefix, SOURCE.parent) + WITNESS)
        cls.startup = cls.directory / "start.S"
        cls.startup.write_text(START)
        cls.binaries = {
            optimization: cls.compile(SOURCE, optimization, "actual")
            for optimization in ("-O0", "-O2")
        }

    @classmethod
    def compile(cls, source: Path, optimization: str, label: str) -> Path:
        binary = cls.directory / (label + optimization)
        aliases = ["D_80210000=0x80210000"] + [
            f"D_802100{offset:X}=0x802100{offset:X}" for offset in range(0x7A, 0x80)
        ]
        result = subprocess.run(
            ["cc", "-m32", optimization, "-std=gnu99", "-D_LANGUAGE_C", "-ffreestanding",
             "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
             "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
             *[f"-Wl,--defsym,{alias}" for alias in aliases],
             str(source), str(cls.witness), str(cls.startup), "-o", str(binary)],
            cwd=REPOSITORY, env={**os.environ, "TMPDIR": str(cls.directory)},
            capture_output=True, text=True, timeout=60,
        )
        if result.returncode:
            raise AssertionError(result.stderr)
        return binary

    def witness_scenario(self, scenario: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(scenario)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_directory_debug_stride_count_and_free_byte(self):
        self.witness_scenario(0)

    def test_sync_status_and_completion_gates(self):
        self.witness_scenario(1)

    def test_directory_completion_result_flags_and_outcomes(self):
        self.witness_scenario(2)

    def test_transfer_cursor_offset_chunk_and_signed_wrap(self):
        self.witness_scenario(3)

    def test_simple_and_unknown_request_completion(self):
        self.witness_scenario(4)

    def test_stage_callback_blocks_and_dialog_completion_mask(self):
        self.witness_scenario(5)

    def test_startup_choice_io_and_directory_state_transitions(self):
        self.witness_scenario(6)

    def test_sector_signed_rounding_fixed_aliases_and_checksum(self):
        self.witness_scenario(7)

    def test_lookup_errors_file_read_arguments_and_completion(self):
        self.witness_scenario(8)

    def test_header_fill_write_and_state7_dialog_fallthrough(self):
        self.witness_scenario(9)

    def test_create_capacity_and_success_then_failure_diagnostics(self):
        self.witness_scenario(10)

    def test_opaque_reloads_and_unknown_modes(self):
        self.witness_scenario(11)

    def mutant(self, old: str, new: str, scenario: int, expected: range):
        text = SOURCE.read_text()
        self.assertEqual(text.count(old), 1, "semantic mutation must identify one operation")
        source = self.directory / f"mutant-{scenario}.c"
        source.write_text(normalized(text.replace(old, new), SOURCE.parent))
        binary = self.compile(source, "-O2", f"mutant-{scenario}")
        result = subprocess.run(
            [str(binary), str(scenario)], cwd=REPOSITORY,
            capture_output=True, text=True, timeout=10,
        )
        self.assertIn(result.returncode, expected, result.stderr)

    def test_wrong_checksum_extent_is_detected(self):
        self.mutant("i < 0x7F", "i < 0x80", 7, range(80, 83))

    def test_wrong_negative_sector_rounding_is_detected(self):
        self.mutant("sector += 63", "sector += 62", 7, range(80, 83))

    def test_stage_status_blocking_is_detected(self):
        self.mutant("if (status >= 0)", "if (status > 1)", 5, range(60, 62))


if __name__ == "__main__":
    unittest.main()
