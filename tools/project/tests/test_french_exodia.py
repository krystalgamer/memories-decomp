import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import struct
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


class FrenchExodiaTests(unittest.TestCase):
    functions = (
        ((4, 2140, "entry"), (0x860, 940, "ring"), (0xC0C, 2976, "spokes")),
        ((4, 2476, "entry_second"), (0x9B0, 1088, "ring_second"),
         (0xDF0, 2416, "petals"), (0x1760, 1500, "beam")),
    )

    def modules(self):
        manifest = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())
        return [m for m in manifest["modules"] if m["name"].startswith("french_exodia_")]

    def test_special_su_slices_are_not_model_records(self):
        modules = self.modules()
        self.assertEqual([m["name"] for m in modules], ["french_exodia_slot0", "french_exodia_slot1"])
        hashes = load_checksum_manifest(ROOT / "config/sles_03948/files.sha256")
        for slot, module in enumerate(modules):
            self.assertEqual(module["archive"], "game/france/DATA/SU.MRG")
            self.assertEqual(module["archive_sha256"], hashes[module["archive"]])
            self.assertEqual(module["sector_offset"], 1686 + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
        self.assertEqual(len({m["sha256"] for m in modules}), 2)

    def test_only_exact_functions_select_c(self):
        c_bytes = assembly_bytes = 0
        for slot, module in enumerate(self.modules()):
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            expected = [
                dict(address=f"0x{base+offset:X}", profile="gcc_2_8_1_g0_split",
                     size=f"0x{size:X}", source=f"src/overlays/model_exodia/{stem}.c")
                for offset, size, stem in self.functions[slot] if stem
            ]
            manifest = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(manifest["functions"], expected)
            self.assertEqual([s["source"] for s in c_segments(ROOT, layout)],
                             [s["source"] for s in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(len(rows), len(self.functions[slot]))
            for row, (offset, size, stem) in zip(rows, self.functions[slot]):
                self.assertEqual(int(row["address"], 0), base + offset)
                self.assertEqual(int(row["size"], 0), size)
                self.assertEqual(row["status"], "matching_c" if stem else "unmatched_asm")
                if stem:
                    c_bytes += size
                else:
                    assembly_bytes += size
                    self.assertIn(f"asm, overlays/{module['name']}/func_{base+offset:X}", layout.read_text())
        self.assertEqual((c_bytes, assembly_bytes), (13536, 0))

    def test_headers_and_unknown_tails_have_real_owners(self):
        bindings = (ROOT / "config/sles_03948/overlays/exodia_linker_symbols.txt").read_text()
        for slot, module in enumerate(self.modules()):
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            tail = 0x17AC if slot == 0 else 0x1D3C
            for offset, size in ((0, 4), (tail, 20480-tail)):
                name = f"D_{base+offset:X}"
                self.assertIn(f"{name} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true", symbols)
                self.assertNotIn(f"{name} =", bindings)
            self.assertIn("data, overlays/" + module["name"] + "/unclassified_tail", layout.read_text())
            self.assertIn("[0x5000]", layout.read_text())

    def test_bindings_are_resident_and_sources_obey_contracts(self):
        bindings = (ROOT / "config/sles_03948/overlays/exodia_linker_symbols.txt").read_text()
        addresses = [int(value, 0) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)]
        self.assertEqual(len(addresses), 34)
        self.assertIn("Model_CopySlotU16Values = 0x8005C0B8;", bindings)
        self.assertIn("SetPolyF4 = 0x80082E88;", bindings)
        self.assertIn("func_80059B90 = 0x8005CC98;", bindings)
        self.assertIn("func_8005B260 = 0x8004D5B8;", bindings)
        with (ROOT / "config/sles_03948/functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        self.assertTrue(all(address in resident and address < 0x80100000 for address in addresses))
        directory = ROOT / "src/overlays/model_exodia"
        self.assertEqual({p.name for p in directory.glob("*.c")},
                         {"entry.c", "entry_second.c", "ring.c", "ring_second.c", "beam.c", "spokes.c", "petals.c"})
        for source in directory.glob("*.c"):
            text = source.read_text()
            self.assertIn('#include "../../types.h"', text)
            self.assertNotRegex(text, r"\b(?:asm|__asm__|extern)\b")

    def test_progress_counts_only_inventoried_exact_functions(self):
        inventories = load_french_overlay_inventories(ROOT)
        for slot, expected in enumerate(((3, 6056), (4, 7480))):
            counts = inventories[f"exodia_slot{slot}"]
            self.assertEqual((counts["matching_c_function_count"], counts["matching_c_bytes"]), expected)

    def test_spoke_recovery_keeps_packed_coordinates_and_honest_attempts(self):
        source = (ROOT / "src/overlays/model_exodia/spokes.c").read_text()
        self.assertIn("s32 projected[16][2];", source)
        self.assertIn("s32 displaced_projected[16][2];", source)
        self.assertIn("s32 x_delta, y_delta;", source)
        self.assertIn("ratan2(y_delta, x_delta)", source)
        self.assertIn("for (j = 0; j < 1; j++)", source)
        with (ROOT / "notes/overlays/exodia-helpers-attempts.csv").open() as handle:
            attempts = [r for r in csv.DictReader(handle) if r["function"] == "func_8013BC0C"]
        self.assertEqual([r["attempt"] for r in attempts], ["01", "02", "03", "04", "05", "06"])
        self.assertEqual([r["result"] for r in attempts], ["nonmatching"] * 5 + ["matched"])
        self.assertEqual((attempts[-1]["instruction_bytes"], attempts[-1]["different_words"]), ("2976", "0"))

    def test_entry_recovery_preserves_coordinate_widths_and_attempts(self):
        source = (ROOT / "src/overlays/model_exodia/entry.c").read_text()
        header = (ROOT / "src/overlays/model_exodia/entry.h").read_text()
        self.assertIn("s16 dx;", source)
        self.assertIn("s32 dy;", source)
        self.assertIn("dx = projection.target.vx - screen_x;", source)
        self.assertIn("dy = projection.target.vy - screen_y;", source)
        self.assertIn("DVECTOR projected;", header)
        self.assertIn("DVECTOR target;", header)
        self.assertIn("ExodiaEntryConfig *G32 config;", header)
        self.assertIn("GsCOORDUNIT *G32 parts[3];", header)
        with (ROOT / "notes/overlays/exodia-helpers-attempts.csv").open() as handle:
            attempts = [r for r in csv.DictReader(handle) if r["function"] == "func_8013B004"]
        self.assertEqual([r["attempt"] for r in attempts], [f"{n:02d}" for n in range(1, 20)])
        self.assertEqual([r["result"] for r in attempts],
                         ["unverified_binding"] + ["nonmatching"] * 17 + ["matched"])
        self.assertEqual((attempts[-1]["instruction_bytes"], attempts[-1]["different_words"]), ("2140", "0"))

    def test_petals_template_and_shared_layout_preserve_target_structure(self):
        source = (ROOT / "src/overlays/model_exodia/petals.c").read_text()
        header = (ROOT / "src/overlays/model_exodia/petals.h").read_text()
        entry = (ROOT / "src/overlays/model_exodia/entry_second.h").read_text()
        self.assertIn('#include "../model_variant/variant418_spiral.h"', header)
        self.assertIn('#include "petals.h"', entry)
        self.assertNotIn("void func_8017BDF0", entry)
        self.assertIn("void func_8017BDF0(u8 *context);", header)
        self.assertIn("s16 i;", source)
        self.assertIn("s16 k;", source)
        self.assertIn("if (radius * MODEL_VARIANT_HALF(work, 0xC8E) < 0)", source)
        self.assertIn("if (eighth * MODEL_VARIANT_HALF(work, 0xC8E) < 0)", source)
        self.assertLess(source.index("poly ="), source.index("radius ="))
        self.assertIn("arm->a[k]", source)
        self.assertIn("&p, &arm->flag[k]", source)
        self.assertEqual(source.count("arm->otz[k] = 0;"), 2)
        self.assertEqual(source.count("arm->flag[k] = 0;"), 2)
        self.assertEqual(source.count("GsSortPoly(poly, ot, arm->otz[k] & 0xFFFF);"), 2)
        self.assertIn("for (k = 0; k < 1; k++)", source)
        with (ROOT / "notes/overlays/exodia-helpers-attempts.csv").open() as handle:
            attempts = [r for r in csv.DictReader(handle) if r["function"] == "func_8017BDF0"]
        self.assertEqual([r["attempt"] for r in attempts], [f"{n:02d}" for n in range(1, 11)])
        self.assertEqual([r["result"] for r in attempts], ["nonmatching"] * 9 + ["matched"])
        self.assertEqual([(r["instruction_bytes"], r["different_words"]) for r in attempts[-3:]],
                         [("2368", "553"), ("2420", "584"), ("2416", "0")])

    def test_petals_retail_storage_and_template_anchors(self):
        image = self.retail_image(1)
        anchors = {
            0xDF0: 0x27BDFED0, 0xE20: 0xAFA400D8,
            0xE44: 0x25350438, 0xE4C: 0x25330AC0,
            0xE74: 0x01020018, 0xE7C: 0x00004812,
            0xE88: 0x00620018, 0xE8C: 0x00004012,
            0x10F8: 0x27BE00D0, 0x114C: 0x27A40080,
            0x11F4: 0x26A20068, 0x1200: 0xAFBE0020, 0x1204: 0xAFA20024,
            0x1214: 0x26A40038, 0x1218: 0x26A50044, 0x1220: 0x27A700D4,
            0x1228: 0xAE42006C, 0x124C: 0xAE420028, 0x1264: 0xAE420048,
            0x1280: 0xA5220074, 0x12AC: 0xA5220078,
            0x12E4: 0x26020064, 0x1320: 0xAE02006C,
            0x13C0: 0x28420002, 0x13E8: 0x2842000C, 0x13F0: 0x26B50084,
            0x14B0: 0x92020058, 0x14F8: 0x92020050,
            0x1540: 0xAE00006C, 0x154C: 0x04400005, 0x1550: 0xAE000064,
            0x1554: 0x9606006C, 0x1674: 0xAE00006C, 0x1684: 0xAE000064,
            0x16A4: 0x1840FF59, 0x16C4: 0x2842000C, 0x16CC: 0x26B50084,
            0x16D8: 0x8D020C6C, 0x16F4: 0xA5030C8C,
            0x1714: 0xA5020C90, 0x172C: 0xA5020C90,
            0x6CC: 0x0C05EF7C, 0x6D0: 0x02402021,
        }
        for offset, word in anchors.items():
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        self.assertEqual(0x438 + 12 * 0x84, 0xA68)
        self.assertEqual(0xAC0 + 52, 0xAF4)
        self.assertEqual(0x80 + 80, 0xD0)
        self.assertEqual(0xD4 + 4, 0xD8)
        self.assertLessEqual(0xC92 + 2, 0xCBC)

    def retail_image(self, slot):
        module = self.modules()[slot]
        archive = ROOT / module["archive"]
        if not archive.is_file():
            self.skipTest("Legally obtained French SU archive is unavailable")
        digest = hashlib.sha256()
        with archive.open("rb") as handle:
            for chunk in iter(lambda: handle.read(1024 * 1024), b""):
                digest.update(chunk)
            self.assertEqual(digest.hexdigest(), module["archive_sha256"])
            handle.seek(module["sector_offset"] * 2048)
            image = handle.read(module["sector_count"] * 2048)
        self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
        return image

    def test_entry_retail_anchors_and_observed_configuration(self):
        image = self.retail_image(0)
        anchors = {
            0x4: 0x27BDFF50, 0x24: 0x269601C0, 0x34: 0x26950298,
            0x64: 0x007E1823, 0x68: 0x000318C0, 0x80: 0xAE830464,
            0x62C: 0x97B00070, 0x630: 0x87B10072,
            0x674: 0x87A3007E, 0x678: 0x00402021, 0x67C: 0x97A2007C,
            0x680: 0x00711823, 0x684: 0xA6830436, 0x688: 0x00501023,
            0x68C: 0xA6820434, 0x780: 0x0C04EE18, 0x7B8: 0x0C04EF03,
            0x85C: 0x27BD00B0,
        }
        for offset, word in anchors.items():
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        self.assertEqual(list(image[0x17AC + 16:0x17AC + 19]), [3, 0, 0])
        self.assertEqual(struct.unpack_from("<5I", image, 0x17AC + 28),
                         (540, 570, 576, 716, 1994))

    def test_second_entry_keeps_initialization_order_and_uncertain_fields(self):
        source = (ROOT / "src/overlays/model_exodia/entry_second.c").read_text()
        header = (ROOT / "src/overlays/model_exodia/entry_second.h").read_text()
        self.assertLess(source.index("beam->field_278 = 0;"), source.index("beam->field_27C = 0;"))
        self.assertLess(source.index("beam->field_27C = 0;"), source.index("beam->field_27A = 0;"))
        self.assertIn("GetTPage(1, 1, 896, 0);", source)
        self.assertIn("GetClut(640, 244);", source)
        self.assertIn("SetPolyF4(flat);", source)
        self.assertIn("func_80059B90(s16 value, s16 *out);", header)
        self.assertIn("ExodiaSecondEntryConfig *G32 config;", header)
        self.assertIn("GsCOORDUNIT *G32 parts[3];", header)
        with (ROOT / "notes/overlays/exodia-helpers-attempts.csv").open() as handle:
            attempts = [r for r in csv.DictReader(handle) if r["function"] == "func_8017B004"]
        self.assertEqual([r["attempt"] for r in attempts], ["01", "02"])
        self.assertEqual([r["result"] for r in attempts], ["nonmatching", "matched"])
        self.assertEqual([(r["instruction_bytes"], r["different_words"]) for r in attempts],
                         [("2476", "10"), ("2476", "0")])

    def test_second_entry_retail_anchors_and_observed_configuration(self):
        image = self.retail_image(1)
        anchors = {
            0x4: 0x27BDFF50, 0x1C: 0x26960308, 0x24: 0x26970438, 0x2C: 0x26950AC0,
            0x34: 0x269E0A68, 0x3C: 0x26900A80, 0x44: 0x26910A9C,
            0x80: 0x00181900, 0x84: 0x00781821, 0x88: 0x00031880,
            0x194: 0x0C020BA2, 0x280: 0x2702027A, 0x2A4: 0xAC400002,
            0x2A8: 0xA4400000, 0x330: 0x26F70084, 0x4B0: 0x26D60098,
            0x618: 0x87A3007E, 0x61C: 0x97A2007C, 0x624: 0xA6830C46,
            0x630: 0xA6820C44, 0x6A4: 0x0C05EE6C, 0x6CC: 0x0C05EF7C,
            0x82C: 0x0C05F1D8, 0x8A8: 0x0C01356E, 0x9AC: 0x27BD00B0,
        }
        for offset, word in anchors.items():
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        self.assertEqual(list(image[0x1D3C + 4:0x1D3C + 7]), [34, 0, 0])
        self.assertEqual(struct.unpack_from("<13I", image, 0x1D3C + 16),
                         (60, 280, 350, 400, 460, 480, 540, 560, 620, 640, 644, 720, 760))


if __name__ == "__main__":
    unittest.main()
