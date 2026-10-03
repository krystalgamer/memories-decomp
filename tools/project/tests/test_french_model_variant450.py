import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_spanish_model_variant450 as spanish_lines
from tools.project.tests import test_spanish_model_variant450_quads as spanish_quads


class FrenchModelVariant450Tests(family435.FrenchModelVariant435Tests):
    family = 450
    module_count = 2
    distinct_images = 2
    binding_count = 35
    tail_start = 0x3940
    spans = ((4, 0xDD8), (0xDD8, 0x1794), (0x1794, 0x1ECC), (0x1ECC, 0x27C0),
             (0x27C0, 0x2D88), (0x2D88, 0x310C), (0x310C, 0x3940))
    helpers = ((0x27C0, 1480, "quads", "func_8013D7C0"),
               (0x2D88, 900, "lines", "func_8013DD88"))
    source_directories = {"quads": "spanish_model_variant", "lines": "spanish_model_variant"}
    reachable_helpers = {0x27C0, 0x2D88}
    local_call_targets = {0xDD8, 0x1ECC, 0x27C0, 0x2D88}
    models_by_stage = ((9, (174,)),)
    entry_anchors = {0xC38: 0x02602021, 0xC40: 0x02602021,
                     0x27C0: 0x27BDFEF8, 0x2D88: 0x27BDFEE0}

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/spanish_model_variant"
        for label, symbol in (("quads", "func_8013D7C0"), ("lines", "func_8013DD88")):
            self.assertEqual((directory / f"variant450_{label}_slot1.c").read_text(),
                             '#include "../../types.h"\n'
                             f'#define {symbol} {symbol.replace("8013", "8017")}\n'
                             f'#include "variant450_{label}.c"\n')
            self.assertNotRegex((directory / f"variant450_{label}.c").read_text(),
                                r"\b(?:extern|asm|__asm__|register|volatile)\b")
        with (family435.ROOT / "notes/overlays/french-model-variant450-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 8)
        self.assertEqual([row["result"] for row in rows], ["text_exact"] * 4 + ["matched"] * 4)
        for row in rows:
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            label, size = ("quads", 1480) if offset == 0x27C0 else ("lines", 900)
            self.assertIn(offset, (0x27C0, 0x2D88))
            self.assertIn(slot, (0, 1))
            source = directory / f"variant450_{label}{'_slot1' if slot else ''}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((int(row["instruction_bytes"]), row["different_words"], row["profile"]),
                             (size, "0", "gcc_2_8_1_g0_split"))
        expected = {("0x27C0", "0"), ("0x27C0", "1"), ("0x2D88", "0"), ("0x2D88", "1")}
        self.assertEqual({(row["function_offset"], row["slot"]) for row in rows[:4]}, expected)
        self.assertEqual({(row["function_offset"], row["slot"]) for row in rows[4:]}, expected)

    def test_french_caller_callees_and_measured_overlap(self):
        root = family435.ROOT
        archive_path, resident_path = root / "game/france/DATA/MODEL.MRG", root / "game/france/SLES_039.48"
        if not archive_path.exists() or not resident_path.exists():
            self.skipTest("legal French MODEL and resident inputs required")
        resident = resident_path.read_bytes()
        self.assertEqual(hashlib.sha256(resident).hexdigest(),
                         "57ecdfb9a9e1faf8b342fe7c7304c23723810861f3ab2fa3bef9eb27b5146b44")
        pointers = struct.unpack_from("<14I", resident, 0x800)
        with (root / "config/sles_03948/functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        with archive_path.open("rb") as archive:
            for module in self.modules:
                slot, base = int(self.instances[module["name"]]["slot"]), int(module["load_address"], 0)
                self.assertEqual(int(self.instances[module["name"]]["command_word"]), 616000)
                self.assertEqual(pointers[5 + slot], base)
                context = pointers[9 + slot]
                self.assertEqual(context, 0x80136000 + slot * 0x40000)
                start, end = pointers[3 + slot], pointers[3 + slot] + 4096
                for extent, overlap in ((0x42F8, 760), (0x42FC, 764)):
                    self.assertEqual(max(0, min(context + extent, end) - max(context, start)), overlap)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                for call, target in ((0xC34, 0x2D88), (0xC3C, 0x27C0)):
                    self.assertEqual(struct.unpack_from("<II", data, call),
                                     (0x0C000000 | ((base + target) >> 2 & 0x3FFFFFF), 0x02602021))
                external = set()
                for start, end in self.spans:
                    for word, in struct.iter_unpack("<I", data[start:end]):
                        if word >> 26 == 3:
                            target = 0x80000000 | ((word & 0x3FFFFFF) << 2)
                            if not base <= target < base + 20480:
                                external.add(target)
                bindings = (root / module["linker_symbols"]).read_text()
                declared = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                self.assertEqual(external, declared)
                self.assertTrue(external <= starts)
                self.assertIn("GsSortPoly = 0x800842A8;", bindings)
                self.assertIn("RotTransPers4 = 0x80087958;", bindings)
                self.assertNotIn("func_spanish_", bindings)

    def test_target_compiled_line_views(self):
        spanish_lines.SpanishModelVariant450Tests.test_target_compiled_partial_view_layout(self)

    def test_target_compiled_quad_views(self):
        spanish_quads.SpanishModelVariant450QuadTests.test_target_compiled_quad_view(self)
