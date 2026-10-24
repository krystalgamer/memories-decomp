from __future__ import annotations

import contextlib
import csv
import io
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import apply_semantic_names as names
from workspace import WorkspaceError


class RegionalSemanticScopeTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="semantic-regional-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        (self.root / "src").mkdir()
        self.write_csv("config/slus_01411/functions.csv", names.FUNCTION_FIELDS, [])
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps({"schema": 1, "functions": []}) + "\n",
        )
        self.symbols = "D_80020000 = 0x80020000;"
        self.write("config/slus_01411/symbols.txt", self.symbols)
        self.regional()

    def write(self, relative: str, text: str) -> Path:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path

    def write_csv(self, relative, fields, rows):
        text = io.StringIO(newline="")
        writer = csv.DictWriter(text, fieldnames=fields, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
        return self.write(relative, text.getvalue())

    def mapping(
        self, kind="regional/slpm_86398/global", name="gText_awGlyphColorMasks",
        address=0x80090CF8, confidence="high",
    ):
        return names.Mapping(kind, address, name, confidence, "local readers", "scope")

    def load(self, mappings):
        path = self.write_csv("notes/semantic-symbol-map.csv", names.MAP_FIELDS, [
            {
                "kind": mapping.kind, "address": f"{mapping.address:#010x}",
                "name": mapping.name, "confidence": mapping.confidence,
                "evidence": mapping.evidence, "notes": mapping.notes,
            }
            for mapping in mappings
        ])
        return names.load_map(path)

    def regional(
        self, image="slpm_86398",
        symbols="D_80090E48 = 0x80090CF8;\n",
        aliases="gText_awGlyphColorMasks = D_80090E48;\n",
    ):
        self.write(f"config/{image}/symbols.txt", symbols)
        self.write(f"config/{image}/link_symbols.ld", aliases)

    def test_applied_alias_is_validation_only(self):
        mappings = self.load([self.mapping()])
        updates, functions, matching, symbols, moves, replacements = names.plan(
            self.root, mappings
        )
        self.assertEqual(updates, {})
        self.assertEqual(functions, [])
        self.assertEqual(matching, {"schema": 1, "functions": []})
        self.assertEqual(symbols, self.symbols)
        self.assertEqual(moves, [])
        self.assertEqual(replacements, {})

    def test_all_supported_images_accept_their_own_provider(self):
        for image in names.REGIONAL_IMAGES:
            with self.subTest(image=image):
                self.regional(image)
                names.plan(self.root, self.load([
                    self.mapping(kind=f"regional/{image}/global")
                ]))

    def test_names_and_addresses_may_repeat_in_different_namespaces(self):
        self.regional("sles_03947")
        mappings = self.load([
            self.mapping(), self.mapping(kind="regional/sles_03947/global"),
            self.mapping(kind="global"),
        ])
        self.assertEqual(len(mappings), 3)

    def test_duplicate_address_in_one_image_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate regional"):
            self.load([self.mapping(), self.mapping(name="gText_Other")])

    def test_duplicate_name_in_one_image_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate semantic name"):
            self.load([self.mapping(), self.mapping(address=0x80090CD8)])

    def test_unknown_and_nonregional_images_are_rejected(self):
        for image in ("unknown", "slus_01411"):
            with self.subTest(image=image):
                with self.assertRaisesRegex(names.SemanticNameError, "unknown regional"):
                    self.load([self.mapping(kind=f"regional/{image}/global")])

    def test_malformed_scope_and_invalid_identifier_are_rejected(self):
        for kind in ("regional/../global", "regional/slpm_86398/function",
                     "regional/slpm_86398/global/extra"):
            with self.subTest(kind=kind):
                with self.assertRaisesRegex(names.SemanticNameError, "unsupported"):
                    self.load([self.mapping(kind=kind)])
        with self.assertRaisesRegex(names.SemanticNameError, "invalid C identifier"):
            self.load([self.mapping(name="not-a-name")])

    def test_wrong_image_does_not_borrow_another_provider(self):
        self.regional("sles_03947", symbols="", aliases="")
        with self.assertRaisesRegex(names.SemanticNameError, "missing regional symbol"):
            names.plan(self.root, [self.mapping(kind="regional/sles_03947/global")])

    def test_unapplied_name_is_rejected(self):
        self.regional(aliases="")
        with self.assertRaisesRegex(names.SemanticNameError, "missing regional symbol"):
            names.plan(self.root, [self.mapping()])

    def test_wrong_address_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "not 0x80090cd8"):
            names.plan(self.root, [self.mapping(address=0x80090CD8)])

    def test_only_high_and_confirmed_confidence_are_accepted(self):
        for confidence in ("medium", "low", "", "tentative"):
            with self.subTest(confidence=confidence):
                with self.assertRaisesRegex(names.SemanticNameError, "confirmed or high"):
                    names.plan(self.root, [self.mapping(confidence=confidence)])
        names.plan(self.root, [self.mapping(confidence="confirmed")])

    def test_alias_chains_offsets_and_comments_are_resolved(self):
        self.regional(
            symbols="D_80090E48 = 0x80090CD8;\n",
            aliases=(
                "/* gText_awGlyphColorMasks = 0; */\n"
                "intermediate = D_80090E48 + 0x24; // offset\n"
                "gText_awGlyphColorMasks = intermediate - 4;\n"
            ),
        )
        names.plan(self.root, [self.mapping()])

    def test_numeric_alias_is_accepted(self):
        self.regional(aliases="gText_awGlyphColorMasks = 0x80090CF8;\n")
        names.plan(self.root, [self.mapping()])

    def test_missing_alias_target_is_rejected(self):
        self.regional(symbols="")
        with self.assertRaisesRegex(names.SemanticNameError, "missing regional symbol"):
            names.plan(self.root, [self.mapping()])

    def test_cycle_is_rejected(self):
        self.regional(aliases=(
            "gText_awGlyphColorMasks = intermediate;\n"
            "intermediate = gText_awGlyphColorMasks;\n"
        ))
        with self.assertRaisesRegex(names.SemanticNameError, "cyclic regional alias"):
            names.plan(self.root, [self.mapping()])

    def test_excessive_alias_depth_is_rejected(self):
        self.regional(aliases=(
            "gText_awGlyphColorMasks = alias0;\n"
            + "".join(f"alias{i} = alias{i + 1};\n" for i in range(65))
            + "alias65 = 0x80090CF8;\n"
        ))
        with self.assertRaisesRegex(names.SemanticNameError, "chain too deep"):
            names.plan(self.root, [self.mapping()])

    def test_conflicting_definitions_are_rejected(self):
        for aliases in (
            "D_80090E48 = 0x80090CD8;\ngText_awGlyphColorMasks = D_80090E48;\n",
            "gText_awGlyphColorMasks = D_80090E48;\ngText_awGlyphColorMasks = 0;\n",
        ):
            with self.subTest(aliases=aliases):
                self.regional(aliases=aliases)
                with self.assertRaisesRegex(names.SemanticNameError, "conflicting"):
                    names.plan(self.root, [self.mapping()])

    def test_consistent_duplicate_definitions_are_accepted(self):
        self.regional(aliases=(
            "D_80090E48 = 0x80090CF8;\n"
            "gText_awGlyphColorMasks = D_80090E48;\n"
            "gText_awGlyphColorMasks = 0x80090CF8;\n"
        ))
        names.plan(self.root, [self.mapping()])

    def test_unsupported_selected_expression_is_rejected(self):
        self.regional(aliases="gText_awGlyphColorMasks = D_80090E48 | 1;\n")
        with self.assertRaisesRegex(names.SemanticNameError, "unsupported regional"):
            names.plan(self.root, [self.mapping()])

    def test_unrelated_expressions_are_not_evaluated(self):
        self.regional(aliases=(
            "gText_awGlyphColorMasks = D_80090E48;\n"
            "unrelated = ALIGN(4);\n"
        ))
        names.plan(self.root, [self.mapping()])

    def test_overflow_and_underflow_are_rejected(self):
        for expression in ("0xFFFFFFFF + 1", "0 - 1"):
            with self.subTest(expression=expression):
                self.regional(aliases=f"gText_awGlyphColorMasks = {expression};\n")
                with self.assertRaisesRegex(names.SemanticNameError, "32-bit range"):
                    names.plan(self.root, [self.mapping()])

    def test_missing_image_metadata_is_rejected(self):
        (self.root / "config/slpm_86398/link_symbols.ld").unlink()
        with self.assertRaises(WorkspaceError):
            names.plan(self.root, [self.mapping()])

    def test_symbol_file_symlink_escape_is_rejected(self):
        for filename in ("symbols.txt", "link_symbols.ld"):
            with self.subTest(filename=filename):
                path = self.root / "config/slpm_86398" / filename
                content = path.read_bytes()
                outside = self.root.parent / f"{self.root.name}-outside"
                outside.write_bytes(content)
                self.addCleanup(outside.unlink, missing_ok=True)
                path.unlink()
                path.symlink_to(outside)
                with self.assertRaises(WorkspaceError):
                    names.plan(self.root, [self.mapping()])
                path.unlink()
                path.write_bytes(content)

    def test_scoped_only_check_and_apply_never_write(self):
        for check in (True, False):
            with self.subTest(check=check):
                with patch.object(Path, "write_text", side_effect=AssertionError("write")):
                    with patch.object(names, "write_csv", side_effect=AssertionError("write")):
                        with patch.object(names, "write_json", side_effect=AssertionError("write")):
                            with contextlib.redirect_stdout(io.StringIO()):
                                self.assertEqual(
                                    names.apply(self.root, [self.mapping()], check=check), 0
                                )

    def test_scoped_only_symbols_preserve_legacy_formatting(self):
        text = "old = 0x80020000;\nother = 0x80020000;"
        self.assertEqual(names.update_symbols(text, [self.mapping()]), text)

    def test_mixed_resident_and_regional_maps_keep_resident_transforms(self):
        updates, _, _, symbols, moves, replacements = names.plan(self.root, [
            self.mapping(),
            self.mapping(kind="global", name="gResident_Value", address=0x80020000),
        ])
        self.assertEqual(updates, {})
        self.assertEqual(symbols, "gResident_Value = 0x80020000;\n")
        self.assertEqual(moves, [])
        self.assertEqual(replacements, {"D_80020000": "gResident_Value"})


if __name__ == "__main__":
    unittest.main()
