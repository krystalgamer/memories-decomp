"""CD-ready declaration controls and executable ILP32 transfer witnesses.

SDK stubs and phase callbacks live in a separate translation unit from the
implementation, without LTO. Only the freestanding Linux startup uses assembly;
no multilib libc, retail inputs, or project-wide build is needed.
"""

from __future__ import annotations

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SOURCE = REPOSITORY / "src/game/func_80013C28.c"
COMPILER = REPOSITORY / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CALLBACK_VIEW = "#define FUNC_80013C28_CALLBACK_VIEW\n"
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
.section .note.GNU-stack,"",@progbits
"""

WITNESS = r"""
#include "src/types.h"
#include "src/psyq/libcd.h"
#include "src/psyq/libds.h"
#include "src/psyq/libgte.h"
#include "src/psyq/libgpu.h"
#include "src/psyq/libspu.h"
#include "src/game/file_transfer.h"
#include "src/game/file_ready_sector.h"

FileTransferDescriptor *D_8009AF18;
volatile u32 D_8009B0F4;
u32 *D_8009B0F8;
u8 D_8009B114;
s32 D_8009B138;
s32 D_8009B130;
volatile u16 D_8009B100;
char D_8009B104[1];
FileTransferDescriptor gFile_PrimaryTransferDescriptor;

static FileTransferDescriptor descriptor, alternate;
static u32 input[2048], buffers[2][1024];
static s32 events[32], event_count, error;
static s32 cd_calls, cd_words, image_calls, image_busy, callback_calls;
static void *cd_destination;
static u32 *image_destination;
static RECT image_rect;
static unsigned long spu_address, spu_size;
static u8 *spu_destination;
static s32 callback_action;
static u32 expected_sequence;

#define CHECK(condition, code) do { if (!(condition)) return (code); } while (0)

static void clear(void *pointer, u32 size)
{
    volatile u8 *p = pointer;
    while (size--) *p++ = 0;
}

static void event(s32 kind)
{
    if (event_count < 32) events[event_count++] = kind;
    else error = 1;
}

int CdGetSector(void *destination, int words)
{
    s32 i;
    u32 *out = destination;
    event(1);
    cd_calls++;
    cd_destination = destination;
    cd_words = words;
    for (i = 0; i < words; i++) out[i] = input[i];
    return 1;
}

void DsEndReadySystem(void) { event(2); }

int DsPacket(u8 mode, DslLOC *pos, u8 command, DslCB callback, int count)
{
    (void)mode;
    (void)pos;
    (void)command;
    (void)callback;
    (void)count;
    return 0;
}

int DsCommand(u8 command, u8 *parameter, DslCB callback, int count)
{
    (void)command;
    (void)parameter;
    (void)callback;
    (void)count;
    return 0;
}

int DsReadySystemMode(int mode)
{
    (void)mode;
    return 0;
}

int DsStartReadySystem(DslRCB callback, int count)
{
    (void)callback;
    (void)count;
    return 0;
}

CdlCB CdReadyCallback(CdlCB callback)
{
    event(3);
    if (callback != 0) error = 2;
    return 0;
}

int LoadImage2(RECT *rect, u32 *data)
{
    event(4);
    image_calls++;
    image_rect = *rect;
    image_destination = data;
    if (image_busy) {
        image_busy--;
        return 1;
    }
    return 0;
}

unsigned long SpuSetTransferStartAddr(unsigned long address)
{
    event(5);
    spu_address = address;
    return address;
}

unsigned long SpuWrite(unsigned char *data, unsigned long size)
{
    event(6);
    spu_destination = data;
    spu_size = size;
    return size;
}

static void phase(FileTransferDescriptor *p, s32 sequence)
{
    event(7);
    callback_calls++;
    if (p != &descriptor || (u32)sequence != expected_sequence ||
        p->result != expected_sequence + 1 || p->phase_size != 0)
        error = 3;
    p->phase_size = 0x1234;
    p->result = 0xABCDEF01;
    if (callback_action == 1) p->total_bytes = 0;
    if (callback_action == 2) p->total_bytes = 0x1800;
    if (callback_action == 3) D_8009AF18 = &alternate;
}

