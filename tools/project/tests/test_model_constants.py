from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class ModelConstantTests(unittest.TestCase):
    def test_model_header_owns_shared_runtime_constants(self) -> None:
        header = (ROOT / "src/game/model.h").read_text()
        for declaration in (
            "#define MODEL_DEFAULT_PROJECTION 0x12C",
            "#define MODEL_SPECIAL_BATTLE_ID 0x309",
            "#define MODEL_MRG_FIRST_GAP_START 0x12C",
            "#define MODEL_MRG_FIRST_GAP_END 0x15E",
            "#define MODEL_MRG_SECOND_GAP_START 0x28A",
            "#define MODEL_MRG_SECOND_GAP_END 0x2BC",
            "#define MODEL_MRG_SINGLE_GAP_ID 0x2D0",
            "#define MODEL_MRG_SECTOR_COUNT 0x114",
            "#define MODEL_SPECIAL_BATTLE_FILE_START_SECTOR 0x3B4",
            "#define MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT 0x113",
            "#define MODEL_AUX_SECTOR_COUNT 0x74",
            "#define MODEL_AUX_FILE_START_SECTOR 0x88",
        ):
            self.assertIn(declaration, header)

    def test_mrg_consumers_share_gap_boundaries(self) -> None:
        loader = (ROOT / "src/game/model_load_monster_merge.c").read_text()
        randomizer = (ROOT / "src/game/model_intro_controller.c").read_text()
        for constant in (
            "MODEL_MRG_FIRST_GAP_START",
            "MODEL_MRG_FIRST_GAP_END",
            "MODEL_MRG_SECOND_GAP_START",
            "MODEL_MRG_SECOND_GAP_END",
            "MODEL_MRG_SINGLE_GAP_ID",
        ):
            self.assertIn(constant, loader)
            self.assertIn(constant, randomizer)

    def test_model_transfer_constants_keep_sector_units(self) -> None:
        loader = (ROOT / "src/game/model_load_monster_merge.c").read_text()
        for constant in (
            "MODEL_MRG_SECTOR_COUNT",
            "MODEL_SPECIAL_BATTLE_FILE_START_SECTOR",
            "MODEL_SPECIAL_BATTLE_FILE_SECTOR_COUNT",
            "MODEL_AUX_SECTOR_COUNT",
            "MODEL_AUX_FILE_START_SECTOR",
        ):
            self.assertIn(constant, loader)
        self.assertNotIn("MODEL_MRG_RECORD_SIZE", loader)
        self.assertNotIn("MODEL_AUX_RECORD_SIZE", loader)

    def test_private_card_grid_bound_uses_card_id_end(self) -> None:
        source = (ROOT / "src/game/func_80029EC4.c").read_text()
        self.assertIn("if (k < CARD_ID_END)", source)
        self.assertNotIn("if (k < 0x2D3)", source)


if __name__ == "__main__":
    unittest.main()
