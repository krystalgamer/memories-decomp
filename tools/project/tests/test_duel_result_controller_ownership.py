"""Declaration/codegen controls and native ILP32 backing for the result UI."""

from __future__ import annotations

import contextlib
import io
import json
import os
from pathlib import Path
import platform
import shutil
import subprocess
import sys
import unittest
import uuid


ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))
from build_baseline import compile_c, tool
from centralize_basic_types import update_source


GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
CC = shutil.which("cc")
PAIR = "#define DUEL_SAVE_WINDOWS_AS_PAIR\n"
RESULT = "#define DUEL_RITUAL_DATA_RESULT_VIEW\n"
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


class DuelResultControllerOwnershipTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        self.scratch = ROOT / "tmp" / f"test-result-ownership-{uuid.uuid4().hex}"
        self.scratch.mkdir(parents=True)
        self.addCleanup(shutil.rmtree, self.scratch)
        self.environment = os.environ.copy()
        self.environment["TMPDIR"] = str(self.scratch)

    def command(self, arguments: list[str]) -> subprocess.CompletedProcess[str]:
        return subprocess.run(
            arguments, cwd=ROOT, env=self.environment, capture_output=True,
            text=True, errors="replace", timeout=60,
        )

    def probe(self, text: str, *, accepted: bool, diagnostic: str = "") -> None:
        source = self.scratch / "probe.c"
        source.write_text(text)
        compilers = []
        if GCC.is_file():
            compilers.append([str(GCC), "-S", "-O2", "-G8", "-Werror"])
        if CC:
            compilers.append([
                CC, "-S", "-m32", "-std=gnu99",
                "-Werror=incompatible-pointer-types", "-Werror=int-conversion",
                "-Werror=discarded-qualifiers", "-Werror=implicit-function-declaration",
            ])
        if not compilers:
            self.skipTest("needs GCC 2.8.1 or a native C compiler")
        for compiler in compilers:
            with self.subTest(compiler=compiler[0], source=text):
                result = self.command(compiler + [
                    "-Wimplicit-function-declaration", "-I", str(ROOT / "src/game"),
                    "-o", str(self.scratch / "probe.s"), str(source),
                ])
                if accepted:
                    self.assertEqual(result.returncode, 0, result.stderr)
                else:
                    self.assertNotEqual(result.returncode, 0, result.stderr)
                    self.assertRegex(result.stderr, diagnostic)

    def test_controller_owner_and_reward_widths(self) -> None:
        header = '#include "duel_result_display.h"\n#include "duel_rewards.h"\n'
        for declaration in (
            "void (*controller)(void) = DuelScene_UpdateResultRewards;",
            "s32 (*drop)(s32) = Duel_SelectCardDrop;",
            "void (*award)(s32) = Duel_AwardCard;",
        ):
            self.probe(header + declaration, accepted=True)
        for declaration in (
            "s32 (*controller)(void) = DuelScene_UpdateResultRewards;",
            "s16 (*drop)(s32) = Duel_SelectCardDrop;",
            "void (*award)(s16) = Duel_AwardCard;",
        ):
            self.probe(
                header + declaration, accepted=False,
                diagnostic="incompatible pointer|incompatible-pointer",
            )
        self.probe(
            header + "void call(void) { DuelScene_UpdateResultRewards(1); }",
            accepted=False, diagnostic="too many arguments",
        )
        self.probe(
            "void (*controller)(void) = DuelScene_UpdateResultRewards;",
            accepted=False, diagnostic="undeclared",
        )

    def test_absolute_repeat_arm_is_a_nonvolatile_scalar(self) -> None:
        for prefix in ("", "#define GINPUT_PAD1_REPEAT_IN_DATA\n"):
            self.probe(
                prefix + '#include "input.h"\n'
                "u16 *repeat = &gInput_wPad1Repeat;\n"
                "typedef char size[sizeof(gInput_wPad1Repeat) == 2 ? 1 : -1];",
                accepted=True,
            )
        self.probe(
            "#define GINPUT_PAD1_REPEAT_IN_DATA_VOLATILE\n"
            '#include "input.h"\nu16 *repeat = &gInput_wPad1Repeat;',
            accepted=False, diagnostic="discard|qualifier",
        )
        self.probe(
            "#define GINPUT_PAD1_REPEAT_IN_DATA\n"
            '#include "input.h"\nu16 (*repeat)[4] = &gInput_wPad1Repeat;',
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )
        self.probe(
            "#define GINPUT_PAD1_REPEAT_SIZED_VOLATILE\n"
            '#include "input.h"\nvolatile u16 (*repeat)[4] = &gInput_wPad1Repeat;',
            accepted=True,
        )

    def test_text_and_resource_owners_preserve_types(self) -> None:
        for prefix in ("", "#define TEXT_STRING_ID_IN_DATA\n"):
            self.probe(
                prefix + '#include "text_constants.h"\n'
                "u16 *id = &D_8009B32E;\nu8 *colors = gText_abColorSlots;",
                accepted=True,
            )
        self.probe(
            '#include "text_staging.h"\n'
            '#include "display_asset_banks.h"\n'
            '#include "duel_scene_state.h"\n'
            "s32 *drop = D_801D56A8;\nu8 *assets = D_801AF000;\n"
            "u16 *state = &gDuel_wSceneStateFlags;",
            accepted=True,
        )
        for header, declaration in (
            ("text_staging.h", "s16 *drop = D_801D56A8;"),
            ("display_asset_banks.h", "u32 *assets = D_801AF000;"),
            ("duel_scene_state.h", "u32 *state = &gDuel_wSceneStateFlags;"),
        ):
            self.probe(
                f'#include "{header}"\n' + declaration, accepted=False,
                diagnostic="incompatible pointer|incompatible-pointer",
            )

    def test_pointer_pair_is_bounded_without_breaking_scalar_views(self) -> None:
        owner = '#include "func_800179F4.h"\n'
        self.probe(
            PAIR + owner
            + "typedef char extent[sizeof(D_8009B1D8) == 8 ? 1 : -1];\n"
            "u8 *(*pair)[2] = &D_8009B1D8;\nu8 *D_8009B1D8[2];",
            accepted=True,
        )
        self.probe(
            owner + "u8 **first = &D_8009B1D8;\nu8 **second = &D_8009B1DC;",
            accepted=True,
        )
        for bad in ("u8 *D_8009B1D8;", "u8 *D_8009B1D8[1];"):
            self.probe(
                PAIR + owner + bad, accepted=False, diagnostic="conflicting types",
            )
        self.probe(
            owner + "u8 *(*pair)[2] = &D_8009B1D8;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    def test_ritual_result_view_is_a_complete_record(self) -> None:
        owner = '#include "duel_check_ritual.h"\n'
        self.probe(
            RESULT + owner
            + "DuelResultDisplayState *state = &gDuel_awRitualData;\n"
            "typedef char extent[sizeof(gDuel_awRitualData) == 0x40 ? 1 : -1];\n"
            "DuelResultDisplayState gDuel_awRitualData;",
            accepted=True,
        )
        self.probe(owner + "u16 *recipe = gDuel_awRitualData;", accepted=True)
        self.probe(
            RESULT + owner + "u16 gDuel_awRitualData[2];",
            accepted=False, diagnostic="conflicting types",
        )
        self.probe(
            RESULT + owner + "u16 *recipe = &gDuel_awRitualData;",
            accepted=False, diagnostic="incompatible pointer|incompatible-pointer",
        )

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_measured_gp_and_absolute_relocations(self) -> None:
        source = self.scratch / "addresses.c"
        includes = (
            "input.h", "text_constants.h", "text_staging.h",
            "func_800179F4.h", "duel_check_ritual.h",
        )
        source.write_text(
            PAIR + RESULT + "#define GINPUT_PAD1_REPEAT_IN_DATA\n"
            "#define TEXT_STRING_ID_IN_DATA\n"
            + "".join(f'#include "{ROOT / "src/game" / h}"\n' for h in includes)
            + "u16 repeat(void) { return gInput_wPad1Repeat; }\n"
            "void message(u16 value) { D_8009B32E = value; }\n"
            "void drop(s32 value) { D_801D56A8[0] = value; }\n"
            "u8 *window(s32 side) { return D_8009B1D8[side]; }\n"
            "DuelResultDisplayState *record(void) { return &gDuel_awRitualData; }\n"
        )
        profiles = json.loads(
            (ROOT / "config/slus_01411/compiler_profiles.json").read_text()
        )["profiles"]
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            obj = compile_c(
                ROOT, tool(ROOT, "as"),
                {"source": str(source.relative_to(ROOT)),
                 "profile": "gcc_2_8_1_g8_split", "object": "addresses.o"},
                profiles, object_directory=str(self.scratch.relative_to(ROOT)),
                asm_directory=str(self.scratch.relative_to(ROOT)),
            )
        result = self.command([str(tool(ROOT, "objdump")), "-dr", str(obj)])
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertRegex(result.stdout, r"R_MIPS_GPREL16\s+D_8009B1D8")
        for symbol in ("gInput_wPad1Repeat", "D_8009B32E",
                       "D_801D56A8", "gDuel_awRitualData"):
            self.assertRegex(result.stdout, r"R_MIPS_HI16\s+" + symbol)
            self.assertRegex(result.stdout, r"R_MIPS_LO16\s+" + symbol)
            self.assertNotRegex(result.stdout, r"R_MIPS_GPREL16\s+" + symbol)
        compiler_text = (self.scratch / "addresses.o.compiler.s").read_text()
        self.assertIn("D_801D56A8", compiler_text)
        self.assertNotIn(".extern\tD_801D56A8, 4", compiler_text)

    def test_existing_image_segments_cover_all_measured_spans(self) -> None:
        import yaml

        layout = yaml.safe_load((ROOT / "config/slus_01411/split.yaml").read_text())
        segments = layout["segments"]
        for name, address, size in (
            ("bss_image_after_viewport", 0x8009B1D8, 8),
            ("overlays/slot_80168000", 0x801799D8, 0x40),
            ("tail_data", 0x801D56A8, 4),
            ("tail_data", 0x801AF000, 1),
        ):
            with self.subTest(name=name, address=hex(address)):
                index = next(
                    i for i, segment in enumerate(segments)
                    if isinstance(segment, dict) and segment.get("name") == name
                )
                current, following = segments[index:index + 2]
                end = following["start"] if isinstance(following, dict) else following[0]
                self.assertEqual(current["type"], "bin")
                self.assertGreaterEqual(address, current["vram"])
                self.assertLessEqual(
                    address + size, current["vram"] + end - current["start"],
                )

    def test_matching_exodia_source_keeps_the_scene_state_contract(self) -> None:
        from candidate_builds import (
            canonical_declaration_index,
            canonical_symbol_contract_hash,
        )

        symbol = "D_8009B23A"
        declarations = canonical_declaration_index({symbol})[symbol]
        self.assertEqual(
            declarations, [("unmatched.h", "extern u16 D_8009B23A;")],
        )
        self.assertEqual(
            canonical_symbol_contract_hash(symbol, declarations),
            "11af64fe9c894b42b7e5bc8afaafe78710739c45feece440551251bc560d7912",
        )
        entries = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )["functions"]
        matching = next(
            entry for entry in entries if int(entry["address"], 0) == 0x80018FEC
        )
        self.assertEqual(matching["source"], "src/game/func_80018FEC.c")
        source = (ROOT / matching["source"]).read_text()
        self.assertIn('#include "../unmatched.h"', source)
        self.assertIn(symbol, source)

    @unittest.skipUnless(GCC.is_file(), "needs GCC 2.8.1")
    def test_migrated_consumers_compile_and_sources_are_normalized(self) -> None:
        names = (
            "func_800179F4.c", "func_8001B170.c", "duel_scene_callbacks.c",
            "campaign_map_load_package_stage.c", "func_80020BE4.c",
            "options_load_package_stage.c",
            "func_8002F4C0.c", "func_800434F4.c", "func_8003C328.c",
            "func_8003B808.c",
        )
        for name in names:
            with self.subTest(name=name):
                path = ROOT / "src/game" / name
                text = path.read_text()
                self.assertEqual(update_source(ROOT, path, text), text)
                compiled = self.command([
                    str(GCC), "-S", "-O2", "-G8", "-Werror",
                    str(path),
                    "-o", str(self.scratch / "existing.s"),
                ])
                self.assertEqual(compiled.returncode, 0, compiled.stderr)
        unmatched = (ROOT / "src/unmatched.h").read_text()
        for declaration in (
            "extern u8 D_801AF000[];",
            "void DuelScene_UpdateResultRewards(void);",
        ):
            self.assertNotIn(declaration, unmatched)

    def native_backing_witness(self, wrong_second_window: bool) -> int:
        if not (
            CC and platform.system() == "Linux"
            and platform.machine().lower() in {"x86_64", "amd64", "i386", "i686"}
        ):
            self.skipTest("needs native Linux x86 ILP32 execution")
        source = self.scratch / "backing.c"
        source.write_text(
            PAIR + RESULT
            + '#include "func_800179F4.h"\n#include "duel_check_ritual.h"\n'
            '#include "save_data.h"\n#include "text_staging.h"\n'
            "static union { SaveDataState state; u8 bytes[TWO_PLAYER_SAVE_SLOT_STRIDE]; } windows[2];\n"
            "u8 *D_8009B1D8[2] = {windows[0].bytes, windows[1].bytes};\n"
            "DuelResultDisplayState gDuel_awRitualData;\n"
            "s32 D_801D56A8[1];\n"
            "u8 *first_scalar(void); u8 *second_scalar(void);\n"
            "u16 recipe_halfword(void);\n"
            "int main(void) {\n"
            " s32 i;\n"
            " if (sizeof(void *) != 4 || sizeof(D_8009B1D8) != 8) return 1;\n"
            " if (first_scalar() != D_8009B1D8[0]) return 2;\n"
            " if (second_scalar() != D_8009B1D8[1]) return 3;\n"
            " if ((u32)D_8009B1D8[1] - (u32)D_8009B1D8[0] != TWO_PLAYER_SAVE_SLOT_STRIDE) return 4;\n"
            " *(u16 *)(D_8009B1D8[0] + SAVE_DATA_DUEL_WINS_OFFSET) = 0xFFFF;\n"
            " *(u16 *)(D_8009B1D8[1] + SAVE_DATA_DUEL_LOSSES_OFFSET) = 9999;\n"
            " *(u32 *)(D_8009B1D8[0] + SAVE_DATA_STARCHIPS_OFFSET) = 999999;\n"
            " if (windows[0].state.duel_wins != 0xFFFF ||\n"
            "     windows[1].state.duel_losses != 9999 ||\n"
            "     windows[0].state.starchips != 999999) return 5;\n"
            " if (windows[0].state.duel_losses || windows[1].state.duel_wins) return 6;\n"
            " gDuel_awRitualData.root = (DisplayObject *)windows[0].bytes;\n"
            " if (recipe_halfword() != (u16)(u32)gDuel_awRitualData.root) return 7;\n"
            " for (i = 0; i < DUEL_RESULT_DISPLAY_CHILD_COUNT; i++)\n"
            "   gDuel_awRitualData.children[i] = (DisplayObject *)windows[1].bytes;\n"
            " gDuel_awRitualData.side_scores[1] = 0x12345678;\n"
            " gDuel_awRitualData.dropped_card_id = -2;\n"
            " D_801D56A8[0] = gDuel_awRitualData.dropped_card_id;\n"
            " if (D_801D56A8[0] != -2 || gDuel_awRitualData.side_scores[1] != 0x12345678) return 8;\n"
            " if (gDuel_awRitualData.children[9] != (DisplayObject *)windows[1].bytes) return 9;\n"
            " return 0;\n}\n"
        )
        scalar = self.scratch / "scalar.c"
        scalar.write_text(
            '#include "func_800179F4.h"\n#include "duel_check_ritual.h"\n'
            "u8 *first_scalar(void) { return D_8009B1D8; }\n"
            "u8 *second_scalar(void) { return D_8009B1DC; }\n"
            "u16 recipe_halfword(void) { return gDuel_awRitualData[0]; }\n"
        )
        startup = self.scratch / "start.S"
        startup.write_text(START)
        executable = self.scratch / "backing"
        alias_offset = 0 if wrong_second_window else 4
        compilation = self.command([
            CC, "-m32", "-std=gnu99", "-O2", "-nostdlib", "-fno-pie", "-no-pie",
            "-ffreestanding", "-fno-builtin", "-fno-stack-protector",
            "-fno-strict-aliasing", "-Werror=incompatible-pointer-types",
            "-I", str(ROOT / "src/game"), str(source), str(scalar), str(startup),
            f"-Wl,--defsym,D_8009B1DC=D_8009B1D8+{alias_offset}",
            "-o", str(executable),
        ])
        self.assertEqual(compilation.returncode, 0, compilation.stderr)
        return self.command([str(executable)]).returncode

    def test_native_complete_save_windows_result_and_word_readback(self) -> None:
        self.assertEqual(self.native_backing_witness(wrong_second_window=False), 0)

    def test_native_scalar_alias_must_reach_the_second_pointer(self) -> None:
        self.assertEqual(self.native_backing_witness(wrong_second_window=True), 3)


if __name__ == "__main__":
    unittest.main()
