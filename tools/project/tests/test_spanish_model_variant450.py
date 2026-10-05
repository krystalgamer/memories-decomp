import csv
import hashlib
import importlib.util
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
SHARED_RIBBON_SOURCE = ROOT / "src/overlays/french_model_variant/variant450_ribbons.c"
SHARED_BAND_SOURCE = ROOT / "src/overlays/french_model_variant/variant450_bands.c"
SHARED_COIL_SOURCE = ROOT / "src/overlays/french_model_variant/variant450_coils.c"
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

    def test_five_c_owners_and_two_assembly_functions_per_image(self):
        counts = load_spanish_overlay_inventories(ROOT)
        for name, module in self.modules.items():
            slot, base = int(self.instances[name]["slot"]), int(module["load_address"], 0)
            path = ROOT / module["layout"]
            ribbon_source = "src/overlays/french_model_variant/variant450_ribbons"+("_slot1" if slot else "")+".c"
            band_source = "src/overlays/french_model_variant/variant450_bands"+("_slot1" if slot else "")+".c"
            selected_source = "src/overlays/spanish_model_variant/variant450_lines"+("_slot1" if slot else "")+".c"
            quad_source = "src/overlays/spanish_model_variant/variant450_quads"+("_slot1" if slot else "")+".c"
            coil_source = "src/overlays/french_model_variant/variant450_coils"+("_slot1" if slot else "")+".c"
            mapping = json.loads(path.with_name(path.stem+"_matching_c.json").read_text())
            self.assertEqual(mapping, {"schema": 1, "functions": [{
                "address": f"0x{base+0xDD8:X}", "size": "0x9BC",
                "source": ribbon_source, "profile": "gcc_2_8_1_g0_split",
            }, {
                "address": f"0x{base+0x1794:X}", "size": "0x738",
                "source": band_source, "profile": "gcc_2_8_1_g0_split",
            }, {
                "address": f"0x{base+0x27C0:X}", "size": "0x5C8",
                "source": quad_source, "profile": "gcc_2_8_1_g0_split",
            }, {
                "address": f"0x{base+0x2D88:X}", "size": "0x384",
                "source": selected_source, "profile": "gcc_2_8_1_g0_split",
            }, {
                "address": f"0x{base+0x310C:X}", "size": "0x834",
                "source": coil_source, "profile": "gcc_2_8_1_g0_split",
            }]})
            self.assertEqual([row["source"] for row in c_segments(ROOT, path)],
                             [ribbon_source, band_source, quad_source, selected_source, coil_source])
            with path.with_name(path.stem+"_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(row["address"], 0)-base, int(row["size"], 0), row["status"]) for row in rows],
                             [(start, end-start, "matching_c" if start in (0xDD8, 0x1794, 0x27C0, 0x2D88, 0x310C)
                               else "unmatched_asm")
                              for start, end in SPANS])
            segments = yaml.safe_load(path.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            expected = [(0, "data")]+[
                (start, "c" if start in (0xDD8, 0x1794, 0x27C0, 0x2D88, 0x310C) else "asm")
                for start, _ in SPANS
            ]+[(0x3940, "data")]
            self.assertEqual([(row["start"], row["vram"], row["subsegments"][0][:2]) for row in segments[:-1]],
                             [(start, base+start, [start, kind]) for start, kind in expected])
            symbols = path.with_name(path.stem+"_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x3940, 0x16C0)):
                self.assertIn(f"D_{base+offset:X} = 0x{base+offset:X}; // type:u8 size:0x{size:X} defined:true", symbols)
            self.assertEqual((counts[path.stem]["function_count"], counts[path.stem]["matching_c_function_count"],
                              counts[path.stem]["matching_c_bytes"]), (7, 5, 8820))

    def test_attempt_history_and_production_fingerprints(self):
        with (ROOT / "notes/overlays/spanish-model-variant450-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), 30)
        expected = [(920, 226), (920, 226), (872, 223), (932, 221), (888, 162), (916, 209),
                    (900, 14), (900, 14), (900, 14), (896, 191), (896, 190), (900, 29),
                    (896, 189), (896, 191), (896, 189), (900, 10), (900, 2), (900, 2), (900, 2), (900, 0)]
        self.assertEqual([(int(row["instruction_bytes"]), int(row["different_words"])) for row in rows[:20]], expected)
        self.assertEqual([row["result"] for row in rows[:20]], ["mismatch"]*19+["text_exact"])
        for slot, row in enumerate(rows[22:24]):
            source = SOURCE.with_name("variant450_lines"+("_slot1" if slot else "")+".c")
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["slot"], row["function_offset"], row["result"], row["different_words"]),
                             (str(slot), "0x2D88", "matched", "0"))
        shared = (
            (SHARED_RIBBON_SOURCE, "0xDD8", "2492"),
            (SHARED_BAND_SOURCE, "0x1794", "1848"),
            (SHARED_COIL_SOURCE, "0x310C", "2100"),
        )
        for function_index, (source, offset, size) in enumerate(shared):
            for slot in range(2):
                row = rows[24 + function_index * 2 + slot]
                selected = source.with_name(source.stem + ("_slot1" if slot else "") + ".c")
                self.assertEqual(row["fingerprint"], hashlib.sha256(selected.read_bytes()).hexdigest())
                self.assertEqual(
                    (row["slot"], row["function_offset"], row["result"], row["instruction_bytes"],
                     row["different_words"]),
                    (str(slot), offset, "matched", size, "0"),
                )

    def test_target_compiled_coil_views(self):
        from tools.project.tests.test_french_model_variant450 import FrenchModelVariant450Tests

        FrenchModelVariant450Tests.test_target_compiled_coil_views(self)

    def test_coil_compiler_objects_calls_and_raw_owners_when_built(self):
        for module in self.modules.values():
            if not (ROOT / f"tmp/overlays/{module['name']}/build/{module['name']}.elf").is_file():
                self.skipTest("Build Spanish MODEL450 images before checking C owners")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools is required for ELF ownership checks")
        from elftools.elf.elffile import ELFFile

        with (CONFIG / "functions.csv").open() as handle:
            resident = {int(row["address"], 0) for row in csv.DictReader(handle)}
        for module in self.modules.values():
            base = int(module["load_address"], 0)
            directory = ROOT / f"tmp/overlays/{module['name']}"
            image = (ROOT / module["output"]).read_bytes()
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                         if "variant450_coils" in s["source"]]
            obj = directory / "build" / selected["object"]
            self.assertIn(f"{obj.relative_to(ROOT)}(.text);",
                          (directory / f"{module['name']}.ld").read_text())
            with (directory / f"build/{module['name']}.elf").open("rb") as linked_handle, obj.open("rb") as handle:
                linked, compiled = ELFFile(linked_handle), ELFFile(handle)
                final_symbols = linked.get_section_by_name(".symtab")
                object_symbols = compiled.get_section_by_name(".symtab")
                name = f"func_{base+0x310C:X}"
                own, = object_symbols.get_symbol_by_name(name)
                definition, = final_symbols.get_symbol_by_name(name)
                for symbol, elf, address in ((own, compiled, 0), (definition, linked, base + 0x310C)):
                    self.assertIsInstance(symbol["st_shndx"], int)
                    self.assertEqual((symbol["st_value"], symbol["st_size"],
                                      symbol["st_info"]["type"]), (address, 2100, "STT_FUNC"))
                    self.assertTrue(elf.get_section(symbol["st_shndx"])["sh_flags"] & 4)
                section = linked.get_section(definition["st_shndx"])
                start = definition["st_value"] - section["sh_addr"]
                self.assertEqual(section.data()[start:start + 2100], image[0x310C:0x3940])
                text = compiled.get_section(own["st_shndx"]).data()
                relocations = compiled.get_section_by_name(".rel.text")
                self.assertIsNotNone(relocations)
                targets = set()
                for relocation in relocations.iter_relocations():
                    if relocation["r_info_type"] != 4:
                        continue
                    target = object_symbols.get_symbol(relocation["r_info_sym"])
                    if target["st_shndx"] != "SHN_UNDEF":
                        continue
                    resolved, = final_symbols.get_symbol_by_name(target.name)
                    self.assertIn(resolved["st_value"], resident)
                    site = relocation["r_offset"]
                    addend = (struct.unpack_from("<I", text, site)[0] & 0x3FFFFFF) << 2
                    word = struct.unpack_from("<I", image, 0x310C + site)[0]
                    address = ((base + 0x310C + site + 4) & 0xF0000000) | ((word & 0x3FFFFFF) << 2)
                    self.assertEqual(address, resolved["st_value"] + addend, target.name)
                    targets.add(target.name)
                self.assertTrue({"RotTransPers", "GsSortPoly"} <= targets)
                for offset, size in ((0, 4), (0x3940, 5824)):
                    raw, = final_symbols.get_symbol_by_name(f"D_{base+offset:X}")
                    self.assertIsInstance(raw["st_shndx"], int)
                    self.assertEqual(raw["st_value"], base + offset)
                    section = linked.get_section(raw["st_shndx"])
                    self.assertFalse(section["sh_flags"] & 4)
                    start = raw["st_value"] - section["sh_addr"]
                    self.assertEqual(section.data()[start:start + size], image[offset:offset + size])

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

    def test_shared_helper_canonical_bindings_agree_with_splat_symbols(self):
        for module in self.modules.values():
            bindings = (ROOT / module["linker_symbols"]).read_text()
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for canonical, alias, address in (
                ("rcos", "func_spanish_800866F8", 0x800866F8),
                ("rsin", "func_spanish_80086628", 0x80086628),
                ("RotTransPers", "func_spanish_80087868", 0x80087868),
            ):
                self.assertIn(f"{canonical} = 0x{address:X};", bindings)
                self.assertIn(f"{canonical} = 0x{address:X}; // type:func absolute:true", symbols)
                self.assertNotIn(alias + " =", bindings)
                self.assertNotIn(alias + " =", symbols)

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