static void setup(s32 mode, s32 software)
{
    s32 i, j;
    clear(&descriptor, sizeof(descriptor));
    clear(&alternate, sizeof(alternate));
    clear(events, sizeof(events));
    for (i = 0; i < 2048; i++) input[i] = 0x12340000 + i;
    for (j = 0; j < 2; j++)
        for (i = 0; i < 1024; i++) buffers[j][i] = 0xCCCCCCCC;
    D_8009AF18 = &descriptor;
    D_8009B0F8 = input;
    D_8009B0F4 = 0x510 | (software ? 0x40000000 : 0);
    D_8009B114 = 9;
    D_8009B138 = 17;
    descriptor.done = mode;
    descriptor.value_08 = (u32)buffers[0];
    descriptor.value_0C = (u32)buffers[1];
    descriptor.total_bytes = 0x1800;
    descriptor.phase_remaining = 0x1800;
    descriptor.phase_size = 0x8888;
    descriptor.result = 41;
    descriptor.w = 64;
    descriptor.h = 16;
    descriptor.field_30.h.counter = 64;
    descriptor.field_30.h.field_32 = 32;
    event_count = error = cd_calls = cd_words = image_calls = image_busy = 0;
    callback_calls = callback_action = 0;
    cd_destination = 0;
    image_destination = 0;
    spu_destination = 0;
    spu_address = spu_size = 0;
    expected_sequence = 41;
}

static s32 copied(s32 buffer, s32 start, s32 count, s32 source_start)
{
    s32 i;
    for (i = 0; i < count; i++)
        if (buffers[buffer][start + i] != input[source_start + i]) return 0;
    return 1;
}

static s32 ignored_events(void)
{
    static const s32 ignored[] = {0, 2, 5, 255, 256, -1};
    s32 i;
    for (i = 0; i < 6; i++) {
        setup(1, 1);
        D_8009B114 = 255;
        File_TransferReadyCallback(ignored[i]);
        CHECK(D_8009B114 == 0 && D_8009B138 == 17, 10);
        CHECK(!event_count && D_8009B0F8 == input, 11);
        CHECK(descriptor.total_bytes == 0x1800 &&
              descriptor.phase_remaining == 0x1800 &&
              descriptor.value_08 == (u32)buffers[0] &&
              buffers[0][0] == 0xCCCCCCCC && D_8009B0F4 == 0x40000510, 12);
    }
    setup(99, 1);
    D_8009B138 = 0x7FFFFFFF;
    File_TransferReadyCallback(0x101);
    CHECK(D_8009B114 == 10 && (u32)D_8009B138 == 0x80000000, 13);
    CHECK(!event_count && D_8009B0F8 == input &&
          descriptor.total_bytes == 0x1800 && D_8009B0F4 == 0x40000510, 14);
    return 0;
}

static s32 ram(void)
{
    s32 software, skip;
    for (software = 0; software < 2; software++)
    for (skip = 0; skip < 2; skip++) {
        setup(1, software);
        if (skip) D_8009B0F4 |= 0x200000;
        File_TransferReadyCallback(0x101);
        CHECK(descriptor.value_08 == (u32)buffers[0] + (skip ? 0 : 2048), 20);
        CHECK(D_8009B0F8 == input + 512 && descriptor.total_bytes == 4096 &&
              descriptor.phase_remaining == 4096 && descriptor.buffer_index == 0, 21);
        CHECK(D_8009B114 == 10 && D_8009B138 == 18 && (D_8009B0F4 & 0x100), 22);
        CHECK(cd_calls == (!software && !skip), 23);
        if (!software && !skip)
            CHECK(cd_words == 512 && cd_destination == buffers[0], 24);
        if (skip) CHECK(buffers[0][0] == 0xCCCCCCCC, 25);
        else CHECK(copied(0, 0, 512, 0) && buffers[0][512] == 0xCCCCCCCC, 26);
        File_TransferReadyCallback(1);
        CHECK(D_8009B0F8 == input + 1024 &&
              descriptor.value_08 == (u32)buffers[0] + (skip ? 0 : 4096), 27);
        if (!skip) CHECK(copied(0, 512, 512, software ? 512 : 0), 28);
    }
    return 0;
}

static s32 ram_descriptor_reload(void)
{
    s32 i;
    setup(1, 1);
    descriptor.value_08 = (u32)&descriptor;
    /* Word two changes the descriptor's destination. Subsequent indexed
       stores must reload it, rather than walking a cached destination. */
    input[2] = (u32)buffers[0];
    File_TransferReadyCallback(1);
    CHECK(descriptor.value_08 == (u32)buffers[0] + 2048, 30);
    for (i = 0; i < 3; i++) CHECK(buffers[0][i] == 0xCCCCCCCC, 31);
    CHECK(copied(0, 3, 509, 3) && buffers[0][512] == 0xCCCCCCCC, 32);
    CHECK(descriptor.total_bytes == 4096 && !error, 33);
    return 0;
}

