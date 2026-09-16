"""ILP32 request-controller witnesses; all BIOS/file calls are local stubs."""

from pathlib import Path
import platform
import re
import shutil
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/game/mem_card_driver.c"
START = """
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
#define GMEMCARD_NIORESULT_IS_VOLATILE
#include "src/types.h"
#include "src/psyq/libapi.h"
#include "src/game/mem_card.h"
#include "src/game/mem_card_directory.h"
#include "src/game/io_event_helpers.h"

typedef char ilp32[sizeof(void *) == 4 && sizeof(long) == 4 ? 1 : -1];
s8 gMemCard_bRequest;
u8 gMemCard_bRequestStep, gMemCard_bChannel, gMemCard_bRetries, D_8009B436;
u16 gMemCard_wRequestOffset, gMemCard_wRequestSize;
volatile s32 gMemCard_nIOResult;
s32 gMemCard_nFreeBlocks, gMemCard_pRequestBuf;
char gMemCard_szRequestPath[32];
long gMemCard_aIOEventHandles[4], gMemCard_aHwIOEventHandles[4];
static struct DIRENTRY entries[2];
struct DIRENTRY *gMemCard_pDirEntries = entries;
static u8 buffer[64];
enum { STATUS, WAIT, DIRECTORY, CLEAR, NEW, RAW_READ, RAW_WRITE,
       OPEN, SEEK, READ, WRITE, CLOSE, FIND, EVENTS };
static s32 counts[EVENTS], cases, status, directory_result, found;
static s32 open_failures, seek_failures, transfer_failures;
static s32 out_state, out_result;

static void output(const char *p, u32 n)
{
    __asm__ volatile("int $0x80" : : "a"(4), "b"(1), "c"(p), "d"(n)
                     : "memory", "cc");
}
static void number(u32 n)
{
    char digits[12], reverse[12];
    u32 i = 0, j = 0;
    do { reverse[i++] = '0' + n % 10; n /= 10; } while (n);
    while (i) digits[j++] = reverse[--i];
    digits[j++] = '\n'; output(digits, j);
}
static void fail(u32 line) __attribute__((noreturn));
static void fail(u32 line)
{
    output("line: ", 6); number(line);
    output("case: ", 6); number(cases);
    __asm__ volatile("int $0x80" : : "a"(1), "b"(73) : "memory");
    __builtin_unreachable();
}
#define CHECK(c) do { if (!(c)) fail(__LINE__); } while (0)

long _card_status(long channel)
{ counts[STATUS]++; CHECK(channel == (gMemCard_bChannel != 0)); return status; }
long _card_wait(long channel)
{ counts[WAIT]++; CHECK(channel == (gMemCard_bChannel != 0)); return 0; }
s32 MemCard_DoLoadDirectory(void)
{
    counts[DIRECTORY]++;
    if (directory_result >= 0) gMemCard_nIOResult = directory_result;
    return directory_result;
}
void MemCard_ClearIOEvents(long *handles)
{
    counts[CLEAR]++;
    CHECK(handles == (gMemCard_bRequest >= 11 ?
                      gMemCard_aHwIOEventHandles : gMemCard_aIOEventHandles));
    gMemCard_nIOResult = -1;
}
void _new_card(void) { counts[NEW]++; }
static long raw(s32 kind, long channel, long offset, u8 *data)
{
    counts[kind]++;
    CHECK(channel == gMemCard_bChannel && offset == gMemCard_wRequestOffset);
    CHECK(data == buffer && counts[CLEAR] == 1 && counts[NEW] == 1);
    gMemCard_nIOResult = 0;
    return 0;
}
long _card_read(long channel, long offset, u8 *data)
{ return raw(RAW_READ, channel, offset, data); }
long _card_write(long channel, long offset, u8 *data)
{ return raw(RAW_WRITE, channel, offset, data); }
long open(char *name, unsigned long mode)
{
    counts[OPEN]++;
    CHECK(name == gMemCard_szRequestPath);
    CHECK(mode == (gMemCard_bRequest == 8 ?
          ((u32)gMemCard_wRequestSize << 16) | 0x200 :
          (gMemCard_bRequest == 4 ? 0x8002 : 0x8001)));
    return counts[OPEN] <= open_failures ? -1 : 3;
}
long lseek(long fd, long offset, long whence)
{
    counts[SEEK]++;
    CHECK(fd == 3 && offset == gMemCard_wRequestOffset && whence == 0);
    return counts[SEEK] <= seek_failures ? -1 : 0;
}
static long transfer(s32 kind, long fd, void *data, long size)
{
    counts[kind]++;
    CHECK(kind == (gMemCard_bRequest == 4 ? WRITE : READ));
    CHECK(fd == 3 && data == buffer && size == gMemCard_wRequestSize);
    CHECK(counts[CLEAR] == 1);
    if (counts[kind] <= transfer_failures) return -1;
    gMemCard_nIOResult = 0;
    return 0;
}
long read(long fd, void *data, long size) { return transfer(READ, fd, data, size); }
long write(long fd, void *data, long size) { return transfer(WRITE, fd, data, size); }
long close(long fd) { counts[CLOSE]++; CHECK(fd == 3); return 0; }
s32 MemCard_FindFiles(s32 channel, const char *name, struct DIRENTRY *cursor,
                     s32 *out_count)
{
    counts[FIND]++;
    CHECK(channel == gMemCard_bChannel && name == gMemCard_szRequestPath);
    CHECK(cursor == entries && out_count == 0);
    return found;
}

static void reset(s32 request, s32 step)
{
    s32 i;
    cases++;
    for (i = 0; i < EVENTS; i++) counts[i] = 0;
    status = directory_result = found = 0;
    open_failures = seek_failures = transfer_failures = 0;
    gMemCard_bRequest = request; gMemCard_bRequestStep = step;
    gMemCard_bChannel = 16; gMemCard_bRetries = 10; D_8009B436 = 20;
    gMemCard_wRequestOffset = 0xFFEE; gMemCard_wRequestSize = 31;
    gMemCard_nFreeBlocks = 0; gMemCard_nIOResult = 9;
    gMemCard_pRequestBuf = (s32)buffer;
    out_state = 123456; out_result = 654321;
}
static s32 run(void) { return MemCard_ProcessRequest(0, &out_state, &out_result); }
static void pending(void)
{ CHECK(out_state == 123456 && out_result == 654321); }
static void done(s32 request, s32 result)
{
    CHECK(out_state == request && out_result == result);
    CHECK(gMemCard_bRequest == -1);
}

int main(void)
{
    s32 request, n, i, j, value, result, count;
    s32 retries[] = {0, 1, 2, 127, 128, 129, 255};
    s32 waits[] = {0, 1, 128, 129, 255};
    s32 operations[] = {3, 4, 11, 12};
    for (request = -128; request < 0; request++) {
        reset(request, 0); CHECK(run() == -1); pending();
        for (i = 0; i < EVENTS; i++) CHECK(counts[i] == 0);
    }
    for (request = 0; request < 128; request++) {
        if (request == 1 || request == 2 || request == 3 || request == 4 ||
            request == 8 || request == 11 || request == 12) continue;
        reset(request, 0); CHECK(run() == 1); done(request, 9);
        CHECK(counts[WAIT] == 1 && counts[OPEN] == 0 && counts[DIRECTORY] == 0);
    }
    for (i = 0; i < 256; i++) {
        reset(3, 0); gMemCard_bChannel = i; status = 2;
        CHECK(MemCard_ProcessRequest(1, &out_state, &out_result) == 0); pending();
        CHECK(counts[STATUS] == 1 && counts[WAIT] == 0 && counts[DIRECTORY] == 0);
    }
    for (request = 1; request <= 2; request++) for (i = -1; i <= 2; i++) {
        reset(request, 0); directory_result = i;
        CHECK(run() == (i < 0 ? 0 : 1));
        if (i < 0) pending(); else done(request, i);
        CHECK(counts[DIRECTORY] == 1);
    }
    for (request = 3; request <= 4; request++) for (n = 0; n <= 11; n++) {
        reset(request, 1); open_failures = n; CHECK(run() == 0); pending();
        CHECK(counts[OPEN] == (n < 11 ? n + 1 : 11));
        CHECK(counts[SEEK] == (n < 11) && counts[CLOSE] == (n < 11));
        CHECK(gMemCard_bRequestStep == (n < 11 ? 2 : 1));
    }
    for (n = 0; n <= 11; n++) {
        reset(3, 1); seek_failures = n; CHECK(run() == 0); pending();
        CHECK(counts[OPEN] == 1 && counts[SEEK] == (n < 11 ? n + 1 : 11));
        CHECK(counts[CLOSE] == 1 && counts[READ] == (n < 11));
        CHECK(gMemCard_bRequestStep == (n < 11 ? 2 : 1));
    }
    for (request = 3; request <= 4; request++) for (n = 0; n <= 11; n++) {
        reset(request, 1); transfer_failures = n; CHECK(run() == 0); pending();
        CHECK(counts[request == 4 ? WRITE : READ] == (n < 11 ? n + 1 : 11));
        CHECK(counts[CLOSE] == 1 && gMemCard_bRequestStep == (n < 11 ? 2 : 1));
    }
    for (i = 0; i < 18; i++) for (j = 0; j < 18; j++) {
        reset(8, 0); gMemCard_nFreeBlocks = i; gMemCard_wRequestSize = j;
        CHECK(run() == 1); done(8, i + j < 16 ? 0 : 7);
        CHECK(counts[FIND] == (i + j < 16) && counts[OPEN] == (i + j < 16));
        CHECK(counts[CLOSE] == (i + j < 16));
    }
    for (i = -1; i <= 2; i++) {
        reset(8, 0); gMemCard_wRequestSize = 1; found = i;
        CHECK(run() == 1); done(8, i == 0 ? 0 : 6);
        CHECK(counts[OPEN] == (i == 0));
    }
    for (i = 0; i < 7; i++) {
        reset(8, 1); gMemCard_bRetries = retries[i]; open_failures = 99;
        value = (s8)(u8)(retries[i] - 1);
        CHECK(run() == (value > 0 ? 0 : 1));
        if (value > 0) pending(); else done(8, 2);
        CHECK(counts[OPEN] == 11 && counts[CLOSE] == 0);
        CHECK(gMemCard_bRetries == (u8)(retries[i] - 1));
    }
    for (i = 0; i < 4; i++) for (j = -1; j <= 1; j++) for (n = 0; n < 5; n++) {
        reset(operations[i], 2); D_8009B436 = waits[n]; gMemCard_nIOResult = j;
        result = j == 0 || (s8)(u8)(waits[n] - 1) < 0;
        CHECK(run() == result);
        if (result) done(operations[i], j); else {
            pending(); CHECK(gMemCard_bRequestStep == 1);
        }
        CHECK(D_8009B436 == (j == 0 ? waits[n] : (u8)(waits[n] - 1)));
    }
    reset(4, 1); gMemCard_wRequestOffset = gMemCard_wRequestSize = 0xFFFF;
    CHECK(run() == 0 && counts[WRITE] == 1); pending();
    reset(1, 0); directory_result = 2;
    CHECK(MemCard_ProcessRequest(0, &out_state, &out_state) == 1 && out_state == 1);
    reset(2, 0);
    CHECK(MemCard_ProcessRequest(0, (s32 *)&gMemCard_nIOResult, &out_result) == 1);
    CHECK(out_result == 0 && gMemCard_nIOResult == 2);
    for (request = 11; request <= 12; request++) {
        reset(request, 0); CHECK(run() == 0 && gMemCard_bRequestStep == 2);
        CHECK(counts[request == 11 ? RAW_READ : RAW_WRITE] == 1);
        CHECK(run() == 1); done(request, 0);
        count = counts[WAIT]; CHECK(run() == -1 && counts[WAIT] == count);
    }
    output("memory-card cases: ", 19); number(cases);
    return 0;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86, a host compiler, and ILP32 execution",
)
class MemCardIOControllerTests(unittest.TestCase):
    def build_and_run(self, optimization, mutation=""):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        scratch = ROOT / "tmp/test-mem-card-io"
        scratch.mkdir(parents=True, exist_ok=True)
        with tempfile.TemporaryDirectory(dir=scratch) as temporary:
            directory = Path(temporary)
            text = SOURCE.read_text()
            preamble, separator, _ = text.partition(
                "void MemCard_ClearIOEvents"
            )
            self.assertTrue(separator)
            marker = "s32 MemCard_ProcessRequest"
            self.assertIn(marker, text)
            text = preamble + text[text.index(marker):]
            if mutation:
                edits = {
                    "attempts": ("for (tries = 0xA;;)", "for (tries = 9;;)"),
                    "capacity": ("gMemCard_wRequestSize < 0x10", "gMemCard_wRequestSize < 0xF"),
                    "busy-mask": ("& 0xE)", "& 0xC)"),
                    "direction": ("gMemCard_bRequest == 4", "gMemCard_bRequest == 3"),
                    "idle-reset": ("gMemCard_bRequest = -1;", "gMemCard_bRequest = 0;"),
                    "output-order": (
                        "*out_result = gMemCard_nIOResult;\n    *out_state = gMemCard_bRequest;",
                        "*out_state = gMemCard_bRequest;\n    *out_result = gMemCard_nIOResult;",
                    ),
                }
                old, new = edits[mutation]
                self.assertIn(old, text)
                text = text.replace(old, new)
            text = re.sub(
                r'^#include "([^"]+)"',
                lambda m: '#include "' +
                    str((SOURCE.parent / m[1]).resolve()) + '"',
                text, flags=re.MULTILINE,
            )
            source = directory / "controller.c"
            source.write_text(text)
            fixture = directory / "fixture.c"
            fixture.write_text(WITNESS)
            start = directory / "start.S"
            start.write_text(START)
            flags = [
                "-std=gnu89", "-m32", "-nostdlib", "-fno-pie", "-no-pie",
                "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                "-fno-strict-aliasing", "-D_LANGUAGE_C",
                "-Werror=implicit-function-declaration", optimization, "-I", str(ROOT),
            ]
            objects = []
            for name, path in (("game", source), ("fixture", fixture), ("start", start)):
                obj = directory / (name + ".o")
                result = subprocess.run(
                    ["cc", *flags, "-c", str(path), "-o", str(obj)],
                    capture_output=True, text=True, timeout=60,
                )
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                objects.append(str(obj))
            binary = directory / "witness"
            result = subprocess.run(
                ["cc", *flags, *objects, "-o", str(binary)],
                capture_output=True, text=True, timeout=60,
            )
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            return subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)

    def test_request_state_and_retry_contract(self):
        for optimization in ("-O0", "-O2"):
            with self.subTest(optimization=optimization):
                result = self.build_and_run(optimization)
                self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                self.assertEqual(result.stdout, "memory-card cases: 973\n")

    def test_behavioral_mutations_are_rejected(self):
        for mutation in ("attempts", "capacity", "busy-mask", "direction", "idle-reset", "output-order"):
            with self.subTest(mutation=mutation):
                result = self.build_and_run("-O2", mutation)
                self.assertEqual(result.returncode, 73, result.stdout + result.stderr)
                self.assertIn("case: ", result.stdout)


if __name__ == "__main__":
    unittest.main()
