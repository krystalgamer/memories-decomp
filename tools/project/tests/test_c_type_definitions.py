from __future__ import annotations

from pathlib import Path
import sys
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import c_type_definitions


class CTypeDefinitionTests(unittest.TestCase):
    def test_detects_typedef_and_tag_definitions(self) -> None:
        text = """
typedef struct {
    int value;
} Local;
struct Tagged {
    int value;
};
enum {
    VALUE = 1
};
"""
        self.assertEqual(c_type_definitions.type_definition_lines(text), [2, 5, 8])

    def test_ignores_comments_literals_and_declarations(self) -> None:
        text = """
/* typedef int Fake; */
const char *description = "struct Fake {";
struct Forward;
struct Forward *use(struct Forward *);
"""
        self.assertEqual(c_type_definitions.type_definition_lines(text), [])


if __name__ == "__main__":
    unittest.main()
