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


if __name__ == "__main__":
    unittest.main()
