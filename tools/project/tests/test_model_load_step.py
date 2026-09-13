"""Independent ILP32 oracle for the eleven-stage model-load dispatcher."""

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/model_load_step.c"
SCRATCH = ROOT / "tmp/astra/model-load-step-witness"
START = r"""
.text
.globl _start
_start:
    andl $-16, %esp
    call main
    movl %eax, %ebx
    movl $1, %eax
    int $0x80
.balign 4
.globl primary0_base
primary0_base:
    .long 0
    jmp primary0_body
.balign 4
.globl primary1_base
primary1_base:
    .long 0
    jmp primary1_body
.section .note.GNU-stack,"",@progbits
"""
FIXTURE = r"""
#include "src/types.h"
#include "src/psyq/libetc.h"
#include "src/psyq/stdio.h"
#include "src/game/model_load_step.h"
#include "src/game/model.h"
#include "src/game/model_graphics_state.h"
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "src/game/high_memory_addresses.h"
#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS
#include "src/game/model_slot_setup.h"
#include "src/game/model_slot_row_tables.h"
#include "src/game/model_slot_support.h"
#include "src/game/model_slot_updates.h"
#include "src/game/model_transfer_flags.h"
#include "src/game/func_8004D914.h"
#include "src/game/func_800582C0.h"
#include "src/game/func_8004DC38.h"
#include "src/game/sound_voice_data.h"
#include "src/game/script_state.h"

typedef char target_pointer_width[sizeof(void *)==4 ? 1 : -1];
enum { SLOT_SIZE=0xE20, SLOTS=3, PARTS=58, PART_SIZE=28, GUARD=32,
       SLOT_BYTES=3*0xE20+64, PART_BYTES=3*58*28+64, GLOBAL_BYTES=36,
       SNAPSHOT=SLOT_BYTES+PART_BYTES+GLOBAL_BYTES,
       CLOCK=1, LOAD, ROWS, LINK, POSITION, SETUP, SPEED, TINT, GATE,
       MODULE0, MODULE1, SEEK, SOUND, PRINT, KINDS, FAILURE=81 };
u8 slots_memory[SLOT_BYTES] __attribute__((aligned(16)));
static u8 parts_memory[PART_BYTES] __attribute__((aligned(16)));
__asm__(".globl D_800F2C40\n.set D_800F2C40, slots_memory+32\n");
static u8 contexts[4][32];
static u32 payloads[3][0x3000];
u8 D_801A8000[3*0x800] __attribute__((aligned(16)));
u8 D_8009AFA0;
u8 *D_80010000;
u8 *D_80010004;
s32 D_80010008;
u8 *D_8001000C;
u8 *D_80010010;
extern u8 primary0_base[],primary1_base[];
char D_80011594[]="LoadModelDO(%d)=%03X\n";

static u8 initial[SNAPSHOT],expected[SNAPSHOT];
typedef struct { s32 kind,args[5]; u8 snapshot[SNAPSHOT]; } Event;
static Event events[80];
static u32 seen[KINDS],event_count,event_cursor;
static u32 cases,calls,module_calls,seek_calls,layout_calls,mutation_cases;
static s32 current_index,reference_run,stress,clock_override,clock_base,clock_step;
static u32 case_number;
static void bytes(void *out,const void *in,u32 size)
{
    u8 *d=out; const u8 *s=in;
    while(size--) *d++=*s++;
}
static void fill(void *out,u8 value,u32 size)
{
    u8 *p=out; while(size--) *p++=value;
}
static s32 equal(const void *a0,const void *b0,u32 size)
{
    const u8 *a=a0,*b=b0;
    while(size--) if(*a++!=*b++) return 0;
    return 1;
}
static u32 get16(const void *memory)
{
    const u8 *p=memory; return p[0]|((u32)p[1]<<8);
}
static u32 get32(const void *memory)
{
    const u8 *p=memory; return get16(p)|(get16(p+2)<<16);
}
static void put16(void *memory,u32 n)
{
    u8 *p=memory; p[0]=n; p[1]=n>>8;
}
static void put32(void *memory,u32 n)
{
    u8 *p=memory; put16(p,n); put16(p+2,n>>16);
}
static u8 *slot(s32 index) { return slots_memory+GUARD+index*SLOT_SIZE; }
static u8 *part(s32 index,s32 n) { return parts_memory+GUARD+(index*PARTS+n)*PART_SIZE; }
static void text(const char *s,u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4),"b"(1),"c"(s),"d"(n) : "memory","cc");
}
static void number(u32 n)
{
    char out[16]; u32 size=0,i;
    do { out[size++]='0'+n%10; n/=10; } while(n);
    for(i=0;i<size/2;i++) { char c=out[i];out[i]=out[size-i-1];out[size-i-1]=c; }
    out[size++]='\n'; text(out,size);
}
static void fail(u32 reason)
{
    text("mismatch: ",10);number(reason);
    text("case: ",6);number(case_number);
    text("event: ",7);number(event_cursor);
    __asm__ volatile("int $0x80" : : "a"(1),"b"(FAILURE) : "memory","cc");
    __builtin_unreachable();
}
static void snapshot(u8 *out)
{
    u8 *g=out+SLOT_BYTES+PART_BYTES;
    bytes(out,slots_memory,SLOT_BYTES);
    bytes(out+SLOT_BYTES,parts_memory,PART_BYTES);
    put32(g,D_8009AFA0);
    put32(g+4,(u32)D_80010000);put32(g+8,(u32)D_80010004);put32(g+12,D_80010008);
    put32(g+16,(u32)D_8001000C);put32(g+20,(u32)D_80010010);
    put32(g+24,payloads[0][0]);put32(g+28,payloads[1][0]);put32(g+32,payloads[2][0]);
}
static void restore(const u8 *in)
{
    const u8 *g=in+SLOT_BYTES+PART_BYTES;
    bytes(slots_memory,in,SLOT_BYTES);bytes(parts_memory,in+SLOT_BYTES,PART_BYTES);
    D_8009AFA0=get32(g);
    D_80010000=(u8 *)get32(g+4);D_80010004=(u8 *)get32(g+8);D_80010008=get32(g+12);
    D_8001000C=(u8 *)get32(g+16);D_80010010=(u8 *)get32(g+20);
    payloads[0][0]=get32(g+24);payloads[1][0]=get32(g+28);payloads[2][0]=get32(g+32);
}
static void hook(s32 kind,u32 occurrence,const s32 *args)
{
    u8 *p=slot(current_index);
    if((stress&1) && kind==CLOCK && occurrence==0) p[0xE14]=clock_override;
    if((stress&2) && kind==CLOCK && occurrence==1) { p[0xE14]=254;p[0xE1F]=0; }
    if((stress&4) && kind==PRINT) { p[0xE14]=255;p[0xE1F]=0; }
    if((stress&8) && kind==PRINT) p[0xE1F]=7;
    if((stress&16) && kind==GATE && args[0]==1) {
        put32(p+0xD10,(u32)-1001);put32(p+0xDE8,(u32)contexts[3]);
        D_8001000C=primary1_base;D_80010010=primary0_base;
    }
    if((stress&32) && kind==GATE && args[0]==0) {
        p[0xBF5]=0;put16(p+0xE06,65535);p[0xE1B]=1;
    }
    if((stress&64) && kind==SEEK) {
        p[0xBF5]=7;
        p[0xE1B]=occurrence==0 ? 3 : 1;
    }
    if((stress&128) && (kind==LOAD || kind==ROWS || kind==LINK || kind==SETUP)) {
        p[0xE14]=201;p[0xE1F]=1;
    }
}
static u32 event(s32 kind,const s32 *args)
{
    u32 occurrence=seen[kind]++;
    Event *e; u8 now[SNAPSHOT];
    snapshot(now);
    if(reference_run) {
        if(event_count==80) fail(1);
        e=&events[event_count++];e->kind=kind;
        bytes(e->args,args,20);bytes(e->snapshot,now,SNAPSHOT);
    } else {
        if(event_cursor==event_count) fail(2);
        e=&events[event_cursor++];
        if(e->kind!=kind || !equal(e->args,args,20)) fail(3);
        if(!equal(e->snapshot,now,SNAPSHOT)) fail(4);
        calls++;
    }
    hook(kind,occurrence,args);
    return occurrence;
}
int VSync(int mode)
{
    s32 args[5]={0};u32 n;
    args[0]=mode;n=event(CLOCK,args);
    return clock_base+(s32)n*clock_step;
}
int printf(const char *format,...)
{
    __builtin_va_list ap;
    s32 args[5]={0};
    if(format!=D_80011594) fail(5);
    __builtin_va_start(ap,format);
    args[0]=__builtin_va_arg(ap,int);args[1]=__builtin_va_arg(ap,int);
    __builtin_va_end(ap);
    event(PRINT,args);return 27;
}
void func_8004CB0C(s32 index,s32 data,s32 size,s32 last)
{
    s32 args[5]={0};args[0]=index;args[1]=data;args[2]=size;args[3]=last;
    event(LOAD,args);
}
void func_8004D75C(s32 index) { s32 a[5]={0};a[0]=index;event(ROWS,a); }
void func_8004D914(s32 index) { s32 a[5]={0};a[0]=index;event(LINK,a); }
void func_800590DC(s32 index) { s32 a[5]={0};a[0]=index;event(SETUP,a); }
void func_8005A4C4(ModelSlot *p,s32 a,s32 b,s32 c,s32 d)
{
    s32 args[5];args[0]=(u8 *)p-(slots_memory+GUARD);
    args[1]=a;args[2]=b;args[3]=c;args[4]=d;event(POSITION,args);
    if(!reference_run) layout_calls++;
}
void func_8005A468(s32 index,s32 speed)
{
    s32 args[5]={0};args[0]=index;args[1]=speed;event(SPEED,args);
}
void func_800582C0(s32 index,s32 tint,s32 level)
{
    s32 args[5]={0};args[0]=index;args[1]=tint;args[2]=level;event(TINT,args);
}
void func_8005F198(s32 value)
{
    s32 args[5]={0};args[0]=value;event(GATE,args);
}
static s32 module(s32 kind,u8 *context,s32 command)
{
    s32 args[5]={0};args[0]=(s32)context;args[1]=command;event(kind,args);
    if(!reference_run) module_calls++;
    return 17;
}
s32 primary0_body(u8 *context,s32 command) { return module(MODULE0,context,command); }
s32 primary1_body(u8 *context,s32 command) { return module(MODULE1,context,command); }
void func_8004DC38(ModelSlot *p,s32 i,s32 n,u32 position)
{
    s32 args[5]={0};
    args[0]=(u8 *)p-(slots_memory+GUARD);args[1]=i;args[2]=n;args[3]=position;
    if(i<0 || i>=58) fail(6);
    event(SEEK,args);if(!reference_run) seek_calls++;
}
void func_80048D08(s32 index,u32 *data)
{
    s32 args[5]={0};args[0]=index;args[1]=(u8 *)data-D_801A8000;event(SOUND,args);
}

static void oracle(s32 index)
{
    u8 *p=slot(index);
    s32 start=VSync(1),phase=p[0xE14],i,amount;
    if(phase==0 || phase==255) return;
    switch(phase) {
    case 1: {
        u8 *data=(u8 *)D_80010008;
        s32 size=0xC000;
        if(index==0) data=D_80010000;
        else if(index==1) data=D_80010004;
        if(get32(data)!=0) size=(s32)get32(data);
        func_8004CB0C(index,(s32)data,size,-1);break;
    }
    case 2: func_8004D75C(index);break;
    case 3: func_8004D914(index);break;
    case 4: {
        s32 count=p[0xE1B];u32 end;
        amount=0;
        if(index<2) for(i=0;i<count;i++) {
            if(get16(p+0x33C+i*2)!=65535)
                amount+=((p[0xBEC+i/8]>>(i%8))&1) ? 20 : 12;
        }
        end=get32(p+0xDE0)+amount;
        put32(p+0xDF0,end);put32(p+0xDF4,end+get16(p+0xE02)*4);
        func_8005A4C4((ModelSlot *)p,0,0,0,index==1 ? 0x800 : 0);break;
    }
    case 5:
        for(i=0;i<p[0xE1B];i++) {
            u8 *q=(u8 *)get32(p+0x1E0+i*4);
            put16(q+8,65535);
            q=(u8 *)get32(p+0x1E0+i*4);put16(q+0x16,get16(q+0x18));
            q=(u8 *)get32(p+0x1E0+i*4);put16(q+0xA,65535);
            q=(u8 *)get32(p+0x1E0+i*4);q[0xC]=q[0x1A];
            q=(u8 *)get32(p+0x1E0+i*4);put16(q+0x10,0);
            q=(u8 *)get32(p+0x1E0+i*4);q[0xD]=16;
        }
        break;
    case 6:
        p[0xE1F]=1;func_800590DC(index);p[0xE1F]=0;break;
    case 7:
        amount=p[0xE0D]*2;
        if(p[0xE16]==35) func_8005A468(index,0);
        else if(p[0xE16]==62) func_8005A468(index,amount);
        else if(p[0xE16]==60) func_8005A468(index,-amount);
        if(index>=2) p[0xE1F]=1;
        break;
    case 8: func_800582C0(index,p[0xE0C],get16(p+0xE0A));break;
    case 9: {
        s32 command=(s32)get32(p+0xD10),selected;
        u8 *context=(u8 *)get32(p+0xDE8),*base;
        u32 position;
        D_8009AFA0=index;
        if(command>=0) {
            base=index ? D_80010010 : D_8001000C;
            func_8005F198(1);
            command=(s32)get32(p+0xD10);
            if(base==primary0_base) primary0_body(context,command-(command/1000)*1000);
            else if(base==primary1_base) primary1_body(context,command-(command/1000)*1000);
            else fail(7);
            func_8005F198(0);
        }
        selected=p[0xBF5];p[0xE0E]=6;position=get16(p+0xE06);
        for(i=0;i<p[0xE1B];i++) {
            s32 current=p[0xBF5];
            if(selected) {
                ((u8 *)get32(p+0x1E0+i*4))[0xC]=selected;current=selected;
            }
            func_8004DC38((ModelSlot *)p,i,current,position);
        }
        if(selected) p[0xBF5]=selected;
        break;
    }
    case 10:
        if(p[0xE1D]==0) func_80048D08(index,(u32 *)(D_801A8000+index*2048));
        break;
    case 11:
        if(p[0xE1D]==0) p[0xE1E]=1;
        p[0xE1F]=1;break;
    }
    amount=VSync(1);printf(D_80011594,p[0xE14],amount-start);
    if(p[0xE1F]) p[0xE14]=255;
    else p[0xE14]=(p[0xE14]+1)&255;
}
static void prepare(s32 index,s32 phase,s32 count)
{
    s32 i,j;u8 *p;
    current_index=index;stress=0;clock_override=9;
    clock_base=2000+(case_number%97);clock_step=case_number%31;
    for(i=0;i<SLOT_BYTES;i++) slots_memory[i]=(i*37+case_number*13)&255;
    for(i=0;i<PART_BYTES;i++) parts_memory[i]=(i*29+case_number*7)&255;
    D_8009AFA0=2;
    D_80010000=(u8 *)payloads[0];D_80010004=(u8 *)payloads[1];D_80010008=(s32)payloads[2];
    D_8001000C=primary0_base;D_80010010=primary1_base;
    payloads[0][0]=0;payloads[1][0]=0x8000;payloads[2][0]=(u32)-1;
    for(i=0;i<3;i++) {
        p=slot(i);
        for(j=0;j<58;j++) {
            put32(p+0x1E0+j*4,(u32)part(i,j));
            put16(p+0x33C+j*2,j%3==0 ? 65535 : j);
        }
        for(j=0;j<8;j++) p[0xBEC+j]=(j*41+case_number)&255;
        p[0xBF5]=0;p[0xE14]=phase;p[0xE1F]=0;p[0xE1D]=0;p[0xE1B]=count;
        p[0xE0C]=7;p[0xE0D]=8;p[0xE16]=62;
        put32(p+0xDE0,(u32)payloads[i]);put32(p+0xDE8,(u32)contexts[i]);
        put32(p+0xD10,12345);put16(p+0xE02,17);put16(p+0xE06,0x8001);put16(p+0xE0A,4096);
    }
}
static void run(void)
{
    u8 actual[SNAPSHOT];
    snapshot(initial);fill(seen,0,sizeof(seen));
    event_count=event_cursor=0;reference_run=1;oracle(current_index);snapshot(expected);
    restore(initial);fill(seen,0,sizeof(seen));event_cursor=0;reference_run=0;
    func_80056828(current_index);snapshot(actual);
    if(event_cursor!=event_count) fail(8);
    if(!equal(actual,expected,SNAPSHOT)) fail(9);
    if(!equal(actual,initial,GUARD) ||
       !equal(actual+SLOT_BYTES-GUARD,initial+SLOT_BYTES-GUARD,GUARD) ||
       !equal(actual+SLOT_BYTES,initial+SLOT_BYTES,GUARD) ||
       !equal(actual+SLOT_BYTES+PART_BYTES-GUARD,initial+SLOT_BYTES+PART_BYTES-GUARD,GUARD)) fail(10);
    cases++;if(stress) mutation_cases++;case_number++;
}
int main(void)
{
    static const s32 counts[8]={0,1,7,8,9,31,57,58};
    static const s32 masks[4]={0,255,85,170};
    static const s32 modes[5]={0,35,60,62,255};
    static const s32 speeds[4]={0,1,128,255};
    static const s32 commands[8]={(s32)0x80000000,-1001,-1,0,1,999,1000,2147483647};
    static const s32 selected[3]={0,1,255};
    static const s32 seek_counts[4]={0,1,8,58};
    static const s32 stresses[13]={1,2,4,8,16,32,64,128,16|32|64,2|4,2|8,16|64,255};
    s32 index,phase,a,b,c,i;
    for(index=0;index<3;index++) for(phase=0;phase<256;phase++) {
        prepare(index,phase,4);run();
    }
    for(index=0;index<3;index++) for(a=0;a<8;a++) for(b=0;b<4;b++) {
        prepare(index,4,counts[a]);
        for(i=0;i<8;i++) slot(index)[0xBEC+i]=masks[b];
        for(i=0;i<58;i++) put16(slot(index)+0x33C+i*2,
            (i+b)%3==0 ? 65535 : (b&1) ? 0x8000+i : i);
        put16(slot(index)+0xE02,a%2 ? 65535 : 0);run();
        prepare(index,5,counts[a]);
        for(i=0;i<counts[a];i++) put32(slot(index)+0x1E0+i*4,(u32)part(index,i%7));
        run();
    }
    for(index=0;index<3;index++) for(a=0;a<5;a++) for(b=0;b<4;b++) {
        prepare(index,7,4);slot(index)[0xE16]=modes[a];slot(index)[0xE0D]=speeds[b];run();
    }
    for(index=0;index<3;index++) for(a=0;a<8;a++) for(b=0;b<3;b++) for(c=0;c<4;c++) {
        prepare(index,9,seek_counts[c]);put32(slot(index)+0xD10,commands[a]);
        slot(index)[0xBF5]=selected[b];put16(slot(index)+0xE06,b ? 65535 : 0);run();
    }
    for(index=0;index<3;index++) for(a=0;a<3;a++) for(phase=10;phase<=11;phase++) {
        prepare(index,phase,4);slot(index)[0xE1D]=selected[a];run();
    }
    for(index=0;index<3;index++) for(a=0;a<13;a++) for(b=0;b<3;b++) {
        prepare(index,b==0 ? 2 : b==1 ? 6 : 9,1);
        stress=stresses[a];clock_override=b==0 ? 0 : b==1 ? 255 : 9;run();
    }
    text("cases: ",7);number(cases);text("callbacks: ",11);number(calls);
    text("module calls: ",14);number(module_calls);text("seek calls: ",12);number(seek_calls);
    text("layout calls: ",14);number(layout_calls);text("mutation cases: ",16);number(mutation_cases);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class ModelLoadStepTests(unittest.TestCase):
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
                    "idle-state": ("state == 0 || state == 255", "state == 0"),
                    "bitfield-base": ("p->field_BEC[byte]", "*((u8 *)p + 0xBEC + i * 2 + byte)"),
                    "third-slot-limit": (
                        "amount = (p->field_DF0 = (s32)p->field_DE0 + sum);",
                        "amount = (s32)p->field_DE0 + sum;\n        if (index < 2) p->field_DF0 = amount;",
                    ),
                    "live-sequence-base": (
                        "s32 current = p->field_BF5;",
                        "s32 current = *((u8 *)p + 0xBF5 + i * 4);",
                    ),
                }
                before, after = changes[mutation]
                self.assertEqual(text.count(before), 1)
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

    def test_dispatcher_contract(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "cases: 1443\ncallbacks: 10071\nmodule calls: 222\n"
                    "seek calls: 4887\nlayout calls: 99\nmutation cases: 117\n",
                )

    def test_actual_source_controls(self):
        for mutation in ("idle-state", "bitfield-base", "third-slot-limit", "live-sequence-base"):
            for optimization in ("-O0", "-O2"):
                with self.subTest(mutation=mutation, optimization=optimization):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 81, result.stdout + result.stderr)
                    self.assertTrue(result.stdout.startswith("mismatch: "), result.stdout)


if __name__ == "__main__":
    unittest.main()
