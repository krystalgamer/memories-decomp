from __future__ import annotations

from fnmatch import fnmatchcase
from pathlib import Path
import re
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]


class CiWorkflowTests(unittest.TestCase):
    def test_regional_workflows_use_authenticated_bundle(self) -> None:
        regions = {
            "matching-build.yml": "usa",
            "overlay-build.yml": "usa",
            "european-build.yml": "europe",
            "french-build.yml": "france",
            "french-overlay-build.yml": "france",
            "german-build.yml": "germany",
            "german-overlay-build.yml": "germany",
            "italian-overlay-build.yml": "italy",
            "japanese-build.yml": "japanese",
            "spanish-overlay-build.yml": "spain",
        }
        for filename, region in regions.items():
            with self.subTest(workflow=filename):
                text = (REPOSITORY / ".github/workflows" / filename).read_text()
                self.assertEqual(text.count("uses: ./.github/actions/retail-inputs"), 1)
                self.assertIn(f"region: {region}\n", text)
                for secret in (
                    "YGOFM_CI_FILES", "YGOFM_CI_FILES_USERNAME", "YGOFM_CI_FILES_PASSWORD"
                ):
                    self.assertIn(f"${{{{ secrets.{secret} }}}}", text)
                self.assertNotRegex(text, r"YGOFM_\w+_URL")
                self.assertEqual(
                    'archives-only: "true"' in text,
                    filename in ("french-overlay-build.yml", "german-overlay-build.yml"),
                )
                self.assertEqual(
                    'executable-only: "true"' in text, filename == "matching-build.yml"
                )

    def test_bundle_download_authentication_and_cleanup(self) -> None:
        action = (REPOSITORY / ".github/actions/retail-inputs/action.yml").read_text()
        self.assertIn('--user "$YGOFM_CI_FILES_USERNAME:$YGOFM_CI_FILES_PASSWORD"', action)
        self.assertIn("--proto '=https' --proto-redir '=https'", action)
        self.assertIn("--retry 3 --retry-all-errors", action)
        self.assertNotIn("--location-trusted", action)
        self.assertIn('trap \'rm -f -- "$archive"\' EXIT', action)
        self.assertIn("python3 tools/project/stage_ci_inputs.py", action)

    def test_build_workflows_skip_only_ignored_changes(self) -> None:
        workflows = sorted((REPOSITORY / ".github/workflows").glob("*build.yml"))
        self.assertTrue(workflows)
        for path in workflows:
            text = path.read_text(encoding="utf-8")
            self.assertIn("\n  workflow_dispatch:", text)
            for event in ("push", "pull_request"):
                with self.subTest(workflow=path.name, event=event):
                    event_match = re.search(
                        rf"(?ms)^  {event}:\n(.*?)(?=^  [a-z_]+:|\npermissions:)",
                        text,
                    )
                    self.assertIsNotNone(event_match)
                    event_text = event_match.group(1)
                    self.assertIn("      - master\n", event_text)
                    ignored_match = re.search(
                        r"(?m)^    paths-ignore:\n((?:      - .+\n)+)", event_text
                    )
                    self.assertIsNotNone(ignored_match)
                    ignored = re.findall(r'      - "([^"]+)"', ignored_match.group(1))
                    self.assertIn("README.md", ignored)
                    self.assertTrue(all(
                        any(fnmatchcase(name, pattern) for pattern in ignored)
                        for name in ("README.md",)
                    ))
                    for changed in (
                        "src/game/example.c", "src/types.h",
                        "config/slus_01411/matching_c.json",
                        "config/sles_03948/overlays/duel_effects.yaml",
                        "tools/project/progress.py", "Makefile",
                        f".github/workflows/{path.name}",
                    ):
                        with self.subTest(changed=changed):
                            self.assertFalse(all(
                                any(fnmatchcase(name, pattern) for pattern in ignored)
                                for name in ("README.md", changed)
                            ))

    def test_metadata_remains_unfiltered_for_readme_only_changes(self) -> None:
        workflow = (REPOSITORY / ".github/workflows/metadata.yml").read_text(encoding="utf-8")
        triggers = workflow.split("\non:\n", 1)[1].split("\npermissions:", 1)[0]
        self.assertIn("  push:\n", triggers)
        self.assertIn("  pull_request:\n", triggers)
        self.assertNotRegex(triggers, r"\bpaths(?:-ignore)?:")
        self.assertIn("-s tools/project/tests -p test_ci_workflows.py", workflow)

    def test_metadata_workflow_validates_external_attempts(self) -> None:
        workflow = (
            REPOSITORY / ".github/workflows/metadata.yml"
        ).read_text(encoding="utf-8")

        self.assertIn(
            "      - name: Verify external attempt ledger\n"
            "        run: make external-attempts\n",
            workflow,
        )

    def test_metadata_workflow_checks_guest_pointer_annotations(self) -> None:
        workflow = (
            REPOSITORY / ".github/workflows/metadata.yml"
        ).read_text(encoding="utf-8")
        makefile = (REPOSITORY / "Makefile").read_text(encoding="utf-8")

        self.assertIn(
            "      - name: Verify guest-width pointer annotations\n"
            "        run: make check-g32\n",
            workflow,
        )
        self.assertIn(
            "\ncheck-g32:\n"
            "\t@$(PYTHON) tools/project/check_g32.py --self-test\n"
            "\t@$(PYTHON) tools/project/check_g32.py --report\n",
            makefile,
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
