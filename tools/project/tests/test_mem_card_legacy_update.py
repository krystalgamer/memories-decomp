"""Legacy card protocol with distinct directory/buffer ownership, no real I/O.

Callback doubles check the measured two-argument caller view, not the resident
handlers' implementation or hardware timing.
"""

from pathlib import Path
import os
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/mem_card_legacy_update.c"
LOOKUP = ROOT / "src/game/mem_card_find_loaded_entry.c"
START = r"""
.text
.globl _start
_start:
    movl $192, %eax
    movl $0x801ff000, %ebx
    movl $0x104000, %ecx
    movl $3, %edx
    movl $0x32, %esi
    movl $-1, %edi
    xorl %ebp, %ebp
    int $0x80
    cmpl $0x801ff000, %eax
    jne map_failed
    andl $-16, %esp
    call main
    movl %eax, %ebx
    jmp exit_process
map_failed:
    movl $90, %ebx
exit_process:
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

# Numeric offsets and event transitions are the retail instruction contract,
# not an inclusion or translation of the game source under test.
FIXTURE = r"""
#include "src/types.h"
#include "src/ygo_types.h"
#include "src/game/mem_card_directory.h"
#include "src/game/mem_card_begin_request.h"
#include "src/game/io_event_helpers.h"
#include "src/game/duel_effect_create_channel.h"
#define MEM_CARD_LEGACY_CALL_WITH_SLOT
#include "src/game/mem_card_legacy_update.h"

