from __future__ import annotations

from pathlib import Path
import shutil
import sys
import unittest
from unittest import mock


REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import candidate_human_bundles


class CandidateHumanBundleTests(unittest.TestCase):
    def test_missing_bundle_directory_is_valid_when_no_notes_remain(self) -> None:
        directory = REPOSITORY / "tmp/test-candidate-human-bundles/missing"
        shutil.rmtree(directory.parent, ignore_errors=True)
        try:
            with mock.patch.object(
                candidate_human_bundles,
                "BUNDLE_DIRECTORY",
                directory,
            ):
                candidate_human_bundles.check_bundles([], {})
        finally:
            shutil.rmtree(directory.parent, ignore_errors=True)


if __name__ == "__main__":
    unittest.main()
