import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant373 as family373
from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant452Tests(family435.FrenchModelVariant435Tests):
    family = 452
    module_count = 6
    distinct_images = 6
    binding_count = 34
    tail_start = 0x23E4
    spans = ((4, 0x10E0), (0x10E0, 0x151C), (0x151C, 0x2010), (0x2010, 0x23E4))
    helpers = ((0x10E0, 1084, "sheets", "func_8013C0E0"),
               (0x151C, 2804, "streamers", "func_8013C51C"),
               (0x2010, 980, "lines", "func_8013D010"))
    reachable_helpers = {0x10E0, 0x151C, 0x2010}
    local_call_targets = {0x10E0, 0x151C, 0x2010}
    models_by_stage = ((9, (1, 360, 550)),)
    entry_anchors = {
        0xF5C: 0x02602021, 0xF64: 0x02602021, 0xF7C: 0x02602021,
        0x2010: 0x27BDFED8,
        0x22EC: 0x10400015, 0x2310: 0x1440000B,
        0x151C: 0x27BDFD88,
        0x151C + 0xA4: 0x01030018, 0x151C + 0xB8: 0x00430018,
        0x151C + 0x430: 0x26A80040, 0x151C + 0x434: 0x26A90048,
        0x151C + 0x43C: 0x26B20010, 0x151C + 0x4C8: 0xAE4200F4,
    }

    def test_wrappers_only_rename_verified_functions(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant452_lines_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013D010 func_8017D010\n'
                         '#include "variant452_lines.c"\n')
        body = (directory / "variant452_lines.c").read_text()
        self.assertIn('#include "variant452_lines.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertEqual(body.count("ratan2("), 4)
        self.assertIn("s16 i, j, k;", body)
        with (family435.ROOT / "notes/overlays/french-model-variant452-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 48)
        rows = [row for row in rows if row["function_offset"] == "0x2010"]
        self.assertEqual(len(rows), 10)
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 6 + ["text_exact"] * 2 + ["matched"] * 2)
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in rows[:8]],
                         [(968, 205)] * 2 + [(972, 204)] * 2 + [(980, 2)] * 2 + [(980, 0)] * 2)
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant452_lines" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["slot"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x2010", str(slot), "gcc_2_8_1_g0_split", "980", "0"))

    def test_caller_bindings_and_completion_branch_destinations(self):
        root = family435.ROOT
        path = root / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with (root / "config/sles_03948/functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        bodies = {0: [], 1: []}
        with path.open("rb") as archive:
            for module in self.modules:
                slot = int(self.instances[module["name"]]["slot"])
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for call, target in ((0xF58, 0x10E0), (0xF60, 0x151C), (0xF78, 0x2010)):
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
                self.assertIn("GsSortGLine = 0x800840B8;", bindings)
                self.assertIn("RotTransPers4 = 0x80087958;", bindings)
                symbols = (root / module["layout"]).with_name(
                    (root / module["layout"]).stem + "_symbols.txt").read_text()
                for name, address in (("GsSortPoly", "0x800842A8"),
                                      ("ReadRotMatrix", "0x800872A8"),
                                      ("SetRotMatrix", "0x80087738"),
                                      ("rsin", "0x80086628"), ("rcos", "0x800866F8"),
                                      ("RotTransPers", "0x80087868")):
                    self.assertIn(f"{name} = {address};", bindings)
                    self.assertIn(f"{name} = {address}; // type:func absolute:true", symbols)
                    self.assertNotIn(f"func_french_{address[2:]}", bindings)
                    self.assertNotIn(f"func_french_{address[2:]}", symbols)
                body = data[0x2010:0x23E4]
                bodies[slot].append(body)
                for branch, target in ((0x2DC, 0x334), (0x300, 0x330)):
                    word, = struct.unpack_from("<I", body, branch)
                    self.assertEqual(branch + 4 + (word & 0xFFFF) * 4, target)
                self.assertEqual(struct.unpack_from("<I", body, 0x330), (0xAFA000E8,))
                self.assertEqual(struct.unpack_from("<I", body, 0x334), (0x97AA00E0,))
        for slot, expected in (
            (0, "1855be463b1e995524d5d5c871d67322e6b9b8379a881f4cc74e367fbf805dd4"),
            (1, "b3f9c892287204f7f77620addcb12fc8bdd90c59a0830ab0375dc0c1697264cc"),
        ):
            self.assertEqual(len(bodies[slot]), 3)
            self.assertEqual(len(set(bodies[slot])), 1)
            self.assertEqual(hashlib.sha256(bodies[slot][0]).hexdigest(), expected)

    def test_target_compiler_measured_grid_layout(self):
        checks = {
            "sizeof(void *)": 4, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(MATRIX)": 32, "sizeof(GsCOORDINATE2)": 80,
            "sizeof(GsGLINE)": 20, "sizeof(PSXLONG)": 4,
            "sizeof(Variant452Grid)": 0x260, "sizeof(Variant452LinesView)": 0x33AC,
            "sizeof(((Variant452LinesView *)0)->grid)": 0x720,
        }
        for typename, fields in (
            ("Variant452Grid", (("inner", 0), ("outer", 0x120), ("color", 0x240), ("count", 0x254))),
            ("Variant452LinesView", (
                ("grid", 0), ("line", 0x32CC), ("translation", 0x3308),
                ("delta_x", 0x331C), ("delta_y", 0x3320), ("screen_delta", 0x3338),
                ("delta", 0x333C), ("step", 0x3360), ("phase", 0x33A8))),
            ("GsGLINE", (("x0", 4), ("x1", 8), ("r0", 12), ("r1", 15))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant452_lines.h")

    def test_sheet_source_and_terminal_ledger(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant452_sheets_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C0E0 func_8017C0E0\n'
                         '#include "variant452_sheets.c"\n')
        body = (directory / "variant452_sheets.c").read_text()
        self.assertIn('#include "variant452_sheets.h"', body)
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertIn("ModelVariantSheet *sheet", body)
        self.assertIn("ReadRotMatrix(&local);", body)
        self.assertIn("SetRotMatrix(&local);", body)
        with (family435.ROOT / "notes/overlays/french-model-variant452-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x10E0"]
        self.assertEqual(len(rows), 5)
        self.assertEqual([row["result"] for row in rows],
                         ["text_exact"] * 2 + ["link_error"] + ["matched"] * 2)
        self.assertEqual([row["slot"] for row in rows], ["0", "1", "0", "0", "1"])
        self.assertEqual((rows[2]["instruction_bytes"], rows[2]["different_words"]), ("", ""))
        for name in ("GsSortPoly", "ReadRotMatrix", "SetRotMatrix"):
            self.assertIn(name, rows[2]["reason"])
        for row in rows[:2] + rows[3:]:
            self.assertEqual((row["instruction_bytes"], row["different_words"], row["profile"]),
                             ("1084", "0", "gcc_2_8_1_g0_split"))
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant452_sheets" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())

    def test_target_compiler_measured_sheet_layout(self):
        checks = {
            "sizeof(ModelVariantSheet)": 0x98, "sizeof(Variant452SheetView)": 0x33AC,
            "sizeof(Variant452SheetTiming)": 0x34, "sizeof(POLY_GT4)": 52,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(((Variant452SheetView *)0)->timing)": 4,
        }
        for typename, fields in (
            ("ModelVariantSheet", (("v0", 0), ("v1", 0x20), ("v2", 0x40), ("v3", 0x60),
                                   ("outer", 0x80), ("inner", 0x84), ("size", 0x88))),
            ("Variant452SheetTiming", (("scale_start", 0x1C), ("scale_end", 0x20),
                                       ("fade_start", 0x2C), ("fade_end", 0x30))),
            ("Variant452SheetView", (
                ("sheets", 0x2AC8), ("poly", 0x3204), ("translation", 0x3308),
                ("flags", 0x3354), ("elapsed", 0x3358), ("step", 0x3360),
                ("timing", 0x3374), ("field3394", 0x3394), ("phase", 0x33A8))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant452_sheets.h")

    def test_streamer_source_and_terminal_ledger(self):
        directory = family435.ROOT / "src/overlays/french_model_variant"
        self.assertEqual((directory / "variant452_streamers_slot1.c").read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C51C func_8017C51C\n'
                         '#include "variant452_streamers.c"\n')
        body = (directory / "variant452_streamers.c").read_text()
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register|volatile)\b")
        self.assertIn("PSXLONG flags[16][5];", body)
        self.assertIn("point = (Variant452Streamer *)((s32 *)streamer + 4);", body)
        self.assertIn("previous, last, previous, last,", body)
        self.assertEqual(body.count("GsSortPoly("), 2)
        self.assertEqual(body.count("flags[i][j] = 0;"), 2)
        with (family435.ROOT / "notes/overlays/french-model-variant452-attempts.csv").open() as handle:
            rows = [row for row in csv.DictReader(handle) if row["function_offset"] == "0x151C"]
        self.assertEqual(len(rows), 33)
        self.assertEqual([row["result"] for row in rows],
                         ["mismatch"] * 28 + ["text_exact"] * 2 + ["link_error"] + ["matched"] * 2)
        self.assertEqual((rows[30]["instruction_bytes"], rows[30]["different_words"]), ("", ""))
        for name in ("rcos", "rsin", "RotTransPers"):
            self.assertIn(name, rows[30]["reason"])
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"]))
                          for row in rows[24:30]],
                         [(2804, 3)] * 2 + [(2776, 467)] * 2 + [(2804, 0)] * 2)
        for slot, row in enumerate(rows[-2:]):
            source = directory / ("variant452_streamers" + ("_slot1" if slot else "") + ".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["profile"], row["instruction_bytes"],
                              row["different_words"]),
                             (str(slot), "gcc_2_8_1_g0_split", "2804", "0"))

    def test_target_compiler_measured_streamer_layout(self):
        checks = {
            "sizeof(Variant452Streamer)": 0x130,
            "sizeof(Variant452StreamersView)": 0x33AC,
            "sizeof(ModelVariantSheet)": 0x98, "sizeof(POLY_GT4)": 52,
            "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4,
            "sizeof(((Variant452StreamersView *)0)->streamers)": 0x1300,
            "sizeof(((Variant452Streamer *)0)->a)": 40,
        }
        for typename, fields in (
            ("Variant452Streamer", (("a", 0x28), ("sa", 0x50), ("angle", 0x64),
                                    ("b", 0x78), ("sb", 0xA0), ("width", 0xB4),
                                    ("inner", 0xC8), ("outer", 0xDC), ("depth", 0xF4),
                                    ("ox", 0x108), ("oy", 0x112))),
            ("Variant452StreamersView", (
                ("streamers", 0x720), ("sheet", 0x2AC8), ("poly", 0x31D0),
                ("translation", 0x3308), ("delta", 0x333C), ("flags", 0x3354),
                ("step", 0x3360), ("rotation", 0x3368), ("field336A", 0x336A),
                ("field336C", 0x336C), ("radius", 0x3398), ("phase", 0x33A8))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        for field, value in (("sa", 0x60), ("angle", 0x74), ("sb", 0xB0),
                             ("width", 0xC4), ("depth", 0x104)):
            checks[f"16 + (u32)&((Variant452Streamer *)0)->{field}[0]"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant452_streamers.h")
