from __future__ import annotations

from pathlib import Path
import struct
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from psyq_library_objects import (
    PLACEHOLDER,
    LibraryError,
    match_labels,
    parse_library,
)

TEXT = 1


def pstring(text: str) -> bytes:
    return bytes([len(text)]) + text.encode("latin-1")


def lnk_object(words: list[int], patched: list[int], labels: list[tuple[str, int]]) -> bytes:
    body = bytearray(b"LNK\x02")
    body += b"\x10" + struct.pack("<HHB", TEXT, 0, 8) + pstring(".text")
    body += b"\x06" + struct.pack("<H", TEXT)
    code = struct.pack(f"<{len(words)}I", *words)
    body += b"\x02" + struct.pack("<H", len(code)) + code
    for index in patched:
        # LO16 patch whose expression is external symbol 1
        body += b"\x0a" + struct.pack("<BH", 84, index * 4) + b"\x02" + struct.pack("<H", 1)
    for number, (name, offset) in enumerate(labels, start=2):
        body += b"\x0c" + struct.pack("<HHI", number, TEXT, offset) + pstring(name)
    body += b"\x0e" + struct.pack("<H", 1) + pstring("external")
    body += b"\x00"
    return bytes(body)


def library(members: list[tuple[str, bytes]]) -> bytes:
    data = bytearray(b"LIB\x01")
    for name, obj in members:
        header = 8 + 4 + 4 + 4 + 1
        data += name.ljust(8).encode("latin-1") + struct.pack("<III", 0, header, header + len(obj))
        data += b"\x00" + obj
    return bytes(data)


class PsyqLibraryObjectTests(unittest.TestCase):
    def setUp(self) -> None:
        temporary = tempfile.TemporaryDirectory(prefix="psyq-library-test-")
        self.addCleanup(temporary.cleanup)
        self.directory = Path(temporary.name)

    def write_library(self, members: list[tuple[str, bytes]]) -> Path:
        path = self.directory / "LIBTEST.LIB"
        path.write_bytes(library(members))
        return path

    def test_reads_labels_text_and_patched_words(self) -> None:
        words = [0x27BDFFE8, 0x3C020000, 0x8C420000, 0x03E00008]
        path = self.write_library([("OBJ_001", lnk_object(words, [1, 2], [("Alpha", 0), ("Beta", 8)]))])
        (only,) = parse_library(path)
        self.assertEqual(only.identity, "LIBTEST.LIB/OBJ_001")
        self.assertEqual(only.sections, {TEXT: ".text"})
        self.assertEqual(bytes(only.text[TEXT]), struct.pack("<4I", *words))
        self.assertEqual(only.patched[TEXT], {1, 2})
        self.assertEqual(only.labels, [("Alpha", TEXT, 0), ("Beta", TEXT, 8)])

    def test_patched_words_are_wildcards_and_matches_must_be_unique(self) -> None:
        words = [0x27BDFFE8, 0x3C020000, 0xAFBF0010, 0x03E00008, 0x27BD0018]
        path = self.write_library([("OBJ_001", lnk_object(words, [1], [("Alpha", 0), ("Beta", 12)]))])
        objects = parse_library(path)
        image = [0] * 4 + words[:]
        image[5] = 0x3C02800A  # relocated by the linker
        found = match_labels(objects, 0x80010000, tuple(image))
        self.assertEqual(found[0x80010010], {("LIBTEST.LIB/OBJ_001", 0, "Alpha")})
        self.assertEqual(found[0x8001001C], {("LIBTEST.LIB/OBJ_001", 12, "Beta")})
        twice = tuple(image + image)
        self.assertEqual(match_labels(objects, 0x80010000, twice), {})
        image[6] = 0xAFBF0014  # an unpatched word differs
        self.assertEqual(match_labels(objects, 0x80010000, tuple(image)), {})

    def test_rejects_unknown_records(self) -> None:
        broken = lnk_object([0x27BDFFE8, 0xAFBF0010, 0x03E00008], [], [("Alpha", 0)])
        broken = broken[:-1] + b"\x63\x00"
        path = self.write_library([("OBJ_001", broken)])
        with self.assertRaisesRegex(LibraryError, "unknown record 99"):
            parse_library(path)

    def test_placeholder_spellings(self) -> None:
        for name in ("func_8007E7F0", "func_german_8008E2A8", "func_spanish_8008E2A8"):
            self.assertRegex(name, PLACEHOLDER)
        for name in ("CdControlB", "CdPosToInt_8007E710", "D_80090DD8_French"):
            self.assertNotRegex(name, PLACEHOLDER)


if __name__ == "__main__":
    unittest.main()
