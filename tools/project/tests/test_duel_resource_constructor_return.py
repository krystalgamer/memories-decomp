"""Check the actual constructor return while preserving explicit caller views."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class DuelResourceConstructorReturnTests(unittest.TestCase):
    def test_exact_return_and_rejected_byte_or_opaque_contracts(self):
        with tempfile.TemporaryDirectory(
            prefix="resource-constructor-return-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = dict(os.environ, TMPDIR=str(directory))
            for result_type, accepted in (
                ("struct DisplayObject *", True),
                ("u8 *", False),
                ("void *", False),
            ):
                with self.subTest(result_type=result_type):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/game/func_800291E0.h"\n'
                        f"{result_type} (*checked)(s32, s32, s32) = func_800291E0;\n"
                        "struct PartialPreview;\n"
                        "struct PartialPreview *partial(void) {\n"
                        " return (struct PartialPreview *)func_800291E0(0, -1, -1);\n"
                        "}\n"
                        "u8 *bytes(void) {\n"
                        " return (u8 *)func_800291E0(0, -1, -1);\n"
                        "}\n"
                        "void *generic(void) { return func_800291E0(0, -1, -1); }\n"
                        "typedef char pointer_width[sizeof(struct DisplayObject *) == 4 ? 1 : -1];\n"
                    )
                    result = subprocess.run(
                        [
                            str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                            "-o", str(directory / "probe.s"), str(source),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
