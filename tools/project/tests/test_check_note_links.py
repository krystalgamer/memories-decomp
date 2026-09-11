from pathlib import Path
from tempfile import TemporaryDirectory
import unittest

from check_note_links import check_note_links, local_target


class CheckNoteLinksTests(unittest.TestCase):
    def test_local_target_ignores_external_and_anchor_links(self) -> None:
        self.assertIsNone(local_target("https://example.com/source.c"))
        self.assertIsNone(local_target("mailto:test@example.com"))
        self.assertIsNone(local_target("#section"))

    def test_local_target_decodes_paths_and_discards_fragments(self) -> None:
        self.assertEqual(local_target("../src/a%20b.c#function"), "../src/a b.c")
        self.assertEqual(local_target("<other-note.md#section>"), "other-note.md")

    def test_check_accepts_existing_links_and_images(self) -> None:
        with TemporaryDirectory() as directory:
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
        with TemporaryDirectory() as directory:
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
        with TemporaryDirectory() as directory:
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


if __name__ == "__main__":
    unittest.main()
