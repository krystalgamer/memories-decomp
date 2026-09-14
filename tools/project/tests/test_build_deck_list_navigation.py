"""Trace-backed Build Deck list and pane ownership."""

import csv
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/slus_01411"


class BuildDeckListNavigationTests(unittest.TestCase):
    def test_semantic_functions_and_sources(self):
        expected_names = {
            "0x800330BC": "BuildDeck_UpdateCardListInput",
            "0x80033500": "BuildDeck_GetActiveCardID",
            "0x8003353C": "BuildDeck_UpdateDeckPaneInput",
            "0x800336F0": "BuildDeck_UpdateChestPaneInput",
            "0x80033998": "BuildDeck_HasOpenDeckSlot",
        }
        with (CONFIG / "functions.csv").open(newline="") as stream:
            rows = {row["address"]: row for row in csv.DictReader(stream)}
        for address, name in expected_names.items():
            self.assertEqual(rows[address]["name"], name)

        matching = json.loads((CONFIG / "matching_c.json").read_text())
        sources = {
            entry["address"]: entry["source"]
            for entry in matching["functions"]
        }
        self.assertEqual(
            sources["0x800330BC"],
            "src/game/build_deck_card_list_input.c",
        )
        self.assertEqual(
            sources["0x80033500"],
            "src/game/build_deck_active_card.c",
        )
        self.assertEqual(
            sources["0x80033998"],
            "src/game/build_deck_deck_capacity.c",
        )

    def test_build_deck_uses_list_local_navigation(self):
        paths = (
            ROOT / "src/game/build_deck_card_list_input.c",
            ROOT / "src/game/build_deck_pane_input.c",
            ROOT / "src/game/build_deck_transition_state.h",
        )
        text = "\n".join(path.read_text() for path in paths)
        self.assertIn("gBuildDeck_pState", text)
        self.assertIn("BuildDeck_UpdateCardListInput", text)
        self.assertNotIn("gCardGrid_bCursorColumn", text)
        self.assertNotIn("gCardGrid_bCursorRow", text)

        table = (ROOT / "src/game/duel_transition_step_table.c").read_text()
        self.assertLess(
            table.index("BuildDeck_UpdateChestPaneInput"),
            table.index("BuildDeck_UpdateDeckPaneInput"),
        )


if __name__ == "__main__":
    unittest.main()
