import csv
import hashlib
import json
from pathlib import Path
import re
import struct
import sys
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from tools.project.overlay_sources import c_segments
from tools.project.progress import load_spanish_overlay_inventories
from tools.project.tests import test_french_model_variant373 as layouts
from tools.project.verify_inputs import load_checksum_manifest

CONFIG = ROOT / "config/sles_03951"
SOURCE = ROOT / "src/overlays/spanish_model_variant/variant450_lines.c"
SPANS = ((4, 0xDD8), (0xDD8, 0x1794), (0x1794, 0x1ECC), (0x1ECC, 0x27C0),
         (0x27C0, 0x2D88), (0x2D88, 0x310C), (0x310C, 0x3940))
IMAGES = (
    (9, 0, 48224, "b2c0697e759ecfe1e5d746ca7ad4bc29bcd3719c05893056acb877274dc6eac8"),
    (10, 1, 48234, "770befcc07901cfa9da9573421c5713062488ea83f6df218a25fe302d5c091b7"),
)


class SpanishModelVariant450Tests(unittest.TestCase):
    def setUp(self):
        with (ROOT / "notes/overlays/spanish-model-variant450-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.modules = {row["name"]: row for row in json.loads(
            (CONFIG / "overlays.json").read_text())["modules"] if row["name"] in self.instances}

    def test_distinct_loader_slices(self):
        expected = {f"spanish_model_variant_174_stage{stage}_slot{slot}" for stage, slot, *_ in IMAGES}
        self.assertEqual(set(self.instances), expected)
        self.assertEqual(set(self.modules), expected)
        checksums = load_checksum_manifest(CONFIG / "files.sha256")
        for stage, slot, sector, digest in IMAGES:
            name = f"spanish_model_variant_174_stage{stage}_slot{slot}"
            module, row = self.modules[name], self.instances[name]
            self.assertEqual(module["archive"], "game/spain/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{0x8013B000+slot*0x40000:X}")
            self.assertEqual((module["sha256"], row["sha256"]), (digest, digest))
            self.assertEqual(tuple(int(row[key]) for key in ("model", "record", "stage", "slot", "header", "command_word")),
                             (174, 174, stage, slot, 450+slot*150, 616000))
            self.assertEqual(sector, 174*276+180+(stage-7)*10)

    def test_one_c_owner_and_six_assembly_functions_per_image(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for name, module in self.modules.items():
            slot, base = int(self.instances[name]["slot"]), int(module["load_address"], 0)
            path = ROOT / module["layout"]
            selected_source = "src/overlays/spanish_model_variant/variant450_lines"+("_slot1" if slot else "")+".c"
            mapping = json.loads(path.with_name(path.stem+"_matching_c.json").read_text())
            self.assertEqual(mapping, {"schema": 1, "functions": [{
                "address": f"0x{base+0x2D88:X}", "size": "0x384",
                "source": selected_source, "profile": "gcc_2_8_1_g0_split",
            }]})
            self.assertEqual([row["source"] for row in c_segments(ROOT, path)], [selected_source])
            with path.with_name(path.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(row["address"], 0)-base, int(row["size"], 0), row["status"]) for row in rows],
                             [(start, end-start, "matching_c" if start == 0x2D88 else "unmatched_asm")
                              for start, end in SPANS])
            segments = yaml.safe_load(path.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            expected = [(0, "data")]+[(start, "c" if start == 0x2D88 else "asm") for start, _ in SPANS]+[(0x3940, "data")]
            self.assertEqual([(row["start"], row["vram"], row["subsegments"][0][:2]) for row in segments[:-1]],
                             [(start, base+start, [start, kind]) for start, kind in expected])
            symbols = path.with_name(path.stem+"_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x3940, 0x16C0)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true", symbols)
            self.assertEqual((counts[path.stem]["function_count"], counts[path.stem]["matching_c_function_count"],
                              counts[path.stem]["matching_c_bytes"]), (7, 1, 900))

    def test_attempt_history_and_production_fingerprints(self):
        with (ROOT / "notes/overlays/spanish-model-variant450-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 24)
        expected = [(920, 226), (920, 226), (872, 223), (932, 221), (888, 162), (916, 209),
                    (900, 14), (900, 14), (900, 14), (896, 191), (896, 190), (900, 29),
                    (896, 189), (896, 191), (896, 189), (900, 10), (900, 2), (900, 2), (900, 2), (900, 0)]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"])) for row in rows[:20]], expected)
        self.assertEqual([row["result"] for row in rows[:20]], ["mismatch"]*19+["text_exact"])
        for slot, row in enumerate(rows[-2:]):
            source = SOURCE.with_name("variant450_lines"+("_slot1" if slot else "")+".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["function_offset"], row["result"], row["different_words"]),
                             (str(slot), "0x2D88", "matched", "0"))

    def test_source_preserves_calls_aliases_and_natural_inductions(self):
        source = SOURCE.read_text()
        self.assertNotRegex(source, r"\b(?:asm|__asm__|register|volatile)\b")
        self.assertEqual(source.count("ratan2("), 4)
        self.assertIn("primary = primary_base + primary_index;", source)
        self.assertIn("fade = fade_base + fade_index;", source)
        self.assertIn("depth >= 0 && flag >= 0 && primary->threshold >= 1024", source)
        self.assertIn("work->field_42F4 += work->field_42B4 * 4;", source)
        self.assertEqual(source.count("(PSXLONG *)&line->x0"), 2)
        self.assertEqual(SOURCE.with_name("variant450_lines_slot1.c").read_text(),
                         '#include "../../types.h"\n#define func_8013DD88 func_8017DD88\n#include "variant450_lines.c"\n')

    def test_retail_slices_caller_and_helper_imports(self):
        archive_path = ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        expected_imports = {0x8005C018, 0x800840B8, 0x80085558, 0x80086258, 0x800872A8,
                            0x800875F8, 0x80087738, 0x80087898, 0x80087CB8, 0x80089928}
        with archive_path.open("rb") as archive:
            for name, module in self.modules.items():
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"]*2048)
                image = archive.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<2I", image, 0xC34),
                                 (0x0C000000 | ((base+0x2D88) >> 2 & 0x3FFFFFF), 0x02602021))
                self.assertEqual(struct.unpack_from("<I", image, 0x2D88)[0], 0x27BDFEE0)
                words = struct.unpack("<225I", image[0x2D88:0x310C])
                calls = {0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3}
                self.assertEqual(calls, expected_imports)
                bindings = {int(value, 16) for value in re.findall(
                    r"= (0x[0-9A-F]+);", (ROOT / module["linker_symbols"]).read_text())}
                self.assertEqual(len(bindings), 35)
                self.assertTrue(calls <= bindings)
            archive.seek((174*276+275)*2048+0x110)
            self.assertEqual(struct.unpack("<3i", archive.read(12)), (70001, 616000, -2))

    def test_target_compiled_partial_view_layout(self):
        checks = {"sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
                  "sizeof(GsCOORDINATE2)": 80, "sizeof(GsGLINE)": 20,
                  "sizeof(Variant450Group)": 104, "sizeof(Variant450Primary)": 536,
                  "sizeof(Variant450Fade)": 160, "sizeof(Variant450View)": 0x42F8}
        for typename, fields in (
            ("Variant450Group", (("color", 72),)),
            ("Variant450Primary", (("translation", 40),)),
            ("Variant450Fade", (("fading", 16), ("brightness", 20))),
            ("GsCOORDINATE2", (("coord", 4), ("super", 72))),
            ("GsGLINE", (("x0", 4), ("x1", 8), ("r0", 12), ("r1", 15))),
            ("Variant450View", (("groups", 0), ("primary", 0x440), ("fade", 0x36C0), ("line", 0x4240),
                                ("field_4280", 0x4280), ("field_4284", 0x4284), ("field_428C", 0x428C),
                                ("field_428E", 0x428E), ("field_4290", 0x4290), ("field_4294", 0x4294),
                                ("field_4298", 0x4298), ("field_42A8", 0x42A8),
                                ("field_42B4", 0x42B4), ("field_42F4", 0x42F4))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 33)
        layouts.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "../spanish_model_variant/variant450_lines.h")
