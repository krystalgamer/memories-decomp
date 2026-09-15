"""Native ILP32 keyboard controller oracle; callees are interface stubs."""

from pathlib import Path
import os
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/password/name_entry_keyboard_update.c"
SCRATCH = ROOT / "tmp/name-entry-keyboard-witness"
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
WITNESS = r"""
#include "src/types.h"
#define GINPUT_PAD1_HELD_IS_VOLATILE
#define GINPUT_PAD1_REPEAT_IS_VOLATILE
#define GINPUT_PAD1_PRESSED_IS_VOLATILE
#include "src/game/input.h"
#include "src/game/display_object_helpers.h"
#include "src/game/sound.h"
#include "src/overlays/password/name_entry_keyboard.h"
#include "src/overlays/password/module_state.h"
#include "src/overlays/password/name_entry_tables.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
PasswordModuleState gPassword_ModuleState;
__asm__(".globl D_8016D400\n.set D_8016D400, gPassword_ModuleState\n"
        ".globl D_8016D401\n.set D_8016D401, gPassword_ModuleState+1\n"
        ".globl D_8016D402\n.set D_8016D402, gPassword_ModuleState+2\n"
        ".globl D_8016D404\n.set D_8016D404, gPassword_ModuleState+4\n"
        ".globl D_8016D418\n.set D_8016D418, gPassword_ModuleState+0x18\n"
        ".globl D_8016D426\n.set D_8016D426, gPassword_ModuleState+0x26\n"
        ".globl D_8016D42C\n.set D_8016D42C, gPassword_ModuleState+0x2C\n"
        ".globl D_8016D434\n.set D_8016D434, gPassword_ModuleState+0x34\n"
        ".globl D_8016D436\n.set D_8016D436, gPassword_ModuleState+0x36\n"
        ".globl D_8016D4D4\n.set D_8016D4D4, gPassword_ModuleState+0xD4\n");
volatile u16 gInput_wPad1Held, gInput_wPad1Repeat, gInput_wPad1Pressed;
s8 D_8016AB38[9][15];
u8 D_8016ABC0[9][2];
static SelectionFrame frame;
static DuelEffectEntry nodes[2];
static u8 sprites[3][0x70] __attribute__((aligned(16)));
static u16 name[12];
typedef struct {
    PasswordModuleState state;
    SelectionFrame frame;
    u8 sprites[3][0x70];
    u16 name[12];
} Snapshot;
enum { SOUND=1, STEP, RESET, ADJUST, GET, SPAWN };
typedef struct { s32 kind,a,b,c,result; Snapshot before; } Event;
static Snapshot expected;
static Event events[8];
static s32 count, cursor, cases;
static u16 held, repeat, pressed;
static void copy(void *d, const void *s, u32 n)
{ u8 *out=d; const u8 *in=s; while (n--) *out++=*in++; }
static void fill(void *p, u32 n, u8 value)
{ u8 *out=p; while (n--) *out++=value; }
static s32 equal(const void *a, const void *b, u32 n)
{ const u8 *p=a,*q=b; while (n--) if (*p++ != *q++) return 0; return 1; }
static void put16(u8 *p, s32 n) { p[0]=n; p[1]=(u32)n>>8; }
static void callback(u8 *p, NameEntryGlyphUpdate update)
{ u32 n=(u32)update; s32 i; for (i=0;i<4;i++) p[0x24+i]=n>>(8*i); }
static void text(const char *p, u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4),"b"(1),"c"(p),"d"(n)
                     : "memory","cc");
}
static void number(u32 n)
{
    char out[12]; s32 i=11; out[i]='\n';
    do { out[--i]='0'+n%10; n/=10; } while (n);
    text(out+i,12-i);
}
static void fail(s32 reason) __attribute__((noreturn));
static void fail(s32 reason)
{
    text("mismatch reason/case: ",21); number(reason); number(cases);
    __asm__ volatile("int $0x80" : : "a"(1),"b"(75) : "memory");
    __builtin_unreachable();
}
static void compare(const Snapshot *s)
{
    if (!equal(&gPassword_ModuleState,&s->state,sizeof(s->state))) fail(1);
    if (!equal(&frame,&s->frame,sizeof(frame))) fail(2);
    if (!equal(sprites,s->sprites,sizeof(sprites))) fail(3);
    if (!equal(name,s->name,sizeof(name))) fail(4);
}
static void record(s32 kind,s32 a,s32 b,s32 c,s32 result)
{
    Event *e;
    if (count == 8) fail(5);
    e=&events[count++]; e->kind=kind; e->a=a; e->b=b; e->c=c;
    e->result=result; copy(&e->before,&expected,sizeof(expected));
}
static s32 consume(s32 kind,s32 a,s32 b,s32 c)
{
    Event *e;
    if (cursor == count) fail(6);
    e=&events[cursor++];
    if (e->kind!=kind || e->a!=a || e->b!=b || e->c!=c) fail(7);
    compare(&e->before);
    return e->result;
}
void SD_SEPlayFull(u32 sound) { consume(SOUND,sound,0,0); }
/* Deliberate no-op movement interfaces, not an interpolation implementation.
 * Snapshots verify width changes precede STEP and timer/deltas follow RESET. */
void DisplayObject_StepPositionXY(DisplayObjectVelocity *p)
{ if (p!=(DisplayObjectVelocity *)&frame) fail(8); consume(STEP,0,0,0); }
void DisplayObject_ResetVelocity(DisplayObjectVelocity *p)
{ if (p!=(DisplayObjectVelocity *)&frame) fail(9); consume(RESET,0,0,0); }
s32 NameEntry_AdjustLength(s32 delta,s32 arg)
{ return consume(ADJUST,delta,arg,0); }
DuelEffectEntry *TextBox_GetGlyphAt(s32 kind,s32 x,s32 y)
{ return (DuelEffectEntry *)consume(GET,kind,x,y); }
void *NameEntry_SpawnGlyphSprite(s32 kind,DuelEffectEntry *node)
{ return sprites[consume(SPAWN,kind,(s32)node,0)]; }
void NameEntry_UpdateGlyphPulse(u8 *p) { fail(10); }
void NameEntry_UpdateGlyphTransfer(u8 *p) { fail(11); }

/* Synthetic cells are intentionally unrelated to retail data. */
static s32 cell(s32 row,s32 col)
{
    if (col==1 || col==8 || col==12) return -1;
    if (col==7) return -2;
    if (col==3) return (row+1)%9;
    if (col==5) return 0x40 | ((row+3)%9);
    if (col==11) return 4;
    if (col==13) return 0x44;
    if (col==14) return 0x46;
    return 0;
}
static void reset(s32 row,s32 col)
{
    s32 r,c;
    cases++; count=cursor=0;
    fill(&gPassword_ModuleState,sizeof(gPassword_ModuleState),0xA5);
    fill(&frame,sizeof(frame),0x5A); fill(sprites,sizeof(sprites),0xD5);
    fill(name,sizeof(name),0xBE); fill(nodes,sizeof(nodes),0);
    nodes[0].code_00=0x1234; nodes[1].code_00=0xABCD;
    gPassword_ModuleState.flags=5; gPassword_ModuleState.keyboardFlags=0x123;
    gPassword_ModuleState.keyboardRow=row; gPassword_ModuleState.keyboardColumn=col;
    gPassword_ModuleState.savedKeyboardRow=(row+4)%9;
    gPassword_ModuleState.selectionFrame=&frame;
    gPassword_ModuleState.nameBuffer=(u8 *)name;
    gPassword_ModuleState.caretIndex=row%5;
    frame.x=321; frame.y=199; frame.width=16; frame.widthBonus=0; frame.timer=0;
    for (r=0;r<9;r++) {
        for (c=0;c<15;c++) D_8016AB38[r][c]=cell(r,c);
        D_8016ABC0[r][0]=(r+7)%9; D_8016ABC0[r][1]=(r+2)%9;
    }
    held=repeat=pressed=0;
}
static void capture(void)
{
    copy(&expected.state,&gPassword_ModuleState,sizeof(expected.state));
    copy(&expected.frame,&frame,sizeof(frame));
    copy(expected.sprites,sprites,sizeof(sprites)); copy(expected.name,name,sizeof(name));
}
static void run(void)
{
    s32 r,c;
    gInput_wPad1Held=held; gInput_wPad1Repeat=repeat; gInput_wPad1Pressed=pressed;
    NameEntry_UpdateKeyboard(); compare(&expected);
    if (cursor!=count) fail(12);
    if (gInput_wPad1Held!=held || gInput_wPad1Repeat!=repeat ||
        gInput_wPad1Pressed!=pressed) fail(13);
    for (r=0;r<9;r++) {
        for (c=0;c<15;c++) if (D_8016AB38[r][c]!=cell(r,c)) fail(14);
        if (D_8016ABC0[r][0]!=(r+7)%9 || D_8016ABC0[r][1]!=(r+2)%9) fail(15);
    }
}
static void idle_and_transition(void)
{
    s32 b,w,t;
    reset(0,0); capture(); run();
    for (b=0;b<2;b++) for (w=-1;w<=1;w++) for (t=0;t<3;t++) {
        reset(0,0); frame.widthBonus=b*20; frame.width=16+b*20+w*6;
        frame.timer=t; gPassword_ModuleState.keyboardFlags|=0x4000;
        gPassword_ModuleState.cursorTargetX=-13; gPassword_ModuleState.cursorTargetY=31;
        if (t!=1) { held=PAD_DIRECTION_RIGHT; repeat=PAD_BUTTON_CONFIRM_MASK; }
        capture(); expected.frame.width-=w*2;
        record(STEP,0,0,0,0); expected.frame.timer=t-1;
        if (t==1) {
            expected.frame.width=16+b*20; expected.frame.x=-13; expected.frame.y=31;
            expected.state.keyboardFlags=0x123;
        }
        run();
    }
}
static void navigation(void)
{
    static const u16 directions[]={PAD_DIRECTION_LEFT,PAD_DIRECTION_RIGHT,
                                   PAD_DIRECTION_UP,PAD_DIRECTION_DOWN};
    s32 r,c,d,row,col,saved,code,x,y;
    for (r=0;r<9;r++) for (c=0;c<15;c++) for (d=0;d<5;d++) {
        if (d==4 && c!=0) continue;
        reset(r,c); held=d<4?directions[d]:0; pressed=PAD_BUTTON_START;
        repeat=PAD_BUTTON_CONFIRM_MASK|PAD_BUTTON_CANCEL; capture();
        row=r; col=c; saved=(r+4)%9;
        if (d==4) { row=8; col=14; }
        else if (d<2) col=(c+(d==0?14:1))%15;
        else {
            row=(r+(c>=11?(d==2?7:2):(d==2?8:1)))%9;
            if (c>=11) col=11;
            saved=row;
        }
        expected.state.keyboardColumn=col; expected.state.keyboardRow=row;
        expected.state.savedKeyboardRow=saved; record(SOUND,47,0,0,0);
        /* Resolve the descriptor independently of the source's byte stride. */
        while ((code=cell(row,col))<0) col+=code;
        row=code==0?saved:code%16; x=col*20+(col>=11?42:22); y=row*18+24;
        expected.state.keyboardColumn=col; expected.state.keyboardRow=row;
        expected.state.cursorTargetX=x; expected.state.cursorTargetY=y;
        expected.frame.widthBonus=(code&64)?20:0;
        record(RESET,0,0,0,0); expected.frame.timer=8;
        expected.frame.stepX=(x-321)*32; expected.frame.stepY=(y-199)*32;
        expected.state.keyboardFlags|=0x4000; run();
    }
}
static void spawn(s32 kind,DuelEffectEntry *node,s32 index)
{ record(SPAWN,kind,(s32)node,0,index); }
static void pulse(s32 index,s32 missing)
{
    u8 *p=expected.sprites[index];
    p[0x6C]=1; callback(p,NameEntry_UpdateGlyphPulse);
    if (missing) put16(p+8,0xD595);
}
static void confirmation(void)
{
    static const s32 columns[]={0,3,5,11,13,14};
    s32 r,c,mask,code,kind,x,y,idx; DuelEffectEntry *first,*second;
    for (r=0;r<9;r++) for (c=0;c<6;c++) for (mask=0;mask<4;mask++) {
        reset(r,columns[c]); repeat=PAD_BUTTON_CONFIRM_MASK|PAD_BUTTON_CANCEL;
        capture(); code=cell(r,columns[c])&15; kind=(code!=4 && code!=6);
        first=(mask&1)?0:&nodes[0]; second=(mask&2)?0:&nodes[1];
        if (code==4) {
            x=columns[c]==11?0:20; y=36;
            record(ADJUST,columns[c]==11?-1:1,6,0,mask>>1);
            if (!(mask>>1)) record(SOUND,9,0,0,0);
        } else if (code==6) {
            x=0; y=72; expected.state.flags|=0x40;
        } else { x=columns[c]*20; y=r*18; record(SOUND,41,0,0,0); }
        record(GET,kind,x,y,(s32)first); spawn(kind,first,0); pulse(0,first==0);
        if (code==6) {
            put16(expected.sprites[0]+0x48,20);
            record(GET,0,20,72,(s32)second); spawn(0,second,1);
            pulse(1,0); put16(expected.sprites[1]+0x48,0);
        }
        if (kind) {
            expected.state.flags|=0x80; idx=r%5;
            expected.name[idx]=first?0x1234:0; spawn(1,first,1);
            put16(expected.sprites[1]+0x60,8);
            callback(expected.sprites[1],NameEntry_UpdateGlyphTransfer);
            put16(expected.sprites[1]+0x46,204);
            put16(expected.sprites[1]+0x44,idx*16+112);
            expected.sprites[1][0x6C]=6;
        }
        run();
    }
}
int main(void)
{
    s32 success;
    idle_and_transition(); navigation(); confirmation();
    for (success=0;success<2;success++) {
        reset(0,0); repeat=PAD_BUTTON_CANCEL; capture();
        record(ADJUST,-1,6,0,success); record(SOUND,success?12:9,0,0,0); run();
    }
    if (cases!=786) fail(16);
    text("idle=1 transition=18 navigation=549 confirm=216 cancel=2\n",
         sizeof("idle=1 transition=18 navigation=549 confirm=216 cancel=2\n")-1);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host compiler and native ILP32 execution",
)
class NameEntryKeyboardUpdateTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run this test from the repository root")
        code = SOURCE.read_text()
        self.assertNotRegex(code, r"\b(?:__asm__|__asm|asm|register|extern)\b")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation is not None:
                original, replacement = {
                    "stride": (
                        "parts: { (s8)D_8016D401, row * 15 }",
                        "parts: { (s8)D_8016D401, row * 14 }",
                    ),
                    "timer": ("w->timer = w->timer - 1;", "w->timer = w->timer - 2;"),
                }[mutation]
                self.assertEqual(code.count(original), 1, "mutation must be active")
                code = code.replace(original, replacement)
                code = re.sub(
                    r'^#include "([^"]+)"',
                    lambda m: '#include "' + str(
                        (SOURCE.parent / m.group(1)).resolve()
                    ) + '"',
                    code, flags=re.MULTILINE,
                )
                source = directory / "mutated.c"
                source.write_text(code)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", "-m32", optimization, "-std=gnu89", "-nostdlib",
                 "-ffreestanding", "-fno-builtin", "-fno-pie", "-no-pie",
                 "-fno-stack-protector", "-fno-strict-aliasing", "-D_LANGUAGE_C",
                 "-I", str(ROOT), str(source), str(fixture), str(start), "-o", str(binary)],
                capture_output=True, text=True, timeout=60,
                env=dict(os.environ, TMPDIR=str(directory)),
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_native_oracle_and_negative_controls(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization, mutation=None):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "idle=1 transition=18 navigation=549 confirm=216 cancel=2\n",
                )
            for mutation in ("stride", "timer"):
                with self.subTest(optimization=optimization, mutation=mutation):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 75, result.stdout + result.stderr)
                    self.assertIn("mismatch reason/case:", result.stdout)


if __name__ == "__main__":
    unittest.main()
