"""Check the actual caller/copier with target ILP32 widths and safe extent guards."""

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
CALLER = REPOSITORY / "src/game/model_effect_state.c"
# Model_PrepareEffectKey shares its unit with the effect-state runtime; the
# witness compiles it alone against the contracts it uses.
CALLER_HEADERS = (
    "../types.h", "camera_view.h", "model_copy_slot_position.h",
    "model_get_slot_readiness.h", "model_keyframe_spline.h", "model_transfer_state.h",
    "model_transfer_flags.h",
)


def caller_source() -> str:
    text = CALLER.read_text()
    for header in CALLER_HEADERS:
        if f'#include "{header}"' not in text:
            raise AssertionError(f"{CALLER.name} no longer includes {header}")
    start = text.index("void Model_PrepareEffectKey(Key *state)\n{")
    end = text.index("\n}\n", start) + 3
    return "".join(
        f'#include "{(CALLER.parent / header).resolve()}"\n'
        for header in CALLER_HEADERS
    ) + "\n" + text[start:end]
COPIER = REPOSITORY / "src/game/model_slot_support.c"
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


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 host C compiler and ILP32 execution",
)
class KeyframeCoordinateExtentTests(unittest.TestCase):
    def run_witness(self, narrow_buffer: bool) -> subprocess.CompletedProcess[str]:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        copier = re.search(
            r"void Model_CopySlotPosition\([^{}]+\)\s*\{[^{}]+\}",
            COPIER.read_text(),
        )
        self.assertIsNotNone(copier)
        copier_source = copier.group(0)
        self.assertEqual(
            re.findall(r"out\[(\d+)\]\s*=", copier_source),
            ["0", "1", "2", "3"],
        )
        with tempfile.TemporaryDirectory(
            prefix="keyframe-extent-", dir=REPOSITORY / "tmp"
        ) as temporary:
            directory = Path(temporary)
            text = caller_source()
            if narrow_buffer:
                self.assertEqual(text.count("s16 slot_position[4];"), 1)
                text = text.replace("s16 slot_position[4];", "s16 slot_position[3];")
            caller = directory / "caller.c"
            caller.write_text(text)
            witness = directory / "witness.c"
            witness.write_text(
                r"""
#include "src/types.h"
#include "src/game/model_copy_slot_position.h"

typedef char pointer_must_be_ilp32[sizeof(void *) == 4 ? 1 : -1];

static void fixture_exit(s32 code) __attribute__((noreturn));
static void fixture_exit(s32 code)
{
    __asm__ volatile("int $0x80" : : "a"(1), "b"(code) : "memory");
    __builtin_unreachable();
}

static void write_text(s32 fd, const char *text, u32 length)
{
    s32 written;
    __asm__ volatile("int $0x80" : "=a"(written)
                     : "0"(4), "b"(fd), "c"(text), "d"(length)
                     : "memory", "cc");
    if (written != (s32)length) fixture_exit(14);
}

static void report_extent(u32 extent)
{
    char message[48] = "coordinate extent: ";
    char digits[10];
    u32 length = sizeof("coordinate extent: ") - 1;
    u32 count = 0;
    do {
        digits[count++] = '0' + extent % 10;
        extent /= 10;
    } while (extent);
    while (count) message[length++] = digits[--count];
    message[length++] = '\n';
    write_text(2, message, length);
}

static void copy_bytes(void *destination, const void *source, u32 length)
{
    u8 *out = destination;
    const u8 *in = source;
    while (length--) *out++ = *in++;
}

static void checked_copy(s32 slot, u16 *out, u32 extent);
#define Model_CopySlotPosition(slot, out) \
    checked_copy((slot), (out), __builtin_object_size((out), 0))
"""
                + '#include "' + str(caller) + '"\n'
                + r"""
#undef Model_CopySlotPosition

/* Only the copier's input field is needed by this host fixture. */
typedef struct { u16 cached_position[4]; } ModelSlot;
static ModelSlot gModel_aSlots[2] = {
    {{0xFFFE, 0xFFFD, 0xFFFC, 0x1357}},
    {{0x8000, 0xFFFF, 0, 0x2468}}
};
SVECTOR gModel_aPoseSnapshot[CAMERA_POSE_VECTOR_COUNT];
Key *gModel_pCurrentEffectKey;
static unsigned copies;
static unsigned roots;
"""
                + copier_source
                + r"""

static void checked_copy(s32 slot, u16 *out, u32 extent)
{
    if (extent != 4 * sizeof(u16)) {
        report_extent(extent);
        fixture_exit(7);
    }
    if (slot < 0 || slot >= 2) fixture_exit(8);
    out[3] = (u16)~gModel_aSlots[slot].cached_position[3];
    Model_CopySlotPosition(slot, out);
    if (*(volatile u16 *)(out + 3) != gModel_aSlots[slot].cached_position[3]) fixture_exit(9);
    copies++;
}

int Model_GetSlotReadiness(int slot)
{
    return slot >= 0 && slot < 2;
}

void Model_RefreshEffectKeySlotValues(Key *data)
{
    (void)data;
}

void Model_EvaluateKeyframeSpline(Key *cur, s32 k, s32 scale, s32 den, s16 *out)
{
    (void)cur; (void)k; (void)scale; (void)den; (void)out;
    fixture_exit(10);
}

long SquareRoot0(long value)
{
    static const long expected[] = {14, 1073676290};
    if (roots >= 2 || value != expected[roots]) fixture_exit(11);
    roots++;
    return 0;
}

int main(void)
{
    Key key = {0};
    const s16 channels[2][4] = {
        {1, 1, 1, 0x80},
        {0, 0, 0, 0x81}
    };
    copy_bytes(&key, channels, sizeof(channels));
    key.magnitude = -1;
    gModel_pCurrentEffectKey = &key;
    gModel_aPoseSnapshot[1].vx = -1;
    Model_PrepareEffectKey(&key);
    if (copies != 2 || roots != 2 || key.ready != 1) return 12;
    if (key.duration != 2 || key.progress != 0) return 13;
    {
        static const char message[] =
            "two four-halfword copies; signed coordinate arithmetic preserved\n";
        write_text(1, message, sizeof(message) - 1);
    }
    return 0;
}
"""
            )
            executable = directory / "witness"
            startup = directory / "start.S"
            startup.write_text(START)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            compilation = subprocess.run(
                [
                    shutil.which("cc"), "-m32", "-std=gnu99", "-O2",
                    "-nostdlib", "-fno-pie", "-no-pie",
                    "-fno-stack-protector", "-ffreestanding", "-fno-builtin",
                    "-fno-strict-aliasing", "-Werror=incompatible-pointer-types",
                    "-I", str(REPOSITORY), str(witness), str(startup),
                    "-o", str(executable),
                ],
                cwd=REPOSITORY, env=environment,
                capture_output=True, text=True, timeout=60,
            )
            self.assertEqual(
                compilation.returncode, 0, compilation.stdout + compilation.stderr
            )
            return subprocess.run(
                [str(executable)], cwd=REPOSITORY, env=environment,
                capture_output=True, text=True, timeout=30,
            )

    def test_ready_slots_copy_four_halfwords_with_signed_coordinates(self) -> None:
        result = self.run_witness(narrow_buffer=False)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertIn("two four-halfword copies", result.stdout)

    def test_three_halfword_buffer_is_rejected(self) -> None:
        result = self.run_witness(narrow_buffer=True)
        self.assertEqual(result.returncode, 7, result.stdout + result.stderr)
        self.assertIn("coordinate extent: 6", result.stderr)


if __name__ == "__main__":
    unittest.main()
