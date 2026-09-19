"""Independent ILP32 caller oracle using the real subdivision helpers."""

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/model_subdivided_effect.c"
SCRATCH = ROOT / "tmp/astra/subdivided-effect-witness"
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
FIXTURE = r"""
#include "src/types.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/psyq/libgs.h"
#include "src/psyq/stdio.h"
#include "src/psyq/strings.h"
#include "src/game/model_subdivided_effect.h"
#include "src/game/triangle_subdivision.h"
#include "src/game/model_geometry_tables.h"
#include "src/game/model_graphics_state.h"
#include "src/game/screen_projection.h"
#include "src/game/gpu_packets.h"
#include "src/game/func_80057E20.h"
#include "src/game/func_80058E1C.h"
#include "src/game/model_copy_slot_u16_values.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
enum { BYTES = 0x1318, GUARD = 32, TOTAL = BYTES + 64, CAPACITY = 384,
       SLOT = 1, TIME, BOUNDS, COLOR, SUBV, SUBC, PRINT, OT, BASE, PUSH,
       P3, P4, LS, COPY, TRANS, ROTATE, MUL, SCALE, PROJECT, SUBMIT_PACKET, LINE,
       POP, KINDS, FAILURE = 79 };

VECTOR D_8001185C = {4096,4096,4096,0};
char D_8001186C[] = "%d (0x%x) Byte\n";
u32 D_800915E8[7];
static u32 alternate[7];
static u8 storage[TOTAL] __attribute__((aligned(16)));
static u8 initial[TOTAL], final_state[TOTAL], initial_config[56], final_config[56];
static u8 *object = storage + GUARD;
static GsOT ordering_table;
static MATRIX base_matrix;
static s32 bounds_value, step_value, stress, reference_run;
static u32 active_case, cases, calls, projection_calls, packets, lines, mutations;
static u32 seen[KINDS], event_count, event_cursor, clears;
typedef struct {
    s32 kind, arguments[8];
    u32 size;
    u8 data[64], state[TOTAL], config[56];
} Event;
static Event events[CAPACITY];

s16 *native_8006BCA4(s16 *,s16 *,s16 *,s16 *,s32);
Triplet *native_8006C120(u8 *,u8 *,u8 *,Triplet *,s32);
void native_8006C2FC(u8 *,s32,s32,s32);

static void bytes(void *out, const void *in, u32 size)
{
    u8 *d=out; const u8 *s=in;
    while (size--) *d++=*s++;
}
static void fill(void *out, u8 value, u32 size)
{
    u8 *d=out;
    while (size--) *d++=value;
}
static s32 equal(const void *left, const void *right, u32 size)
{
    const u8 *a=left,*b=right;
    while (size--) if (*a++ != *b++) return 0;
    return 1;
}
static void write_text(const char *text, u32 length)
{
    __asm__ volatile("int $0x80" : : "a"(4),"b"(1),"c"(text),"d"(length) : "memory","cc");
}
static void number(u32 value)
{
    char buffer[16]; u32 n=0,i;
    do { buffer[n++]='0'+value%10; value/=10; } while(value);
    for(i=0;i<n/2;i++) {
        char c=buffer[i]; buffer[i]=buffer[n-i-1]; buffer[n-i-1]=c;
    }
    buffer[n++]='\n'; write_text(buffer,n);
}
static void fail(u32 reason)
{
    write_text("mismatch: ",10); number(reason);
    write_text("case: ",6); number(active_case);
    write_text("event: ",7); number(event_cursor);
    __asm__ volatile("int $0x80" : : "a"(1),"b"(FAILURE) : "memory","cc");
    __builtin_unreachable();
}
static u32 get16(const void *memory)
{
    const u8 *p=memory; return p[0] | ((u32)p[1]<<8);
}
static u32 get32(const void *memory)
{
    const u8 *p=memory; return get16(p) | (get16(p+2)<<16);
}
static s32 signed16(const void *memory)
{
    u32 n=get16(memory); return n<32768 ? n : (s32)n-65536;
}
static void put16(void *memory,u32 value)
{
    u8 *p=memory; p[0]=value; p[1]=value>>8;
}
static void put32(void *memory,u32 value)
{
    u8 *p=memory; put16(p,value); put16(p+2,value>>16);
}
static s32 location(const void *pointer)
{
    u32 address=(u32)pointer, begin=(u32)object;
    if(address>=begin && address<=begin+BYTES) return address-begin;
    fail(1); return 0;
}
static void config_snapshot(u8 *out)
{
    bytes(out,D_800915E8,28); bytes(out+28,alternate,28);
}
static void hook(s32 kind,u32 occurrence)
{
    if(!stress) return;
    if((stress&1) && kind==TIME) put32(object+0x1310,3);
    if((stress&2) && kind==OT) put32(object,(u32)alternate);
    if((stress&4) && kind==PROJECT && occurrence==0) {
        put32(object+0x1310,51); put32(object+0x1314,43);
        put16((u8 *)D_800915E8+24,17);
    }
    if((stress&8) && kind==POP) {
        put32(object+0x1310,500); put32(object+0x1314,2);
    }
    if((stress&16) && kind==BOUNDS) {
        put16((u8 *)D_800915E8+18,444);
        put16((u8 *)D_800915E8+20,555);
        put16((u8 *)D_800915E8+24,31);
    }
}
static u32 event(s32 kind,const s32 *arguments,const void *data,u32 size)
{
    Event *e;
    u8 config[56];
    u32 occurrence=seen[kind]++;
    if(size>64) fail(2);
    config_snapshot(config);
    if(reference_run) {
        if(event_count==CAPACITY) fail(3);
        e=&events[event_count++];
        e->kind=kind; e->size=size;
        bytes(e->arguments,arguments,32);
        bytes(e->data,data,size);
        bytes(e->state,storage,TOTAL); bytes(e->config,config,56);
    } else {
        if(event_cursor==event_count) fail(4);
        e=&events[event_cursor++];
        if(e->kind!=kind || e->size!=size || !equal(e->arguments,arguments,32)
           || !equal(e->data,data,size)) fail(5);
        if(!equal(e->state,storage,TOTAL) || !equal(e->config,config,56)) fail(6);
        calls++;
    }
    hook(kind,occurrence);
    return occurrence;
}
static u32 simple(s32 kind)
{
    s32 args[8]={0}; return event(kind,args,0,0);
}
static void matrix_bytes(u8 *out,const MATRIX *m)
{
    bytes(out,m->m,18); bytes(out+18,m->t,12);
}
void *memset(void *out,int value,int size)
{
    if(size!=8 || value!=0) fail(7);
    clears++; fill(out,value,size); return out;
}
s32 Model_GetActiveSlotIndex(void) { return simple(SLOT)&1; }
int Model_GetFrameStep(void) { simple(TIME); return step_value; }
void func_80057E20(s32 slot,ModelEffectAdjustment *out)
{
    s32 args[8]={0}; args[0]=slot; event(BOUNDS,args,0,0);
    put16((u8 *)out,73); put16((u8 *)out+2,121);
    put16((u8 *)out+4,31); put16((u8 *)out+6,bounds_value);
}
void func_8006C2FC(u8 *out,s32 r,s32 g,s32 b)
{
    s32 args[8]={0};
    args[0]=location(out); args[1]=r; args[2]=g; args[3]=b;
    event(COLOR,args,0,0); native_8006C2FC(out,r,g,b);
}
s16 *func_8006BCA4(s16 *a,s16 *b,s16 *c,s16 *out,s32 depth)
{
    s32 args[8]={0};
    args[0]=location(a); args[1]=location(b); args[2]=location(c);
    args[3]=location(out); args[4]=depth;
    if(depth!=2 || args[3]<0x94 || args[3]+384>0xC94) fail(8);
    event(SUBV,args,0,0); return native_8006BCA4(a,b,c,out,depth);
}
Triplet *func_8006C120(u8 *a,u8 *b,u8 *c,Triplet *out,s32 depth)
{
    s32 args[8]={0};
    args[0]=location(a); args[1]=location(b); args[2]=location(c);
    args[3]=location(out); args[4]=depth;
    if(depth!=2 || args[3]<0xD0C || args[3]+192>0x130C) fail(9);
    event(SUBC,args,0,0); return native_8006C120(a,b,c,out,depth);
}
int printf(const char *format,...)
{
    __builtin_va_list ap;
    s32 args[8]={0};
    __builtin_va_start(ap,format);
    args[0]=__builtin_va_arg(ap,int); args[1]=__builtin_va_arg(ap,int);
    __builtin_va_end(ap);
    if(format!=D_8001186C) fail(10);
    event(PRINT,args,format,16); return 0;
}
GsOT *func_80058F10(void) { simple(OT); return &ordering_table; }
void *Model_GetLightSourceMatrix(void) { simple(BASE); return &base_matrix; }
void PushMatrix(void) { simple(PUSH); }
void PopMatrix(void) { simple(POP); }
void SetPolyG3(POLY_G3 *p)
{
    simple(P3); fill(p,0x6D,sizeof(*p));
    p->tag=0x06000000; p->code=0x30;
}
void SetPolyG4(POLY_G4 *p)
{
    simple(P4); fill(p,0x8B,sizeof(*p));
    p->tag=0x08000000; p->code=0x38;
}
void GsSetLsMatrix(MATRIX *m)
{
    s32 args[8]={0}; u8 data[30]; matrix_bytes(data,m); event(LS,args,data,30);
}
void Model_CopySlotU16Values(s32 slot,u16 *out)
{
    s32 args[8]={0}; args[0]=slot; event(COPY,args,0,0);
    out[0]=19; out[1]=(u16)-27; out[2]=41; out[3]=0x6A2B;
}
void RotTrans(SVECTOR *input,VECTOR *output,long *flag)
{
    s32 args[8]={0};
    event(TRANS,args,input,8);
    output->vx=input->vx*2+7;
    output->vy=input->vy*3-11;
    output->vz=input->vz*4+13;
    *flag=0x123;
}
MATRIX *RotMatrix(SVECTOR *rotation,MATRIX *m)
{
    s32 args[8]={0}; s32 i,j;
    event(ROTATE,args,rotation,8);
    for(i=0;i<3;i++) for(j=0;j<3;j++)
        m->m[i][j]=(rotation->vy+i*71+j*19)&2047;
    return m;
}
MATRIX *MulMatrix2(MATRIX *a,MATRIX *b)
{
    s32 args[8]={0}; u8 data[60]; s32 i,j;
    matrix_bytes(data,a); matrix_bytes(data+30,b); event(MUL,args,data,60);
    for(i=0;i<3;i++) for(j=0;j<3;j++) b->m[i][j]+=a->m[i][j];
    return b;
}
MATRIX *ScaleMatrix(MATRIX *m,VECTOR *scale)
{
    s32 args[8]={0}; u8 data[46]; s32 i,j;
    matrix_bytes(data,m); bytes(data+30,scale,16); event(SCALE,args,data,46);
    for(i=0;i<3;i++) for(j=0;j<3;j++) m->m[i][j]=m->m[i][j]*scale->vx/4096;
    return m;
}
long RotAverageNclip3(SVECTOR *a,SVECTOR *b,SVECTOR *c,long *p0,long *p1,
                     long *p2,long *p,long *depth,long *flag)
{
    static const s32 depths[6]={-7,0,1,65535,65536,131073};
    s32 args[8]={0}; u8 data[24]; u32 n;
    args[0]=location(a); args[1]=location(b); args[2]=location(c);
    bytes(data,a,8); bytes(data+8,b,8); bytes(data+16,c,8);
    n=event(PROJECT,args,data,24);
    *p0=(u16)(a->vx+n) | ((u32)(u16)(a->vy-n)<<16);
    *p1=(u16)(b->vx+n+3) | ((u32)(u16)(b->vy+n+5)<<16);
    *p2=(u16)(c->vx-n-7) | ((u32)(u16)(c->vy+n+11)<<16);
    *p=n*17; *depth=depths[(n+active_case)%6];
    *flag=(n+active_case)%5==0 ? (s32)0x80000000 : 0x20;
    if(!reference_run) projection_calls++;
    return (n+active_case)%4==0 ? 0 : ((n+active_case)%4==1 ? -3 : 23);
}
void func_8005B260(u32 *packet,GsOT *ot,s32 depth,s32 flags)
{
    s32 args[8]={0}; args[0]=ot==&ordering_table; args[1]=depth; args[2]=flags;
    event(SUBMIT_PACKET,args,packet,28); if(!reference_run) packets++;
}
void GsSortLine(GsLINE *line,GsOT *ot,unsigned short depth)
{
    s32 args[8]={0}; args[0]=ot==&ordering_table; args[1]=depth;
    event(LINE,args,line,15); if(!reference_run) lines++;
}

/* The oracle addresses the verified record directly, independently of its
   implementation type. Accepted subdivision code is shared as a dependency. */
static s32 oracle(s32 mode)
{
    MATRIX base,matrix;
    POLY_G3 triangle; POLY_G4 quad;
    GsLINE line;
    SVECTOR rotation,position;
    VECTOR scale;
    u8 bounds[8],*config,*color;
    SVECTOR *vertex;
    GsOT *ot;
    s32 ticks,i,j,radius,height,elapsed,remaining,facing;
    long flag,interpolation,depth;
    memset(&rotation,0,8); memset(&position,0,8);
    scale=D_8001185C; Model_GetActiveSlotIndex(); ticks=Model_GetFrameStep();
    if(mode>=0) {
        config=(u8 *)D_800915E8; put32(object,(u32)config);
        func_80057E20(Model_GetActiveSlotIndex(),(ModelEffectAdjustment *)bounds);
        radius=height=signed16(bounds+6)/2;
        if(radius<signed16(config+20)) radius=signed16(config+20);
        else if(radius>700) radius=700;
        if(height<signed16(config+18)) height=signed16(config+18);
        else if(height>700) height=700;
        for(i=0;i<6;i++) {
            put16(object+4+i*8, i==2 ? -radius : i==4 ? radius : 0);
            put16(object+6+i*8, i==0 ? -height : i==5 ? height : 0);
            put16(object+8+i*8, i==1 ? -radius : i==3 ? radius : 0);
        }
        for(i=0;i<6;i++) func_8006C2FC(object+0xC94+i*4,config[i],config[i+6],config[i+12]);
        for(i=0;i<8;i++) {
            s32 corners[3],side=i%4+1,next=side%4+1;
            corners[0]=i<4 ? 0 : 5;
            corners[1]=i<4 ? side : next;
            corners[2]=i<4 ? next : side;
            for(j=0;j<3;j++) {
                put32(object+0x34+(i*3+j)*4,(u32)(object+4+corners[j]*8));
                put32(object+0xCAC+(i*3+j)*4,(u32)(object+0xC94+corners[j]*4));
            }
        }
        for(i=0;i<8;i++) {
            s16 *a=(s16 *)get32(object+0x34+i*12);
            s16 *b=(s16 *)get32(object+0x38+i*12);
            s16 *c=(s16 *)get32(object+0x3C+i*12);
            func_8006BCA4(a,b,c,(s16 *)(object+0x94+i*384),2);
            func_8006C120((u8 *)get32(object+0xCAC+i*12),
                (u8 *)get32(object+0xCB0+i*12),(u8 *)get32(object+0xCB4+i*12),
                (Triplet *)(object+0xD0C+i*192),2);
        }
        put32(object+0x1310,0); put32(object+0x1314,signed16(config+24));
        object[0x130C]=0; object[0x130D]=0;
        printf(D_8001186C,0x1318,0x1318); return 0;
    }
    config=(u8 *)get32(object); ot=func_80058F10();
    base=*(MATRIX *)Model_GetLightSourceMatrix(); PushMatrix();
    SetPolyG3(&triangle); SetPolyG4(&quad); line.attribute=0x50000000;
    elapsed=(s32)get32(object+0x1310); remaining=(s32)get32(object+0x1314);
    rotation.vx=0; rotation.vy=(u32)elapsed<<5; rotation.vz=0;
    GsSetLsMatrix(&base);
    Model_CopySlotU16Values(Model_GetActiveSlotIndex(),(u16 *)&position);
    GsSetLsMatrix(&base); RotTrans(&position,(VECTOR *)matrix.t,&flag);
    RotMatrix(&rotation,&matrix); MulMatrix2(&base,&matrix);
    ScaleMatrix(&matrix,&scale); GsSetLsMatrix(&matrix);
    color=object+0xD0C;
    for(i=0;i<128;i++) {
        s32 out[3][3],factor=0,duration=1,growth=0;
        vertex=(SVECTOR *)(object+0x94+i*24);
        facing=RotAverageNclip3(vertex,vertex+1,vertex+2,
            (long *)&triangle.x0,(long *)&triangle.x1,(long *)&triangle.x2,
            &interpolation,&depth,&flag);
        if(remaining>=0 && remaining<signed16(config+24)) {
            factor=remaining; duration=signed16(config+24);
        } else if(elapsed>=0 && elapsed<signed16(config+22)) {
            factor=elapsed; duration=signed16(config+22); growth=1;
        } else factor=-1;
        for(j=0;j<3;j++) {
            s32 k,index=growth && j==2 ? 1 : j;
            for(k=0;k<3;k++) out[j][k]=factor<0 ? color[index*4+k] :
                color[index*4+k]*factor/duration;
        }
        triangle.r0=out[0][0]; triangle.g0=out[0][1]; triangle.b0=out[0][2];
        triangle.r1=out[1][0]; triangle.g1=out[1][1]; triangle.b1=out[1][2];
        triangle.r2=out[2][0]; triangle.g2=out[2][1]; triangle.b2=out[2][2];
        color+=12;
        if(depth>=0 && flag>=0 && facing>0) func_8005B260((u32 *)&triangle,ot,(u16)depth,1);
    }
    {
        s32 shade=32;
        if(remaining>=0 && remaining<signed16(config+24)) shade=remaining*32/signed16(config+24);
        else if(elapsed>=0 && elapsed<signed16(config+22)) shade=elapsed*32/signed16(config+22);
        line.r=shade; line.g=shade; line.b=shade;
    }
    for(i=0;i<8;i++) {
        facing=RotAverageNclip3((SVECTOR *)get32(object+0x34+i*12),
            (SVECTOR *)get32(object+0x38+i*12),(SVECTOR *)get32(object+0x3C+i*12),
            (long *)&triangle.x0,(long *)&triangle.x1,(long *)&triangle.x2,
            &interpolation,&depth,&flag);
        if(depth>=0 && flag>=0 && facing>0) {
            line.x0=triangle.x0; line.y0=triangle.y0;
            line.x1=triangle.x1; line.y1=triangle.y1;
            GsSortLine(&line,ot,(u16)depth);
            line.x0=triangle.x2; line.y0=triangle.y2; GsSortLine(&line,ot,(u16)depth);
            line.x1=triangle.x0; line.y1=triangle.y0; GsSortLine(&line,ot,(u16)depth);
        }
    }
    PopMatrix();
    put32(object+0x1310,get32(object+0x1310)+(u32)ticks);
    if(mode==-2) put32(object+0x1314,get32(object+0x1314)-(u32)ticks);
    return (s32)get32(object+0x1314)<0 ? 2 : 0;
}
static void prepare(s32 growth,s32 fade,s32 elapsed,s32 remaining)
{
    u32 i,j;
    static const u32 original[7]={0x0000A0A0,0x00A000A0,0xA0A000A0,
        0xA00000A0,0x012CA000,0x000A012C,0x0000005A};
    for(i=0;i<TOTAL;i++) storage[i]=(i*37+active_case*13+0xA5)&255;
    bytes(D_800915E8,original,28);
    put16((u8 *)D_800915E8+22,growth); put16((u8 *)D_800915E8+24,fade);
    bytes(alternate,D_800915E8,28);
    put16((u8 *)alternate+22,3); put16((u8 *)alternate+24,11);
    put32(object,(u32)D_800915E8);
    put32(object+0x1310,elapsed); put32(object+0x1314,remaining);
    for(i=0;i<6;i++) for(j=0;j<3;j++) put16(object+4+i*8+j*2,(i*53+j*97)%401-200);
    for(i=0;i<8;i++) for(j=0;j<3;j++) {
        u32 side=i%4+1,next=side%4+1,index;
        index=j==0 ? (i<4 ? 0 : 5) : j==1 ? (i<4 ? side : next) : (i<4 ? next : side);
        put32(object+0x34+(i*3+j)*4,(u32)(object+4+index*8));
    }
    for(i=0;i<384;i++) for(j=0;j<3;j++)
        put16(object+0x94+i*8+j*2,(i*31+j*71)%601-300);
    for(i=0;i<384;i++) for(j=0;j<3;j++)
        object[0xD0C+i*4+j]=(i*67+j*103+active_case)&255;
    fill(&base_matrix,0,sizeof(base_matrix));
    for(i=0;i<3;i++) for(j=0;j<3;j++) base_matrix.m[i][j]=i*13+j*29+47;
    base_matrix.t[0]=71; base_matrix.t[1]=-83; base_matrix.t[2]=97;
}
static void run_case(s32 mode)
{
    s32 expected,actual;
    bytes(initial,storage,TOTAL); config_snapshot(initial_config);
    fill(seen,0,sizeof(seen)); event_count=event_cursor=clears=0; reference_run=1;
    expected=oracle(mode);
    if(clears!=2) fail(11);
    bytes(final_state,storage,TOTAL); config_snapshot(final_config);
    bytes(storage,initial,TOTAL); bytes(D_800915E8,initial_config,28);
    bytes(alternate,initial_config+28,28);
    fill(seen,0,sizeof(seen)); event_cursor=clears=0; reference_run=0;
    actual=func_8006AF74((ModelSubdividedEffect *)object,mode);
    if(actual!=expected || event_cursor!=event_count || clears!=2) fail(12);
    if(!equal(storage,final_state,TOTAL)) fail(13);
    {
        u8 config[56]; config_snapshot(config);
        if(!equal(config,final_config,56)) fail(14);
    }
    if(!equal(storage,initial,GUARD) || !equal(object+BYTES,initial+GUARD+BYTES,GUARD)) fail(15);
    cases++; if(stress) mutations++;
    active_case++;
}
int main(void)
{
    static const s32 values[12]={-32768,-3,-1,0,1,2,3,599,600,1399,1400,32767};
    static const s32 minima[6][2]={{-2,-3},{0,0},{300,300},{700,701},{1000,2000},{-32768,32767}};
    static const s32 durations[6][2]={{10,90},{1,1},{0,0},{-1,-1},{7,13},{32767,32767}};
    static const s32 steps[5]={0,1,6,255,256};
    s32 a,b,c,d,mode,e[6],r[6];
    for(a=0;a<12;a++) for(b=0;b<6;b++) for(mode=0;mode<2;mode++) {
        prepare(10,90,0,0); bounds_value=values[a]; step_value=steps[active_case%5]; stress=0;
        put16((u8 *)D_800915E8+18,minima[b][0]);
        put16((u8 *)D_800915E8+20,minima[b][1]);
        run_case(mode ? 999 : 0);
    }
    for(a=0;a<6;a++) {
        e[0]=-1;e[1]=0;e[2]=1;e[3]=durations[a][0]-1;e[4]=durations[a][0];e[5]=durations[a][0]+1;
        r[0]=-1;r[1]=0;r[2]=1;r[3]=durations[a][1]-1;r[4]=durations[a][1];r[5]=durations[a][1]+1;
        for(b=0;b<6;b++) for(c=0;c<6;c++) for(d=0;d<2;d++) {
            prepare(durations[a][0],durations[a][1],e[b],r[c]);
            step_value=steps[active_case%5]; stress=0; run_case(d ? -2 : -1);
        }
    }
    for(a=1;a<=31;a++) {
        stress=a; step_value=6;
        prepare(10,90,2,23); bounds_value=1200; run_case(-2);
        prepare(10,90,2,23); run_case(0);
    }
    write_text("cases: ",7);number(cases);
    write_text("callbacks: ",11);number(calls);
    write_text("projections: ",13);number(projection_calls);
    write_text("packets: ",9);number(packets);
    write_text("lines: ",7);number(lines);
    write_text("mutation scenarios: ",20);number(mutations);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class ModelSubdividedEffectTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation:
                text = SOURCE.read_text()
                changes = {
                    "color-stride": ("color += 2;", "color++;", 1),
                    "retire-at-zero": (
                        "return (effect->remaining < 0) * 2;",
                        "return (effect->remaining <= 0) * 2;",
                        1,
                    ),
                    "link-wrap": ("&effect->vertices[i % 4] + 1", "&effect->vertices[i % 5] + 1", 2),
                    "late-config": (
                        "ot = func_80058F10();",
                        "ot = func_80058F10();\n        config = effect->config;",
                        1,
                    ),
                }
                before, after, count = changes[mutation]
                self.assertEqual(text.count(before), count)
                text = text.replace(before, after)
                text = re.sub(
                    r'^#include "([^"]+)"',
                    lambda m: '#include "' + str((SOURCE.parent / m[1]).resolve()) + '"',
                    text, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(FIXTURE)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", optimization, "-I", str(ROOT),
            ]
            renames = [
                "-Dfunc_800" + suffix + "=native_800" + suffix
                for suffix in ("6BCA4", "6C120", "6C2FC", "6C30C", "6C330")
            ]
            objects = []
            for name, path, extra in (
                ("game", source, []), ("fixture", fixture, []), ("start", start, []),
                ("subdivision", ROOT / "src/game/triangle_subdivision.c", renames),
            ):
                obj = directory / (name + ".o")
                result = subprocess.run(
                    ["cc", *flags, *extra, "-c", str(path), "-o", str(obj)],
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

    def test_effect_contract(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "cases: 638\ncallbacks: 99033\nprojections: 62968\n"
                    "packets: 19755\nlines: 3714\nmutation scenarios: 62\n",
                )

    def test_source_mutations_are_rejected(self):
        for mutation in ("color-stride", "retire-at-zero", "link-wrap", "late-config"):
            for optimization in ("-O0", "-O2"):
                with self.subTest(mutation=mutation, optimization=optimization):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 79, result.stdout + result.stderr)
                    self.assertTrue(result.stdout.startswith("mismatch: "), result.stdout)


if __name__ == "__main__":
    unittest.main()