static s32 image(void)
{
    s32 software, index, horizontal, wrap;
    for (software = 0; software < 2; software++)
    for (index = 0; index < 2; index++)
    for (horizontal = 0; horizontal < 2; horizontal++)
    for (wrap = 0; wrap < 3; wrap++) {
        u16 y = wrap == 0 ? 32 : wrap == 1 ? 240 : 496;
        u16 x = wrap == 2 ? 0xFFC0 : 64;
        setup(2, software);
        descriptor.buffer_index = index ? 0xFFFF : 0xFFFE;
        descriptor.field_30.h.counter = x;
        descriptor.field_30.h.field_32 = y;
        if (horizontal) D_8009B0F4 |= 0x20000;
        image_busy = 2;
        File_TransferReadyCallback(1);
        CHECK(image_calls == 3 && image_destination == buffers[index] &&
              image_rect.x == (s16)x && image_rect.y == y &&
              image_rect.w == 64 && image_rect.h == 16, 40);
        CHECK(copied(index, 0, 512, 0) &&
              buffers[index ^ 1][0] == 0xCCCCCCCC &&
              buffers[index][512] == 0xCCCCCCCC, 41);
        CHECK(descriptor.field_30.h.counter ==
              (u16)(x + ((horizontal || wrap) ? 64 : 0)), 42);
        CHECK(descriptor.field_30.h.field_32 ==
              (horizontal ? y : wrap == 0 ? 48 : wrap == 1 ? 0 : 256), 43);
        CHECK(descriptor.buffer_index == (index ? 0 : 0xFFFF) &&
              descriptor.total_bytes == 4096 && descriptor.phase_remaining == 4096, 44);
        CHECK(D_8009B0F8 == input + (software ? 512 : 0) &&
              cd_calls == !software, 45);
        CHECK(descriptor.value_08 == (u32)buffers[0] &&
              descriptor.value_0C == (u32)buffers[1] && (D_8009B0F4 & 0x100), 46);
        if (!software) CHECK(cd_destination == buffers[index] && cd_words == 512, 47);
    }
    return 0;
}

static s32 spu(void)
{
    static const s32 sizes[] = {1, 3, 4, 7, 2047, 2048, 4096};
    s32 software, j;
    for (software = 0; software < 2; software++)
    for (j = 0; j < 7; j++) {
        s32 n = sizes[j] < 2048 ? sizes[j] : 2048;
        setup(3, software);
        descriptor.phase_remaining = sizes[j];
        descriptor.field_30.word = 0xFFFFFFFC;
        descriptor.buffer_index = 0xFFFF;
        File_TransferReadyCallback(1);
        CHECK(copied(0, 0, n / 4, 0) &&
              buffers[0][n / 4] == 0xCCCCCCCC &&
              buffers[1][0] == 0xCCCCCCCC, 50);
        CHECK(D_8009B0F8 == (u32 *)((u8 *)input + (software ? n : 0)), 51);
        CHECK(spu_address == 0xFFFFFFFC && spu_size == (u32)n &&
              spu_destination == (u8 *)buffers[0] &&
              descriptor.field_30.word == (u32)(0xFFFFFFFC + (u32)n), 52);
        CHECK(descriptor.total_bytes == 4096 && descriptor.buffer_index == 0 &&
              descriptor.value_08 == (u32)buffers[0] && (D_8009B0F4 & 0x100), 53);
        CHECK(descriptor.phase_remaining == (sizes[j] > 2048 ? 2048 : 0) &&
              descriptor.phase_size == (sizes[j] > 2048 ? 0x8888 : 0), 54);
        CHECK(cd_calls == !software && events[software ? 0 : 1] == 5 &&
              events[software ? 1 : 2] == 6, 55);
        if (!software) CHECK(cd_words == n / 4 && cd_destination == buffers[0], 56);
    }
    return 0;
}

