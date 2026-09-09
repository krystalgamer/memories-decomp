from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from global_usage import (
    inline_assembly_function_text,
    load_declarations,
    load_included_declarations,
)


class SharedDeclarationTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="global-usage-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.header = Path(temporary.name) / "unmatched.h"

    def test_loads_shared_global_widths_and_arrays(self) -> None:
        self.header.write_text(
            "extern u8 D_80010000;\n"
            "extern u16 D_80010002[];\n"
            "void func_80020000(void);\n",
            encoding="utf-8",
        )

        widths, arrays = load_declarations(
            self.header, set()
        )

        self.assertEqual(
            widths, {"D_80010000": "8", "D_80010002": "16"}
        )
        self.assertEqual(arrays, {"D_80010002"})

    def test_isolates_named_inline_assembly_function(self) -> None:
        text = (
            '__asm__(\n'
            '    ".ent func_80010000\\n"\n'
            '    "func_80010000:\\n"\n'
            '    ".word 0x00000000\\n"\n'
            '    ".end func_80010000\\n"\n'
            '    ".ent func_80010004\\n"\n'
            '    "func_80010004:\\n"\n'
            '    ".word 0x00000001\\n"\n'
            '    ".end func_80010004\\n"\n'
            ');\n'
        )

        self.assertEqual(
            inline_assembly_function_text(text, "func_80010004"),
            "func_80010004:\n.word 0x00000001\n.end func_80010004",
        )

    def test_loads_direct_project_header_declarations(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text("extern s32 gSharedValues[];\n", encoding="utf-8")
        source = self.header.parent / "source.c"
        text = '#include "shared.h"\n'
        source.write_text(text, encoding="utf-8")

        widths, arrays = load_included_declarations(
            self.header.parent, source, text, {"gSharedValues"}
        )

        self.assertEqual(widths, {"gSharedValues": "32"})
        self.assertEqual(arrays, {"gSharedValues"})

    def test_loads_declarations_from_non_utf8_header(self) -> None:
        self.header.write_bytes(
            b"/* extended byte: \\x89 */\nextern u16 D_80010002;\n"
        )

        widths, arrays = load_declarations(self.header, set())

        self.assertEqual(widths, {"D_80010002": "16"})
        self.assertEqual(arrays, set())


if __name__ == "__main__":
    unittest.main()
