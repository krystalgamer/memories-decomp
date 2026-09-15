"""Current frontend mode declaration views, forwarding, and candidate ownership."""

import itertools
import json
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

import candidate_builds


ROOT = Path(__file__).resolve().parents[3]
OWNER = ROOT / "src/game/main_mode_state.h"
SCRATCH = ROOT / "tmp/tests/main-mode-state"
SYMBOLS = ("D_8009B269", "D_8009B26C")
DECLARATION = re.compile(
    r"\b(?P<extern>extern\s+)?u8\s+"
    r"(?P<name>D_8009B269|D_8009B26C)\s*"
    r"(?P<array>\[\s*\])?\s*"
    r"(?P<data>__attribute__\s*\(\(\s*section\s*\(\s*\"\.data\"\s*\)\s*\)\))?\s*;"
)
CONSUMERS = (
    "src/game/func_80018FEC.c",
    "src/candidates/func_8001F55C.c",
    "src/candidates/func_800283F4.c",
    "src/game/script_op_save_prompt.c",
    "src/overlays/password/shop.c",
    "src/game/debug_menu_two_player_entry.c",
    "src/game/duel_effect_basic_commands.c",
    "src/game/duel_effect_mode_7.c",
    "src/game/frontend_scene_states.c",
    "src/game/func_80024DC8.c",
    "src/game/library_grid_cursor.c",
    "src/game/func_8002EB48.c",
    "src/game/script_op_return_to_menu.c",
    "src/game/debug_menu_campaign_entry.c",
    "src/game/debug_menu_mapped_mode.c",
    "src/game/main_apply_menu_selection.c",
    "src/game/main_debug.c",
    "src/game/main_init.c",
    "src/game/main_loop.c",
    "src/game/main_run_duel.c",
    "src/game/main_run_animated_battle.c",
    "src/game/main_run_build_deck_menu.c",
    "src/game/main_run_campaign.c",
    "src/game/main_run_credits.c",
    "src/game/main_run_duel_and_library.c",
    "src/game/main_run_free_duel_menu.c",
    "src/game/main_run_frontend_menus.c",
    "src/game/main_run_name_entry.c",
    "src/game/main_run_options_menu.c",
    "src/game/main_run_password_menu.c",
    "src/game/main_run_selection_menus.c",
    "src/game/main_run_trade.c",
    "src/game/main_run_two_player_duel_setup.c",
    "src/game/script_control_commands.c",
    "src/game/text_start_campaign_duel.c",
    "src/overlays/free_duel/screen_runtime.c",
    "src/overlays/overworld/set_location.c",
)
NEXT_DATA = {
    "src/game/func_80018FEC.c", "src/candidates/func_8001F55C.c",
    "src/game/script_op_save_prompt.c",
    "src/game/main_init.c",
}
NEXT_ARRAY = {"src/game/library_grid_cursor.c", "src/game/script_control_commands.c"}
ACTIVE_DATA = {
    "src/game/func_80018FEC.c", "src/candidates/func_8001F55C.c",
    "src/candidates/func_800283F4.c",
    "src/game/script_op_save_prompt.c", "src/game/debug_menu_campaign_entry.c",
    "src/game/debug_menu_mapped_mode.c",
}
ACTIVE_ARRAY = {
    "src/game/debug_menu_two_player_entry.c", "src/game/duel_effect_basic_commands.c",
    "src/game/duel_effect_mode_7.c", "src/game/frontend_scene_states.c",
    "src/game/library_grid_cursor.c", "src/game/func_8002EB48.c",
    "src/game/script_op_return_to_menu.c",
    "src/game/main_init.c", "src/game/script_control_commands.c",
}
COMMON_ACTIVE = {
    "src/game/main_run_animated_battle.c", "src/game/main_run_build_deck_menu.c",
    "src/game/main_run_duel.c",
    "src/game/main_run_name_entry.c",
    "src/game/main_run_options_menu.c", "src/game/main_run_trade.c",
    "src/game/main_run_two_player_duel_setup.c",
}