static s32 phase_callbacks(void)
{
    s32 mode, action, finish;
    for (mode = 1; mode <= 3; mode++)
    for (action = 0; action < 4; action++)
    for (finish = 0; finish < 2; finish++) {
        setup(mode, 1);
        descriptor.phase_remaining = 2048;
        descriptor.total_bytes = finish ? 2048 : 6144;
        descriptor.phase_callback = (FileTransferCallback)phase;
        descriptor.result = expected_sequence = 0xFFFFFFFF;
        callback_action = action;
        alternate.total_bytes = 4096;
        alternate.buffer_index = 9;
        File_TransferReadyCallback(1);
        CHECK(!error && callback_calls == 1 && descriptor.result == 0xABCDEF01 &&
              descriptor.phase_remaining == 0x1234 && descriptor.phase_size == 0x1234, 60);
        CHECK(events[event_count - 1] == 7, 61);
        if (finish) CHECK(events[0] == 2 && events[1] == 3, 62);
        else CHECK(events[0] != 2 && events[0] != 3, 63);
        CHECK((D_8009B0F4 & 0x100) ==
              ((action == 1 || (finish && action == 0)) ? 0 : 0x100), 64);
        CHECK((D_8009B0F4 & ~0x100) == 0x40000410, 65);
        CHECK(descriptor.buffer_index == (mode != 1 && action != 3) &&
              alternate.buffer_index == (mode != 1 && action == 3 ? 10 : 9), 66);
        CHECK(D_8009AF18 == (action == 3 ? &alternate : &descriptor), 67);
    }
    return 0;
}

