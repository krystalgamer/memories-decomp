from __future__ import annotations

import contextlib
import csv
import io
import itertools
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import audit_repository
import integrate_verified_match
import record_external_attempt as recorder


class ReclassificationMatchTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        self.address = 0x80012345
        self.function = {
            "address": "0x80012345", "size": "0x20", "name": "func_80012345",
            "status": "unmatched_asm", "module": "game", "notes": "",
        }
        self.prior = {
            "mode": "post_terminal_resolution", "address": "0x80012345",
            "attempt": "1", "reference_path": "", "reference_sha256": "",
            "profile": "old_profile", "candidate_source": "tmp/old.c",
            "candidate_sha256": "1" * 64, "result": "matched",
            "summary": "Historical pinned match, subsequently reclassified.",
        }
        self.current = {
            **self.prior, "mode": "reclassification_match",
            "profile": "new_profile", "candidate_source": "tmp/new.c",
            "candidate_sha256": "2" * 64, "summary": "New discriminator: pure C.",
        }
        self.profiles = {
            name: {
                "compiler_flags": ["-G8"],
                "maspsx_flags": ["-G8"],
            }
            for name in ("old_profile", "new_profile")
        }

    def validate(self, rows: list[dict[str, str]], integrated: bool = False) -> None:
        function = {**self.function, "status": "matching_c" if integrated else "unmatched_asm"}
        recorder.validate_rows(
            rows, {self.address: function},
            {self.address} if integrated else set(),
            self.profiles, {self.address},
        )

    def test_history_valid_before_and_after_integration(self) -> None:
        for integrated in (False, True):
            self.validate([self.prior, self.current], integrated)

    def test_prior_success_is_required(self) -> None:
        deferred = {
            **self.prior, "mode": "reference_match", "result": "deferred",
            "reference_path": "tmp/references/ygofm-decomp/src/func_80012345.c",
            "reference_sha256": "3" * 64,
        }
        for prior in ([], [deferred]):
            with self.assertRaisesRegex(recorder.ExternalAttemptError, "lacks prior"):
                self.validate([*prior, self.current])

    def test_new_record_must_be_a_single_success(self) -> None:
        with self.assertRaisesRegex(recorder.ExternalAttemptError, "must be matched"):
            self.validate([self.prior, {**self.current, "result": "nonmatch"}])
        with self.assertRaisesRegex(recorder.ExternalAttemptError, "exceeds 1"):
            self.validate([self.prior, self.current, {**self.current, "attempt": "2"}])

    def test_new_evidence_wins_independently_of_ledger_order(self) -> None:
        old_reference = {**self.prior, "mode": "reference_match"}
        for rows in itertools.permutations([self.prior, old_reference, self.current]):
            self.assertIs(recorder.latest_successes(list(rows))[self.address], self.current)

    def test_legacy_success_selection_is_unchanged(self) -> None:
        reference = {**self.prior, "mode": "reference_match"}
        self.assertIs(
            recorder.latest_successes([self.prior, reference])[self.address], reference
        )

    def test_later_inline_refinement_can_supersede_reclassification(self) -> None:
        refinement = {
            **self.current, "mode": "inline_refinement",
            "candidate_sha256": "3" * 64,
            "summary": f"{recorder.REFINEMENT_PARENT_PREFIX}{'2' * 64}; Improved.",
        }
        self.validate([self.prior, self.current, refinement], integrated=True)
        for rows in itertools.permutations([self.prior, self.current, refinement]):
            self.assertIs(recorder.latest_successes(list(rows))[self.address], refinement)

    def test_earlier_inline_success_is_rejected_in_every_order(self) -> None:
        earlier = {**self.prior, "mode": "inline_refinement"}
        for rows in itertools.permutations([self.prior, earlier, self.current]):
            with self.assertRaisesRegex(recorder.ExternalAttemptError, "ambiguous"):
                recorder.latest_successes(list(rows))
            with self.assertRaisesRegex(recorder.ExternalAttemptError, "ambiguous"):
                self.validate(list(rows), integrated=True)

    def test_later_marker_does_not_hide_an_ambiguous_earlier_success(self) -> None:
        earlier = {**self.prior, "mode": "inline_refinement"}
        later = {
            **self.current, "mode": "inline_refinement", "attempt": "2",
            "summary": f"{recorder.REFINEMENT_PARENT_PREFIX}{'2' * 64}; Later.",
        }
        for rows in itertools.permutations([self.prior, earlier, self.current, later]):
            with self.assertRaisesRegex(recorder.ExternalAttemptError, "ambiguous"):
                recorder.latest_successes(list(rows))

    def test_refinement_parent_must_exist_and_match(self) -> None:
        for parent in ("3" * 64, "not-a-hash"):
            refinement = {
                **self.current, "mode": "inline_refinement",
                "summary": f"{recorder.REFINEMENT_PARENT_PREFIX}{parent}; Later.",
            }
            with self.assertRaisesRegex(recorder.ExternalAttemptError, "parent"):
                self.validate([self.prior, self.current, refinement], integrated=True)
        orphan = {
            **self.current, "mode": "inline_refinement",
            "summary": f"{recorder.REFINEMENT_PARENT_PREFIX}{'2' * 64}; Later.",
        }
        with self.assertRaisesRegex(recorder.ExternalAttemptError, "parent"):
            recorder.latest_successes([self.prior, orphan])
        with self.assertRaisesRegex(recorder.ExternalAttemptError, "lacks prior"):
            self.validate([self.current, orphan], integrated=True)

    def test_unsuccessful_linked_refinement_keeps_the_replacement(self) -> None:
        refinement = {
            **self.current, "mode": "inline_refinement", "result": "nonmatch",
            "candidate_sha256": "3" * 64,
            "summary": f"{recorder.REFINEMENT_PARENT_PREFIX}{'2' * 64}; Nonmatch.",
        }
        self.validate([self.prior, self.current, refinement], integrated=True)
        for rows in itertools.permutations([self.prior, self.current, refinement]):
            self.assertIs(recorder.latest_successes(list(rows))[self.address], self.current)

    def test_record_integrate_and_audit_preserve_old_evidence(self) -> None:
        with tempfile.TemporaryDirectory(
            prefix="reclassification-test-", dir=REPOSITORY / "tmp"
        ) as temporary:
            root = Path(temporary)
            config = root / "config/slus_01411"
            config.mkdir(parents=True)
            (config / "overlays").mkdir()
            candidate = root / "tmp/probe/candidate.c"
            candidate.parent.mkdir(parents=True)
            source = '#include "../../src/types.h"\nvoid func_80012345(void) {}\n'
            candidate.write_text(source)
            (root / "src").mkdir()
            (root / "src/types.h").write_text("")

            def write_csv(name: str, fields: tuple[str, ...], rows: list[dict[str, str]]) -> None:
                with (config / name).open("w", newline="") as handle:
                    writer = csv.DictWriter(handle, fieldnames=fields, lineterminator="\n")
                    writer.writeheader()
                    writer.writerows(rows)

            write_csv("functions.csv", tuple(self.function), [self.function])
            write_csv("external_attempts.csv", recorder.FIELDS, [self.prior])
            write_csv("attempts.csv", audit_repository.ATTEMPT_FIELDS, [{
                "address": "0x80012345", "attempt": "1", "compiler": "old_profile",
                "flags": "historical", "result": "deferred", "summary": "Original campaign.",
            }])
            (config / "matching_c.json").write_text(json.dumps({"schema": 1, "functions": []}))
            (config / "compiler_profiles.json").write_text(
                json.dumps({"schema": 1, "profiles": self.profiles})
            )
            for name in ("symbols.txt", "c_symbols.ld", "link_symbols.ld"):
                (config / name).write_text("")
            arguments = [
                "record_external_attempt.py", "0x80012345",
                "--mode", "reclassification_match", "--profile", "new_profile",
                "--candidate", "tmp/probe/candidate.c", "--result", "matched",
                "--new-discriminator", "Removed register pins.",
                "--summary", "Exact replacement.",
            ]
            original_ledger = (config / "external_attempts.csv").read_bytes()
            earlier = {**self.prior, "mode": "inline_refinement"}
            write_csv("external_attempts.csv", recorder.FIELDS, [self.prior, earlier])
            unsupported_ledger = (config / "external_attempts.csv").read_bytes()
            with (
                patch.object(recorder, "require_workspace_root", return_value=root),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stderr(io.StringIO()) as errors,
            ):
                self.assertEqual(recorder.main(), 1)
                self.assertIn("cannot reclassify a successful", errors.getvalue())
            self.assertEqual(
                (config / "external_attempts.csv").read_bytes(), unsupported_ledger
            )
            write_csv("external_attempts.csv", recorder.FIELDS, [self.prior])
            missing_discriminator = arguments.copy()
            index = missing_discriminator.index("--new-discriminator")
            del missing_discriminator[index:index + 2]
            with (
                patch.object(recorder, "require_workspace_root", return_value=root),
                patch.object(sys, "argv", missing_discriminator),
                contextlib.redirect_stderr(io.StringIO()) as errors,
            ):
                self.assertEqual(recorder.main(), 1)
                self.assertIn("new-discriminator", errors.getvalue())
            self.assertEqual((config / "external_attempts.csv").read_bytes(), original_ledger)

            write_csv("functions.csv", tuple(self.function), [
                {**self.function, "status": "matching_c"}
            ])
            with (
                patch.object(recorder, "require_workspace_root", return_value=root),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stderr(io.StringIO()) as errors,
            ):
                self.assertEqual(recorder.main(), 1)
                self.assertIn("requires unmatched assembly", errors.getvalue())
            self.assertEqual((config / "external_attempts.csv").read_bytes(), original_ledger)
            write_csv("functions.csv", tuple(self.function), [self.function])

            with (
                patch.object(recorder, "require_workspace_root", return_value=root),
                patch.object(recorder, "preprocess_candidate", return_value=source),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stdout(io.StringIO()),
            ):
                self.assertEqual(recorder.main(), 0)
            ledger = config / "external_attempts.csv"
            rows = recorder.load_rows(ledger)
            self.assertEqual(rows[0], self.prior)
            self.assertEqual(rows[1]["candidate_sha256"], recorder.sha256(candidate))

            arguments = [
                "integrate_verified_match.py", "0x80012345",
                "--source", "tmp/probe/candidate.c",
                "--destination", "src/game/func_80012345.c",
                "--profile", "new_profile", "--evidence-source", "reclassification",
                "--note", "Pure-C replacement.",
            ]
            with (
                patch.object(integrate_verified_match, "require_workspace_root", return_value=root),
                patch.object(integrate_verified_match, "preprocess_source", return_value=source),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stdout(io.StringIO()),
            ):
                self.assertEqual(integrate_verified_match.main(), 0)
            self.assertEqual((root / "src/game/func_80012345.c").read_text(), source)
            for ordered in (rows, list(reversed(rows))):
                write_csv("external_attempts.csv", recorder.FIELDS, ordered)
                audit_repository.audit_attempts(root)

            for ordered in itertools.permutations([*rows, earlier]):
                write_csv("external_attempts.csv", recorder.FIELDS, list(ordered))
                with self.assertRaisesRegex(integrate_verified_match.IntegrationError, "ambiguous"):
                    integrate_verified_match.require_matched_attempt(
                        ledger, self.address, mode="reclassification_match"
                    )
                with self.assertRaisesRegex(audit_repository.AuditError, "ambiguous"):
                    audit_repository.audit_attempts(root)
            write_csv("external_attempts.csv", recorder.FIELDS, rows)

            refined = candidate.with_name("refined.c")
            refined_source = source + "/* A later exact source refinement. */\n"
            refined.write_text(refined_source)
            arguments = [
                "record_external_attempt.py", "0x80012345",
                "--mode", "inline_refinement", "--profile", "new_profile",
                "--candidate", "tmp/probe/refined.c", "--result", "matched",
                "--summary", "Later exact refinement.",
            ]
            with (
                patch.object(recorder, "require_workspace_root", return_value=root),
                patch.object(recorder, "preprocess_candidate", return_value=refined_source),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stdout(io.StringIO()),
            ):
                self.assertEqual(recorder.main(), 0)
            refined_rows = recorder.load_rows(ledger)
            refinement = next(row for row in refined_rows if row["mode"] == "inline_refinement")
            self.assertEqual(recorder.refinement_parent(refinement), rows[1]["candidate_sha256"])
            self.assertIn(self.prior, refined_rows)
            self.assertIn(rows[1], refined_rows)
            with self.assertRaisesRegex(integrate_verified_match.IntegrationError, "superseded"):
                integrate_verified_match.require_matched_attempt(
                    ledger, self.address, mode="reclassification_match"
                )
            arguments = [
                "integrate_verified_match.py", "0x80012345",
                "--source", "tmp/probe/refined.c",
                "--destination", "src/game/func_80012345.c",
                "--profile", "new_profile", "--evidence-source", "refinement",
                "--replace-existing", "--note", "Later pure-C refinement.",
            ]
            with (
                patch.object(integrate_verified_match, "require_workspace_root", return_value=root),
                patch.object(
                    integrate_verified_match,
                    "preprocess_source",
                    return_value=refined_source,
                ),
                patch.object(sys, "argv", arguments),
                contextlib.redirect_stdout(io.StringIO()),
            ):
                self.assertEqual(integrate_verified_match.main(), 0)
            self.assertEqual((root / "src/game/func_80012345.c").read_text(), refined_source)
            for ordered in itertools.permutations(refined_rows):
                write_csv("external_attempts.csv", recorder.FIELDS, list(ordered))
                audit_repository.audit_attempts(root)


if __name__ == "__main__":
    unittest.main()
