"""Independent ILP32 execution oracle for finite retail camera-key programs."""

from __future__ import annotations

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/model_keyframe_update.c"
SCRATCH = REPOSITORY / "tmp/astra/keyframe-update-witness"
START = r"""
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.section .note.GNU-stack,"",@progbits
"""

# This oracle uses the verified instruction contract's offsets and explicit
# signed narrowing/truncation, not the implementation's macros or C structs.
WITNESS = r"""
#include "src/types.h"
#include "src/game/camera_view.h"
#include "src/game/func_8005EBF4.h"
#include "src/game/model_transfer_flags.h"
#include "src/game/func_80058DD8.h"
#include "src/game/func_80058E1C.h"
#include "src/game/model_copy_slot_u16_values.h"
#include "src/game/model_update_view_metrics.h"
#define FUNC_80058434_CALL_WITH_UNUSED_ARG
#include "src/game/func_80058434.h"
#include "src/psyq/rand.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
typedef char key_size[sizeof(Key) == 40 ? 1 : -1];
typedef char pose_size[sizeof(SVECTOR) == 8 ? 1 : -1];
typedef char view_size[sizeof(GsRVIEW2) == 32 ? 1 : -1];
enum {
    SIZE = 960, CURRENT = 32, COUNT = 36, INIT = 37, COUNTER = 38,
    FLAG0 = 39, FLAG1 = 40, SELECTED_VIEW = 44, KEYS = 128,
    POSES = 592, VIEW0 = 688, VIEW1 = 784, POSITIONS = 880, EVALUATED = 912,
    GET = 1, PREPARE, TIMING, READY, COPY, EVALUATE, RANDOM, COS, SIN,
    SNAPSHOT, METRICS, ORIENT, KINDS, MISMATCH = 75
};
u8 memory[SIZE] __attribute__((aligned(16)));
__asm__(".globl D_8009B074\n.set D_8009B074, memory+32\n"
        ".globl D_8009B078\n.set D_8009B078, memory+36\n"
        ".globl D_8009B079\n.set D_8009B079, memory+37\n"
        ".globl D_8009B07A\n.set D_8009B07A, memory+38\n"
        ".globl D_8009B07B\n.set D_8009B07B, memory+39\n"
        ".globl D_8009B07C\n.set D_8009B07C, memory+40\n"
        ".globl D_800F5788\n.set D_800F5788, memory+128\n"
        ".globl D_800F5768\n.set D_800F5768, memory+592\n"
        ".globl D_800F56F0\n.set D_800F56F0, memory+688\n"
        ".globl D_800F5710\n.set D_800F5710, memory+784\n");
void func_8005DBA4(void);

static u8 expected[SIZE];
static u32 cases, calls, scripts_run, recursive_gets, channel_coverage[2][9];
static u32 family, active_case;
static s32 returns[KINDS][64];
static u32 actual_seen[KINDS], expected_seen[KINDS];
typedef struct { s32 kind; u32 occurrence, offset, width, value; } Patch;
static Patch patches[24];
static u32 patch_count;
typedef struct { s32 kind, args[5]; u8 memory[SIZE]; } Event;
static Event events[128];
static u32 event_count, event_cursor;

static void copy_bytes(void *out, const void *in, u32 size)
{
    u8 *d = out;
    const u8 *s = in;
    while (size--) *d++ = *s++;
}
static s32 equal(const void *left, const void *right, u32 size)
{
    const u8 *a = left, *b = right;
    while (size--) if (*a++ != *b++) return 0;
    return 1;
}
static u32 u16at(const u8 *p) { return p[0] | ((u32)p[1] << 8); }
static u32 u32at(const u8 *p) { return u16at(p) | (u16at(p+2) << 16); }
static void put16(u8 *p, u32 n) { p[0] = n; p[1] = n >> 8; }
static void put32(u8 *p, u32 n) { put16(p,n); put16(p+2,n >> 16); }
static s32 narrow(s32 n)
{
    u32 bits = (u32)n & 65535;
    return bits < 32768 ? bits : (s32)bits - 65536;
}
static s32 s16at(const u8 *p) { return narrow(u16at(p)); }
static s32 quotient(s32 n, s32 d)
{
    u32 a = n < 0 ? -(u32)n : (u32)n;
    u32 b = d < 0 ? -(u32)d : (u32)d;
    s32 q = a / b;
    return (n < 0) != (d < 0) ? -q : q;
}
static s32 remainder(s32 n, s32 d) { return n - quotient(n,d) * d; }
static s32 shifted(s32 n)
{
    return n >= 0 ? n / 256 : -(s32)((-(u32)n + 255) / 256);
}
static s32 limit(s32 n, s32 amount)
{
    if (n > 0) return n < amount ? n : amount;
    return n > -amount ? n : -amount;
}
static void write_text(const char *text, u32 size)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(text), "d"(size)
                     : "memory", "cc");
}
static void number(u32 value)
{
    char out[12], reverse[10];
    u32 n = 0, length = 0;
    do { reverse[n++] = '0' + value % 10; value /= 10; } while (value);
    while (n) out[length++] = reverse[--n];
    out[length++] = '\n';
    write_text(out,length);
}
static void fail(u32 reason) __attribute__((noreturn));
static void fail(u32 reason)
{
    write_text("witness mismatch: ",18); number(reason);
    write_text("case: ",6); number(active_case);
    write_text("family: ",8); number(family);
    write_text("event: ",7); number(event_cursor);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(MISMATCH) : "memory");
    __builtin_unreachable();
}
static u32 address(u32 offset) { return (u32)memory + offset; }
static u32 key_offset(const u8 *m)
{
    u32 offset = u32at(m+CURRENT) - (u32)memory;
    if (offset < KEYS || offset >= KEYS+400 || (offset-KEYS)%40) fail(1);
    return offset;
}
static void patch(s32 kind, u32 occurrence, u32 offset, u32 width, u32 value)
{
    Patch *p;
    if (patch_count == 24 || offset + width > SIZE) fail(2);
    p = &patches[patch_count++];
    p->kind=kind; p->occurrence=occurrence; p->offset=offset;
    p->width=width; p->value=value;
}
static void effects(s32 kind, u32 occurrence, u8 *m)
{
    u32 i;
    for (i=0; i<patch_count; i++) {
        Patch *p=&patches[i];
        if (p->kind != kind || p->occurrence != occurrence) continue;
        if (p->width == 1) m[p->offset]=p->value;
        if (p->width == 2) put16(m+p->offset,p->value);
        if (p->width == 4) put32(m+p->offset,p->value);
    }
}
static s32 invoke(s32 actual, s32 kind, s32 a, s32 b, s32 c, s32 d, s32 e,
                  s16 *output)
{
    u8 *m = actual ? memory : expected;
    u32 *seen = actual ? actual_seen : expected_seen;
    u32 occurrence=seen[kind]++, i;
    s32 args[5], result;
    Event *event;
    if (occurrence == 64) fail(3);
    args[0]=a; args[1]=b; args[2]=c; args[3]=d; args[4]=e;
    if (actual) {
        if (event_cursor == event_count) fail(4);
        event=&events[event_cursor++];
        if (event->kind != kind || !equal(event->args,args,sizeof(args))) fail(5);
        if (!equal(event->memory,m,SIZE)) fail(6);
        calls++;
        if (kind == GET && occurrence) recursive_gets++;
    } else {
        if (event_count == 128) fail(7);
        event=&events[event_count++];
        event->kind=kind;
        copy_bytes(event->args,args,sizeof(args));
        copy_bytes(event->memory,m,SIZE);
    }
    /* The copier and evaluator write all four halfwords, including the
     * otherwise-unused fourth, before any scripted external mutation. */
    if (kind == COPY || kind == EVALUATE) {
        u32 offset=kind == COPY ? POSITIONS+a*8 : EVALUATED+b*8;
        if (!output || (kind == COPY && (u32)a > 1) || (kind == EVALUATE && (u32)b > 1))
            fail(8);
        for (i=0; i<4; i++) output[i]=s16at(m+offset+i*2);
    }
    result=returns[kind][occurrence];
    effects(kind,occurrence,m);
    if (kind == GET) result=u32at(m+SELECTED_VIEW);
    return result;
}
static s32 call(s32 kind,s32 a,s32 b,s32 c,s32 d,s32 e,s16 *out)
{ return invoke(0,kind,a,b,c,d,e,out); }
void *func_800591FC(void) { return (void *)invoke(1,GET,0,0,0,0,0,0); }
void func_8005E808(u8 *p) { invoke(1,PREPARE,(u32)p,0,0,0,0,0); }
s32 func_80058E1C(void) { return invoke(1,TIMING,0,0,0,0,0,0); }
s32 func_80058DD8(s32 slot) { return invoke(1,READY,slot,0,0,0,0,0); }
void Model_CopySlotU16Values(s32 slot,u16 *out)
{ invoke(1,COPY,slot,out != 0,0,0,0,(s16 *)out); }
void func_8005EBF4(Key *key,s32 channel,s32 elapsed,s32 denominator,s16 *out)
{ invoke(1,EVALUATE,(u32)key,channel,elapsed,denominator,out != 0,out); }
int rand(void) { return invoke(1,RANDOM,0,0,0,0,0,0); }
int rcos(int angle) { return invoke(1,COS,angle,0,0,0,0,0); }
int rsin(int angle) { return invoke(1,SIN,angle,0,0,0,0,0); }
void func_8005F070(s32 value) { invoke(1,SNAPSHOT,value,0,0,0,0,0); }
void Model_UpdateViewMetrics(u8 *view) { invoke(1,METRICS,(u32)view,0,0,0,0,0); }
void func_80058434(s32 a,s32 b,s32 c,s32 d,s32 e)
{ invoke(1,ORIENT,a,b,c,d,e,0); }

static void oracle(u32 depth)
{
    u8 *m=expected;
    u32 view, old, key, rec, channel, axis, i, active=0, persistent=0;
    s32 elapsed,duration,remaining,kind,amount,slot,value,step;
    s16 target[4];
    if (depth > 5) fail(9); /* Fixture programs, not arbitrary recursive input. */
    view=(u32)call(GET,0,0,0,0,0,0)-(u32)memory;
    if (view != VIEW0 && view != VIEW1) fail(10);
    if (!m[COUNT] || !u32at(m+CURRENT)) return;
    if (m[INIT] && (m[FLAG0] != 1 || m[FLAG1] != 1)) {
        for (i=0; i<m[COUNT]; i++) call(PREPARE,address(KEYS+i*40),0,0,0,0,0);
        m[INIT]=0; m[COUNTER]=255;
    }
    key=key_offset(m);
    if (u16at(m+key+34) != 0x4000) {
        old=key;
        step=call(TIMING,0,0,0,0,0,0);
        key=key_offset(m);
        if ((s32)u16at(m+key+36)+step < (s32)u16at(m+key+34)) {
            step=call(TIMING,0,0,0,0,0,0);
            key=key_offset(m);
            elapsed=u16at(m+key+36)+step;
        } else elapsed=u16at(m+key+34);
        put16(m+old+36,elapsed);
    }
    for (channel=0; channel<2; channel++) {
        u32 out=view+channel*12, pose=POSES+channel*8;
        key=key_offset(m); rec=key+channel*8; kind=s16at(m+rec+6);
        if (!kind) continue;
        active++;
        if (kind == 128 || kind == 129) {
            slot=kind-128; elapsed=u16at(m+key+36); duration=u16at(m+key+34);
            if (call(READY,slot,0,0,0,0,0) != 1) continue;
            call(COPY,slot,1,0,0,0,target);
            rec=key_offset(m)+channel*8;
            for (axis=0; axis<3; axis++) {
                value=narrow((s32)target[axis]+u16at(m+rec+axis*2));
                value=quotient(s16at(m+pose+axis*2)*(duration-elapsed),duration)
                    +quotient(value*elapsed,duration);
                put32(m+out+axis*4,value);
            }
        } else if (kind == 1) {
            call(EVALUATE,address(key),channel,u16at(m+key+36),0,1,target);
            for (axis=0; axis<3; axis++) put32(m+out+axis*4,target[axis]);
        } else if (kind == 2 || kind == 3) {
            persistent++;
            slot=s16at(m+rec); amount=s16at(m+rec+2);
            if (call(READY,slot,0,0,0,0,0) != 1) continue;
            call(COPY,slot,1,0,0,0,target);
            rec=key_offset(m)+channel*8; kind=s16at(m+rec+6);
            if (!amount) amount=kind == 2 ? 10 : 2000;
            for (axis=0; axis<3; axis++) {
                value=narrow((s32)target[axis]-u16at(m+rec+16+axis*2));
                if (kind == 2) {
                    if (value > 0) { value-=amount; if (value < 0) value=0; }
                    else { value+=amount; if (value > 0) value=0; }
                    value=limit(narrow(value),60);
                    put32(m+out+axis*4,(s32)u32at(m+out+axis*4)+value);
                    put16(m+rec+16+axis*2,u16at(m+rec+16+axis*2)+value);
                } else {
                    value=narrow(limit(value,amount));
                    put32(m+out+axis*4,s16at(m+pose+axis*2)+value);
                }
            }
        } else if (kind == 5) {
            s32 a,b;
            persistent++; amount=s16at(m+rec);
            if (u16at(m+key+36) >= u16at(m+key+34)) {
                put32(m+out,s16at(m+pose));
                put32(m+out+8,s16at(m+pose+4));
            } else {
                a=call(RANDOM,0,0,0,0,0,0); b=call(RANDOM,0,0,0,0,0,0);
                put32(m+out,s16at(m+pose)+remainder(shifted(a),amount)
                      -quotient(remainder(shifted(b),amount),2));
                a=call(RANDOM,0,0,0,0,0,0); b=call(RANDOM,0,0,0,0,0,0);
                put32(m+out+8,s16at(m+pose+4)+remainder(shifted(a),amount)
                      -quotient(remainder(shifted(b),amount),2));
            }
        }
    }
    for (channel=0; channel<2; channel++) {
        s32 radius,yaw,pitch,cosine,sine,horizontal,vertical,z;
        u32 out=view+channel*12, opposite=view+(channel^1)*12;
        key=key_offset(m); rec=key+channel*8;
        if (s16at(m+rec+6) != 4) continue;
        elapsed=u16at(m+key+36); duration=u16at(m+key+34);
        yaw=quotient(s16at(m+rec+2)*elapsed,duration)+s16at(m+POSES+18);
        pitch=quotient(s16at(m+rec+4)*elapsed,duration)+s16at(m+POSES+20);
        radius=s16at(m+POSES+16); remaining=duration-elapsed;
        if (s16at(m+rec))
            radius=quotient(radius*remaining+s16at(m+rec)*elapsed,duration);
        cosine=call(COS,pitch,0,0,0,0,0); sine=call(SIN,pitch,0,0,0,0,0);
        horizontal=quotient(radius*cosine,4096);
        vertical=quotient(radius*sine,4096);
        cosine=call(COS,yaw,0,0,0,0,0); sine=call(SIN,yaw,0,0,0,0,0);
        z=quotient(horizontal*sine,4096); horizontal=quotient(horizontal*cosine,4096);
        put32(m+out,(s32)u32at(m+opposite)+horizontal);
        put32(m+out+4,(s32)u32at(m+opposite+4)+vertical);
        put32(m+out+8,(s32)u32at(m+opposite+8)+z);
    }
    key=key_offset(m);
    if (!active && u16at(m+key+34) == 0x4000) {
        put32(m+CURRENT,0); m[COUNT]=0; return;
    }
    if (u16at(m+key+36) >= u16at(m+key+34)) {
        call(SNAPSHOT,0,0,0,0,0,0);
        key=key_offset(m)+40; put32(m+CURRENT,address(key));
        if ((key-KEYS)/40 >= m[COUNT]) {
            if (m[COUNT] >= 2) { key=KEYS; put32(m+CURRENT,address(key)); m[key+38]=0; }
            else { put32(m+CURRENT,0); m[COUNT]=0; }
        }
        if (u32at(m+CURRENT)) {
            call(PREPARE,u32at(m+CURRENT),0,0,0,0,0);
            if (!s16at(m+key_offset(m)+32)) oracle(depth+1);
        }
    }
    if (persistent == 2) call(ORIENT,1,4096,0,0,0,0);
    else call(METRICS,0,0,0,0,0,0);
}

static void defaults(void)
{
    static const s32 random_values[8]={-513,1025,-1025,257,0x7FFFFFFF,
                                       -2147483647-1,769,-769};
    u32 i,k;
    patch_count=0;
    for (i=0; i<SIZE; i++) memory[i]=(u8)(cases*13+i*37+(i>>3));
    put32(memory+CURRENT,address(KEYS)); memory[COUNT]=1; memory[INIT]=0;
    memory[COUNTER]=0x49; memory[FLAG0]=memory[FLAG1]=0;
    put32(memory+SELECTED_VIEW,address(cases&1 ? VIEW1 : VIEW0));
    for (i=0; i<10; i++) {
        u32 key=KEYS+i*40, axis;
        for (axis=0; axis<16; axis++) put16(memory+key+axis*2,0);
        put16(memory+key+32,1); put16(memory+key+34,7); put16(memory+key+36,2);
        memory[key+38]=0xA5; memory[key+39]=0x5A;
    }
    for (i=0; i<3; i++) {
        put16(memory+POSES+i*8,(s32)i*17-9);
        put16(memory+POSES+i*8+2,13-(s32)i*33);
        put16(memory+POSES+i*8+4,(s32)i*57-19);
    }
    for (i=0; i<6; i++) {
        put32(memory+VIEW0+i*4,(s32)i*83-301);
        put32(memory+VIEW1+i*4,509-(s32)i*97);
    }
    for (i=0; i<8; i++) {
        put16(memory+POSITIONS+i*2,(s32)i*53-107);
        put16(memory+EVALUATED+i*2,(s32)i*791-32768);
    }
    for (k=0; k<KINDS; k++)
      for (i=0; i<64; i++) {
        returns[k][i]=0;
        if (k == READY) returns[k][i]=1;
        if (k == RANDOM) returns[k][i]=random_values[i%8];
        if (k == COS) returns[k][i]=i&1 ? 2049 : -4095;
        if (k == SIN) returns[k][i]=i&1 ? -1537 : 3073;
      }
}
static void record(u32 key,u32 channel,s32 kind,s32 x,s32 y,s32 z)
{
    u8 *p=memory+KEYS+key*40+channel*8;
    put16(p,x); put16(p+2,y); put16(p+4,z); put16(p+6,kind);
}
static void run_case(void)
{
    u32 i,channel;
    cases++; active_case=cases;
    if (patch_count) scripts_run++;
    if (u32at(memory+CURRENT))
        for (channel=0; channel<2; channel++) {
            s32 kind=s16at(memory+key_offset(memory)+channel*8+6);
            u32 bucket=kind >= 0 && kind <= 5 ? kind : kind == 128 ? 6 : kind == 129 ? 7 : 8;
            channel_coverage[channel][bucket]++;
        }
    copy_bytes(expected,memory,SIZE);
    event_count=event_cursor=0;
    for (i=0; i<KINDS; i++) actual_seen[i]=expected_seen[i]=0;
    oracle(0);
    func_8005DBA4();
    if (event_count != event_cursor) fail(11);
    if (!equal(expected,memory,SIZE)) fail(12);
}

static void channels(void)
{
    static const s32 kinds[9]={0,1,2,3,4,5,128,129,-1};
    static const s32 ready[4]={-1,0,1,2};
    static const u32 durations[4]={1,7,0x4000,0xFFFF};
    u32 a,b,r,t,e;
    family=1;
    for (a=0; a<9; a++)
     for (b=0; b<9; b++)
      for (r=0; r<4; r++)
       for (t=0; t<4; t++)
        for (e=0; e<3; e++) {
            defaults();
            record(0,0,kinds[a],kinds[a] == 5 ? -7 : 0,-13,29);
            record(0,1,kinds[b],kinds[b] == 5 ? 9 : 1,17,-31);
            put16(memory+KEYS+34,durations[t]);
            put16(memory+KEYS+36,e == 0 ? 0 : e == 1 ? durations[t]-1 : durations[t]);
            returns[READY][0]=returns[READY][1]=ready[r];
            run_case();
        }
}
static void arithmetic(void)
{
    static const s32 amounts[12]={-32768,-32767,-2001,-61,-1,0,1,10,60,61,2000,32767};
    static const s32 deltas[15]={-32768,-32767,-2001,-61,-60,-11,-1,0,1,11,60,61,2001,32766,32767};
    static const s32 halves[6]={-32768,-32767,-1,0,1,32767};
    u32 kind,a,b,ch,axis;
    family=2;
    for (kind=2; kind<=3; kind++)
     for (a=0; a<12; a++)
      for (b=0; b<15; b++)
       for (ch=0; ch<2; ch++) {
        defaults(); record(0,ch,kind,ch,amounts[a],0);
        for (axis=0; axis<3; axis++) {
            u32 stored=axis == 0 ? 0xFFFF : axis == 1 ? 0x8000 : 0x1234;
            put16(memory+KEYS+ch*8+16+axis*2,stored);
            put16(memory+POSITIONS+ch*8+axis*2,stored+deltas[(b+axis*5)%15]);
        }
        run_case();
       }
    family=3;
    for (a=0; a<6; a++)
     for (b=0; b<6; b++)
      for (ch=0; ch<2; ch++) {
        defaults(); record(0,ch,128+ch,halves[a],halves[b],-1);
        for (axis=0; axis<3; axis++) {
            put16(memory+POSES+ch*8+axis*2,halves[(a+axis)%6]);
            put16(memory+POSITIONS+ch*8+axis*2,halves[(b+axis)%6]);
        }
        run_case();
      }
    family=4;
    for (a=0; a<12; a++)
     for (ch=0; ch<2; ch++) {
        if (!amounts[a]) continue;
        defaults(); record(0,ch,5,amounts[a],0,0); run_case();
     }
    family=5;
    for (a=0; a<6; a++)
     for (b=0; b<6; b++) {
        defaults(); record(0,0,4,halves[a],-29,31); record(0,1,4,halves[b],33,-35);
        put16(memory+POSES+16,halves[(a+b)%6]); run_case();
     }
}
static void timing_and_programs(void)
{
    static const s32 steps[6]={-3,0,1,5,6,9};
    static const s32 unknown[6]={6,127,130,32767,-32768,-1};
    static const u32 counts[4]={1,2,3,10};
    u32 a,b,c;
    family=6;
    for (a=0; a<6; a++)
     for (b=0; b<6; b++) {
        defaults(); record(0,0,-1,0,0,0);
        returns[TIMING][0]=steps[a]; returns[TIMING][1]=steps[b];
        run_case();
     }
    family=7;
    for (a=0; a<3; a++)
     for (b=0; b<3; b++)
      for (c=0; c<3; c++) {
        defaults(); memory[COUNT]=3; memory[INIT]=a;
        memory[FLAG0]=b; memory[FLAG1]=c; run_case();
      }
    defaults(); memory[COUNT]=0; run_case();
    /* Keep a valid key for coverage bookkeeping; the GET callback can clear
     * current before the caller's early-return test. */
    defaults(); patch(GET,0,CURRENT,4,0); run_case();
    defaults(); patch(GET,0,COUNT,1,0); run_case();
    defaults(); put32(memory+CURRENT,0); run_case();
    defaults(); memory[COUNT]=0; put32(memory+CURRENT,0); run_case();
    defaults(); memory[COUNT]=0; patch(GET,0,COUNT,1,1); run_case();
    defaults(); put32(memory+CURRENT,0); patch(GET,0,CURRENT,4,address(KEYS)); run_case();
    defaults(); memory[COUNT]=10; memory[INIT]=1; run_case();
    for (a=0; a<4; a++)
     for (b=0; b<counts[a]; b++) {
        defaults(); memory[COUNT]=counts[a]; put32(memory+CURRENT,address(KEYS+b*40));
        put16(memory+KEYS+b*40+36,7); run_case();
     }
    for (a=0; a<6; a++)
     for (b=0; b<2; b++) {
        defaults(); record(0,b,unknown[a],0,0,0);
        put16(memory+KEYS+34,0x4000); run_case();
     }
    family=8;
    /* Finite recursive programs: each zero-control entry is already complete,
     * and the last entry is nonzero-control. No zero denominators or cycles. */
    for (a=1; a<=3; a++) {
        defaults(); memory[COUNT]=a+2;
        for (b=0; b<=a; b++) {
            record(b,0,2,0,10,0); record(b,1,3,1,20,0);
            put16(memory+KEYS+b*40+36,7);
            if (b) put16(memory+KEYS+b*40+32,0);
        }
        run_case();
    }
    defaults(); memory[COUNT]=2; put16(memory+KEYS+36,7);
    put16(memory+KEYS+40+32,0); put16(memory+KEYS+40+34,0x4000);
    run_case();
}

static void mutations(void)
{
    u32 ch,kind,which,i;
    family=9;
    /* Initialization's count is live, while the walking pointer is local. */
    for (which=0; which<2; which++) {
        defaults(); memory[INIT]=1; memory[COUNT]=which ? 1 : 3;
        patch(PREPARE,0,COUNT,1,which ? 3 : 1);
        patch(PREPARE,0,CURRENT,4,address(KEYS+40));
        patch(PREPARE,0,INIT,1,7); patch(PREPARE,0,COUNTER,1,41);
        run_case();
    }
    /* Two timing callbacks use fresh current pointers but store elapsed into
     * the pointer captured before the first call. */
    for (which=0; which<2; which++) {
        defaults(); memory[COUNT]=3;
        record(0,0,-1,0,0,0); record(1,0,-1,0,0,0); record(2,0,-1,0,0,0);
        patch(TIMING,0,CURRENT,4,address(KEYS+40));
        patch(TIMING,0,KEYS+40+34,2,which ? 3 : 19);
        patch(TIMING,0,KEYS+40+36,2,3);
        patch(TIMING,1,CURRENT,4,address(KEYS+80));
        patch(TIMING,1,KEYS+80+36,2,4);
        returns[TIMING][0]=1; returns[TIMING][1]=2;
        run_case();
    }
    family=10;
    /* Capture slot/amount/times before ready; copy outputs precede its
     * mutations; relative records and pose are then read live. */
    for (ch=0; ch<2; ch++)
     for (kind=0; kind<4; kind++)
      for (which=0; which<2; which++) {
        s32 original=kind < 2 ? 128+kind : kind;
        defaults(); memory[COUNT]=3;
        record(0,ch,original,ch,-32768,3);
        record(1,ch,3,1-ch,61,11);
        record(2,ch,which ? 2 : -1,1-ch,32767,17);
        put16(memory+KEYS+80+34,17); put16(memory+KEYS+80+36,3);
        patch(READY,0,CURRENT,4,address(KEYS+40));
        patch(READY,0,SELECTED_VIEW,4,address(VIEW1));
        patch(READY,0,KEYS+40+34,2,13);
        patch(READY,0,KEYS+40+36,2,5);
        patch(COPY,0,CURRENT,4,address(KEYS+80));
        patch(COPY,0,POSES+ch*8,2,-37);
        patch(COPY,0,POSES+ch*8+2,2,41);
        patch(COPY,0,POSES+ch*8+4,2,-43);
        for (i=0; i<3; i++) {
            put16(memory+POSITIONS+ch*8+i*2,32767-i);
            patch(COPY,0,KEYS+80+ch*8+16+i*2,2,i ? 0xFFFF : 0x8000);
        }
        run_case();
      }
    family=11;
    for (ch=0; ch<2; ch++) {
        defaults(); memory[COUNT]=2; record(0,ch,1,0,0,0);
        record(1,1-ch,1,0,0,0);
        patch(EVALUATE,0,CURRENT,4,address(KEYS+40));
        patch(EVALUATE,0,KEYS+40+36,2,3);
        patch(EVALUATE,0,SELECTED_VIEW,4,address(VIEW1));
        patch(EVALUATE,0,POSES+6,2,0xFEDC);
        run_case();
    }
    family=12;
    for (ch=0; ch<2; ch++) {
        defaults(); record(0,ch,5,-7,0,0);
        patch(RANDOM,0,KEYS+ch*8,2,19);
        patch(RANDOM,1,POSES+ch*8,2,-43);
        patch(RANDOM,3,POSES+ch*8+4,2,53);
        patch(RANDOM,1,SELECTED_VIEW,4,address(VIEW1));
        run_case();
    }
    family=13;
    for (ch=0; ch<2; ch++) {
        defaults(); memory[COUNT]=2; record(0,ch,4,-101,37,-43);
        record(1,ch^1,4,103,-47,53);
        patch(COS,0,CURRENT,4,address(KEYS+40));
        patch(COS,0,POSES+16,2,307);
        patch(COS,0,POSES+18,2,-311);
        patch(COS,0,POSES+20,2,313);
        patch(SIN,1,SELECTED_VIEW,4,address(VIEW1));
        patch(SIN,1,VIEW0+(ch^1)*12,4,-997);
        patch(SIN,1,VIEW1+(ch^1)*12,4,991);
        run_case();
    }
    family=14;
    for (which=0; which<3; which++) {
        defaults(); memory[COUNT]=3; put16(memory+KEYS+36,7);
        patch(SNAPSHOT,0,CURRENT,4,address(KEYS+40));
        patch(SNAPSHOT,0,COUNT,1,which+1);
        patch(PREPARE,0,CURRENT,4,address(KEYS+80));
        patch(PREPARE,0,COUNT,1,3);
        patch(PREPARE,0,KEYS+80+32,2,0);
        patch(PREPARE,0,SELECTED_VIEW,4,address(VIEW1));
        run_case();
    }
    defaults(); memory[COUNT]=2; put16(memory+KEYS+36,7);
    record(0,0,2,0,1,0); record(0,1,3,1,1,0);
    patch(PREPARE,0,KEYS+40+32,2,0);
    patch(METRICS,0,COUNT,1,1);
    patch(METRICS,0,POSES+22,2,0xBEEF);
    run_case();
}

int main(void)
{
    u32 c,k,covered=0;
    channels(); arithmetic(); timing_and_programs(); mutations();
    for (c=0; c<2; c++) for (k=0; k<9; k++) covered+=channel_coverage[c][k] != 0;
    write_text("channel-kind buckets: ",22); number(covered);
    write_text("cases: ",7); number(cases);
    write_text("callbacks: ",11); number(calls);
    write_text("recursive entries: ",19); number(recursive_gets);
    write_text("mutation scenarios: ",20); number(scripts_run);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host compiler and ILP32 execution",
)
class ModelKeyframeUpdateTests(unittest.TestCase):
    def build_and_run(
        self, optimization: str, mutation: str | None = None
    ) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation is not None:
                text = SOURCE.read_text()
                replacements = {
                    "combined-division": (
                        "pose[0] * remaining / duration\n"
                        "                + position[0] * elapsed / duration",
                        "(pose[0] * remaining + position[0] * elapsed) / duration",
                    ),
                    "late-elapsed-store": (
                        "*(u16 *)(previous + 0x24) = elapsed;",
                        "*(u16 *)(D_8009B074 + 0x24) = elapsed;",
                    ),
                    "clamp-before-narrow": (
                        "delta[0] = value;",
                        "delta[0] = value > 60 ? 60 : value < -60 ? -60 : value;",
                    ),
                }
                original, replacement = replacements[mutation]
                self.assertEqual(text.count(original), 1)
                text = text.replace(original, replacement)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda match: '#include "' + str(
                        (SOURCE.parent / match.group(1)).resolve()
                    ) + '"',
                    text, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", optimization, "-I", str(REPOSITORY),
            ]
            objects = []
            for name, path in (("game", source), ("fixture", fixture), ("start", start)):
                obj = directory / (name + ".o")
                result = subprocess.run(
                    ["cc", *flags, "-c", str(path), "-o", str(obj)],
                    text=True, capture_output=True, timeout=60,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", *flags, *objects, "-o", str(binary)],
                text=True, capture_output=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], text=True, capture_output=True, timeout=120)

    def test_finite_keyframe_programs(self) -> None:
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "channel-kind buckets: 18\n"
                    "cases: 4871\n"
                    "callbacks: 30948\n"
                    "recursive entries: 10\n"
                    "mutation scenarios: 34\n",
                )

    def test_actual_source_mutations_are_rejected(self) -> None:
        failures = {
            "combined-division": (365, 1, 6),
            "late-elapsed-store": (4844, 9, 4),
            "clamp-before-narrow": (3889, 2, 6),
        }
        for mutation, (case, family, event) in failures.items():
            for optimization in ("-O0", "-O2"):
                with self.subTest(mutation=mutation, optimization=optimization):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 75, result.stdout + result.stderr)
                    self.assertEqual(
                        result.stdout,
                        f"witness mismatch: 6\ncase: {case}\n"
                        f"family: {family}\nevent: {event}\n",
                    )


if __name__ == "__main__":
    unittest.main()
