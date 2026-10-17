"""Check the shared Library declaration with the supported 32-bit compiler."""

import json
import os
from pathlib import Path
import re
import struct
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, tool


class LibraryStateLayoutTests(unittest.TestCase):
    def test_card_view_text_box_uses_returned_channel(self):
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_UpdateCardView(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bDuelEffectChannel\s*\*\s*box\s*;")
        self.assertEqual(body.count("box = TextBox_Create("), 1)
        self.assertEqual(
            re.findall(r"box->(\w+)\s*=", body),
            ["field_54", "field_53", "field_59"],
        )
        self.assertNotRegex(body, r"\bB\s*\(\s*box\s*,")
        without_comments = re.sub(r"/\*.*?\*/", "", body, flags=re.S)
        self.assertIn(
            "box=TextBox_Create("
            "0,layout,LIBRARY_CARD_VIEW_X,LIBRARY_CARD_VIEW_Y,"
            "LIBRARY_CARD_VIEW_BOX_WIDTH,LIBRARY_CARD_VIEW_BOX_HEIGHT);"
            "#ifndefVERSION_EUROPE"
            "box->field_54=LIBRARY_CARD_VIEW_BOX_FIELD_54;"
            "#endif"
            "box->field_53=1;box->field_59=4;",
            re.sub(r"\s+", "", without_comments),
        )

    def test_initializer_keeps_the_owner_and_integer_card_carrier(self):
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_InitializeScreen(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bLibraryState\s*\*\s*r\s*;")
        self.assertRegex(
            body,
            r"Library_CreateGridSectionSprites\(\);\s*r = &gLibrary_State;\s*"
            r"gLibrary_State\.motion\.state = 0;",
        )
        for statement in (
            "r->motion.rest_x = x;", "r->motion.x = x;",
            "r->motion.rest_y = y;", "r->motion.y = y;",
            "Library_UpdateGridScroll(&r->motion);",
            "o, r->motion.x, r->motion.y,",
            "r->motion.display_objects.by_role.render = DISPLAY_OBJECT_VIEW(o);",
        ):
            with self.subTest(statement=statement):
                self.assertIn(statement, body)
        self.assertEqual(body.count("r->motion."), 7)
        self.assertNotIn("LIBRARY_MOTION_STATE_VIEW", source)
        self.assertRegex(body, r"\bs32 rb;")
        self.assertEqual(body.count("rb = (s32)r;"), 1)
        self.assertEqual(body.count("((LibraryState *)rb)->cards[n]"), 5)
        palette = body[body.index("rb = (s32)r;"):body.index("D_801D5608")]
        self.assertRegex(
            palette,
            r"do \{\s*\(\(LibraryState \*\)rb\)->cards\[n\]\.flags = 0;\s*"
            r"r->cards\[0\]\.clut_x = 0;",
        )
        self.assertRegex(palette, r"n\+\+;\s*\} while \(n < CARD_ID_END\);")

    def test_package_stage_european_aliases(self):
        for relative in (
            "european/library_runtime_8002BD0C.c",
            "european/library_runtime_8002BFCC.c",
        ):
            with self.subTest(adapter=relative):
                text = (ROOT / "src/game" / relative).read_text()
                self.assertIn(
                    "#define Library_LoadPackageStage func_8002BD4C", text
                )
                self.assertNotIn("#define func_8002BD0C ", text)

    def test_resident_global_bindings(self):
        bindings = (
            ("slus_01411/c_symbols.ld", 0x800EA1E8),
            ("slpm_86398/symbols.txt", 0x800EA0C8),
            ("sles_03947/symbols.txt", 0x800EB150),
            ("sles_03948/link_symbols.ld", 0x800EB570),
            ("sles_03949/link_symbols.ld", 0x800EB570),
            ("sles_03950/link_symbols.ld", 0x800EB570),
            ("sles_03951/link_symbols.ld", 0x800EB570),
        )
        for relative, address in bindings:
            with self.subTest(region=relative):
                text = (ROOT / "config" / relative).read_text()
                rows = re.findall(
                    r"(?m)^gLibrary_State = (0x[0-9A-Fa-f]+);", text
                )
                self.assertEqual(rows, [f"0x{address:08X}"])
                self.assertNotRegex(text, r"(?m)^D_800EA1E8 = ")
                self.assertIn(
                    f"D_800EAD88 = 0x{address + 0xBA0:08X};", text
                )
        french = (ROOT / "config/sles_03948/symbols.txt").read_text()
        self.assertIn("D_800EA1E8_French = 0x800EA1E8;", french)
        self.assertNotIn("gLibrary_State = 0x800EA1E8;", french)

    def test_helper_pointer_contracts(self):
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        compiler = ROOT / profiles["gcc_2_8_1_g0"]["compiler"]
        if not compiler.is_file():
            self.skipTest("requires the supported local GCC 2.8.1 toolchain")
        contracts = (
            ("Library_UpdateGridScroll", "void", "const LibraryMotionState *", "u8 *"),
            ("Library_GetGridCursorCardId", "s32", "const LibraryState *", "u8 *"),
            ("Library_UpdateGridCursor", "void", "LibraryState *", "u8 *"),
            ("Library_RefreshSelectedCardText", "void", "const LibraryState *", "u8 *"),
            ("Library_UpdateCardView", "void", "LibraryState *", "u8 *"),
            ("Library_EnterGridState", "void", "LibraryState *", "u8 *"),
            ("Library_UpdateScreen", "void", "void", "u8 *"),
            ("Library_InitializeScreen", "void", "void", "u8 *"),
            ("Library_CreateGridSectionSprites", "void", "void", "u8 *"),
            ("Library_DrawWireframeGlobe", "void", "void", "u8 *"),
            (
                "Library_DrawGlobeXSpan", "void",
                "LINE_G3 *, GsOT *, SVECTOR *, PSXLONG *, s32, s32",
                "LINE_G2 *, GsOT *, SVECTOR *, PSXLONG *, s32, s32",
            ),
            (
                "Library_DrawGlobeZSpan", "void",
                "LINE_G3 *, GsOT *, SVECTOR *, PSXLONG *, s32, s32",
                "LINE_G3 *, GsOT *, SVECTOR *, s16 *, s32, s32",
            ),
            ("Library_LoadPackageStage", "void", "FileTransferDescriptor *, s32", "u8 *, s32"),
        )
        with tempfile.TemporaryDirectory(
            prefix="library-state-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for symbol, result_type, canonical, rejected in contracts:
                for argument, accepted in ((canonical, True), (rejected, False)):
                    with self.subTest(symbol=symbol, argument=argument):
                        source = directory / "probe.c"
                        source.write_text(
                            '#include "../../src/types.h"\n'
                            '#include "../../src/game/library_update_grid_scroll.h"\n'
                            '#include "../../src/game/library_grid_cursor.h"\n'
                            '#include "../../src/game/library_refresh_selected_card_text.h"\n'
                            '#include "../../src/game/library_runtime.h"\n'
                            '#include "../../src/game/library_create_grid_section_sprites.h"\n'
                            '#include "../../src/game/library_draw_wireframe_globe.h"\n'
                            '#include "../../src/game/duel_projection_axes.h"\n'
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

    def test_card_view_uses_canonical_model_light_records(self):
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_UpdateCardView(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bModelSlotLightEntry\s*\*\s*light\s*;")
        self.assertEqual(
            re.findall(
                r"light\s*=\s*\(ModelSlotLightEntry\s*\*\)\s*"
                r"Model_GetFlatLight\(0,\s*([02])\)", body,
            ),
            ["2", "0"],
        )
        self.assertEqual(
            re.findall(
                r"\blight(->|\[[01]\]\.)(field_\w+)\s*=\s*([^;]+);", body,
            ),
            [
                ("->", "field_00", "b"),
                ("->", "field_04", "c"),
                ("->", "field_08", "a"),
                ("[0].", "field_04", "-0x1000"),
                ("[0].", "field_00", "0"),
                ("[0].", "field_08", "0"),
                ("[1].", "field_00", "0"),
                ("[1].", "field_04", "0x1000"),
                ("[1].", "field_08", "0"),
            ],
        )
        self.assertEqual(
            re.findall(r"\bW\(light,\s*(0x[\dA-F]+)\)\s*=\s*([^;]+);", body),
            [
                ("0xC", "0xFFFFFF"),
                ("0x1C", "0x404040"),
                ("0x2C", "0xC0C0C0"),
            ],
        )

    def test_card_view_uses_actual_display_object_local(self):
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_UpdateCardView(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bDisplayObject\s*\*\s*o\s*;")
        self.assertEqual(body.count("o = state->card_view;"), 2)
        self.assertIn("o = (DisplayObject *)func_800291E0(0, -1, -1);", body)
        self.assertNotRegex(body, r"\bH\(o,\s*8\)")
        self.assertEqual(
            [line.strip() for line in body.splitlines() if "o->flags" in line],
            [
                "o->flags |= 0x28;",
                "bits = o->flags;",
                "o->flags = bits;",
                "o->flags |= 8;",
                "o->flags &= 0xFFFB;",
                "o->flags |= 4;",
                "if (o->flags & 4) {",
                "o->flags &= 0xFFFB;",
                "if (o->flags & 4) {",
            ],
        )
        self.assertRegex(
            body, r"bits = o->flags;\s*bits \|= 4;\s*o->flags = bits;"
        )

    def test_dispatcher_keeps_the_captured_library_owner(self):
        source = (ROOT / "src/game/library_runtime.c").read_text()
        start = source.index("void Library_UpdateScreen(")
        body = source[start:source.index("\n}\n", start)]
        self.assertRegex(body, r"\bLibraryState\s*\*\s*state\s*;")
        self.assertRegex(
            body,
            r"state = &gLibrary_State;\s*one = 1;\s*"
            r"mode = gLibrary_State\.motion\.state;\s*mode &= 0xF;",
        )
        for mode, call in (
            (0, "Library_EnterGridState(state);"),
            (1, "Library_UpdateGridCursor(state);"),
            (2, "Library_UpdateCardView(state);"),
            (3, "func_8002BAAC((u8 *)state);"),
        ):
            with self.subTest(mode=mode):
                self.assertRegex(body, rf"case {mode}:\s*" + re.escape(call))
        self.assertNotIn("LIBRARY_MOTION_STATE_VIEW(state)", body)
        self.assertEqual(body.count("state->motion.animation_countdown"), 3)
        self.assertRegex(
            body,
            r"count = state->motion\.animation_countdown - 1;\s*"
            r"state->motion\.animation_countdown = count;\s*"
            r"if \(count <= 0\) \{\s*"
            r"state->motion\.animation_countdown = 0x12C;",
        )

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
                '#include "../../src/psyq/libgte.h"\n'
                '#include "../../src/psyq/libgpu.h"\n'
                '#include "../../src/psyq/libgs.h"\n'
                '#include "../../src/game/library_runtime.h"\n'
                '#include "../../src/game/func_8002A3CC.h"\n'
                '#include "../../src/game/func_80029EB0.h"\n'
                '#include "../../src/game/japanese/duel_effect_channel.h"\n'
                '#include "../../src/game/model.h"\n'
                '#include "../../src/game/display_object.h"\n'
                "LibraryState *library_owner = &gLibrary_State;\n"
                "#define OFFSET(member) ((u32)&((LibraryState *)0)->member)\n"
                "const u32 library_layout[] = {\n"
                " sizeof(LibraryCardRecord), sizeof(LibraryMotionState),\n"
                " sizeof(LibraryState), sizeof(LibraryDisplayObjects),\n"
                " OFFSET(motion.state),\n"
                " OFFSET(motion.card_view_phase),\n"
                " OFFSET(motion.card_view_subphase),\n"
                " OFFSET(motion.card_view_slide_phase),\n"
                " OFFSET(motion.model_load_phase),\n"
                " OFFSET(motion.selected_card_id), OFFSET(motion.x),\n"
                " OFFSET(motion.y), OFFSET(motion.animation_countdown),\n"
                " OFFSET(motion.display_objects.by_role.slots[0]),\n"
                " OFFSET(motion.display_objects.by_role.slots[7]),\n"
                " OFFSET(motion.display_objects.by_role.render),\n"
                " OFFSET(motion.display_objects.all[0]),\n"
                " OFFSET(motion.display_objects.all[8]),\n"
                " OFFSET(selector_sprite),\n"
                " OFFSET(card_view), OFFSET(loading_icon),\n"
                " OFFSET(cards[0].clut_x), OFFSET(cards[0].flags),\n"
                " OFFSET(cards[1].flags), OFFSET(cards[CARD_COUNT].clut_x),\n"
                " OFFSET(cards[CARD_COUNT].flags),\n"
                " LIBRARY_CARD_FLAG_KNOWN, LIBRARY_CARD_FLAG_UNOWNED,\n"
                " (u32)&((DuelEffectChannel *)0)->field_54,\n"
                " sizeof(((DuelEffectChannel *)0)->field_54),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->field_54,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->field_54),\n"
                " (u32)&((DuelEffectChannel *)0)->field_53,\n"
                " sizeof(((DuelEffectChannel *)0)->field_53),\n"
                " (u32)&((DuelEffectChannel *)0)->field_59,\n"
                " sizeof(((DuelEffectChannel *)0)->field_59),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->field_53,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->field_53),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->field_59,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->field_59),\n"
                " sizeof(ModelSlotLightEntry),\n"
                " sizeof(((ModelSlot *)0)->field_D70),\n"
                " (u32)&((ModelSlot *)0)->field_D70,\n"
                " (u32)&((ModelSlotLightEntry *)0)->field_00,\n"
                " (u32)&((ModelSlotLightEntry *)0)->field_04,\n"
                " (u32)&((ModelSlotLightEntry *)0)->field_08,\n"
                " (u32)&((ModelSlotLightEntry *)0)->field_0C,\n"
                " sizeof(((ModelSlotLightEntry *)0)->field_0C),\n"
                " (u32)&((ModelSlotLightEntry *)0)->pad_0F,\n"
                " sizeof(((ModelSlotLightEntry *)0)->field_00),\n"
                " sizeof(GsF_LIGHT), (u32)&((GsF_LIGHT *)0)->r,\n"
                " sizeof(SVECTOR), (u32)&((SVECTOR *)0)->vx,\n"
                " (u32)&((SVECTOR *)0)->vy, (u32)&((SVECTOR *)0)->vz,\n"
                " sizeof(((SVECTOR *)0)->vx), (u32)&((SVECTOR *)0)->pad,\n"
                " sizeof(((ModelSlot *)0)->field_DD0),\n"
                " sizeof(DisplayObject), (u32)&((DisplayObject *)0)->flags,\n"
                " sizeof(((DisplayObject *)0)->flags)\n"
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
                0x4, 0x48, 0xBA0, 0x24, 0, 1, 2, 3, 4, 6, 8, 0xA, 0x20,
                0x24, 0x40, 0x44, 0x24, 0x44,
                0x48, 0x4C, 0x50,
                0x54, 0x56, 0x5A, 0xB9C, 0xB9E, 0x80, 1,
                0x54, 1, 0x54, 1,
                0x53, 1, 0x59, 1, 0x53, 1, 0x59, 1,
                0x10, 0x30, 0xD70, 0, 4, 8, 0xC, 3, 0xF, 4, 0x10, 0xC,
                8, 0, 2, 4, 2, 6, 8,
                0x70, 8, 2,
            )
            compiled = data.read_bytes()
            length = len(expected) * 4
            self.assertEqual(compiled[:length], struct.pack("<62I", *expected))
            self.assertEqual(compiled[length:], bytes(len(compiled) - length))


if __name__ == "__main__":
    unittest.main()
