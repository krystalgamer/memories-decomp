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

    def test_build_deck_uses_list_local_navigation(self):
        paths = (
            ROOT / "src/game/card_list_sort.c",
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

    def test_exit_handler_treats_open_slot_as_incomplete_deck(self):
        capacity = (ROOT / "src/game/build_deck_deck_capacity.c").read_text()
        self.assertIn("if (entry->flags == 0) {\n            return 1;", capacity)

        exit_handler = (ROOT / "src/game/func_800339D0.c").read_text()
        open_slot_branch = exit_handler.index(
            "if (BuildDeck_HasOpenDeckSlot() != 0) {"
        )
        confirmation_flag = exit_handler.index(
            "workspace->state |= 0x4000;",
            open_slot_branch,
        )
        self.assertLess(open_slot_branch, confirmation_flag)
        self.assertIn("if the deck has an open slot", exit_handler)


if __name__ == "__main__":
    unittest.main()
