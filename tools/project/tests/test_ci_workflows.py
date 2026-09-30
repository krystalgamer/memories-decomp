from __future__ import annotations

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
                self.assertIn("cache-key: ${{ inputs.retail-cache-key }}", text)
                self.assertIn("prepare: ${{ !inputs.coordinated }}", text)
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
        action = (REPOSITORY / ".github/actions/prepare-retail-inputs/action.yml").read_text()
        self.assertIn('--user "$YGOFM_CI_FILES_USERNAME:$YGOFM_CI_FILES_PASSWORD"', action)
        self.assertIn("--proto '=https' --proto-redir '=https'", action)
        self.assertIn("--retry 3 --retry-all-errors", action)
        self.assertNotIn("--location-trusted", action)
        self.assertIn('trap \'rm -f -- "$archive"\' EXIT', action)
        self.assertIn('python3 tools/project/ci_bundle.py encrypt --archive "$archive"', action)
        self.assertIn("if: steps.cache.outputs.cache-hit != 'true'", action)
        self.assertIn("uses: actions/cache/save@v4", action)
        self.assertEqual(action.count("path: tmp/ci-retail-cache/ci-files.zip.gpg"), 2)
        self.assertNotIn("restore-keys:", action)
        consumer = (REPOSITORY / ".github/actions/retail-inputs/action.yml").read_text()
        self.assertNotIn("curl ", consumer)
        self.assertIn("fail-on-cache-miss: true", consumer)
        self.assertIn("if: inputs.prepare == 'true'", consumer)
        self.assertIn("python3 tools/project/ci_bundle.py stage", consumer)

    def test_regional_workflows_are_reusable_and_remain_manually_dispatchable(self) -> None:
        workflows = sorted((REPOSITORY / ".github/workflows").glob("*build.yml"))
        workflows = [path for path in workflows if path.name != "build.yml"]
        self.assertEqual(len(workflows), 10)
        for path in workflows:
            text = path.read_text(encoding="utf-8")
            with self.subTest(workflow=path.name):
                self.assertIn("\n  workflow_dispatch:", text)
                self.assertIn("\n  workflow_call:", text)
                self.assertNotIn("\n  push:", text)
                self.assertNotIn("\n  pull_request:", text)
                self.assertIn("      coordinated:\n", text)
                self.assertIn("        default: true\n        type: boolean", text)

    def test_single_coordinator_prepares_cache_before_all_builds(self) -> None:
        text = (REPOSITORY / ".github/workflows/build.yml").read_text()
        self.assertEqual(text.count("uses: ./.github/actions/prepare-retail-inputs"), 1)
        self.assertEqual(text.count("needs: prepare-inputs"), 10)
        self.assertEqual(text.count("retail-cache-key: ${{ needs.prepare-inputs.outputs.cache-key }}"), 10)
        self.assertEqual(text.count("secrets: inherit"), 10)
        self.assertIn("fetch-depth: 0", text)
        self.assertIn('ci_build_scope.py >> "$GITHUB_OUTPUT"', text)
        workflows = sorted(path.name for path in (REPOSITORY / ".github/workflows").glob("*build.yml")
                           if path.name != "build.yml")
        called = re.findall(r"uses: \./\.github/workflows/([a-z-]+\.yml)", text)
        self.assertEqual(sorted(called), workflows)
        self.assertEqual(text.count('      - "README.md"'), 2)
        self.assertIn("\n  workflow_dispatch:", text)
        self.assertEqual(text.count("      - master\n"), 2)

    def test_coordinator_preserves_documentation_and_trace_build_groups(self) -> None:
        text = (REPOSITORY / ".github/workflows/build.yml").read_text()
        blocks = re.split(r"(?m)^  ([a-z-]+):\n", text.split("\njobs:\n", 1)[1])
        jobs = dict(zip(blocks[1::2], blocks[2::2]))
        filtered = {
            "north-american-resident", "north-american-overlays", "german-resident",
            "german-overlays", "italian", "spanish",
        }
        for job, body in jobs.items():
            if job == "prepare-inputs":
                continue
            with self.subTest(job=job):
                self.assertEqual("if: needs.prepare-inputs.outputs.code-builds == 'true'" in body,
                                 job in filtered)

    def test_only_ciphertext_is_cached_and_no_retail_artifacts_are_uploaded(self) -> None:
        for path in (REPOSITORY / ".github/actions").glob("*/action.yml"):
            text = path.read_text()
            self.assertNotIn("actions/upload-artifact", text)
            if "retail" in path.parent.name:
                self.assertNotRegex(text, r"(?m)^\s+path: (?:game|.*\.zip)\s*$")
                self.assertNotIn("actions/cache@v4", text)

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
