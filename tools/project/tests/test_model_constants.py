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
            "#define MODEL_MRG_RECORD_SIZE 0x114",
            "#define MODEL_AUX_RECORD_SIZE 0x74",
        ):
            self.assertIn(declaration, header)

    def test_mrg_consumers_share_gap_boundaries(self) -> None:
        loader = (ROOT / "src/game/model_load_monster_merge.c").read_text()
        randomizer = (ROOT / "src/game/func_80050584.c").read_text()
        for constant in (
            "MODEL_MRG_FIRST_GAP_START",
            "MODEL_MRG_FIRST_GAP_END",
            "MODEL_MRG_SECOND_GAP_START",
            "MODEL_MRG_SECOND_GAP_END",
            "MODEL_MRG_SINGLE_GAP_ID",
        ):
            self.assertIn(constant, loader)
            self.assertIn(constant, randomizer)

    def test_private_card_grid_bound_uses_card_id_end(self) -> None:
        candidate = (ROOT / "src/candidates/func_80029EC4.c").read_text()
        self.assertIn("if (k < CARD_ID_END)", candidate)
        self.assertNotIn("if (k < 0x2D3)", candidate)


if __name__ == "__main__":
    unittest.main()