@unittest.skipUnless(shutil.which("cc"), "requires a host C preprocessor/compiler")
class MainModeStateTests(unittest.TestCase):
    def setUp(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run from the repository root")
        SCRATCH.mkdir(parents=True, exist_ok=True)
        self.temporary = tempfile.TemporaryDirectory(prefix="run-", dir=SCRATCH)
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)

    def preprocess(self, path):
        result = subprocess.run(
            ["cc", "-E", "-P", "-std=gnu89", "-D_LANGUAGE_C", "-Dpsx",
             "-D__psx__", "-D__psx", "-I", str(ROOT), str(path)],
            text=True, capture_output=True, timeout=30,
        )
        self.assertEqual(result.returncode, 0, str(path) + "\n" + result.stderr)
        return list(DECLARATION.finditer(result.stdout))

    def assert_views(self, declarations, expected, common=()):
        for symbol, view in zip(SYMBOLS, expected):
            selected = [row for row in declarations if row["name"] == symbol]
            actual = {
                "data" if row["data"] else "array" if row["array"] else "scalar"
                for row in selected
            }
            self.assertEqual(actual, {view}, symbol)
            self.assertEqual(sum(row["extern"] is not None for row in selected), 1, symbol)
            self.assertEqual(sum(row["extern"] is None for row in selected), int(symbol in common), symbol)
        # Compile only the selected declarations and typed access probes.
        # Full-host SDK front-end errors in Main_Init are unrelated to this contract.
        text = '#include "src/types.h"\n'
        text += "\n".join(row.group(0) for row in declarations) + "\n"
        for index, (symbol, view) in enumerate(zip(SYMBOLS, expected)):
            expression = symbol if view == "array" else "&" + symbol
            text += f"u8 *probe_{index}(void) {{ return {expression}; }}\n"
        path = self.directory / "selected.c"
        path.write_text(text)
        result = subprocess.run(
            ["cc", "-std=gnu89", "-Werror", "-fsyntax-only", "-I", str(ROOT), str(path)],
            text=True, capture_output=True, timeout=30,
        )
        self.assertEqual(result.returncode, 0, result.stderr)

    def selectors(self, namespace, next_view, active_view):
        if namespace == "semantic":
            suffixes = {"scalar": "AS_SCALAR", "array": "AS_ARRAY", "data": "IN_DATA"}
            return (
                f"#define MAIN_MODE_STATE_NEXT_{suffixes[next_view]}\n"
                f"#define MAIN_MODE_STATE_ACTIVE_{suffixes[active_view]}\n"
            )
        next_flags = {"scalar": "", "array": "#define D_8009B269_AS_ARRAY\n",
                      "data": "#define D_8009B269_AS_SCALAR_DATA\n"}
        active_flags = {
            "scalar": "#define D_8009B26C_AS_SCALAR\n",
            "array": "#define D_8009B26C_AS_ARRAY\n",
            "data": "#define D_8009B26C_AS_SCALAR_DATA\n",
        }
        return next_flags[next_view] + active_flags[active_view]

    def test_independent_matrix_and_include_order(self):
        owner = '#include "src/game/main_mode_state.h"\n'
        forward = '#include "src/unmatched.h"\n'
        for namespace, next_view, active_view, includes in itertools.product(
            ("semantic", "legacy"), ("scalar", "array", "data"),
            ("scalar", "array", "data"), (owner, forward, owner + forward, forward + owner),
        ):
            with self.subTest(namespace=namespace, next=next_view, active=active_view, includes=includes):
                path = self.directory / "matrix.c"
                path.write_text(self.selectors(namespace, next_view, active_view) + includes)
                self.assert_views(self.preprocess(path), (next_view, active_view))

    def test_defaults_and_next_only_regression(self):
        for selectors, expected in (
            ("", ("scalar", "array")),
            ("#define MAIN_MODE_STATE_NEXT_IN_DATA\n", ("data", "array")),
            ("#define D_8009B269_AS_SCALAR_DATA\n", ("data", "array")),
            ("#define MAIN_MODE_STATE_NEXT_AS_ARRAY\n", ("array", "array")),
        ):
            for includes in (
                '#include "src/game/main_mode_state.h"\n#include "src/unmatched.h"\n',
                '#include "src/unmatched.h"\n#include "src/game/main_mode_state.h"\n',
            ):
                with self.subTest(selectors=selectors, includes=includes):
                    path = self.directory / "default.c"
                    path.write_text(selectors + includes)
                    self.assert_views(self.preprocess(path), expected)

    def test_mixed_legacy_and_semantic_selectors(self):
        for next_view, active_view in itertools.product(("scalar", "array", "data"), repeat=2):
            for reverse in (False, True):
                semantic = self.selectors("semantic", next_view, active_view).splitlines(keepends=True)
                legacy = self.selectors("legacy", next_view, active_view).splitlines(keepends=True)
                if reverse:
                    selected = [line for line in semantic if "ACTIVE" in line]
                    selected += [line for line in legacy if "D_8009B269" in line]
                else:
                    selected = [line for line in semantic if "NEXT" in line]
                    selected += [line for line in legacy if "D_8009B26C" in line]
                with self.subTest(next=next_view, active=active_view, reverse=reverse):
                    path = self.directory / "mixed.c"
                    path.write_text("".join(selected) + '#include "src/unmatched.h"\n')
                    self.assert_views(self.preprocess(path), (next_view, active_view))

    def test_current_successors_and_common_inputs(self):
        self.assertEqual(len(CONSUMERS), 37)
        commons = 0
        for source in CONSUMERS:
            with self.subTest(source=source):
                path = ROOT / source
                self.assertTrue(path.is_file(), source)
                self.assertIn("main_mode_state.h", path.read_text())
                expected = (
                    "data" if source in NEXT_DATA else "array" if source in NEXT_ARRAY else "scalar",
                    "data" if source in ACTIVE_DATA else "array" if source in ACTIVE_ARRAY else "scalar",
                )
                common = []
                if source in COMMON_ACTIVE:
                    common.append("D_8009B26C")
                if source == "src/game/main_run_trade.c":
                    common.append("D_8009B269")
                commons += len(common)
                self.assert_views(self.preprocess(path), expected, common)
        self.assertEqual(commons, 8)

    def test_one_owner_and_current_candidate_keys(self):
        index = candidate_builds.canonical_declaration_index(set(SYMBOLS), ROOT / "src")
        for symbol in SYMBOLS:
            self.assertEqual({path for path, _ in index[symbol]}, {"game/main_mode_state.h"})
        unmatched = (ROOT / "src/unmatched.h").read_text()
        self.assertEqual(unmatched.count('#include "game/main_mode_state.h"'), 1)
        self.assertFalse(list(DECLARATION.finditer(unmatched)))
        manifest = json.loads((ROOT / "config/slus_01411/candidates.json").read_text())
        covered = set()
        for candidate in manifest["candidates"]:
            source = ROOT / candidate["source"]
            text = source.read_text()
            required = {name for name in SYMBOLS if re.search(r"\b" + name + r"\b", text)}
            if not required:
                continue
            with self.subTest(source=candidate["source"]):
                self.assertTrue(required <= candidate["canonical_contracts"].keys())
                for name in required:
                    expected = candidate_builds.canonical_symbol_contract_hash(name, index[name])
                    self.assertEqual(candidate["canonical_contracts"][name], expected)
                covered.add(candidate["source"])
        self.assertEqual(covered, {path for path in CONSUMERS if path.startswith("src/candidates/")})

    def test_wrong_scalar_default_is_rejected(self):
        text = OWNER.read_text().replace('#include "../types.h"', '#include "src/types.h"')
        old = "#else\nextern u8 D_8009B26C[];"
        self.assertEqual(text.count(old), 1)
        text = text.replace(old, "#else\nextern u8 D_8009B26C;")
        path = self.directory / "wrong-default.c"
        path.write_text(
            "#define MAIN_MODE_STATE_NEXT_IN_DATA\n" + text
            + "\nu8 *active_array_view(void) { return D_8009B26C; }\n"
        )
        result = subprocess.run(
            ["cc", "-std=gnu89", "-Werror", "-fsyntax-only", "-I", str(ROOT), str(path)],
            text=True, capture_output=True, timeout=30,
        )
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("D_8009B26C", result.stderr)
        self.assertIn("return", result.stderr)


if __name__ == "__main__":
    unittest.main()
