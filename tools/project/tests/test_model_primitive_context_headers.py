"""Check the primitive decoder's SDK descriptor/context boundary."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class ModelPrimitiveContextHeaderTests(unittest.TestCase):
    def probe(self, body, *, accepted):
        with tempfile.TemporaryDirectory(
            prefix="model-primitive-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            source = directory / "probe.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/game/model_slot_row_tables.h"\n' + body
            )
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            result = subprocess.run(
                [
                    str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                    "-o", str(directory / "probe.s"), str(source),
                ],
                cwd=ROOT, env=environment, text=True, capture_output=True,
                timeout=60,
            )
            if accepted:
                self.assertEqual(result.returncode, 0, result.stderr)
            else:
                self.assertNotEqual(result.returncode, 0, result.stderr)
                self.assertIn("incompatible pointer type", result.stderr)

    def test_typed_function_pointer(self):
        self.probe(
            "s32 (*checked)(s32, const GsTYPEUNIT *, GsARGUNIT *, s32 *, s32 *)"
            " = tent_Model_RebaseTexturedPrimitives;\n",
            accepted=True,
        )

    def test_normal_and_shared_sdk_prefixes(self):
        self.probe(
            "typedef char normal_primtop[(u32)&((GsARGUNIT_NORMAL *)0)->primtop"
            " == 0x14 ? 1 : -1];\n"
            "typedef char shared_primtop[(u32)&((GsARGUNIT_SHARED *)0)->primtop"
            " == 0x14 ? 1 : -1];\n"
            "void checked(GsTYPEUNIT *unit, GsARGUNIT_NORMAL *normal,\n"
            "             GsARGUNIT_SHARED *shared, s32 *best, s32 *total) {\n"
            "  tent_Model_RebaseTexturedPrimitives(\n"
            "      0, unit, (GsARGUNIT *)normal, best, total);\n"
            "  tent_Model_RebaseTexturedPrimitives(\n"
            "      1, unit, (GsARGUNIT *)shared, best, total);\n"
            "}\n",
            accepted=True,
        )

    def test_raw_descriptor_and_byte_context_contracts_are_rejected(self):
        for descriptor, context in (
            ("u16 *", "GsARGUNIT *"),
            ("const GsTYPEUNIT *", "u8 *"),
        ):
            with self.subTest(descriptor=descriptor, context=context):
                self.probe(
                    f"s32 (*checked)(s32, {descriptor}, {context}, s32 *, s32 *)"
                    " = tent_Model_RebaseTexturedPrimitives;\n",
                    accepted=False,
                )


if __name__ == "__main__":
    unittest.main()
