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
            "src/game/card_list_sort.c",
        )
        self.assertEqual(
            sources["0x80033500"],
            "src/game/build_deck_active_card.c",
        )
        self.assertEqual(
            sources["0x80033998"],
            "src/game/build_deck_deck_capacity.c",
        )




if __name__ == "__main__":
    unittest.main()
