import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
OWNER = "src/game/mem_card_dialog.c"


class GroupedMemCardDialogTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_controller_load_save_and_runtime_have_one_owner(self) -> None:
        for address in (
            "0x8003DC1C",  # MemCardWork_UpdateOperation, the low-level card controller
            "0x8003E854",  # MemCardDialog_UpdateSave
            "0x8003EE90",  # MemCardDialog_StepSave
            "0x8003EEC8",  # MemCardDialog_StepNone
            "0x8003F758",  # MemCardDialog_Request
        ):
            with self.subTest(address=address):
                self.assertEqual(self.functions[address]["source"], OWNER)

    def test_save_transfer_runtime_shares_the_owner(self) -> None:
        for address in (
            "0x8003F7D4",  # SaveData_RequestLoad
            "0x8003FE14",  # SaveData_RequestTradeWrite
        ):
            with self.subTest(address=address):
                self.assertEqual(self.functions[address]["source"], OWNER)

    def test_mask_seed_setter_stays_separate(self) -> None:
        self.assertNotEqual(self.functions["0x8003FE70"]["source"], OWNER)


if __name__ == "__main__":
    unittest.main()
