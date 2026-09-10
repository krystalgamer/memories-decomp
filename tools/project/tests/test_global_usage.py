from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from global_usage import (
    classify_c_access,
    inline_assembly_function_text,
    load_declarations,
    load_included_declarations,
    override_declarations,
    parse_c_functions,
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

        widths, arrays, declarations = load_declarations(
            self.header, set()
        )

        self.assertEqual(
            widths, {"D_80010000": "8", "D_80010002": "16"}
        )
        self.assertEqual(arrays, {"D_80010002"})
        self.assertEqual(
            declarations, {"D_80010000", "D_80010002"}
        )

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

        widths, arrays, declarations = load_included_declarations(
            self.header.parent, source, text, {"gSharedValues"}
        )

        self.assertEqual(widths, {"gSharedValues": "32"})
        self.assertEqual(arrays, {"gSharedValues"})
        self.assertEqual(declarations, {"gSharedValues"})

    def test_loads_declarations_from_non_utf8_header(self) -> None:
        self.header.write_bytes(
            b"/* extended byte: \\x89 */\nextern u16 D_80010002;\n"
        )

        widths, arrays, declarations = load_declarations(self.header, set())

        self.assertEqual(widths, {"D_80010002": "16"})
        self.assertEqual(arrays, set())
        self.assertEqual(declarations, {"D_80010002"})

    def test_array_of_pointers_uses_pointer_width(self) -> None:
        self.header.write_text(
            "extern GsOT *D_800E9D90[4];\n"
            "typedef char slots_size_must_be_16[\n"
            "    sizeof(D_800E9D90) == 16 ? 1 : -1\n"
            "];\n",
            encoding="utf-8",
        )

        widths, arrays, declarations = load_declarations(
            self.header, set()
        )

        self.assertEqual(widths, {"D_800E9D90": "32"})
        self.assertEqual(arrays, {"D_800E9D90"})
        self.assertEqual(declarations, {"D_800E9D90"})

    def test_conflicting_views_are_conservatively_unknown(self) -> None:
        self.header.write_text(
            "extern ModelPrefix D_80010000[];\n"
            "extern u8 *D_80010000;\n",
            encoding="utf-8",
        )

        widths, arrays, declarations = load_declarations(
            self.header, set()
        )

        self.assertEqual(widths, {"D_80010000": ""})
        self.assertEqual(arrays, {"D_80010000"})
        self.assertEqual(declarations, {"D_80010000"})

    def test_included_view_overrides_shared_fallback(self) -> None:
        widths, arrays = override_declarations(
            {"D_80010000": "8"},
            {"D_80010000"},
            {"D_80010000": ""},
            set(),
            {"D_80010000"},
        )

        self.assertEqual(widths, {"D_80010000": ""})
        self.assertEqual(arrays, {"D_80010000"})

    def test_member_access_does_not_decay_global_array(self) -> None:
        functions, _ = parse_c_functions(
            "void test(void) { id = D_8015C424.cards[n].id; }\n"
        )
        tokens = functions[0].tokens
        index = next(
            index
            for index, token in enumerate(tokens)
            if token.value == "D_8015C424"
        )

        self.assertEqual(
            classify_c_access(tokens, index, {"D_8015C424"}),
            "read",
        )


if __name__ == "__main__":
    unittest.main()
