import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class GroupedDuelSearchUnitTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_field_equip_search_has_one_owner(self) -> None:
        owner = "src/game/duel_field_equip_search.c"
        for address in ("0x80026C0C", "0x80026C6C", "0x80026D18", "0x80026DC8"):
            self.assertEqual(self.functions[address]["source"], owner)
        for source in ("func_80026C0C.c", "func_80026C6C.c"):
            self.assertFalse((ROOT / "src/game" / source).exists())

    def test_duel_object_collectors_have_one_owner(self) -> None:
        owner = "src/game/func_8002C938.c"
        self.assertEqual(self.functions["0x8002C938"]["source"], owner)
        self.assertEqual(self.functions["0x8002C9B4"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002C9B4.c").exists())

    def test_functions_remain_in_image_order(self) -> None:
        field_source = (ROOT / "src/game/duel_field_equip_search.c").read_text()
        field_names = (
            "s32 func_80026C0C(",
            "s32 Duel_CollectFieldCardsBelowType(",
            "s32 Duel_CollectFieldCardsByType(",
            "s32 func_80026DC8(",
        )
        positions = [field_source.index(name) for name in field_names]
        self.assertEqual(positions, sorted(positions))

        object_source = (ROOT / "src/game/func_8002C938.c").read_text()
        self.assertLess(
            object_source.index("void func_8002C938("),
            object_source.index("void func_8002C9B4("),
        )


if __name__ == "__main__":
    unittest.main()
