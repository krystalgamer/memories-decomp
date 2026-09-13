from __future__ import annotations

from pathlib import Path
import sys
import unittest
from itertools import permutations

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import record_external_attempt as attempts
import audit_repository


class ReclassificationAttemptTests(unittest.TestCase):
    address = 0x80070710
    profile = "gcc_2_8_1_g8_split"

    def row(self, mode: str, **changes: str) -> dict[str, str]:
        row = dict.fromkeys(attempts.FIELDS, "")
        row.update(
            mode=mode,
            address=f"0x{self.address:08X}",
            attempt="1",
            profile=self.profile,
            candidate_source="tmp/candidates/ai_get_hand_size.c",
            candidate_sha256="a" * 64,
            result="matched",
            summary="New discriminator: absolute selector declaration",
        )
        row.update(changes)
        return row

    def validate(self, rows: list[dict[str, str]], status: str = "matching_c") -> None:
        attempts.validate_rows(
            rows,
            {self.address: {"module": "game", "status": status}},
            {self.address} if status == "matching_c" else set(),
            {self.profile: {}},
            {self.address},
        )

    def test_new_match_preserves_the_original_terminal_record(self) -> None:
        original = self.row("post_terminal_resolution")
        snapshot = original.copy()
        self.validate([original, self.row("reclassification_match")])
        self.assertEqual(original, snapshot)

    def test_valid_before_promotion_too(self) -> None:
        self.validate(
            [self.row("post_terminal_resolution"), self.row("reclassification_match")],
            status="unmatched_asm",
        )

    def test_previous_external_match_is_required(self) -> None:
        with self.assertRaisesRegex(attempts.ExternalAttemptError, "lacks prior"):
            self.validate([self.row("reclassification_match")])

    def test_prior_reference_success_is_supported(self) -> None:
        reference = self.row(
            "reference_match",
            reference_path="tmp/references/ygofm-decomp/src/func_80070710.c",
            reference_sha256="b" * 64,
        )
        for status in ("matching_c", "unmatched_asm"):
            self.validate([reference, self.row("reclassification_match")], status)

    def test_nonmatching_reclassification_cannot_be_recorded(self) -> None:
        for result in ("nonmatch", "deferred"):
            with self.subTest(result=result):
                with self.assertRaisesRegex(attempts.ExternalAttemptError, "must be matched"):
                    self.validate([
                        self.row("post_terminal_resolution"),
                        self.row("reclassification_match", result=result),
                    ])

    def test_original_terminal_history_remains_closed(self) -> None:
        with self.assertRaisesRegex(attempts.ExternalAttemptError, "exceeds 1"):
            self.validate([
                self.row("post_terminal_resolution"),
                self.row("post_terminal_resolution", attempt="2"),
            ])

    def test_reclassification_is_a_single_result(self) -> None:
        with self.assertRaisesRegex(attempts.ExternalAttemptError, "exceeds 1"):
            self.validate([
                self.row("post_terminal_resolution"),
                self.row("reclassification_match"),
                self.row("reclassification_match", attempt="2"),
            ])

    def test_audit_recognizes_the_same_history_modes_and_limits(self) -> None:
        self.assertEqual(attempts.MODES, audit_repository.EXTERNAL_MODES)
        self.assertEqual(attempts.MODE_MAX_ATTEMPTS, audit_repository.EXTERNAL_MODE_LIMITS)

    def test_reclassification_supersedes_sorted_reference_success(self) -> None:
        original = self.row("post_terminal_resolution")
        reference = self.row(
            "reference_match",
            reference_path="tmp/references/ygofm-decomp/src/func_80070710.c",
            reference_sha256="b" * 64,
        )
        current = self.row("reclassification_match", candidate_sha256="c" * 64)
        rows = attempts.sort_rows([reference, current, original])
        self.validate(rows)
        self.assertIs(
            attempts.latest_successes(rows)[self.address], current
        )

    def test_reclassification_selection_is_independent_of_row_order(self) -> None:
        original = self.row("post_terminal_resolution")
        reference = self.row("reference_match")
        current = self.row("reclassification_match")
        for rows in permutations([original, reference, current]):
            with self.subTest(modes=[row["mode"] for row in rows]):
                self.assertIs(
                    attempts.latest_successes(list(rows))[self.address],
                    current,
                )

    def test_legacy_success_order_is_unchanged_without_reclassification(self) -> None:
        original = self.row("post_terminal_resolution")
        reference = self.row("reference_match")
        self.assertIs(
            attempts.latest_successes([original, reference])[self.address],
            reference,
        )

    def test_unsuccessful_rows_do_not_replace_successes(self) -> None:
        original = self.row("post_terminal_resolution")
        unsuccessful = self.row("reclassification_match", result="nonmatch")
        self.assertIs(
            attempts.latest_successes([original, unsuccessful])[self.address],
            original,
        )


if __name__ == "__main__":
    unittest.main()
