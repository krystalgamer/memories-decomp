"""Source-only ILP32 SD_SEPlay dispatch oracle, not sound hardware emulation."""

from pathlib import Path
import os
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/sound_effect_request.c"
SCRATCH = ROOT / "tmp/sound-effect-request-witness"
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
#include "src/game/sound.h"
#include "src/game/sound_output_state.h"
#include "src/game/sound_voice_allocator.h"
typedef char ilp32[sizeof(void *) == 4 && sizeof(int) == 4 ? 1 : -1];
typedef struct {
    u32 leading[4];
    SDValue state;
    u16 indices[65536];
    SDNote notes[65536];
    u32 trailing[4];
} Memory;
typedef char word_snapshot[sizeof(Memory)%4 == 0 ? 1 : -1];
static Memory memory, before;
SDValue *g_SDValue;
static SDValue *root_before;
enum { NONE, STOP, ALLOCATE, DIRECT, MAPPED };
static s32 cases, expected_call, seen, expected_args[6];
static void text(const char *p,u32 n)
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
static void snapshot(void)
{
    u32 i,*dst=(u32 *)&before,*src=(u32 *)&memory;
    for (i=0;i<sizeof(memory)/4;i++) dst[i]=src[i];
    root_before=g_SDValue;
}
static void unchanged(void)
{
    u32 i,*a=(u32 *)&before,*b=(u32 *)&memory;
    if (g_SDValue!=root_before) fail(1);
    for (i=0;i<sizeof(memory)/4;i++) if (a[i]!=b[i]) fail(2);
}
/* Interface witnesses only: neither callee implements a queue or an SPU. */
s16 func_800451E0(u16 id,s32 unused)
{
    unchanged();
    if (expected_call!=STOP || seen++) fail(3);
    if (id!=expected_args[0] || unused!=0) fail(4);
    return -32767;
}
void func_800482B0(s32 id,s16 pitch,u8 volume,s16 pan,u32 mode,u8 value)
{
    unchanged();
    if (expected_call!=ALLOCATE || seen++) fail(5);
    if (id!=expected_args[0] || pitch!=0 || volume!=expected_args[2] ||
        pan!=expected_args[3] || mode!=(u32)expected_args[4] ||
        value!=expected_args[5]) fail(6);
}
/* The scenario supplies its expected dispatch class and resolved ID; the
 * oracle does not classify the request using the implementation's masks.
 * Entire state/index/note allocations, including unused records, are frozen. */
