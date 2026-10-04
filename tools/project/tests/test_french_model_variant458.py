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
    helpers = ((0xC10, 1344, "sheets", "func_8013BC10"),
               (0x19EC, 2336, "streamers", "func_8013C9EC"))
    reachable_helpers = {0xC10, 0x19EC}
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
        0x340: 0x0C020BAA, 0x344: 0x02A02021, 0x3A0: 0x26B50028,
        0x3A4: 0x0C020BAA, 0x3A8: 0x02A02021,
        0x6A0: 0xA06501D4, 0x6A4: 0xA06501D5, 0x6A8: 0xA06501D6,
        0x6B0: 0x2882000D, 0x6B8: 0x24630004, 0x6C4: 0x28E20003,
        0x6CC: 0x27180274, 0xA8C: 0x8FA400D0, 0xA94: 0,
        0x19EC: 0x27BDFE10,
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
        self.assertEqual(len(rows), 29)
        self.assertEqual({row["function_offset"] for row in rows}, {"0xC10", "0x19EC"})
        rows = [row for row in rows if row["function_offset"] == "0xC10"]
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
                for call_offset, helper_offset in ((0xAB8, 0xC10), (0xA90, 0x19EC)):
                    call, = struct.unpack_from("<I", data, call_offset)
                    self.assertEqual(call >> 26, 3)
                    self.assertEqual(0x80000000 | ((call & 0x3FFFFFF) << 2),
                                     base + helper_offset)
                    self.assertEqual(struct.unpack_from("<I", data, call_offset + 4), (0,))
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
                for name, address in (("rsin", "0x80086628"), ("rcos", "0x800866F8"),
                                      ("RotTransPers", "0x80087868")):
                    self.assertIn(f"{name} = {address};", bindings)
                    symbols = (root / module["layout"]).with_name(
                        (root / module["layout"]).stem + "_symbols.txt").read_text()
                    self.assertIn(f"{name} = {address}; // type:func absolute:true", symbols)

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

    def test_streamer_sources_and_attempts(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant458_streamers_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C9EC func_8017C9EC\n'
                         '#include "variant458_streamers.c"\n')
        body = (directory / "variant458_streamers.c").read_text()
        self.assertIn('#include "variant458_streamers.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertIn("PSXLONG flags[3][13];", body)
        self.assertIn("work->flags % 4 == 1", body)
        self.assertIn("work->flags % 10 == 0", body)
        self.assertIn("for (j = 0; j < 7; j++)", body)
        self.assertEqual(body.count("streamer->ox[k] ="), 2)
        self.assertEqual(body.count("streamer->oy[k] ="), 2)
        self.assertNotIn("poly++", body)
        with (family435.ROOT / "notes/overlays/french-model-variant458-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x19EC"]
        self.assertEqual(len(rows), 17)
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 10 + ["text_exact"] * 4 + ["link_error"] + ["matched"] * 2)
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in rows[:14]],
                         [(2324, 553)] * 2 + [(2332, 176)] * 2 +
                         [(2332, 149)] * 4 + [(2340, 228)] * 2 + [(2336, 0)] * 4)
        self.assertEqual((rows[14]["slot"], rows[14]["instruction_bytes"],
                          rows[14]["different_words"]), ("0", "", ""))
        self.assertIn("RotTransPers", rows[14]["reason"])
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant458_streamers" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"],
                              row["different_words"]),
                             (str(slot), "gcc_2_8_1_g0_split", "2336", "0"))

    def test_target_compiler_streamer_layout(self):
        checks = {
            "sizeof(Variant458Streamer)": 0x274,
            "sizeof(Variant458StreamerView)": 0x1F74,
            "sizeof(POLY_FT4)": 40, "sizeof(CVECTOR)": 4,
            "sizeof(((Variant458Streamer *)0)->color)": 52,
            "sizeof(((Variant458StreamerView *)0)->streamers)": 3 * 0x274,
        }
        for typename, fields in (
            ("Variant458Streamer", (
                ("a", 0), ("sa", 0x68), ("angle", 0x9C), ("b", 0xD0),
                ("sb", 0x138), ("width", 0x16C), ("color", 0x1D4),
                ("depth", 0x20C), ("ox", 0x240), ("oy", 0x25A))),
            ("Variant458StreamerView", (
                ("streamers", 0x106C), ("quads", 0x1BD8), ("origins", 0x1C60),
                ("factor", 0x1E00), ("scale", 0x1E08), ("deltas", 0x1E20),
                ("axis_x", 0x1EF4), ("axis_y", 0x1EF8), ("axis_z", 0x1EFC),
                ("flags", 0x1F0C), ("length", 0x1F64), ("rotation_x", 0x1F68),
                ("rotation_y", 0x1F6C), ("rotation_z", 0x1F70))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant458_streamers.h")
