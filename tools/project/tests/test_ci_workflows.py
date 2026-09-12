from __future__ import annotations

from pathlib import Path
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]


class CiWorkflowTests(unittest.TestCase):
    def test_metadata_workflow_validates_external_attempts(self) -> None:
        workflow = (
            REPOSITORY / ".github/workflows/metadata.yml"
        ).read_text(encoding="utf-8")

        self.assertIn(
            "      - name: Verify external attempt ledger\n"
            "        run: make external-attempts\n",
            workflow,
        )

    def test_external_attempt_check_does_not_require_retail_input(self) -> None:
        makefile = (REPOSITORY / "Makefile").read_text(encoding="utf-8")

        self.assertIn(
            "\nexternal-attempts:\n"
            "\t@$(PYTHON) tools/project/record_external_attempt.py --check\n",
            makefile,
        )


if __name__ == "__main__":
    unittest.main()
