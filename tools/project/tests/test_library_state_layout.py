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
                    f"gDuelEffect_aRequests = 0x{address + 0xBA0:08X};", text
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
                "Library_CreateFadingCardOutline", "DisplayObject *",
                "DisplayObject *, s32", "DisplayObject *, u8 *",
            ),
            (
                "Library_DrawFadingCardOutline", "void",
                "DisplayObject *, GsOT *", "DisplayObject *, u8 *",
            ),
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
                            '#include "../../src/game/library_fading_card_outline.h"\n'
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
                '#include "../../src/game/library_card_grid.h"\n'
                '#include "../../src/game/japanese/duel_effect_channel.h"\n'
                '#include "../../src/game/model.h"\n'
                '#include "../../src/game/display_object.h"\n'
                "LibraryState *library_owner = &gLibrary_State;\n"
                "typedef char count_text_font_a[\n"
                " (u32)&((DuelEffectChannel *)0)->glyph_advance_x == 0x5A &&\n"
                " sizeof(((DuelEffectChannel *)0)->glyph_advance_x) == 1 &&\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->glyph_advance_x == 0x5A &&\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->glyph_advance_x) == 1 ? 1 : -1];\n"
                "typedef char count_text_font_b[\n"
                " (u32)&((DuelEffectChannel *)0)->line_advance_y == 0x5B &&\n"
                " sizeof(((DuelEffectChannel *)0)->line_advance_y) == 1 &&\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->line_advance_y == 0x5B &&\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->line_advance_y) == 1 ? 1 : -1];\n"
                "typedef char count_text_object[\n"
                " (u32)&((DuelEffectChannel *)0)->render_object == 0x28 &&\n"
                " sizeof(((DuelEffectChannel *)0)->render_object) == 4 ? 1 : -1];\n"
                "typedef char view_copy_size[sizeof(GsRVIEW2) ==\n"
                " 2 * sizeof(LibraryViewQuad) ? 1 : -1];\n"
                "typedef char view_copy_first[(u32)&((GsRVIEW2 *)0)->vpx\n"
                " == 0 ? 1 : -1];\n"
                "typedef char view_copy_second[(u32)&((GsRVIEW2 *)0)->vry\n"
                " == sizeof(LibraryViewQuad) ? 1 : -1];\n"
                "#define OFFSET(member) ((u32)&((LibraryState *)0)->member)\n"
                "typedef char object_5f_byte[\n"
                " (u32)&((DisplayObject *)0)->field_5E + 1 == 0x5F &&\n"
                " sizeof(((DisplayObject *)0)->field_5E) == 2 ? 1 : -1];\n"
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
                " (u32)&((DuelEffectChannel *)0)->color_index_54,\n"
                " sizeof(((DuelEffectChannel *)0)->color_index_54),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->color_index_54,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->color_index_54),\n"
                " (u32)&((DuelEffectChannel *)0)->glyph_delay,\n"
                " sizeof(((DuelEffectChannel *)0)->glyph_delay),\n"
                " (u32)&((DuelEffectChannel *)0)->depth_offset,\n"
                " sizeof(((DuelEffectChannel *)0)->depth_offset),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->glyph_delay,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->glyph_delay),\n"
                " (u32)&((JapaneseDuelEffectChannel *)0)->depth_offset,\n"
                " sizeof(((JapaneseDuelEffectChannel *)0)->depth_offset),\n"
                " sizeof(ModelSlotLightEntry),\n"
                " sizeof(((ModelSlot *)0)->flat_lights),\n"
                " (u32)&((ModelSlot *)0)->flat_lights,\n"
                " (u32)&((ModelSlotLightEntry *)0)->direction_x,\n"
                " (u32)&((ModelSlotLightEntry *)0)->direction_y,\n"
                " (u32)&((ModelSlotLightEntry *)0)->direction_z,\n"
                " (u32)&((ModelSlotLightEntry *)0)->rgb,\n"
                " sizeof(((ModelSlotLightEntry *)0)->rgb),\n"
                " (u32)&((ModelSlotLightEntry *)0)->pad_0F,\n"
                " sizeof(((ModelSlotLightEntry *)0)->direction_x),\n"
                " sizeof(GsF_LIGHT), (u32)&((GsF_LIGHT *)0)->r,\n"
                " sizeof(SVECTOR), (u32)&((SVECTOR *)0)->vx,\n"
                " (u32)&((SVECTOR *)0)->vy, (u32)&((SVECTOR *)0)->vz,\n"
                " sizeof(((SVECTOR *)0)->vx), (u32)&((SVECTOR *)0)->pad,\n"
                " sizeof(((ModelSlot *)0)->cached_position),\n"
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
