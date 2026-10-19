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
        owner = "src/game/duel_field_effects.c"
        for address in ("0x80026C0C", "0x80026C6C", "0x80026D18", "0x80026DC8"):
            self.assertEqual(self.functions[address]["source"], owner)
        for source in ("func_80026C0C.c", "func_80026C6C.c"):
            self.assertFalse((ROOT / "src/game" / source).exists())

    def test_duel_object_collectors_have_one_owner(self) -> None:
        owner = "src/game/duel_field_card_objects.c"
        self.assertEqual(self.functions["0x8002C938"]["source"], owner)
        self.assertEqual(self.functions["0x8002C9B4"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002C9B4.c").exists())

    def test_effect_request_pool_has_one_owner(self) -> None:
        owner = "src/game/duel_effect_object_pool.c"
        addresses = (
            "0x8002C570",
            "0x8002C598",
            "0x8002C5CC",
            "0x8002C604",
            "0x8002C68C",
            "0x8002C6C8",
        )
        for address in addresses:
            self.assertEqual(self.functions[address]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_8002C604.c").exists())
        self.assertFalse((ROOT / "src/game/duel_effect_request_create.c").exists())
        self.assertFalse((ROOT / "src/game/duel_effect_request_update.c").exists())

    def test_field_effect_steps_have_one_owner(self) -> None:
        owner = "src/game/duel_field_effects.c"
        self.assertEqual(self.functions["0x80025F3C"]["source"], owner)
        self.assertEqual(self.functions["0x800260D0"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_800260D0.c").exists())



if __name__ == "__main__":
    unittest.main()
