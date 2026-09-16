import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class GroupedMemCardDialogTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_save_state_and_step_have_one_owner(self) -> None:
        owner = "src/game/mem_card_dialog_load_save.c"
        self.assertEqual(self.functions["0x8003E854"]["source"], owner)
        self.assertEqual(self.functions["0x8003EE90"]["source"], owner)
        self.assertEqual(
            self.functions["0x8003EEC8"]["source"],
            "src/game/mem_card_dialog_runtime.c",
        )

    def test_save_step_follows_state_machine(self) -> None:
        source = (ROOT / "src/game/mem_card_dialog_load_save.c").read_text()
        self.assertLess(
            source.index("void MemCardDialog_UpdateSave("),
            source.index("void MemCardDialog_StepSave("),
        )
        runtime = (ROOT / "src/game/mem_card_dialog_runtime.c").read_text()
        self.assertNotIn("void MemCardDialog_StepSave(", runtime)


if __name__ == "__main__":
    unittest.main()
