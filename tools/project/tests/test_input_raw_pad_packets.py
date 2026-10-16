"""Compare the resident raw-pad decoder with an independent byte-layout oracle."""

from pathlib import Path
import os
import platform
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
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
FIXTURE = r"""
#include "src/types.h"
#include "src/game/input.h"

InputRawPadPacket gInput_abRawPadBuffers[INPUT_PAD_COUNT];
u32 gInput_dwPendingHeld;
static u32 cases;

static int check(u32 pending)
{
    u8 *raw = (u8 *)gInput_abRawPadBuffers;
    u8 snapshot[0x44];
    u32 expected = pending;
    s32 port;
    s32 i;

    for (i = 0; i < 0x44; i++) {
        snapshot[i] = raw[i];
    }
    for (port = 0; port < 2; port++) {
        u8 *packet = snapshot + port * 0x22;
        if (packet[0] == 0 && (packet[1] & 15) != 0) {
            u32 buttons = (packet[2] * 256 + packet[3]) ^ 65535;
            expected |= buttons << (port * 16);
        }
    }
    gInput_dwPendingHeld = pending;
    Input_ReadRawPads();
    if (gInput_dwPendingHeld != expected) {
        return 1;
    }
    for (i = 0; i < 0x44; i++) {
        if (raw[i] != snapshot[i]) {
            return 2;
        }
    }
    cases++;
    return 0;
}

int main(void)
{
    u8 *raw = (u8 *)gInput_abRawPadBuffers;
    u32 buttons;
    s32 status;
    s32 format;
    s32 i;
    s32 result;

    if (sizeof(gInput_abRawPadBuffers) != 0x44) {
        return 3;
    }
    for (i = 0; i < 0x44; i++) {
        raw[i] = i * 37 + 11;
    }
    raw[0] = raw[0x22] = 0;
    raw[1] = raw[0x23] = 0x41;
    for (buttons = 0; buttons < 65536; buttons++) {
        raw[2] = buttons >> 8;
        raw[3] = buttons;
        raw[0x24] = (buttons ^ 65535) >> 8;
        raw[0x25] = buttons ^ 65535;
        result = check(buttons & 1 ? 0xA500005A : 0);
        if (result) {
            return result;
        }
    }
    raw[2] = 0x12;
    raw[3] = 0x34;
    raw[0x24] = 0xAB;
    raw[0x25] = 0xCD;
    for (status = 0; status < 256; status++) {
        for (format = 0; format < 256; format++) {
            raw[0] = status;
            raw[1] = format;
            raw[0x22] = status ^ 255;
            raw[0x23] = format ^ 255;
            result = check(0x80000001);
            if (result) {
                return result;
            }
        }
    }
    return cases == 131072 ? 0 : 4;
}
"""


@unittest.skipUnless(
    platform.system() == "Linux"
    and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
    and shutil.which("cc"),
    "requires a Linux x86 compiler and ILP32 execution",
)
class InputRawPadPacketTests(unittest.TestCase):
    def test_all_button_words_and_status_format_bytes(self):
        with tempfile.TemporaryDirectory(
            prefix="input-raw-pad-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            fixture = directory / "fixture.c"
            fixture.write_text(FIXTURE)
            start = directory / "start.S"
            start.write_text(START)
            # Complete the real layout before modern GCC sees its extern array.
            layout = (ROOT / "src/game/graphics_frame_buffer.h").read_text()
            definition = re.search(
                r"struct GraphicsFrameBuffer \{.*?\n\};", layout, re.DOTALL
            )
            self.assertIsNotNone(definition)
            compatibility = directory / "host-types.h"
            compatibility.write_text(
                '#ifndef __ASSEMBLER__\n#include "src/types.h"\n'
                '#include "src/psyq/libgte.h"\n#include "src/psyq/libgpu.h"\n'
                '#include "src/psyq/libgs.h"\n'
                + definition[0] + "\n#endif\n"
            )
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for optimization in ("-O0", "-O2"):
                with self.subTest(optimization=optimization):
                    binary = directory / "witness"
                    command = [
                        "cc", "-std=gnu89", "-m32", "-nostdlib",
                        "-fno-pie", "-no-pie", "-fno-stack-protector",
                        "-ffreestanding", "-fno-builtin",
                        "-ffunction-sections", "-fdata-sections",
                        "-Wl,--gc-sections", optimization, "-I", str(ROOT),
                        "-include", str(compatibility),
                        str(ROOT / "src/game/input_pads.c"),
                        str(fixture), str(start), "-o", str(binary),
                    ]
                    result = subprocess.run(
                        command, cwd=ROOT, text=True, capture_output=True,
                        timeout=60, env=environment,
                    )
                    self.assertEqual(
                        result.returncode, 0, result.stdout + result.stderr
                    )
                    result = subprocess.run(
                        [str(binary)], cwd=ROOT, text=True,
                        capture_output=True, timeout=30, env=environment,
                    )
                    self.assertEqual(
                        result.returncode, 0, result.stdout + result.stderr
                    )


if __name__ == "__main__":
    unittest.main()
