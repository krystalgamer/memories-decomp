from __future__ import annotations

from pathlib import Path
import struct
import sys
import contextlib
import io
import tempfile
import unittest
from unittest import mock

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import psyq_library_objects
from psyq_library_objects import (
    PLACEHOLDER,
    REFERENCE,
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


    def run_main(self, argv: list[str], images: dict, inventories: dict) -> tuple[int, mock.Mock]:
        """Run the CLI against synthetic images and inventories; apply() is recorded."""
        applied = mock.Mock(return_value=0)
        with contextlib.ExitStack() as stack:
            module = psyq_library_objects
            stack.enter_context(mock.patch.object(module, "require_workspace_root", return_value=self.directory))
            stack.enter_context(mock.patch.object(module, "load_image", side_effect=lambda root, image: images[image]))
            stack.enter_context(mock.patch.object(module, "load_inventory", side_effect=lambda root, image: inventories[image]))
            stack.enter_context(mock.patch.object(module, "pinned_resolutions", return_value={}))
            stack.enter_context(mock.patch.object(module, "apply", applied))
            stack.enter_context(contextlib.redirect_stdout(io.StringIO()))
            stack.enter_context(contextlib.redirect_stderr(io.StringIO()))
            status = module.main(["--library-dir", str(self.directory)] + argv)
        return status, applied

    def test_zero_evidence_control_is_inconclusive(self) -> None:
        words = [0x27BDFFE8, 0xAFBF0010, 0x03E00008, 0x27BD0018]
        unrelated = (0x80010000, (0x00000000,) * 8)
        images = {REFERENCE: unrelated, "slpm_86398": unrelated}
        inventories = {
            REFERENCE: [{"address": "0x80010000", "name": "Alpha", "status": "sdk_asm"}],
            "slpm_86398": [{"address": "0x80010000", "name": "func_80010000", "status": "sdk_asm"}],
        }
        for members in ([], [("OBJ_001", lnk_object(words, [], [("Alpha", 0)]))]):
            with self.subTest(members=len(members)):
                self.write_library(members)
                for argv in (["--check"], ["--report", "--image", "slpm_86398"],
                             ["--apply", "--image", "slpm_86398"]):
                    status, applied = self.run_main(argv, images, inventories)
                    self.assertEqual(status, 2, argv)
                    applied.assert_not_called()

    def test_valid_control_with_no_target_proposals_succeeds(self) -> None:
        words = [0x27BDFFE8, 0xAFBF0010, 0x03E00008, 0x27BD0018]
        self.write_library([("OBJ_001", lnk_object(words, [], [("Alpha", 0)]))])
        images = {REFERENCE: (0x80010000, tuple(words)), "slpm_86398": (0x80010000, (0,) * 4)}
        inventories = {
            REFERENCE: [{"address": "0x80010000", "name": "Alpha", "status": "sdk_asm"}],
            "slpm_86398": [{"address": "0x80010000", "name": "func_80010000", "status": "sdk_asm"}],
        }
        status, applied = self.run_main(["--check"], images, inventories)
        self.assertEqual(status, 0)
        status, applied = self.run_main(["--apply", "--image", "slpm_86398"], images, inventories)
        self.assertEqual(status, 0)
        applied.assert_called_once()
        self.assertEqual(applied.call_args.args[2], {})


if __name__ == "__main__":
    unittest.main()
