import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant373 as family373
from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant458Tests(family435.FrenchModelVariant435Tests):
    family = 458
    module_count = 2
    distinct_images = 2
    binding_count = 34
    tail_start = 0x230C
    spans = ((4, 0xC10), (0xC10, 0x1150), (0x1150, 0x19EC), (0x19EC, 0x230C))
    helpers = ((0xC10, 1344, "sheets", "func_8013BC10"),)
    reachable_helpers = {0xC10}
    local_call_targets = {0xC10, 0x1150, 0x19EC}
    models_by_stage = ((7, (202,)),)
    entry_anchors = {
        0x1C: 0x26D80C00, 0x2C: 0x26D71B20, 0x8C: 0x24111C60,
        0xB4: 0xAEC21F24, 0xD8: 0x0C02290A, 0xE0: 0x26310020,
        0xE4: 0x2A420007, 0x1F0: 0x0C020BBA, 0x240: 0x26F70034,
        0x244: 0x0C020BBA, 0x854: 0x0C02290A, 0x91C: 0xAE221E20,
        0x938: 0x26730020, 0x964: 0x2A420007, 0x96C: 0x26310010,
        0xAB4: 0x8FA400D0, 0xABC: 0, 0xC10: 0x27BDFEE0,
        0x10CC: 0x8E421F24, 0x10D0: 0, 0x10D4: 0x8C430024,
        0x10D8: 0x8E421F10, 0x10DC: 0, 0x10E0: 0x0043102A,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant458_sheets_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013BC10 func_8017BC10\n'
                         '#include "variant458_sheets.c"\n')
        body = (directory / "variant458_sheets.c").read_text()
        self.assertIn('#include "variant458_sheets.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register)\b")
        with (family435.ROOT / "notes/overlays/french-model-variant458-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 12)
        self.assertEqual([row["result"] for row in rows],
                         ["link_error"] + ["mismatch"] * 2 + ["probe_error"] +
                         ["mismatch"] * 2 + ["text_exact"] * 4 + ["matched"] * 2)
        measured = [row for row in rows if row["result"] in ("mismatch", "text_exact")]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in measured],
                         [(1312, 323)] * 2 + [(1340, 41)] * 2 + [(1344, 0)] * 4)
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant458_sheets" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0xC10", str(slot), "gcc_2_8_1_g0_split", "1344", "0"))

    def test_caller_descriptor_and_resident_callees(self):
        root = family435.ROOT
        archive_path = root / "game/france/DATA/MODEL.MRG"
        resident_path = root / "game/france/SLES_039.48"
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
                row = self.instances[module["name"]]
                slot, base = int(row["slot"]), int(module["load_address"], 0)
                self.assertEqual(int(row["command_word"]), 624000)
                self.assertEqual(pointers[5 + slot], base)
                context = pointers[9 + slot]
                self.assertEqual(context, 0x80136000 + slot * 0x40000)
                self.assertEqual(context + 0x1F80, 0x80137F80 + slot * 0x40000)
                for start, size in ((pointers[slot], 96 * 2048),
                                    (pointers[3 + slot], 2 * 2048), (base, 10 * 2048)):
                    self.assertTrue(context + 0x1F80 <= start or start + size <= context)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                call, = struct.unpack_from("<I", data, 0xAB8)
                self.assertEqual(call >> 26, 3)
                self.assertEqual(0x80000000 | ((call & 0x3FFFFFF) << 2), base + 0xC10)
                high, = struct.unpack_from("<I", data, 0x94)
                low, = struct.unpack_from("<I", data, 0xA0)
                self.assertEqual(high >> 16, 0x3C03)
                self.assertEqual(low >> 16, 0x2463)
                immediate = low & 0xFFFF
                self.assertEqual(((high & 0xFFFF) << 16) +
                                 (immediate - 0x10000 if immediate & 0x8000 else immediate),
                                 base + 0x2408)
                self.assertEqual(struct.unpack_from("<5i", data, 0x2408 + 20),
                                 (52, 100, 110, 200, 370))
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

    def test_target_compiler_record_layout(self):
        checks = {
            "sizeof(void *)": 4, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(MATRIX)": 32, "sizeof(GsCOORDINATE2)": 80,
            "sizeof(POLY_GT4)": 52, "sizeof(Variant458Sheet)": 136,
            "sizeof(Variant458Timing)": 40, "sizeof(Variant458View)": 0x1F80,
            "sizeof(((Variant458View *)0)->timing)": 4,
        }
        for typename, fields in (
            ("Variant458Sheet", (("points", 0), ("color", 0x80))),
            ("Variant458Timing", (("scale_start", 20), ("scale_end", 24),
                                  ("move_start", 28), ("move_end", 32), ("fade_start", 36))),
            ("Variant458View", (
                ("sheets", 0xC00), ("quads", 0x1B20), ("reference", 0x1C58),
                ("origins", 0x1C60), ("destinations", 0x1D40), ("factor", 0x1E00),
                ("scale", 0x1E08), ("delta", 0x1E20), ("axis_x", 0x1EF4),
                ("axis_y", 0x1EF8), ("axis_z", 0x1EFC), ("flags", 0x1F0C),
                ("elapsed", 0x1F10), ("step", 0x1F18), ("timing", 0x1F24), ("phase", 0x1F7C))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant458_sheets.h")
