import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import struct
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_function_inventory import walk_function
from overlay_sources import c_segments
from progress import load_french_overlay_inventories
from verify_inputs import load_checksum_manifest


class FrenchModelVariant393Tests(unittest.TestCase):
    instances = (
        (190, 190, 9, 2), (217, 217, 9, 0), (221, 221, 9, 1),
        (296, 296, 9, 7), (427, 377, 7, 6), (457, 407, 9, 3),
        (458, 408, 7, 4), (459, 409, 7, 5), (598, 548, 9, 2),
        (612, 562, 9, 0), (647, 597, 9, 7),
    )
    spans = ((4, 3544), (0xDDC, 2344), (0x1704, 1160), (0x1B8C, 2084))
    anchors = {
        0xC: 0x00809821, 0x14: 0x0260B021, 0x1C: 0x26D203A4,
        0x20: 0x26D804D4, 0x68: 0x04A0021D, 0x78: 0xA6C20E08,
        0x84: 0x8FB800D4, 0x8C: 0x00181900, 0x90: 0x00781821,
        0x94: 0x00031880, 0x98: 0x00621821, 0x9C: 0xAEC30DB4,
        0x80C: 0x26520098, 0x820: 0x2A220002, 0x828: 0x24630098,
        0xC38: 0x02602021,
        0x1704: 0x27BDFEF8, 0x170C: 0x00809021, 0x1714: 0x265503A4,
        0x1744: 0x8E420DC8, 0x1750: 0x26510C38, 0x1754: 0x241E0002,
        0x1764: 0x2650042C, 0x1768: 0x8E420DA0,
        0x17A4: 0x8E420D30, 0x17B0: 0x8E420D34, 0x17BC: 0x8E420D38,
        0x17C8: 0x86420D3C, 0x17D4: 0x86420D3E, 0x17E0: 0x86420D40,
        0x19FC: 0x8C640024, 0x1A00: 0x8C630028, 0x1A44: 0xAE420DF8,
        0x1A48: 0x8E450DB4, 0x1A4C: 0x8E430DA4, 0x1A50: 0x8CA40030,
        0x1A74: 0x8CA20034, 0x1AE4: 0x8E420DAC, 0x1AEC: 0x000212C0,
        0x1B2C: 0x8E420DAC, 0x1B34: 0x00021200,
        0x1B4C: 0x26100098, 0x1B58: 0x26B50098,
    }

    def setUp(self):
        manifest = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())
        self.modules = {row["name"]: row for row in manifest["modules"]}

    def selected(self):
        for model, record, stage, selector in self.instances:
            for slot in (0, 1):
                name = f"french_model_variant_{model}_stage{stage + slot}_slot{slot}"
                yield self.modules[name], model, record, stage, selector, slot

    def test_independent_loader_slices_and_instance_ledger(self):
        checksums = load_checksum_manifest(ROOT / "config/sles_03948/files.sha256")
        selected = list(self.selected())
        self.assertEqual(len(selected), 22)
        self.assertEqual(len({module["sha256"] for module, *_ in selected}), 22)
        with (ROOT / "notes/overlays/french-model-variant393-instances.csv").open() as handle:
            instances = {row["module"]: row for row in csv.DictReader(handle)}
        self.assertEqual(set(instances), {module["name"] for module, *_ in selected})
        for module, model, record, stage, selector, slot in selected:
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"], checksums[module["archive"]])
            self.assertEqual(module["sector_offset"], record * 276 + (180 if stage == 7 else 200) + slot * 10)
            self.assertEqual(module["sector_count"], 10)
            self.assertEqual(int(module["load_address"], 0), 0x8013B000 + slot * 0x40000)
            self.assertNotIn("duplicate_sector_offsets", module)
            row = instances[module["name"]]
            self.assertEqual((int(row["model"]), int(row["record"]), int(row["stage"]),
                              int(row["slot"]), int(row["command_word"]), int(row["header"])),
                             (model, record, stage + slot, slot, 559000 + selector, 393 + slot * 150))
            self.assertEqual(row["sha256"], module["sha256"])
            self.assertEqual(int(row["sector_offset"]), module["sector_offset"])
            self.assertEqual(row["load_address"], module["load_address"])

    def test_entry_and_rings_are_c_and_raw_regions_have_real_storage(self):
        counts = load_french_overlay_inventories(ROOT)
        for module, *_, slot in self.selected():
            base = int(module["load_address"], 0)
            layout = ROOT / module["layout"]
            source = f"src/overlays/french_model_variant/variant393_rings{'_slot1' if slot else ''}.c"
            entry = f"src/overlays/french_model_variant/variant393_entry{'_slot1' if slot else ''}.c"
            expected = [
                {"address": f"0x{base + 4:X}", "size": "0xDD8",
                 "profile": "gcc_2_8_1_g0_split", "source": entry},
                {"address": f"0x{base + 0x1704:X}", "size": "0x488",
                 "profile": "gcc_2_8_1_g0_split", "source": source},
            ]
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": expected})
            self.assertEqual([row["source"] for row in c_segments(ROOT, layout)], [entry, source])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual([(int(row["address"], 0) - base, int(row["size"], 0)) for row in rows],
                             list(self.spans))
            self.assertEqual([row["status"] for row in rows],
                             ["matching_c", "unmatched_asm", "matching_c", "unmatched_asm"])
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 4704)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 2)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x23B0, 0x2C50)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; // type:u8 size:0x{size:X} defined:true",
                              symbols)
            self.assertIn(f"[0x23B0, data, overlays/{module['name']}/unclassified_tail]", layout.read_text())
            for offset in (0xDDC, 0x1B8C):
                self.assertIn(f"[0x{offset:X}, asm,", layout.read_text())
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertEqual(len(re.findall(r"^\w+ =", bindings, re.M)), 34)
            self.assertNotRegex(bindings, r"=\s*0x801[37]")
            self.assertIn("func_800593D0 = 0x8005C4D8;", bindings)
            for alias, address in re.findall(r"^(\w+) = (0x[0-9A-F]+);", bindings, re.M):
                self.assertIn(f"{alias} = {address}; // type:func absolute:true", symbols)

    def test_unchanged_shared_body_and_attempts(self):
        for slot in (0, 1):
            path = ROOT / f"src/overlays/french_model_variant/variant393_rings{'_slot1' if slot else ''}.c"
            self.assertEqual(path.read_text(), '#include "../../types.h"\n'
                             f"#define func_8013C6D8 func_{0x8013C704 + slot * 0x40000:X}\n"
                             '#include "../model_variant/variant376_rings.c"\n')
        with (ROOT / "notes/overlays/french-model-variant393-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 32)
        self.assertEqual({(int(r["function_offset"], 0), int(r["slot"])) for r in attempts[:6]},
                         {(offset, slot) for offset in (0xDDC, 0x1704, 0x1B8C) for slot in (0, 1)})
        for row in attempts[:6]:
            self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
            offset, slot = int(row["function_offset"], 0), int(row["slot"])
            self.assertEqual((row["result"], int(row["instruction_bytes"]), int(row["different_words"])),
                             {0xDDC: ("mismatch", 2320, 328), 0x1704: ("matched", 1160, 0),
                              0x1B8C: ("mismatch", 2024, 515)}[offset])
            if offset == 0x1704:
                source = ROOT / f"src/overlays/french_model_variant/variant393_rings{'_slot1' if slot else ''}.c"
                self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
        expected = [(3524, 661), (3548, 686), (3548, 238), (3544, 3), (3544, 18),
                    (3552, 245), (3544, 3), (3544, 3), (3544, 3), (3544, 3),
                    (3552, 226), (3544, 0)]
        for index, (size, differences) in enumerate(expected):
            for slot in (0, 1):
                row = attempts[6 + index * 2 + slot]
                self.assertEqual((int(row["function_offset"], 0), int(row["slot"])), (4, slot))
                self.assertEqual(row["profile"], "gcc_2_8_1_g0_split")
                self.assertEqual((int(row["instruction_bytes"]), int(row["different_words"])),
                                 (size, differences))
                self.assertEqual(row["result"], "text_exact" if differences == 0 else "mismatch")
        for slot, row in enumerate(attempts[-2:]):
            source = ROOT / f"src/overlays/french_model_variant/variant393_entry{'_slot1' if slot else ''}.c"
            self.assertEqual((row["result"], int(row["slot"]), int(row["function_offset"], 0),
                              int(row["instruction_bytes"]), int(row["different_words"])),
                             ("matched", slot, 4, 3544, 0))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())

    def test_complete_entry_slot_wrapper(self):
        wrapper = '#include "../../types.h"\n'
        for offset in (4, 0xDDC, 0x1704, 0x1B8C):
            wrapper += f"#define func_{0x8013B000 + offset:X} func_{0x8017B000 + offset:X}\n"
        wrapper += '#define D_8013D3B0 D_8017D3B0\n#include "variant393_entry.c"\n'
        self.assertEqual((ROOT / "src/overlays/french_model_variant/variant393_entry_slot1.c").read_text(),
                         wrapper)

    def test_entry_observed_descriptor_domain(self):
        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        pairs, selectors, translations = {}, set(), set()
        with path.open("rb") as archive:
            for module, _, record, stage, selector, _ in self.selected():
                archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage == 9) * 4)
                request, = struct.unpack("<i", archive.read(4))
                self.assertEqual(request, 559000 + selector)
                selectors.add(request % 1000)
                archive.seek(module["sector_offset"] * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), module["sha256"])
                offset = 0x24AC + selector * 68
                count, = struct.unpack_from("<H", payload, offset + 0x1C)
                mode, = struct.unpack_from("<i", payload, offset + 0x20)
                pairs[(mode, count)] = pairs.get((mode, count), 0) + 1
                translations.add(struct.unpack_from("<3i", payload, offset + 0x38))
        self.assertEqual(selectors, set(range(8)))
        self.assertEqual(pairs, {(0, 2): 14, (1, 2): 6, (0, 3): 2})
        self.assertEqual(translations, {(0, -64, 32), (0, 0, 0), (0, 0, 80), (0, 16, -32)})
        source = (ROOT / "src/overlays/french_model_variant/variant393_entry.c").read_text()
        self.assertIn("if (work->config->mode == 1)", source)
        self.assertIn("if (config->mode == 0)", source)
        self.assertIn("work->matrix.t[0] += work->config->offset[0];", source)
        self.assertIn("work->matrix.t[0] -= work->config->offset[0];", source)
        self.assertIn("if (config->count != 0)", source)
        self.assertIn("while (work->part_index < config->count)", source)
        self.assertEqual(source.count("Model_GetFrameStep()"), 2)

    def test_retail_cfg_calls_and_68_byte_descriptor_stride(self):
        archive_path = ROOT / "game/france/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal French MODEL input required")
        with archive_path.open("rb") as archive:
            for module, _, record, stage, selector, slot in self.selected():
                base = int(module["load_address"], 0)
                archive.seek(module["sector_offset"] * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), module["sha256"])
                self.assertEqual(struct.unpack_from("<I", payload)[0], 393 + slot * 150)
                archive.seek((record * 276 + 275) * 2048 + 0x110 + (stage == 9) * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 559000 + selector)
                self.assertLessEqual(0x24AC + selector * 68 + 68, 0x5000)
                address = base + 0x24AC
                self.assertEqual(struct.unpack_from("<I", payload, 0x7C)[0],
                                 0x3C020000 | ((address + 0x8000) >> 16 & 0xFFFF))
                self.assertEqual(struct.unpack_from("<I", payload, 0x80)[0], 0x24420000 | (address & 0xFFFF))
                for offset, word in self.anchors.items():
                    self.assertEqual(struct.unpack_from("<I", payload, offset)[0], word, hex(offset))
                self.assertEqual(struct.unpack_from("<I", payload, 0xC34)[0],
                                 0x0C000000 | ((base + 0x1704) >> 2 & 0x3FFFFFF))
                for offset, size in self.spans:
                    result = walk_function(payload, base, offset, size)
                    self.assertTrue(result["closed"])
                    self.assertEqual(result["visited"], set(range(offset, offset + size, 4)))
                    self.assertEqual(result["returns"], 1)
                    self.assertEqual(result["problems"], set())
                    self.assertEqual(result["indirect_calls"], 0)
                    if offset == 4:
                        self.assertEqual(result["calls"], {base + start for start, _ in self.spans[1:]})
                    if offset == 0x1704:
                        self.assertEqual(result["external"], {
                            0x8005C018, 0x800842A8, 0x80085558, 0x80086258, 0x800872A8,
                            0x800875F8, 0x80087738, 0x80087958, 0x80087CB8})

    def test_target_compiled_partial_views(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from elftools.elf.elffile import ELFFile
        from build_baseline import TOOLCHAIN, compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles["gcc_2_8_1_g0_split"]["compiler"],
                     f"{TOOLCHAIN}/mipsel-none-elf-as", "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        constants = {"sizeof(ModelVariant337Ring)": 152, "sizeof(ModelVariant376Config)": 56,
                     "sizeof(ModelVariant376State)": 0xDFC, "sizeof(SVECTOR)": 8,
                     "sizeof(CVECTOR)": 4, "sizeof(MATRIX)": 32, "sizeof(POLY_GT4)": 52,
                     "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4}
        constants.update({
            "sizeof(Variant393EntryConfig)": 0x44, "sizeof(Variant393EntryState)": 0xE0C,
            "sizeof(Variant337EntryRecord)": 0x3A4, "sizeof(Variant337EntryRing)": 0x98,
            "sizeof(Variant337EntryStreamer)": 0x378, "sizeof(Variant393EntryProjection)": 16,
            "(PSXLONG)-1 < 0": 1,
        })
        for name, fields in (
            ("Variant393EntryConfig", {"inner": 0, "ribbon": 4, "outer": 8, "parts": 0xC,
                                      "vertices": 0x10, "count": 0x1C, "mode": 0x20,
                                      "start": 0x24, "end": 0x28, "field_2C": 0x2C,
                                      "fade_start": 0x30, "fade_end": 0x34, "offset": 0x38}),
            ("Variant337EntryRecord", {"inner": 0x220, "outer": 0x224, "scale": 0x228,
                                      "field_238": 0x238, "field_23A": 0x23A,
                                      "field_23C": 0x23C, "velocity": 0x2C8}),
            ("Variant337EntryRing", {"points": 0, "inner": 0x80, "outer": 0x84,
                                    "scale": 0x88, "field_8C": 0x8C, "field_90": 0x90}),
            ("Variant337EntryStreamer", {"color": 0x264, "field_2A8": 0x2A8}),
            ("Variant393EntryState", {"records": 0, "rings": 0x3A4, "streamers": 0x4D4,
                                     "triangle": 0xBC4, "quad": 0xBE0, "textured": 0xC04,
                                     "flat_textured": 0xC6C, "extra_flat": 0xCBC,
                                     "matrix": 0xD1C, "target": 0xD3C, "positions": 0xD44,
                                     "direction": 0xD74, "screen_delta": 0xD84,
                                     "view_delta": 0xD88, "angles": 0xD98,
                                     "frame_count": 0xDA0, "frame": 0xDA4,
                                     "animation_frame": 0xDA8, "step": 0xDAC,
                                     "fade": 0xDB0, "config": 0xDB4, "parts": 0xDBC,
                                     "part_index": 0xDC8, "field_DCC": 0xDCC,
                                     "field_DCE": 0xDCE, "field_DD0": 0xDD0, "width": 0xDD2,
                                     "field_DD4": 0xDD4, "field_DD8": 0xDD8,
                                     "field_DDC": 0xDDC, "field_DE0": 0xDE0,
                                     "field_DE4": 0xDE4, "field_DE8": 0xDE8,
                                     "field_DEC": 0xDEC, "field_DF0": 0xDF0,
                                     "field_DF4": 0xDF4, "phase": 0xDF8, "tint": 0xE04,
                                     "slot": 0xE08, "command": 0xE0A}),
            ("Variant393EntryProjection", {"projected": 0, "interpolation": 4,
                                          "flag": 8, "target": 12}),
            ("ModelVariant337Ring", {"points": 0, "points[1]": 0x20, "color": 0x80,
                                     "color[1]": 0x84, "scale": 0x88}),
            ("ModelVariant376Config", {"start": 0x24, "end": 0x28, "fade_start": 0x30, "fade_end": 0x34}),
            ("ModelVariant376State", {"rings": 0x3A4, "rings[1]": 0x43C, "quad": 0xC38,
                                      "transform": 0xD1C, "target": 0xD3C, "frame": 0xDA0,
                                      "elapsed": 0xDA4, "step": 0xDAC, "config": 0xDB4,
                                      "single": 0xDC8, "state": 0xDF8}),
        ):
            for field, offset in fields.items():
                constants[f"(u32)&(({name} *)0)->{field}"] = offset
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="rings393-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/model_variant/variant376_rings.h"\n'
                '#include "../../src/overlays/french_model_variant/variant393_entry.h"\n'
                "const u32 layouts[] = {" + ", ".join(constants) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"), {
                "kind": "text", "source": str(source), "object": "layout.o", "profile": "gcc_2_8_1_g0_split"},
                profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(symbol["st_value"], 0)
                data = elf.get_section(symbol["st_shndx"]).data()
                self.assertEqual(struct.unpack(f"<{len(constants)}I", data), tuple(constants.values()))
