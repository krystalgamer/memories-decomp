"""Durable contracts from completed controlled duel traces."""

from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelControlledTraceFindingTests(unittest.TestCase):
    def test_forest_mage_is_no_longer_a_terrain_target(self):
        script = (
            ROOT / "tools/trace/campaign_duel_home_terrain.lua"
        ).read_text()
        self.assertIn("value ~= 23", script)
        self.assertIn("not an unresolved terrain target", script)

        game_notes = (ROOT / "notes/research/the-game.md").read_text()
        self.assertIn(
            "Forest Mage | Forest Shrine gate | forced | "
            "home field Forest (controlled trace)",
            game_notes,
        )

    def test_completed_fusion_trace_is_retired(self):
        for relative in (
            "tools/trace/duel_face_down_fusion_counter.lua",
            "tools/trace/result/duel_face_down_fusion_counter.txt",
            "tools/trace/tests/duel_face_down_fusion_counter_test.lua",
        ):
            self.assertFalse((ROOT / relative).exists(), relative)

        note = (ROOT / "notes/duel-controlled-trace-findings.md").read_text()
        self.assertIn("face-down counter is expected", note)
        self.assertIn("fusion counter independently records", note)


if __name__ == "__main__":
    unittest.main()