static void run(s32 family,u32 request,s32 resolved,s32 note,
                s32 volume,s32 pan,u8 mode,u8 value)
{
    s32 bank,index,i,j,pan_bits;
    cases++; seen=0; expected_call=NONE;
    for (i=0;i<2;i++) for (j=0;j<32;j++) memory.state.field_044C[i][j]=65535;
    memory.state.field_043C=memory.indices;
    memory.state.field_0444=memory.notes;
    g_SDValue=&memory.state;
    if (family==STOP) {
        /* Even forming a state-derived table access must not precede stop. */
        g_SDValue=0; expected_call=STOP;
        expected_args[0]=request%65536;
    } else {
        if (family==MAPPED) {
            bank=(request/256)%2; index=request%32;
            memory.state.field_044C[bank][index]=resolved;
        }
        if (resolved==65535) {
            memory.state.field_043C=0; memory.state.field_0444=0;
        } else {
            memory.indices[resolved]=note;
            if (note==65535) {
                memory.state.field_0444=0;
            } else {
                /* Canonical SDNote fields, not a copied reference layout. */
                memory.notes[note].pad0002[0]=value;
                memory.notes[note].pad0002[1]=mode;
                expected_call=ALLOCATE; expected_args[0]=resolved;
                expected_args[2]=(u32)volume%256;
                pan_bits=(u32)pan%65536;
                expected_args[3]=pan_bits<32768?pan_bits:pan_bits-65536;
                expected_args[4]=mode; expected_args[5]=value;
            }
        }
    }
    snapshot(); SD_SEPlay((s32)request,volume,pan); unchanged();
    if (seen!=(expected_call!=NONE)) fail(7);
}
static void bank_grid(void)
{
    static const s32 pans[]={-32768,-1,0,32767};
    s32 bank,index,v,p;
    for (bank=0;bank<2;bank++) for (index=0;index<32;index++)
     for (v=0;v<2;v++) for (p=0;p<4;p++)
        run(MAPPED,0x4000+bank*256+index,23,5,v*255,pans[p],49,128);
}
static void narrowing(void)
{
    static const u32 ids[]={0,7,31,0x3FFF,0x4200,0x4FFF,0x5000,0x7FFF,
                            0x10007,0xFFFF0007};
    static const s32 families[]={DIRECT,DIRECT,DIRECT,DIRECT,MAPPED,MAPPED,
                                 DIRECT,DIRECT,DIRECT,DIRECT};
    static const s32 volumes[]={-1,0,256,0x7FFFFFFF};
    static const s32 pans[]={-65537,-32769,0,32768,65537};
    s32 id,v,p,missing,key;
    for (id=0;id<10;id++) for (v=0;v<4;v++) for (p=0;p<5;p++)
     for (missing=0;missing<2;missing++) {
        key=families[id]==MAPPED?23:ids[id]%65536;
        run(families[id],ids[id],key,missing?65535:5,volumes[v],pans[p],49,128);
    }
}
static void stops_and_sentinels(void)
{
    static const u32 ids[]={0x8000,0xFFFF,0x18007,0x1234FFFF,0xFFFFFFFF,0xFFFF8000};
    static const s32 volumes[]={0,-1,0x7FFFFFFF};
    static const s32 pans[]={-32768,0,32767};
    s32 id,v,p,bank,index,missing_map;
    for (id=0;id<6;id++) for (v=0;v<3;v++) for (p=0;p<3;p++)
        run(STOP,ids[id],0,0,volumes[v],pans[p],0,0);
    for (bank=0;bank<2;bank++) for (index=0;index<32;index++)
     for (missing_map=0;missing_map<2;missing_map++)
        run(MAPPED,0x4000+bank*256+index,missing_map?65535:23,65535,255,-37,49,128);
}
static void note_grid(void)
{
    static const s32 notes[]={0,1,255,256,32767,65534};
    static const u8 bytes[]={0,1,255};
    s32 n,m,v;
    for (n=0;n<6;n++) for (m=0;m<3;m++) for (v=0;v<3;v++)
        run(MAPPED,0x14103,65534,notes[n],255,-37,bytes[m],bytes[v]);
}
static void high_bits(void)
{
    /* Bit 31 is not the stop bit; upper-word bits must not alter dispatch.
     * Mapped results with bit 15 set remain allocator IDs, not new requests. */
    run(DIRECT,0x80000007,7,1,0x12345678,0x12348001,255,0);
    run(DIRECT,0xFFFF7FFF,32767,256,-257,-65536,128,255);
    run(MAPPED,0x80004103,0x8001,32767,256,0x7FFFFFFF,255,128);
    run(MAPPED,0xFFFF4000,0,0,-1,0x80000000,0,1);
    run(MAPPED,0xFFFF411F,65534,65534,257,65535,1,255);
    run(STOP,0x8000C103,0,0,255,32768,0,0);
    run(STOP,0x40008000,0,0,-256,-65537,0,0);
    run(STOP,0x0000C000,0,0,0x7FFFFFFF,0x7FFFFFFF,0,0);
}
int main(void)
{
    u32 i; u8 *bytes=(u8 *)&memory;
    for (i=0;i<sizeof(memory);i++) bytes[i]=(i*13+17)%256;
    for (i=0;i<65536;i++) memory.indices[i]=65535;
    bank_grid(); narrowing(); stops_and_sentinels(); note_grid(); high_bits();
    if (cases!=1156) fail(8);
    text("bank=512 narrowing=400 stop=54 sentinel=128 note=54 high=8\n",
         sizeof("bank=512 narrowing=400 stop=54 sentinel=128 note=54 high=8\n")-1);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host compiler and native ILP32 execution",
)
class SoundEffectRequestTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=None):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run this test from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH) as temporary:
            directory = Path(temporary)
            source = SOURCE
            if mutation is not None:
                original, replacement, occurrences = {
                    "bank": ("hi = (hi != 0) << 6;", "hi = (hi != 0) << 5;", 1),
                    "stride": ("n * 8 +", "n * 4 +", 2),
                }[mutation]
                code = SOURCE.read_text()
                self.assertEqual(code.count(original), occurrences, "mutation must be active")
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
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=120)

    def test_native_oracle_and_negative_controls(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization, mutation=None):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(
                    result.stdout,
                    "bank=512 narrowing=400 stop=54 sentinel=128 note=54 high=8\n",
                )
            for mutation in ("bank", "stride"):
                with self.subTest(optimization=optimization, mutation=mutation):
                    result = self.build_and_run(optimization, mutation)
                    self.assertEqual(result.returncode, 75, result.stdout + result.stderr)
                    self.assertIn("mismatch reason/case:", result.stdout)


if __name__ == "__main__":
    unittest.main()