static s32 completion(void)
{
    s32 mode, software, partial;
    for (mode = 1; mode <= 3; mode++)
    for (software = 0; software < 2; software++)
    for (partial = 0; partial < 2; partial++) {
        setup(mode, software);
        descriptor.total_bytes = partial ? 1 : 2048;
        descriptor.phase_remaining = 2048;
        descriptor.buffer_index = 0xFFFF;
        File_TransferReadyCallback(1);
        CHECK(descriptor.total_bytes == (partial ? -2047 : 0) &&
              !(D_8009B0F4 & 0x100) && !error, 70);
        CHECK(events[software ? 0 : 1] == 2 && events[software ? 1 : 2] == 3, 71);
        CHECK(descriptor.phase_size == 0 && descriptor.phase_remaining == 0 &&
              descriptor.result == 41 && callback_calls == 0, 72);
        CHECK(descriptor.buffer_index == (mode == 1 ? 0xFFFF : 0), 73);
        CHECK((D_8009B0F4 & ~0x100) == (0x410 | (software ? 0x40000000 : 0)), 74);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (sizeof(void *) != 4 || sizeof(long) != 4 ||
        sizeof(FileTransferDescriptor) != 0x48 || argc != 2) return 90;
    switch (argv[1][0]) {
    case '0': return ignored_events();
    case '1': return ram();
    case '2': return ram_descriptor_reload();
    case '3': return image();
    case '4': return spu();
    case '5': return phase_callbacks();
    case '6': return completion();
    }
    return 91;
}
"""


def scratch(test: unittest.TestCase) -> Path:
    if Path.cwd().resolve() != REPOSITORY:
        raise RuntimeError("run these tests from the repository root")
    (REPOSITORY / "tmp").mkdir(exist_ok=True)
    temporary = tempfile.TemporaryDirectory(prefix="cd-ready-test-", dir=REPOSITORY / "tmp")
    test.addCleanup(temporary.cleanup)
    return Path(temporary.name)


def environment(directory: Path) -> dict[str, str]:
    return {**os.environ, "TMPDIR": str(directory)}


@unittest.skipUnless(COMPILER.is_file(), "needs the GCC 2.8.1 toolchain")
class CdReadyHeaderTests(unittest.TestCase):
    def setUp(self):
        self.directory = scratch(self)

    def compile(self, text: str, *, warnings_as_errors: bool = True):
        source = self.directory / "probe.c"
        source.write_text(text)
        return subprocess.run(
            [str(COMPILER), "-S", "-O2", "-G8",
             *(["-Werror"] if warnings_as_errors else []),
             "-Wimplicit-function-declaration", f"-I{REPOSITORY}",
             "-o", str(self.directory / "probe.s"), str(source)],
            cwd=REPOSITORY, env=environment(self.directory),
            capture_output=True, text=True, errors="replace", timeout=60,
        )

    def test_both_positive_pointer_and_call_views(self):
        for prefix, signature, arguments in (
            ("", "s32", "1"),
            (CALLBACK_VIEW, "u8, u8 *, u32 *", "1, 0, 0"),
        ):
            with self.subTest(callback_view=bool(prefix)):
                result = self.compile(
                    prefix + '#include "src/game/file_transfer.h"\n'
                    f"void (*checked)({signature}) = func_80013C28;\n"
                    f"void caller(void) {{ func_80013C28({arguments}); }}\n"
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_opposite_pointer_views_warn_and_werror_is_effective(self):
        for prefix, signature in (
            ("", "u8, u8 *, u32 *"), (CALLBACK_VIEW, "s32"),
        ):
            with self.subTest(callback_view=bool(prefix)):
                text = prefix + '#include "src/game/file_transfer.h"\n'
                text += f"void (*checked)({signature}) = func_80013C28;\n"
                warning = self.compile(text, warnings_as_errors=False)
                self.assertEqual(warning.returncode, 0, warning.stderr)
                self.assertIn("incompatible pointer type", warning.stderr)
                rejected = self.compile(text)
                self.assertNotEqual(rejected.returncode, 0, rejected.stderr)
                self.assertIn("incompatible pointer type", rejected.stderr)

    def test_both_views_reject_wrong_argument_counts(self):
        for prefix, valid in (("", "1"), (CALLBACK_VIEW, "1, 0, 0")):
            for arguments in ("", "1", "1, 0", "1, 0, 0", "1, 0, 0, 0"):
                if arguments == valid:
                    continue
                with self.subTest(callback_view=bool(prefix), arguments=arguments):
                    result = self.compile(
                        prefix + '#include "src/game/file_transfer.h"\n'
                        f"void caller(void) {{ func_80013C28({arguments}); }}\n"
                    )
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, r"too (?:few|many) arguments")

    def test_missing_owner_is_not_rescued_by_unmatched(self):
        for prefix, arguments in (("", "1"), (CALLBACK_VIEW, "1, 0, 0")):
            with self.subTest(callback_view=bool(prefix)):
                call = f"void caller(void) {{ func_80013C28({arguments}); }}\n"
                result = self.compile(
                    prefix + '#include "src/types.h"\n#include "src/unmatched.h"\n' + call
                )
                self.assertNotEqual(result.returncode, 0, result.stderr)
                self.assertIn("implicit declaration of function `func_80013C28'", result.stderr)

    def test_grouped_definition_and_runtime_use_the_owner_view(self):
        result = self.compile(f'#include "{SOURCE.relative_to(REPOSITORY)}"\n')
        self.assertEqual(result.returncode, 0, result.stderr)


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires Linux x86 and a host compiler with ILP32 execution",
)
class CdReadyBehaviorTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="cd-ready-test-", dir=REPOSITORY / "tmp")
        cls.addClassCleanup(temporary.cleanup)
        directory = Path(temporary.name)
        witness = directory / "witness.c"
        witness.write_text(WITNESS)
        startup = directory / "start.S"
        startup.write_text(START)
        cls.binaries = {}
        for optimization in ("-O0", "-O2"):
            binary = directory / ("witness" + optimization)
            result = subprocess.run(
                ["cc", "-m32", optimization, "-std=gnu99", "-ffreestanding",
                 "-fno-builtin", "-fno-strict-aliasing", "-fno-pie", "-no-pie",
                 "-fno-stack-protector", "-nostdlib", f"-I{REPOSITORY}",
                 str(SOURCE), str(witness), str(startup), "-o", str(binary)],
                cwd=REPOSITORY, env=environment(directory),
                capture_output=True, text=True, timeout=60,
            )
            if result.returncode:
                raise AssertionError(result.stderr)
            cls.binaries[optimization] = binary

    def witness(self, scenario: int):
        for optimization, binary in self.binaries.items():
            with self.subTest(optimization=optimization):
                result = subprocess.run(
                    [str(binary), str(scenario)], cwd=REPOSITORY,
                    capture_output=True, text=True, timeout=10,
                )
                self.assertEqual(result.returncode, 0, result.stderr)

    def test_ignored_events_low_byte_and_counter_wrap(self):
        self.witness(0)

    def test_ram_hardware_software_skip_and_two_sector_stepping(self):
        self.witness(1)

    def test_ram_reloads_descriptor_destination_each_word(self):
        self.witness(2)

    def test_image_buffers_retry_coordinates_wrap_and_step(self):
        self.witness(3)

    def test_spu_partial_word_copy_address_wrap_and_upload(self):
        self.witness(4)

    def test_callback_postincrement_phase_mutation_and_descriptor_switch(self):
        self.witness(5)

    def test_completion_order_flags_and_null_callback(self):
        self.witness(6)


if __name__ == "__main__":
    unittest.main()
