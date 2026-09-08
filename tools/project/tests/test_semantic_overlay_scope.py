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


class OverlaySemanticScopeTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="semantic-scope-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.write_csv(
            "config/slus_01411/functions.csv", names.FUNCTION_FIELDS, []
        )
        self.write(
            "config/slus_01411/matching_c.json",
            json.dumps({"functions": [], "schema": 1}) + "\n",
        )
        self.symbols = "D_80020000 = 0x80020000;\n"
        self.write("config/slus_01411/symbols.txt", self.symbols)
        self.write(
            "src/game/caller.c",
            "extern void func_80168004(void);\n"
            "extern unsigned char D_80020000;\n",
        )
        self.write(
            "src/overlays/password/example.c",
            "void func_80168004(void) {}\n",
        )

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
        self, kind="overlay/password/function", name="Password_DrawFrame",
        address=0x80168004, confidence="high",
    ):
        return names.Mapping(kind, address, name, confidence, "local code", "scope")

    def load(self, mappings):
        rows = [
            {
                "kind": mapping.kind,
                "address": f"{mapping.address:#010x}",
                "name": mapping.name,
                "confidence": mapping.confidence,
                "evidence": mapping.evidence,
                "notes": mapping.notes,
            }
            for mapping in mappings
        ]
        path = self.write_csv(
            "notes/semantic-symbol-map.csv", names.MAP_FIELDS, rows
        )
        return names.load_map(path)

    def overlay(self, module="password", name="Password_DrawFrame", **changes):
        row = {
            "address": "0x80168004",
            "size": "0x10",
            "name": name,
            "status": "matching_c",
            "module": f"overlay/{module}",
            "notes": "",
        }
        row.update(changes)
        self.write_csv(
            f"config/slus_01411/overlays/{module}_functions.csv",
            names.FUNCTION_FIELDS,
            [row],
        )
        return row

    def test_resident_global_behavior_is_unchanged(self):
        mappings = self.load([
            self.mapping("global", "gResident_Value", 0x80020000)
        ])
        updates, _, _, symbols, _, replacements = names.plan(self.root, mappings)
        self.assertEqual(replacements, {"D_80020000": "gResident_Value"})
        self.assertIn("gResident_Value", updates[self.root / "src/game/caller.c"])
        self.assertEqual(symbols, "gResident_Value = 0x80020000;\n")

    def test_resident_function_behavior_is_unchanged(self):
        self.write_csv(
            "config/slus_01411/functions.csv", names.FUNCTION_FIELDS, [{
                "address": "0x80010000", "size": "0x10",
                "name": "func_80010000", "status": "unmatched_asm",
                "module": "game", "notes": "",
            }],
        )
        self.write("src/game/function.c", "void func_80010000(void) {}\n")
        mappings = self.load([
            self.mapping("function", "Game_Update", 0x80010000)
        ])
        updates, rows, _, symbols, _, replacements = names.plan(self.root, mappings)
        self.assertEqual(replacements, {"func_80010000": "Game_Update"})
        self.assertEqual(rows[0]["name"], "Game_Update")
        self.assertIn("Game_Update", updates[self.root / "src/game/function.c"])
        self.assertIn("Game_Update = 0x80010000; // type:func", symbols)

    def test_scoped_record_does_not_rewrite_any_resident_identity(self):
        self.overlay()
        mappings = self.load([self.mapping()])
        updates, _, _, symbols, moves, replacements = names.plan(self.root, mappings)
        self.assertEqual(updates, {})
        self.assertEqual(symbols, self.symbols)
        self.assertEqual(moves, [])
        self.assertEqual(replacements, {})
        self.assertEqual(names.update_symbols(self.symbols, mappings), self.symbols)

    def test_same_address_and_name_can_exist_in_distinct_overlay_namespaces(self):
        modules = ("overworld_before_coup", "overworld_after_coup")
        mappings = []
        for module in modules:
            self.overlay(module, "CampaignMap_Draw")
            mappings.append(self.mapping(
                f"overlay/{module}/function", "CampaignMap_Draw"
            ))
        loaded = self.load(mappings)
        self.assertEqual(len(loaded), 2)
        self.assertEqual(names.plan(self.root, loaded)[5], {})

    def test_same_address_can_have_different_names_in_distinct_modules(self):
        self.overlay()
        self.overlay("free_duel", "FreeDuel_Draw")
        mappings = self.load([
            self.mapping(),
            self.mapping("overlay/free_duel/function", "FreeDuel_Draw"),
        ])
        self.assertEqual(names.plan(self.root, mappings)[5], {})

    def test_duplicate_mapping_in_one_namespace_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate"):
            self.load([self.mapping(), self.mapping(name="Password_Other")])

    def test_duplicate_name_in_one_overlay_namespace_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate semantic name"):
            self.load([self.mapping(), self.mapping(address=0x80168014)])

    def test_duplicate_resident_name_across_kinds_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate semantic name"):
            self.load([
                self.mapping("function", "Repeated", 0x80010000),
                self.mapping("global", "Repeated", 0x80020000),
            ])

    def test_invalid_qualified_kind_is_rejected(self):
        for kind in (
            "overlay/../function", "overlay/password/global",
            "overlay//function", "overlay/password/function/extra",
        ):
            with self.subTest(kind=kind):
                with self.assertRaisesRegex(names.SemanticNameError, "unsupported"):
                    self.load([self.mapping(kind)])

    def test_unknown_module_is_rejected(self):
        with self.assertRaisesRegex(names.SemanticNameError, "unknown overlay"):
            names.plan(self.root, [self.mapping()])

    def test_inventory_symlink_cannot_escape_the_workspace(self):
        outside = tempfile.TemporaryDirectory(
            prefix="semantic-outside-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(outside.cleanup)
        target = Path(outside.name) / "functions.csv"
        target.write_text(",".join(names.FUNCTION_FIELDS) + "\n")
        link = self.root / "config/slus_01411/overlays/password_functions.csv"
        link.parent.mkdir(parents=True)
        link.symlink_to(target)
        with self.assertRaises(WorkspaceError):
            names.plan(self.root, [self.mapping()])

    def test_missing_overlay_address_is_rejected(self):
        self.overlay(address="0x80168014")
        with self.assertRaisesRegex(names.SemanticNameError, "absent from overlay"):
            names.plan(self.root, [self.mapping()])

    def test_unapplied_overlay_name_is_rejected(self):
        self.overlay(name="func_80168004")
        with self.assertRaisesRegex(names.SemanticNameError, "module-specific workflow"):
            names.plan(self.root, [self.mapping()])

    def test_wrong_inventory_namespace_is_rejected(self):
        row = self.overlay()
        row["module"] = "overlay/free_duel"
        self.write_csv(
            "config/slus_01411/overlays/password_functions.csv",
            names.FUNCTION_FIELDS, [row],
        )
        with self.assertRaisesRegex(names.SemanticNameError, "namespace disagrees"):
            names.plan(self.root, [self.mapping()])

    def test_unsubstantiated_overlay_name_is_rejected(self):
        self.overlay()
        with self.assertRaisesRegex(names.SemanticNameError, "confirmed or high"):
            names.plan(self.root, [self.mapping(confidence="review")])

    def test_duplicate_inventory_addresses_are_rejected(self):
        row = self.overlay()
        self.write_csv(
            "config/slus_01411/overlays/password_functions.csv",
            names.FUNCTION_FIELDS, [row, row],
        )
        with self.assertRaisesRegex(names.SemanticNameError, "duplicate overlay"):
            names.plan(self.root, [self.mapping()])

    def test_scoped_only_apply_is_a_read_only_noop(self):
        self.overlay()
        with patch.object(Path, "write_text", side_effect=AssertionError("write")):
            with patch.object(names, "write_csv", side_effect=AssertionError("write")):
                with patch.object(names, "write_json", side_effect=AssertionError("write")):
                    with contextlib.redirect_stdout(io.StringIO()):
                        result = names.apply(self.root, [self.mapping()], check=False)
        self.assertEqual(result, 0)


if __name__ == "__main__":
    unittest.main()
