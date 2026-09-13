from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from global_usage import (
    classify_c_access,
    combined_access,
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

    def test_loads_only_active_conditional_declaration_view(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "#ifndef SHARED_H\n"
            "#define SHARED_H\n"
            "#ifdef USE_ARRAY_VIEW\n"
            "extern u16 gSharedValues[];\n"
            "#else\n"
            "extern u16 gSharedValues;\n"
            "#endif\n"
            "#endif\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"

        scalar = '#include "shared.h"\n'
        scalar_widths, scalar_arrays, scalar_declarations = (
            load_included_declarations(
                self.header.parent,
                source,
                scalar,
                {"gSharedValues"},
            )
        )
        array = '#define USE_ARRAY_VIEW\n#include "shared.h"\n'
        array_widths, array_arrays, array_declarations = (
            load_included_declarations(
                self.header.parent,
                source,
                array,
                {"gSharedValues"},
            )
        )

        self.assertEqual(scalar_widths, {"gSharedValues": "16"})
        self.assertEqual(scalar_arrays, set())
        self.assertEqual(scalar_declarations, {"gSharedValues"})
        self.assertEqual(array_widths, {"gSharedValues": "16"})
        self.assertEqual(array_arrays, {"gSharedValues"})
        self.assertEqual(array_declarations, {"gSharedValues"})

    def test_inactive_source_define_does_not_select_header_view(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "#ifdef USE_BYTE_VIEW\n"
            "extern u8 gSharedValues[];\n"
            "#else\n"
            "extern u32 gSharedValues;\n"
            "#endif\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"
        text = (
            "#if 0\n"
            "#define USE_BYTE_VIEW\n"
            "#endif\n"
            '#include "shared.h"\n'
        )

        widths, arrays, declarations = load_included_declarations(
            self.header.parent,
            source,
            text,
            {"gSharedValues"},
        )

        self.assertEqual(widths, {"gSharedValues": "32"})
        self.assertEqual(arrays, set())
        self.assertEqual(declarations, {"gSharedValues"})

    def test_inactive_source_undef_keeps_header_view(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "#ifdef USE_BYTE_VIEW\n"
            "extern u8 gSharedValues[];\n"
            "#else\n"
            "extern u32 gSharedValues;\n"
            "#endif\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"
        text = (
            "#define USE_BYTE_VIEW\n"
            "#if 0\n"
            "#undef USE_BYTE_VIEW\n"
            "#endif\n"
            '#include "shared.h"\n'
        )

        widths, arrays, declarations = load_included_declarations(
            self.header.parent,
            source,
            text,
            {"gSharedValues"},
        )

        self.assertEqual(widths, {"gSharedValues": "8"})
        self.assertEqual(arrays, {"gSharedValues"})
        self.assertEqual(declarations, {"gSharedValues"})

    def test_inactive_source_include_contributes_no_declarations(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "extern u32 gSharedValues;\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"
        text = '#if 0\n#include "shared.h"\n#endif\n'

        widths, arrays, declarations = load_included_declarations(
            self.header.parent,
            source,
            text,
            {"gSharedValues"},
        )

        self.assertEqual(widths, {})
        self.assertEqual(arrays, set())
        self.assertEqual(declarations, set())

    def test_unsupported_source_condition_keeps_views_unresolved(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "#ifdef USE_BYTE_VIEW\n"
            "extern u8 gSharedValues[];\n"
            "#else\n"
            "extern u32 gSharedValues;\n"
            "#endif\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"
        text = (
            "#if FEATURE_FLAG || OTHER_FLAG\n"
            "#define USE_BYTE_VIEW\n"
            "#endif\n"
            '#include "shared.h"\n'
        )

        widths, arrays, declarations = load_included_declarations(
            self.header.parent,
            source,
            text,
            {"gSharedValues"},
        )

        self.assertEqual(widths, {"gSharedValues": ""})
        self.assertEqual(arrays, {"gSharedValues"})
        self.assertEqual(declarations, {"gSharedValues"})

    def test_unsupported_condition_keeps_conflicting_views(self) -> None:
        header = self.header.parent / "shared.h"
        header.write_text(
            "#if defined(USE_BYTE_VIEW) || defined(USE_WORD_VIEW)\n"
            "extern u8 gSharedValues[];\n"
            "#else\n"
            "extern u16 gSharedValues;\n"
            "#endif\n",
            encoding="utf-8",
        )
        source = self.header.parent / "source.c"
        text = '#include "shared.h"\n'

        widths, arrays, declarations = load_included_declarations(
            self.header.parent,
            source,
            text,
            {"gSharedValues"},
        )

        self.assertEqual(widths, {"gSharedValues": ""})
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

    def test_included_scalar_view_overrides_shared_fallback(self) -> None:
        widths, arrays = override_declarations(
            {"D_80010000": "8"},
            {"D_80010000"},
            {"D_80010000": ""},
            set(),
            {"D_80010000"},
        )

        self.assertEqual(widths, {"D_80010000": ""})
        self.assertEqual(arrays, set())

    def test_scalar_override_preserves_bare_read_write_access(self) -> None:
        _widths, arrays = override_declarations(
            {"D_8009B27C": "16"},
            {"D_8009B27C"},
            {"D_8009B27C": "16"},
            set(),
            {"D_8009B27C"},
        )
        functions, _ = parse_c_functions(
            "void test(void) {\n"
            "    u16 value = D_8009B27C;\n"
            "    D_8009B27C = value;\n"
            "}\n"
        )
        tokens = functions[0].tokens
        accesses = {
            classify_c_access(tokens, index, arrays)
            for index, token in enumerate(tokens)
            if token.value == "D_8009B27C"
        }

        self.assertEqual(accesses, {"read", "write"})
        self.assertEqual(combined_access(accesses), "read_write")

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

    def test_cast_array_dereference_classifies_pointee_access(self) -> None:
        functions, _ = parse_c_functions(
            "void test(void) {\n"
            "    value = *(Record *)(D_801D4200_raw + 32);\n"
            "    *(Record *)(D_801D4200_raw + 32) = value;\n"
            "    *(Record *)(D_8009B1F0[index] + 32) = value;\n"
            "}\n"
        )
        tokens = functions[0].tokens
        accesses = [
            classify_c_access(tokens, index, {"D_801D4200_raw"})
            for index, token in enumerate(tokens)
            if token.value == "D_801D4200_raw"
        ]
        pointer_array_access = next(
            classify_c_access(
                tokens,
                index,
                {"D_801D4200_raw", "D_8009B1F0"},
            )
            for index, token in enumerate(tokens)
            if token.value == "D_8009B1F0"
        )

        self.assertEqual(accesses, ["read", "write"])
        self.assertEqual(pointer_array_access, "read")

    def test_pointer_member_assignment_reads_global_pointer(self) -> None:
        functions, _ = parse_c_functions(
            "void test(void) {\n"
            "    gState->field = 1;\n"
            "    value = gState->field;\n"
            "    size = sizeof(gState->field);\n"
            "}\n"
        )
        tokens = functions[0].tokens
        accesses = [
            classify_c_access(tokens, index, set())
            for index, token in enumerate(tokens)
            if token.value == "gState"
        ]

        self.assertEqual(accesses, ["read", "read", "unknown"])


if __name__ == "__main__":
    unittest.main()
