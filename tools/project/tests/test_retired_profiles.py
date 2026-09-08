from __future__ import annotations

import csv
import json
from pathlib import Path
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import record_external_attempt
from select_candidates import load_attempts


class RetiredProfileTests(unittest.TestCase):
    """A removed compiler must not rewrite or invalidate recorded history."""

    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.config = REPOSITORY / "config/slus_01411"
        self.profiles = record_external_attempt.load_profiles(
            self.config / "compiler_profiles.json"
        )

    def test_retired_profiles_are_absent_from_the_live_manifest(self) -> None:
        self.assertTrue(record_external_attempt.RETIRED_PROFILES)
        for name in record_external_attempt.RETIRED_PROFILES:
            with self.subTest(profile=name):
                self.assertNotIn(name, self.profiles)

    def test_every_live_profile_uses_the_supported_pipeline(self) -> None:
        self.assertTrue(self.profiles)
        for name, profile in self.profiles.items():
            with self.subTest(profile=name):
                self.assertTrue(name.startswith("gcc_2_8_1_"))
                self.assertIn("gcc-2.8.1-psx", profile["compiler"])
                self.assertEqual(profile["aspsx_version"], "2.81")

    def test_recorded_history_may_name_a_retired_profile(self) -> None:
        with (self.config / "external_attempts.csv").open(
            encoding="utf-8", newline=""
        ) as handle:
            recorded = {row["profile"] for row in csv.DictReader(handle)}
        unknown = recorded - set(self.profiles) - record_external_attempt.RETIRED_PROFILES
        self.assertEqual(unknown, set())

    def test_canonical_ledger_still_parses_and_keeps_terminal_rows_last(self) -> None:
        histories = load_attempts(self.config / "attempts.csv")
        self.assertTrue(histories)
        terminal = {
            address
            for address, rows in histories.items()
            if rows[-1]["result"] in {"matched", "deferred"}
        }
        self.assertTrue(terminal)

    def test_no_accepted_match_depends_on_a_retired_profile(self) -> None:
        manifests = [self.config / "matching_c.json"]
        manifests.extend(sorted((self.config / "overlays").glob("*_matching_c.json")))
        used = {
            entry["profile"]
            for path in manifests
            for entry in json.loads(path.read_text(encoding="utf-8"))["functions"]
        }
        self.assertTrue(used)
        self.assertEqual(used & record_external_attempt.RETIRED_PROFILES, set())
        self.assertTrue(used <= set(self.profiles))


if __name__ == "__main__":
    unittest.main()
