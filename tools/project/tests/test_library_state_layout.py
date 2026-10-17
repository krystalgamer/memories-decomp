"""Check the shared Library declaration with the supported 32-bit compiler."""

import json
import os
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, tool


class LibraryStateLayoutTests(unittest.TestCase):
    def test_helper_pointer_contracts(self):
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        compiler = ROOT / profiles["gcc_2_8_1_g0"]["compiler"]
        if not compiler.is_file():
            self.skipTest("requires the supported local GCC 2.8.1 toolchain")
        contracts = (
            ("Library_UpdateGridScroll", "void", "const LibraryMotionState *"),
            ("Library_GetGridCursorCardId", "s32", "const LibraryState *"),
            ("Library_UpdateGridCursor", "void", "LibraryState *"),
            ("func_8002A2F4", "void", "const LibraryState *"),
            ("func_8002ACA4", "void", "LibraryState *"),
        )
        with tempfile.TemporaryDirectory(
            prefix="library-state-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for symbol, result_type, canonical in contracts:
                for argument, accepted in ((canonical, True), ("u8 *", False)):
                    with self.subTest(symbol=symbol, argument=argument):
                        source = directory / "probe.c"
                        source.write_text(
                            '#include "../../src/types.h"\n'
                            '#include "../../src/game/library_update_grid_scroll.h"\n'
                            '#include "../../src/game/library_grid_cursor.h"\n'
                            '#include "../../src/game/func_8002A2F4.h"\n'
                            '#include "../../src/game/library_runtime.h"\n'
                            f"{result_type} (*checked)({argument}) = {symbol};\n"
                        )
                        result = subprocess.run(
                            [
                                str(compiler), "-S", "-O2", "-G0", "-Werror",
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

    def test_shared_headers_and_card_id_boundaries(self):
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        compiler = ROOT / profiles["gcc_2_8_1_g0"]["compiler"]
        if not compiler.is_file():
            self.skipTest("requires the supported local GCC 2.8.1 toolchain")

        with tempfile.TemporaryDirectory(
            prefix="library-state-layout-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            relative = str(directory.relative_to(ROOT))
            source = directory / "layout.c"
            source.write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/game/library_runtime.h"\n'
                '#include "../../src/game/func_8002A3CC.h"\n'
                '#include "../../src/game/func_80029EB0.h"\n'
                "#define OFFSET(member) ((u32)&((LibraryState *)0)->member)\n"
                "const u32 library_layout[] = {\n"
                " sizeof(LibraryCardRecord), sizeof(LibraryMotionState),\n"
                " sizeof(LibraryState), OFFSET(motion.state),\n"
                " OFFSET(motion.selected_card_id), OFFSET(motion.x),\n"
                " OFFSET(motion.y), OFFSET(selector_sprite),\n"
                " OFFSET(card_view), OFFSET(loading_icon),\n"
                " OFFSET(cards[0].clut_x), OFFSET(cards[0].flags),\n"
                " OFFSET(cards[1].flags), OFFSET(cards[CARD_COUNT].clut_x),\n"
                " OFFSET(cards[CARD_COUNT].flags),\n"
                " LIBRARY_CARD_FLAG_KNOWN, LIBRARY_CARD_FLAG_UNOWNED\n"
                "};\n"
            )
            obj = compile_c(
                ROOT,
                tool(ROOT, "as"),
                {
                    "source": str(source.relative_to(ROOT)),
                    "profile": "gcc_2_8_1_g0",
                    "object": "layout.o",
                },
                profiles,
                object_directory=relative,
                asm_directory=relative,
            )
            data = directory / "layout.bin"
            subprocess.run(
                [
                    str(tool(ROOT, "objcopy")), "-O", "binary",
                    "-j", ".rodata", "-j", ".rdata", str(obj), str(data),
                ],
                cwd=ROOT,
                check=True,
            )
            expected = (
                0x4, 0x48, 0xBA0, 0, 6, 8, 0xA, 0x48, 0x4C, 0x50,
                0x54, 0x56, 0x5A, 0xB9C, 0xB9E, 0x80, 1,
            )
            compiled = data.read_bytes()
            length = len(expected) * 4
            self.assertEqual(compiled[:length], struct.pack("<17I", *expected))
            self.assertEqual(compiled[length:], bytes(len(compiled) - length))


if __name__ == "__main__":
    unittest.main()
