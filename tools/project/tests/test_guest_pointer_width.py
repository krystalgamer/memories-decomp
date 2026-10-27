"""Guest-pointer zero extension on LP64, not covered by the PSX hash."""

from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]


class GuestPointerWidthTests(unittest.TestCase):
    def test_native_guest_pointer_zero_extension(self):
        compiler = shutil.which("clang")
        if compiler is None or platform.machine().lower() not in ("x86_64", "amd64", "aarch64"):
            self.skipTest("64-bit x86/ARM clang required for native guest-pointer check")
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french400-pointer-") as name:
            directory = Path(name)
            source = directory / "pointer.c"
            source.write_text(
                '#include "../../src/types.h"\n#include "../../src/port_ptr.h"\n'
                'int main(void) {\n'
                '    u8 work[0x2060] = {0};\n'
                '    *(u8 *G32 *)(work + 0x205C) = (u8 *)0x8013D844u;\n'
                '    return sizeof(u8 *G32) != 4 ||\n'
                '        (u64)*(u8 *G32 *)(work + 0x205C) != 0x8013D844u;\n'
                '}\n')
            binary = directory / "pointer"
            subprocess.run([compiler, "-DMEMORIES_PC", "-fms-extensions", "-Werror",
                            str(source), "-o", str(binary)], cwd=ROOT, check=True, capture_output=True)
            subprocess.run([str(binary)], cwd=ROOT, check=True, capture_output=True)



if __name__ == "__main__":
    unittest.main()
