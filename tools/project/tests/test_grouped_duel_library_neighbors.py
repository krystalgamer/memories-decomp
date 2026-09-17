import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class GroupedDuelLibraryNeighborTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_display_parent_pair_has_one_owner(self) -> None:
        owner = "src/game/display_parent_links.c"
        self.assertEqual(self.functions["0x80022F98"]["source"], owner)
        self.assertEqual(self.functions["0x80022FF0"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_80022FF0.c").exists())

    def test_library_motion_pair_has_one_owner(self) -> None:
        owner = "src/game/func_8002A3CC.c"
        self.assertEqual(self.functions["0x8002A3CC"]["source"], owner)
        self.assertEqual(self.functions["0x8002A4A8"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002A4A8.c").exists())

    def test_library_cursor_pair_has_one_owner(self) -> None:
        owner = "src/game/library_grid_cursor.c"
        self.assertEqual(self.functions["0x8002A6B8"]["source"], owner)
        self.assertEqual(self.functions["0x8002A788"]["source"], owner)
        self.assertFalse((ROOT / "src/game/card_grid_cursor_card_id.c").exists())
        self.assertFalse((ROOT / "src/game/func_8002A788.c").exists())
        self.assertFalse((ROOT / "src/game/func_8002A788.h").exists())

    def test_library_cursor_lookup_has_one_typed_contract(self) -> None:
        header = (ROOT / "src/game/library_grid_cursor.h").read_text()
        owner = (ROOT / "src/game/library_grid_cursor.c").read_text()
        staging = (ROOT / "src/game/func_8002A2F4.c").read_text()
        self.assertEqual(
            header.count("s32 Library_GetGridCursorCardId(u8 *state);"), 1
        )
        self.assertIn("Library_GetGridCursorCardId(state)", owner)
        self.assertIn("Library_GetGridCursorCardId(p)", staging)

    def test_grouped_functions_remain_in_image_order(self) -> None:
        display_source = (ROOT / "src/game/display_parent_links.c").read_text()
        library_source = (ROOT / "src/game/func_8002A3CC.c").read_text()
        self.assertLess(
            display_source.index("void DuelSelection_LinkDisplayObject("),
            display_source.index("void DuelSelection_LinkDisplayObjects("),
        )
        self.assertLess(
            library_source.index("s32 func_8002A3CC("),
            library_source.index("void func_8002A4A8("),
        )
        cursor_source = (ROOT / "src/game/library_grid_cursor.c").read_text()
        self.assertLess(
            cursor_source.index("s32 Library_GetGridCursorCardId("),
            cursor_source.index("void Library_UpdateGridCursor("),
        )


if __name__ == "__main__":
    unittest.main()