typedef char target_width[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
typedef char directory_size[sizeof(struct DIRENTRY) == 40 ? 1 : -1];
typedef char channel_size[sizeof(DuelEffectChannel) == 100 ? 1 : -1];
enum {
    ROOT_SIZE = 0x4F0 + 64, GLOBAL_SIZE = 96, DIR_SIZE = 1280 + 64,
    CHANNEL_SIZE = 400 + 64, HEADER_SIZE = 512 + 64,
    FLAGS = 32, SELECTOR = 34, EVENT = 36, IO_RESULT = 40, UI = 44,
    STAGE = 45, OUTCOME = 46, MODE = 47, CHOICE = 48,
    DIRECTORY = 52, COUNT = 56, FREE = 60,
    FONT_ENTRY = 1, FONT_FREE, POLL, UI_CALL, TEXT_STEP, INIT, INIT_EVENTS,
    DIRECTORY_LOAD, FIND, READ_SECTOR, READ_FILE, MESSAGE, COPY, FILL,
    WRITE_FILE, WRITE_SECTOR, CREATE_FILE, PRINT_MAKE, PRINT_FAIL, KIND_COUNT,
    REGION_ROOT, REGION_GLOBAL, REGION_DIR, REGION_CHANNEL, REGION_MAP,
    MAP_SIZE = 0x104000, MISMATCH = 77
};
#define MAP ((u8 *)0x801FF000)
#define READ_BUFFER ((u8 *)0x80200000)
#define SECTOR_BUFFER ((u8 *)0x80210000)
#define WRITE_BUFFER ((u8 *)0x80300000)
u8 root_memory[ROOT_SIZE] __attribute__((aligned(16)));
u8 global_memory[GLOBAL_SIZE] __attribute__((aligned(16)));
u8 directory_memory[DIR_SIZE] __attribute__((aligned(16)));
u8 channel_memory[CHANNEL_SIZE] __attribute__((aligned(16)));
u8 header_memory[HEADER_SIZE] __attribute__((aligned(16)));
__asm__(
 ".globl D_800EF6D0\n.set D_800EF6D0,root_memory+32\n"
 ".globl D_8009B3CC\n.set D_8009B3CC,global_memory+32\n"
 ".globl D_8009B3DD\n.set D_8009B3DD,global_memory+34\n"
 ".globl D_8009B3E4\n.set D_8009B3E4,global_memory+36\n"
 ".globl D_8009B3C8\n.set D_8009B3C8,global_memory+40\n"
 ".globl D_8009B3C1\n.set D_8009B3C1,global_memory+44\n"
 ".globl D_8009B3CF\n.set D_8009B3CF,global_memory+45\n"
 ".globl D_8009B3D5\n.set D_8009B3D5,global_memory+46\n"
 ".globl D_8009B3C7\n.set D_8009B3C7,global_memory+47\n"
 ".globl gDialog_bChoice\n.set gDialog_bChoice,global_memory+48\n"
 ".globl gMemCard_pDirEntries\n.set gMemCard_pDirEntries,global_memory+52\n"
 ".globl gMemCard_nDirEntries\n.set gMemCard_nDirEntries,global_memory+56\n"
 ".globl gMemCard_nFreeBlocks\n.set gMemCard_nFreeBlocks,global_memory+60\n"
 ".globl D_800EB0F8\n.set D_800EB0F8,channel_memory+32\n"
 ".globl gSaveData_aHeaderTemplate\n.set gSaveData_aHeaderTemplate,header_memory+32\n");
char D_8009AF6C[] = "%s\n";
char D_80010378[] = "\nFREE %d\n\n";
char D_80010398[] = "Make File\n";
char D_800103A4[] = "Fail File\n";
u8 gMemCard_szSaveFileName[] = "fixture-save";

static void barrier(void) { __asm__ volatile("" : : : "memory"); }
static u32 byte(const void *p) { return *(const volatile u8 *)p; }
static u32 word16(const void *p)
{
    const u8 *s = p;
    return byte(s) | (byte(s + 1) << 8);
}
static u32 word32(const void *p)
{
    const u8 *s = p;
    return word16(s) | (word16(s + 2) << 16);
}
static s32 signed16(u32 x) { x &= 65535; return x < 32768 ? x : (s32)x - 65536; }
static void put8(void *p, u32 x) { *(volatile u8 *)p = x; }
static void put16(void *p, u32 x) { u8 *d=p; put8(d,x); put8(d+1,x>>8); }
static void put32(void *p, u32 x) { u8 *d=p; put16(d,x); put16(d+2,x>>16); }
static void copy_bytes(void *out, const void *in, u32 n)
{
    u8 *d=out; const u8 *s=in;
    while (n--) *d++ = byte(s++);
}
static s32 equal(const void *a, const void *b, u32 n)
{
    const u8 *x=a,*y=b;
    while (n--) if (byte(x++) != byte(y++)) return 0;
    return 1;
}
static void pattern(u8 *p, u32 n, u32 seed)
{
    u32 i;
    for (i=0;i<n;i++) p[i]=(u8)(seed+i*17+(i>>8));
}
static void text(const char *s)
{
    u32 n=0; while(s[n]) n++;
    __asm__ volatile("int $0x80" : : "a"(4),"b"(1),"c"(s),"d"(n) : "memory","cc");
}
static void number(u32 x)
{
    char a[12],b[12]; u32 n=0,i;
    do { a[n++]=(char)('0'+x%10); x/=10; } while(x);
    for(i=0;i<n;i++) b[i]=a[n-i-1];
    b[n++]='\n';
    __asm__ volatile("int $0x80" : : "a"(4),"b"(1),"c"(b),"d"(n) : "memory","cc");
}
static u32 cases, callbacks, scripted, lookups, raw_writes;
static void fail(u32 reason)
{
    text("legacy mismatch: "); number(reason);
    text("case: "); number(cases);
    __asm__ volatile("int $0x80" : : "a"(1),"b"(MISMATCH) : "memory","cc");
    __builtin_unreachable();
}
#define G8(x) byte(global_memory+(x))
#define G16(x) word16(global_memory+(x))
#define G32(x) word32(global_memory+(x))
#define S8(x,v) put8(global_memory+(x),(v))
#define S16(x,v) put16(global_memory+(x),(v))
#define S32(x,v) put32(global_memory+(x),(v))

typedef struct {
    u32 flags, selector, stage, ui, outcome, mode, choice;
    s32 poll, event, io_result, lookup, count, free_blocks, head;
    u32 remaining, chunk, progress, required, free_slot, text_flags;
} Input;
static Input input;
typedef struct { u32 kind, occurrence, region, offset, width, value; } Patch;
static Patch patches[8];
static u32 patch_count;
static void patch(u32 kind,u32 occurrence,u32 region,u32 offset,u32 width,u32 value)
{
    Patch *p=&patches[patch_count++];
    if(patch_count>8) fail(90);
    p->kind=kind; p->occurrence=occurrence; p->region=region;
    p->offset=offset; p->width=width; p->value=value;
}
static void mutate(u32 kind,u32 occurrence)
{
    u32 i;
    for(i=0;i<patch_count;i++) {
        Patch *p=&patches[i]; u8 *base;
        if(p->kind!=kind || p->occurrence!=occurrence) continue;
        switch(p->region) {
        case REGION_ROOT: base=root_memory; break;
        case REGION_GLOBAL: base=global_memory; break;
        case REGION_DIR: base=directory_memory; break;
        case REGION_CHANNEL: base=channel_memory; break;
        case REGION_MAP: base=MAP; break;
        default: fail(91); return;
        }
        if(p->width==1) put8(base+p->offset,p->value);
        else if(p->width==2) put16(base+p->offset,p->value);
        else put32(base+p->offset,p->value);
    }
    barrier();
}
typedef struct {
    u8 root[ROOT_SIZE], globals[GLOBAL_SIZE], dirs[DIR_SIZE];
    u8 channels[CHANNEL_SIZE], header[HEADER_SIZE];
    u8 read[0x2000+64], sector[128+64], write[0xA00+64];
} Snapshot;
static void snapshot(Snapshot *s)
{
    barrier();
    copy_bytes(s->root,root_memory,ROOT_SIZE);
    copy_bytes(s->globals,global_memory,GLOBAL_SIZE);
    copy_bytes(s->dirs,directory_memory,DIR_SIZE);
    copy_bytes(s->channels,channel_memory,CHANNEL_SIZE);
    copy_bytes(s->header,header_memory,HEADER_SIZE);
    copy_bytes(s->read,READ_BUFFER-32,sizeof(s->read));
    copy_bytes(s->sector,SECTOR_BUFFER-32,sizeof(s->sector));
    copy_bytes(s->write,WRITE_BUFFER-32,sizeof(s->write));
}
typedef struct { u32 kind,args[6]; Snapshot state; } Event;
static Event events[24];
static Snapshot current, expected;
static u8 expected_map[MAP_SIZE];
static u32 recording,event_count,event_cursor,seen[KIND_COUNT],last_occurrence;
static void event(u32 kind,u32 a,u32 b,u32 c,u32 d,u32 e,u32 f)
{
    u32 args[6]; Event *p;
    args[0]=a;args[1]=b;args[2]=c;args[3]=d;args[4]=e;args[5]=f;
    if(kind>=KIND_COUNT) fail(92);
    last_occurrence=++seen[kind];
    snapshot(&current);
    if(recording) {
        if(event_count>=24) fail(93);
        p=&events[event_count++]; p->kind=kind;
        copy_bytes(p->args,args,sizeof(args));
        copy_bytes(&p->state,&current,sizeof(current));
    } else {
        if(event_cursor>=event_count) fail(1);
        p=&events[event_cursor++];
        if(p->kind!=kind || !equal(p->args,args,sizeof(args))) {
            u32 i;
            text("callback: "); number(kind);
            text("expected callback: "); number(p->kind);
            for(i=0;i<6;i++) if(p->args[i]!=args[i]) {
                text("argument: "); number(i);
                text("expected: "); number(p->args[i]);
                text("actual: "); number(args[i]);
            }
            fail(2);
        }
        if(!equal(&p->state,&current,sizeof(current))) fail(3);
        callbacks++;
        if(kind==FIND) lookups++;
        if(kind==WRITE_SECTOR) raw_writes++;
    }
}
#define CALL(k,a,b,c,d,e) event(k,(u32)(a),(u32)(b),(u32)(c),(u32)(d),(u32)(e),0)
#define DONE(k) mutate(k,last_occurrence)
int FntPrint(char *format,...)
{
    __builtin_va_list ap;
    __builtin_va_start(ap,format);
    if(format==D_8009AF6C) {
        struct DIRENTRY *entry=__builtin_va_arg(ap,struct DIRENTRY *);
        CALL(FONT_ENTRY,entry,0,0,0,0); DONE(FONT_ENTRY);
    } else {
        s32 n=__builtin_va_arg(ap,s32);
        if(format!=D_80010378) fail(94);
        CALL(FONT_FREE,n,0,0,0,0); DONE(FONT_FREE);
    }
    __builtin_va_end(ap); return 0;
}
int printf(char *format,...)
{
    u32 kind=format==D_80010398?PRINT_MAKE:PRINT_FAIL;
    if(format!=D_80010398 && format!=D_800103A4) fail(95);
    CALL(kind,0,0,0,0,0); DONE(kind); return 0;
}
s32 func_80044838(s32 once,s32 *request,s32 *result)
{
    CALL(POLL,once,request,result,0,0);
    put32(request,input.event); put32(result,input.io_result);
    DONE(POLL); return input.poll;
}
static void ui(MenuRecord *root,u8 *slot,u32 which)
{
    CALL(UI_CALL,root,slot,which,0,0); DONE(UI_CALL);
}
static void ui0(MenuRecord *r,u8 *s) { ui(r,s,0); }
static void ui1(MenuRecord *r,u8 *s) { ui(r,s,1); }
static void ui2(MenuRecord *r,u8 *s) { ui(r,s,2); }
static void ui3(MenuRecord *r,u8 *s) { ui(r,s,3); }
static void ui4(MenuRecord *r,u8 *s) { ui(r,s,4); }
void (*D_80090F88[5])(MenuRecord *,u8 *)={ui0,ui1,ui2,ui3,ui4};
void func_80039794(void) { CALL(TEXT_STEP,0,0,0,0,0); DONE(TEXT_STEP); }
void MemCard_Init(long value) { CALL(INIT,value,0,0,0,0); DONE(INIT); }
void MemCard_InitIOEvents(void) { CALL(INIT_EVENTS,0,0,0,0,0); DONE(INIT_EVENTS); }
s32 MemCard_ReqLoadDirectory(s32 channel)
{ CALL(DIRECTORY_LOAD,channel,0,0,0,0); DONE(DIRECTORY_LOAD); return 1; }
s32 MemCard_FindEntry(u8 *name,struct DIRENTRY *entries,s32 count)
{ CALL(FIND,name,entries,count,0,0); DONE(FIND); return input.lookup; }
s32 MemCard_ReqReadSector(s32 a,s32 b,s32 c)
{ CALL(READ_SECTOR,a,b,c,0,0); DONE(READ_SECTOR); return 1; }
s32 MemCard_ReqReadFile(s32 a,s32 b,s32 c,s32 d,s32 e)
{ CALL(READ_FILE,a,b,c,d,e); DONE(READ_FILE); return 1; }
DuelEffectChannel *DuelEffect_CreateChannel(s32 value,s32 bits)
{
    CALL(MESSAGE,value,bits,0,0,0); DONE(MESSAGE);
    return (DuelEffectChannel *)(channel_memory+32);
}
void Util_CopyWords(u8 *out,u8 *in,u32 length)
{
    CALL(COPY,out,in,length,0,0);
    copy_bytes(out,in,length); DONE(COPY);
}
void Util_FillMemory(u8 *out,s32 value,u32 length)
{
    u32 i;
    CALL(FILL,out,value,length,0,0);
    for(i=0;i<length;i++) out[i]=(u8)value;
    DONE(FILL);
}
s32 MemCard_ReqWriteFile(s32 a,s32 b,s32 c,s32 d,s32 e)
{ CALL(WRITE_FILE,a,b,c,d,e); DONE(WRITE_FILE); return 1; }
s32 MemCard_ReqWriteSector(s32 a,s32 b,s32 c)
{ CALL(WRITE_SECTOR,a,b,c,0,0); DONE(WRITE_SECTOR); return 1; }
s32 MemCard_ReqCreateFile(s32 a,s32 b,s32 c)
{ CALL(CREATE_FILE,a,b,c,0,0); DONE(CREATE_FILE); return 1; }

static s32 lookup(void)
{
    return MemCard_FindEntry(gMemCard_szSaveFileName,
        (struct DIRENTRY *)G32(DIRECTORY),(s32)G32(COUNT));
}
static s32 head(s32 index)
{
    return (s32)word32((u8 *)G32(DIRECTORY)+index*40+32);
}
static void message(u32 id)
{
    DuelEffect_CreateChannel(id,1);
    S16(FLAGS,G16(FLAGS)|0x400);
}
static void reference(void)
{
    u8 *r=root_memory+32;
    u8 *slot=r+28+G8(SELECTOR)*616;
    s32 result=-1,i,index;
    u32 phase;
    if(G16(FLAGS)&0x2000) {
        for(i=0;i<(s32)G32(COUNT);i++)
            FntPrint(D_8009AF6C,(struct DIRENTRY *)((u8 *)G32(DIRECTORY)+i*40));
        FntPrint(D_80010378,(s32)byte(slot+12));
    }
    if(G16(FLAGS)&0x4000)
        result=func_80044838(1,(s32 *)(global_memory+EVENT),(s32 *)(global_memory+IO_RESULT));
    if((G16(FLAGS)&0x1000) && result==1) {
        s32 request=(s32)G32(EVENT), outcome=(s32)G32(IO_RESULT);
        if(request==2) {
            S16(FLAGS,G16(FLAGS)|0x2000);
            if(outcome==0 || outcome==3) {
                if(outcome==3) S16(FLAGS,G16(FLAGS)|0x800);
                put8(slot+12,G32(FREE)); S8(OUTCOME,0);
            } else if(outcome==4) {
                S16(FLAGS,G16(FLAGS)|2); S8(OUTCOME,1);
            } else S8(OUTCOME,2);
        } else if(request==3 || request==4) {
            if(outcome) S8(OUTCOME,1);
            else {
                s32 amount=signed16(word16(r+22));
                s32 left=signed16(word16(r+18)-amount);
                put16(r+18,left);
                if(left<=0) S8(OUTCOME,0);
                else {
                    put32(slot,word32(slot)+(u32)amount);
                    put16(r+20,word16(r+20)+amount);
                    if(left<signed16(word16(r+22))) put16(r+22,left);
                }
            }
        } else if(request==8 || request==11 || request==12) {
            S8(OUTCOME,outcome!=0);
        }
    }
    if(G8(UI)&15) {
        result=1;
        ui((MenuRecord *)r,slot,G8(UI)&15);
    }
    if(result>=0) return;
    if(G16(FLAGS)&0x400) {
        u8 *channel=channel_memory+32+byte(r+26)*100;
        func_80039794();
        if((word32(channel+52)&0x2008)==0x2000)
            S16(FLAGS,G16(FLAGS)&~0x400);
        return;
    }
    phase=G8(STAGE)&15;
    for(;;) {
        switch(phase) {
        case 0:
            if(!(G8(STAGE)&128)) { S8(STAGE,G8(STAGE)|128); S8(UI,1); return; }
            if(G8(CHOICE)) { S8(UI,2); return; }
            MemCard_Init(1); MemCard_InitIOEvents();
            S8(STAGE,1); S16(FLAGS,G16(FLAGS)|0x4000);
            phase=1; continue;
        case 1:
            if(!(G8(STAGE)&128)) {
                S8(STAGE,G8(STAGE)|128); S16(FLAGS,G16(FLAGS)|0x1000);
                MemCard_ReqLoadDirectory(0); return;
            }
            S8(STAGE,4); phase=4; continue;
        case 4:
            if(!(G16(FLAGS)&0x2000)) return;
            S16(FLAGS,G16(FLAGS)&~0x1000);
            if(G8(OUTCOME)==2) message(0xD2);
            else S8(STAGE,(G8(MODE)&1)?7:5);
            return;
        case 5:
            if(!(G8(STAGE)&128)) {
                S8(STAGE,G8(STAGE)|128); index=lookup();
                if(index<0 || (G16(FLAGS)&2)) message(0xD3);
                else MemCard_ReqReadSector(0,0x80210000,head(index)/64);
                return;
            }
            if(G8(OUTCOME)) { message(0xD8); return; }
            S8(STAGE,6); phase=6; continue;
        case 6:
            if(!(G8(STAGE)&128)) {
                S8(STAGE,G8(STAGE)|128); index=lookup();
                if(index<0 || (G16(FLAGS)&2)) { message(0xD3); return; }
                DuelEffect_CreateChannel(0x80D5,0);
                put8(slot+13,index); S16(FLAGS,G16(FLAGS)|0x1000);
                MemCard_ReqReadFile(0,(s32)gMemCard_szSaveFileName,0x80200000,512,0x1E00);
            } else message(G8(OUTCOME)==1?0xD8:0xD7);
            return;
        case 7:
            if(!(G8(STAGE)&128)) {
                S8(STAGE,G8(STAGE)|128);
                if(G16(FLAGS)&2) { S8(STAGE,9); return; }
                if(lookup()<0) { S8(STAGE,10); return; }
                DuelEffect_CreateChannel(0x80D6,0);
                Util_CopyWords(WRITE_BUFFER,header_memory+32,512);
                Util_FillMemory(WRITE_BUFFER+512,0x86,2048);
                MemCard_ReqWriteFile(0,(s32)gMemCard_szSaveFileName,0x80300000,0,2560);
                S16(FLAGS,G16(FLAGS)|0x1000); return;
            }
            if(G8(OUTCOME)==1) DuelEffect_CreateChannel(0xD9,1);
            else { S8(STAGE,8); DuelEffect_CreateChannel(0xD7,1); }
            S16(FLAGS,G16(FLAGS)|0x400); S8(STAGE,8);
            phase=8; continue;
        case 8:
            if(!(G8(STAGE)&128)) {
                u32 sum=0; s32 sector;
                S8(STAGE,G8(STAGE)|128); index=lookup();
                put8(SECTOR_BUFFER+126,1);
                for(i=122;i<126;i++) put8(SECTOR_BUFFER+i,255);
                for(i=0;i<127;i++) sum^=byte(SECTOR_BUFFER+i);
                sector=head(index);
                put8(SECTOR_BUFFER+127,sum);
                MemCard_ReqWriteSector(0,0x80210000,sector/64);
            } else if(G8(OUTCOME)) message(0xD9);
            return;
        case 9: case 10:
            if(!(G8(STAGE)&128)) {
                S8(STAGE,G8(STAGE)|128);
                if(byte(slot+12)>=byte(r+16))
                    MemCard_ReqCreateFile(0,(s32)gMemCard_szSaveFileName,byte(r+16));
            } else {
                if(!G8(OUTCOME)) printf(D_80010398);
                printf(D_800103A4);
            }
            return;
        default: return;
        }
    }
}

static void defaults(u32 stage)
{
    input.flags=0; input.selector=0; input.stage=stage;
    input.ui=0; input.outcome=0; input.mode=0; input.choice=0;
    input.poll=-1; input.event=3; input.io_result=0; input.lookup=1;
    input.count=3; input.free_blocks=14; input.head=-129;
    input.remaining=300; input.chunk=128; input.progress=0xFFE0;
    input.required=5; input.free_slot=6; input.text_flags=0xABCD2000;
    patch_count=0;
}
static void reset(void)
{
    u32 i,bank;
    u8 *r=root_memory+32;
    pattern(root_memory,ROOT_SIZE,31); pattern(global_memory,GLOBAL_SIZE,47);
    pattern(directory_memory,DIR_SIZE,61); pattern(channel_memory,CHANNEL_SIZE,79);
    pattern(header_memory,HEADER_SIZE,97); pattern(MAP,MAP_SIZE,113);
    S16(FLAGS,input.flags); S8(SELECTOR,input.selector);
    S32(EVENT,input.event); S32(IO_RESULT,input.io_result);
    S8(UI,input.ui); S8(STAGE,input.stage); S8(OUTCOME,input.outcome);
    S8(MODE,input.mode); S8(CHOICE,input.choice);
    S32(DIRECTORY,(u32)(directory_memory+32));
    S32(COUNT,input.count); S32(FREE,input.free_blocks);
    put8(r+16,input.required); put16(r+18,input.remaining);
    put16(r+20,input.progress); put16(r+22,input.chunk); put8(r+26,0);
    for(i=0;i<2;i++) {
        u8 *slot=r+28+i*616;
        put32(slot,0x80200400+i*0x800);
        put8(slot+12,input.free_slot); put8(slot+13,0xEE);
    }
    for(bank=0;bank<2;bank++)
        for(i=0;i<16;i++) {
            u8 *entry=directory_memory+32+bank*640+i*40;
            put8(entry,'A'+i); put8(entry+1,0);
            put32(entry+24,0x6000+i);
            put32(entry+32,(u32)input.head+i*64+bank*4096);
        }
    for(i=0;i<4;i++) put32(channel_memory+32+i*100+52,input.text_flags);
    for(i=0;i<KIND_COUNT;i++) seen[i]=0;
    barrier();
}
static void run_case(void)
{
    cases++; if(patch_count) scripted++;
    recording=1; event_count=0; reset(); reference();
    snapshot(&expected); copy_bytes(expected_map,MAP,MAP_SIZE);
    recording=0; event_cursor=0; reset(); func_8003DC1C();
    snapshot(&current);
    if(event_cursor!=event_count) fail(4);
    if(!equal(&current,&expected,sizeof(current))) fail(5);
    if(!equal(MAP,expected_map,MAP_SIZE)) fail(6);
}
static void mutation_cases(void)
{
    u32 slot;
    for(slot=0;slot<2;slot++) {
        defaults(5); input.flags=0x2000; input.selector=slot;
        patch(FONT_ENTRY,1,REGION_GLOBAL,COUNT,4,1);
        patch(FONT_ENTRY,1,REGION_GLOBAL,SELECTOR,1,slot^1);
        patch(FONT_ENTRY,1,REGION_ROOT,32+28+slot*616+12,1,9); run_case();
        defaults(5); input.flags=0x2000;
        patch(FONT_ENTRY,1,REGION_GLOBAL,DIRECTORY,4,(u32)(directory_memory+32+640)); run_case();
        defaults(8); input.flags=0x5000; input.poll=1; input.event=2; input.selector=slot;
        patch(POLL,1,REGION_GLOBAL,SELECTOR,1,slot^1);
        patch(POLL,1,REGION_GLOBAL,FREE,4,0x105); run_case();
        defaults(8); input.flags=0x5000; input.poll=1; input.event=3;
        patch(POLL,1,REGION_GLOBAL,FLAGS,2,0); run_case();
        defaults(7); input.ui=0x81; input.selector=slot;
        patch(UI_CALL,1,REGION_GLOBAL,UI,1,0);
        patch(UI_CALL,1,REGION_GLOBAL,FLAGS,2,0);
        patch(UI_CALL,1,REGION_GLOBAL,STAGE,1,8); run_case();
        defaults(7); input.flags=0x400; input.text_flags=0x2008;
        patch(TEXT_STEP,1,REGION_ROOT,32+26,1,1);
        patch(TEXT_STEP,1,REGION_CHANNEL,32+52,4,0x2000); run_case();
        defaults(6); input.selector=slot;
        patch(MESSAGE,1,REGION_GLOBAL,SELECTOR,1,slot^1);
        patch(MESSAGE,1,REGION_GLOBAL,FLAGS,2,0x20); run_case();
        defaults(7);
        patch(COPY,1,REGION_GLOBAL,FLAGS,2,0x20);
        patch(WRITE_FILE,1,REGION_GLOBAL,FLAGS,2,0x40); run_case();
        defaults(0x87); input.outcome=2;
        patch(MESSAGE,1,REGION_GLOBAL,STAGE,1,0xE2);
        patch(MESSAGE,1,REGION_GLOBAL,FLAGS,2,0x20);
        patch(FIND,1,REGION_GLOBAL,DIRECTORY,4,(u32)(directory_memory+32+640)); run_case();
        defaults(5);
        patch(FIND,1,REGION_GLOBAL,DIRECTORY,4,(u32)(directory_memory+32+640)); run_case();
        defaults(0x80);
        patch(INIT,1,REGION_GLOBAL,FLAGS,2,0x20);
        patch(INIT_EVENTS,1,REGION_GLOBAL,FLAGS,2,0x40); run_case();
    }
}
int main(void)
{
    static const u32 boundaries[]={0,1,2,127,128,129,255,32767,32768,65535};
    static const s32 results[]={-1,0,1,2,3,4,99};
    static const s32 heads[]={-2147483647-1,-129,-65,-64,-63,-1,0,1,63,64,65,2147483647};
    u32 s,l,o,f,slot,e,a,b,i;
    /* Exercise the mutation controls before the larger sweeps. */
    defaults(8); input.lookup=0; run_case();
    defaults(2); input.flags=0x5000; input.poll=1; input.event=4;
    input.remaining=128; input.chunk=128; input.outcome=0x5A; run_case();
    for(s=0;s<256;s++) { defaults(s); run_case(); }
    for(s=0;s<16;s++) for(l=0;l<2;l++) for(o=0;o<3;o++)
     for(f=0;f<3;f++) for(slot=0;slot<2;slot++) {
        defaults(s+l*128); input.outcome=o; input.selector=slot;
        input.flags=f==0?0:f==1?0x2000:0x2002;
        input.mode=slot; input.choice=slot; run_case();
     }
    for(s=0;s<16;s++) for(l=0;l<2;l++) for(i=0;i<17;i++) {
        defaults(s+l*128);
        input.flags=i==16?65535:1u<<i;
        run_case();
    }
    for(e=0;e<17;e++) for(i=0;i<7;i++) for(slot=0;slot<2;slot++) {
        defaults(2); input.flags=0x5000; input.poll=1;
        input.event=e; input.io_result=results[i]; input.selector=slot; run_case();
    }
    for(e=3;e<=4;e++) for(a=0;a<10;a++) for(b=0;b<10;b++) for(slot=0;slot<2;slot++) {
        defaults(2); input.flags=0x5000; input.poll=1; input.event=e;
        input.remaining=boundaries[a]; input.chunk=boundaries[b];
        input.selector=slot; input.outcome=0x5A; run_case();
    }
    for(i=0;i<4;i++) for(f=0;f<5;f++) {
        static const u32 flags[]={0,0x1000,0x4000,0x5000,0x7000};
        defaults(4); input.poll=results[i]; input.flags=flags[f]; run_case();
    }
    for(s=0;s<2;s++) for(i=0;i<12;i++) {
        defaults(s?8:5); input.lookup=0; input.head=heads[i]; run_case();
    }
    for(s=5;s<=7;s++) {
        defaults(s); input.lookup=-1; run_case();
    }
    for(s=9;s<=10;s++) for(i=4;i<=6;i++) {
        defaults(s); input.free_slot=i; run_case();
    }
    for(i=0;i<5;i++) {
        defaults(7); input.ui=i|0xF0; run_case();
    }
    mutation_cases();
    text("cases: "); number(cases);
    text("callbacks: "); number(callbacks);
    text("lookup calls: "); number(lookups);
    text("sector writes: "); number(raw_writes);
    text("mutation scenarios: "); number(scripted);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 compiler and ILP32 execution",
)
class LegacyMemoryCardUpdateTests(unittest.TestCase):
    def run_witness(self, optimization: str, mutation: str | None = None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run from the repository root")
        scratch = ROOT / "tmp"
        scratch.mkdir(exist_ok=True)
        environment = os.environ.copy()
        environment["TMPDIR"] = str(scratch)
        with tempfile.TemporaryDirectory(prefix="legacy-card-", dir=scratch) as temp:
            directory = Path(temp)
            source = SOURCE
            lookup_source = LOOKUP
            if mutation:
                replacements = {
                    "checksum": ("D_8021007F = checksum;", "D_8021007F = checksum ^ 1;"),
                    "signed-sector": ("sector += 63;", "sector += 0;"),
                    "progress-boundary": (
                        "if (*(s16 *)(state + 0x12) <= 0)",
                        "if (*(s16 *)(state + 0x12) < 0)",
                    ),
                    "lookup-return": (
                        "return MemCard_FindEntry(name, gMemCard_pDirEntries, gMemCard_nDirEntries);",
                        "MemCard_FindEntry(name, gMemCard_pDirEntries, gMemCard_nDirEntries);\n"
                        "    return 0;",
                    ),
                }
                old, new = replacements[mutation]
                original_source = LOOKUP if mutation == "lookup-return" else SOURCE
                contents = original_source.read_text()
                expected_count = 2 if mutation == "progress-boundary" else 1
                self.assertEqual(contents.count(old), expected_count)
                contents = contents.replace(old, new)
                contents = re.sub(
                    r'^#include "([^"]+)"',
                    lambda m: '#include "' + str((original_source.parent / m[1]).resolve()) + '"',
                    contents,
                    flags=re.MULTILINE,
                )
                mutated_source = directory / "mutant.c"
                mutated_source.write_text(contents)
                if mutation == "lookup-return":
                    lookup_source = mutated_source
                else:
                    source = mutated_source
            fixture = directory / "fixture.c"
            fixture.write_text(FIXTURE)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-D_LANGUAGE_C", optimization, "-I", str(ROOT),
            ]
            objects = []
            for name, path in (("game", source), ("lookup", lookup_source), ("fixture", fixture), ("start", start)):
                output = directory / (name + ".o")
                result = subprocess.run(
                    ["cc", *flags, "-c", str(path), "-o", str(output)],
                    capture_output=True, text=True, env=environment, timeout=60,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(output))
            aliases = [
                f"-Wl,--defsym,D_8021007{digit}=0x8021007{digit}"
                for digit in "ABCDEF"
            ]
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", *flags, *objects, *aliases, "-o", str(binary)],
                capture_output=True, text=True, env=environment, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run(
                [str(binary)], capture_output=True, text=True,
                env=environment, timeout=300,
            )

    def test_retail_protocol_and_callback_boundaries(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.run_witness(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "cases: 2096\n"
                    "callbacks: 3808\n"
                    "lookup calls: 268\n"
                    "sector writes: 97\n"
                    "mutation scenarios: 22\n",
                )

    def test_actual_source_mutations_are_semantic_failures(self):
        for optimization in ("-O0", "-O2"):
            for mutation in ("checksum", "signed-sector", "progress-boundary", "lookup-return"):
                with self.subTest(optimization=optimization, mutation=mutation):
                    result = self.run_witness(optimization, mutation)
                    self.assertEqual(result.returncode, 77, result.stdout + result.stderr)
                    self.assertIn("legacy mismatch: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
