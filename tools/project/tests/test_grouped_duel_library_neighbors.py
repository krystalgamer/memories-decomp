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

    def test_library_section_sprites_have_one_regional_owner(self) -> None:
        owner = "src/game/library_create_grid_section_sprites.c"
        addresses = {
            "slus_01411": "0x80029590",
            "slpm_86398": "0x80029370",
            "sles_03947": "0x800295E4",
            "sles_03948": "0x80029728",
            "sles_03949": "0x80029728",
            "sles_03950": "0x80029728",
            "sles_03951": "0x80029728",
        }
        for region, address in addresses.items():
            with self.subTest(region=region):
                manifest = json.loads(
                    (ROOT / "config" / region / "matching_c.json").read_text()
                )
                entries = [
                    item for item in manifest["functions"]
                    if item["source"] == owner
                ]
                self.assertEqual(len(entries), 1)
                self.assertEqual(entries[0]["address"], address)
                self.assertEqual(int(entries[0]["size"], 0), 0xF4)
                self.assertEqual(entries[0]["profile"], "gcc_2_8_1_g8_split")
        self.assertFalse((ROOT / "src/game/func_80029590.c").exists())
        self.assertFalse((ROOT / "src/game/func_80029590.h").exists())

    def test_library_selection_motion_group_has_one_owner(self) -> None:
        owner = "src/game/func_8002A3CC.c"
        self.assertEqual(self.functions["0x8002A2F4"]["source"], owner)
        self.assertEqual(self.functions["0x8002A3CC"]["source"], owner)
        self.assertEqual(self.functions["0x8002A4A8"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002A2F4.c").exists())
        self.assertFalse((ROOT / "src/game/func_8002A4A8.c").exists())

    def test_library_cursor_pair_has_one_owner(self) -> None:
        owner = "src/game/library_grid_cursor.c"
        self.assertEqual(self.functions["0x8002A6B8"]["source"], owner)
        self.assertEqual(self.functions["0x8002A788"]["source"], owner)
        self.assertFalse((ROOT / "src/game/card_grid_cursor_card_id.c").exists())
        self.assertFalse((ROOT / "src/game/func_8002A788.c").exists())
        self.assertFalse((ROOT / "src/game/func_8002A788.h").exists())

    def test_globe_spans_keep_their_contiguous_owner(self) -> None:
        pairs = {
            "slus_01411": (0x80029684, 0x800297DC),
            "slpm_86398": (0x80029464, 0x800295BC),
            "sles_03947": (0x800296D8, 0x80029830),
            "sles_03948": (0x8002981C, 0x80029974),
            "sles_03949": (0x8002981C, 0x80029974),
            "sles_03950": (0x8002981C, 0x80029974),
            "sles_03951": (0x8002981C, 0x80029974),
        }
        for region, addresses in pairs.items():
            with self.subTest(region=region):
                manifest = json.loads(
                    (ROOT / "config" / region / "matching_c.json").read_text()
                )
                entries = [
                    item for item in manifest["functions"]
                    if item["source"] == "src/game/duel_projection_axes.c"
                ]
                self.assertEqual(
                    [int(item["address"], 0) for item in entries], list(addresses)
                )
                self.assertEqual(addresses[1] - addresses[0], 0x158)
                for entry in entries:
                    self.assertEqual(int(entry["size"], 0), 0x158)
                    self.assertEqual(entry["profile"], "gcc_2_8_1_g0")
        source = (ROOT / "src/game/duel_projection_axes.c").read_text()
        self.assertLess(
            source.index("void Library_DrawGlobeXSpan("),
            source.index("void Library_DrawGlobeZSpan("),
        )

    def test_library_cursor_lookup_has_one_typed_contract(self) -> None:
        header = (ROOT / "src/game/library_grid_cursor.h").read_text()
        owner = (ROOT / "src/game/library_grid_cursor.c").read_text()
        staging = (ROOT / "src/game/func_8002A3CC.c").read_text()
        self.assertEqual(
            header.count(
                "s32 Library_GetGridCursorCardId(const LibraryState *state);"
            ), 1
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
            library_source.index("void Library_RefreshSelectedCardText("),
            library_source.index("s32 Library_UpdateCursorMotion("),
        )
        self.assertLess(
            library_source.index("s32 Library_UpdateCursorMotion("),
            library_source.index("void Library_StartCursorMotion("),
        )
        cursor_source = (ROOT / "src/game/library_grid_cursor.c").read_text()
        self.assertLess(
            cursor_source.index("s32 Library_GetGridCursorCardId("),
            cursor_source.index("void Library_UpdateGridCursor("),
        )


if __name__ == "__main__":
    unittest.main()
