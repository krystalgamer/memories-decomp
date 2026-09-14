import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class GroupedScriptGameOverUnitTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_script_state_functions_have_one_owner(self) -> None:
        owner = "src/game/script_flag_commands.c"
        for address in ("0x8002E918", "0x8002E9A0", "0x8002EA0C"):
            self.assertEqual(self.functions[address]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002E918.c").exists())
        self.assertFalse(
            (ROOT / "src/game/script_update_viewport_tween.c").exists()
        )

    def test_main_mode_runners_have_one_owner(self) -> None:
        owner = "src/game/main_run_options_menu.c"
        self.assertEqual(self.functions["0x8002D6C8"]["source"], owner)
        self.assertEqual(self.functions["0x8002D730"]["source"], owner)
        self.assertFalse((ROOT / "src/game/main_run_game_over.c").exists())

    def test_game_over_runtime_has_one_owner(self) -> None:
        owner = "src/game/game_over.c"
        self.assertEqual(self.functions["0x8003C950"]["source"], owner)
        self.assertEqual(self.functions["0x8003CA5C"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8003CA5C.c").exists())

    def test_grouped_functions_remain_in_image_order(self) -> None:
        groups = (
            (
                "src/game/script_flag_commands.c",
                (
                    "void Script_OpStoryFlag(",
                    "void Script_OpViewportTween(",
                    "void Script_UpdateViewportTween(",
                ),
            ),
            (
                "src/game/main_run_options_menu.c",
                ("void Main_RunOptionsMenu(", "void Main_RunGameOver("),
            ),
            (
                "src/game/game_over.c",
                ("void func_8003C950(", "s32 func_8003CA5C("),
            ),
        )
        for source, names in groups:
            text = (ROOT / source).read_text()
            positions = [text.index(name) for name in names]
            self.assertEqual(positions, sorted(positions))


if __name__ == "__main__":
    unittest.main()
