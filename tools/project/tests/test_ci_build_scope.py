from __future__ import annotations

import contextlib
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from ci_build_scope import ScopeError, code_builds, requires_code_builds


class CiBuildScopeTests(unittest.TestCase):
    def setUp(self) -> None:
        scratch = REPOSITORY / "tmp"
        scratch.mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(prefix="test-ci-scope-", dir=scratch)
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.environment = {
            **os.environ, "GIT_AUTHOR_NAME": "Copilot", "GIT_COMMITTER_NAME": "Copilot",
            "GIT_AUTHOR_EMAIL": "223556219+Copilot@users.noreply.github.com",
            "GIT_COMMITTER_EMAIL": "223556219+Copilot@users.noreply.github.com",
        }
        self.git("init", "--quiet", "--initial-branch=master")
        self.base = self.commit("README.md")

    def git(self, *args: str) -> str:
        return subprocess.check_output(
            ["git", *args], cwd=self.root, env=self.environment, text=True, stderr=subprocess.PIPE,
        ).strip()

    def commit(self, name: str) -> str:
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text("synthetic fixture\n")
        self.git("add", "--", name)
        self.git("commit", "--quiet", "-m",
                 "Add synthetic fixture\n\nCo-authored-by: Copilot <223556219+Copilot@users.noreply.github.com>")
        return self.git("rev-parse", "HEAD")

    def test_existing_path_filters(self) -> None:
        self.assertFalse(requires_code_builds(["README.md", "notes/a.md", "tools/trace/a.py"]))
        for path in ("src/game/a.c", "tools/project/a.py", ".github/workflows/build.yml",
                     "config/sles_03948/overlays.json", "Makefile", "notes-other/file"):
            with self.subTest(path=path):
                self.assertTrue(requires_code_builds(["notes/a.md", path]))

    def test_push_uses_changed_paths(self) -> None:
        notes = self.commit("notes/test.md")
        self.assertFalse(code_builds(self.root, "push", self.base, notes))
        code = self.commit("src/test.c")
        self.assertTrue(code_builds(self.root, "push", notes, code))

    def test_pull_request_uses_merge_base_not_unrelated_master_changes(self) -> None:
        new_master = self.commit("src/master.c")
        self.git("switch", "--quiet", "-c", "topic", self.base)
        topic = self.commit("notes/topic.md")
        self.assertFalse(code_builds(self.root, "pull_request", new_master, topic))

    def test_manual_and_new_branch_runs_select_all_builds(self) -> None:
        self.assertTrue(code_builds(self.root, "workflow_dispatch", "", ""))
        with contextlib.redirect_stderr(io.StringIO()) as output:
            self.assertTrue(code_builds(self.root, "push", "0" * 40, self.base))
        self.assertIn("running all build gates", output.getvalue())

    def test_missing_old_commit_runs_all_builds_with_warning(self) -> None:
        with contextlib.redirect_stderr(io.StringIO()) as output:
            self.assertTrue(code_builds(self.root, "push", "f" * 40, self.base))
        self.assertIn("::warning::", output.getvalue())

    def test_bad_event_and_revision_inputs_are_errors(self) -> None:
        with self.assertRaisesRegex(ScopeError, "unsupported"):
            code_builds(self.root, "schedule", self.base, self.base)
        with self.assertRaisesRegex(ScopeError, "head SHA"):
            code_builds(self.root, "push", self.base, "--output=bad")
        with self.assertRaisesRegex(ScopeError, "base SHA"):
            code_builds(self.root, "push", "--bad", self.base)

    def test_failed_diff_is_not_reported_as_documentation_only(self) -> None:
        with self.assertRaisesRegex(ScopeError, "cannot inspect changed files"):
            code_builds(self.root, "push", self.base, "f" * 40)


if __name__ == "__main__":
    unittest.main()
