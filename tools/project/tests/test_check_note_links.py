from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

from check_note_links import check_note_links, local_target

LOCAL_TMP = Path(__file__).resolve().parents[3] / "tmp"
LOCAL_TMP.mkdir(exist_ok=True)


def local_temporary_directory() -> TemporaryDirectory:
    return TemporaryDirectory(dir=LOCAL_TMP)


class CheckNoteLinksTests(unittest.TestCase):
    def test_local_target_ignores_external_and_anchor_links(self) -> None:
        self.assertIsNone(local_target("https://example.com/source.c"))
        self.assertIsNone(local_target("mailto:test@example.com"))
        self.assertIsNone(local_target("#section"))

    def test_local_target_decodes_paths_and_discards_fragments(self) -> None:
        self.assertEqual(local_target("../src/a%20b.c#function"), "../src/a b.c")
        self.assertEqual(local_target("<other-note.md#section>"), "other-note.md")

    def test_check_accepts_existing_links_and_images(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src"
            notes.mkdir()
            source.mkdir()
            (source / "file.c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "image.png").write_bytes(b"png")
            (notes / "a.md").write_text(
                "[source](../src/file.c#f)\n![image](image.png)\n",
                encoding="utf-8",
            )

            note_count, link_count, problems = check_note_links(root)

        self.assertEqual((note_count, link_count), (1, 2))
        self.assertEqual(problems, [])

    def test_check_reports_missing_and_escaping_paths(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            notes.mkdir()
            (notes / "a.md").write_text(
                "[missing](missing.md)\n[escape](../../outside.md)\n",
                encoding="utf-8",
            )

            _, _, problems = check_note_links(root)

        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [
                ("missing.md", "does not exist"),
                ("../../outside.md", "escapes repository"),
            ],
        )

    def test_check_accepts_optional_link_titles(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            notes.mkdir()
            (notes / "target.md").write_text("# Target\n", encoding="utf-8")
            (notes / "a.md").write_text(
                '[target](target.md "Title")\n',
                encoding="utf-8",
            )

            _, link_count, problems = check_note_links(root)

        self.assertEqual(link_count, 1)
        self.assertEqual(problems, [])

    def test_check_accepts_root_relative_paths_in_code_spans(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src/game"
            notes.mkdir()
            source.mkdir(parents=True)
            (source / "file.c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "a.md").write_text(
                "`src/game/file.c` and `src/game/file.c:12-14`\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 2)
        self.assertEqual(problems, [])

    def test_check_reports_missing_code_span_path(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            notes.mkdir()
            (notes / "a.md").write_text(
                "`src/candidates/missing.c`\n",
                encoding="utf-8",
            )

            _, _, problems = check_note_links(root)

        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [("src/candidates/missing.c", "does not exist")],
        )

    def test_check_ignores_external_reference_tree_paths(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            research = notes / "research"
            notes.mkdir()
            research.mkdir()
            (notes / "a.md").write_text(
                "`src/hirata/H_mctrl1.c` and "
                "`tools/vendor/maspsx/fixture.yaml`\n",
                encoding="utf-8",
            )
            (research / "snapshot.md").write_text(
                "`tools/upstream_generator.py`\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 0)
        self.assertEqual(problems, [])

    def test_inline_paths_normalize_before_scope_and_exemption_checks(self) -> None:
        with local_temporary_directory() as directory:
            parent = Path(directory)
            root = parent / "root"
            notes = root / "notes"
            notes.mkdir(parents=True)
            (parent / "outside.md").write_text("# Outside\n", encoding="utf-8")
            (notes / "a.md").write_text(
                "`src/../../outside.md`\n"
                "`tools/vendor/../../src/missing.c`\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 2)
        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [
                ("src/../../outside.md", "escapes repository"),
                ("tools/vendor/../../src/missing.c", "does not exist"),
            ],
        )

    def test_reference_style_links_resolve_definitions(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src"
            notes.mkdir()
            source.mkdir()
            (source / "file.c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "a.md").write_text(
                "[existing][source] and [missing][absent]\n"
                "[source]: ../src/file.c\n"
                "[absent]: ../src/missing.c\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 2)
        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [("../src/missing.c", "does not exist")],
        )

    def test_reference_definitions_use_first_normalized_label(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src"
            notes.mkdir()
            source.mkdir()
            (source / "file.c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "first-missing.md").write_text(
                "[x][ref]\n"
                "[ref]: ../src/missing.c\n"
                "[ REF ]: ../src/file.c\n",
                encoding="utf-8",
            )
            (notes / "first-existing.md").write_text(
                "[x][ref]\n"
                "[ref]: ../src/file.c\n"
                "[ REF ]: ../src/missing.c\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 2)
        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [("../src/missing.c", "does not exist")],
        )

    def test_reference_destinations_continue_on_next_line(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src"
            notes.mkdir()
            source.mkdir()
            (source / "file.c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "a.md").write_text(
                "[existing][source] and [missing][absent] and [angle][angled]\n"
                "[source]:\n"
                "    ../src/file.c\n"
                "[absent]:\n"
                "../src/missing.c\n"
                "[angled]:\n"
                "<../src/file.c>\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 3)
        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [("../src/missing.c", "does not exist")],
        )

    def test_parenthesized_and_escaped_destinations_are_not_truncated(self) -> None:
        with local_temporary_directory() as directory:
            root = Path(directory)
            notes = root / "notes"
            source = root / "src"
            notes.mkdir()
            source.mkdir()
            (source / "a(b).c").write_text("void f(void) {}\n", encoding="utf-8")
            (notes / "a.md").write_text(
                "[balanced](../src/a(b).c)\n"
                "[escaped](../src/a\\(b\\).c)\n"
                "[missing](../src/missing(x).c)\n",
                encoding="utf-8",
            )

            _, reference_count, problems = check_note_links(root)

        self.assertEqual(reference_count, 3)
        self.assertEqual(
            [(problem.target, problem.reason) for problem in problems],
            [("../src/missing(x).c", "does not exist")],
        )


if __name__ == "__main__":
    unittest.main()
