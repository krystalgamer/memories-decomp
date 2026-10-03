import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/sles_03948"
SOURCE = ROOT / "src/overlays/french_model_variant/variant336_strand.c"
PROFILE = "gcc_2_8_1_g0_split"
IMAGES = (
    (7, 0, 116652, "92f3bfde96ea59a253aced4cb76da1f9ccdb852c130c4714f1a4b70942ec2fc2"),
    (8, 1, 116662, "9963d5f1415f4d06e4d4dbdc25cd56d0dbd4b460a5316a0fb9120cd7b55b0cec"),
)
SPANS = ((4, 0xA9C), (0xA9C, 0x13D4), (0x13D4, 0x1860),
         (0x1860, 0x1B6C), (0x1B6C, 0x23B8))
HELPERS = ((0xA9C, "ribbon", 2360, 304), (0x13D4, "sheets", 1164, 272),
           (0x1860, "strand", 780, 264), (0x1B6C, "streamers", 2124, 328))


class FrenchModelVariant336Tests(unittest.TestCase):
    def setUp(self):
        self.modules = {row["name"]: row for row in json.loads(
            (CONFIG / "overlays.json").read_text())["modules"]}
        with (ROOT / "notes/overlays/french-model-variant336-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_two_independent_manifest_identities(self):
        names = {f"french_model_variant_472_stage{stage}_slot{slot}"
                 for stage, slot, *_ in IMAGES}
        self.assertEqual(set(self.instances), names)
        for stage, slot, sector, digest in IMAGES:
            name = f"french_model_variant_472_stage{stage}_slot{slot}"
            module, instance = self.modules[name], self.instances[name]
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"],
                             "0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3")
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{0x8013B000 + slot * 0x40000:X}")
            self.assertEqual(module["sha256"], digest)
            self.assertEqual(module["linker_symbols"],
                             "config/sles_03948/overlays/model_variant475_linker_symbols.txt")
            self.assertEqual(module["output"], f"tmp/overlays/{name}/module.bin")
            self.assertEqual(module["layout"],
                             f"config/sles_03948/overlays/{name.removeprefix('french_')}.yaml")
            self.assertEqual((int(instance["model"]), int(instance["stage"]),
                              int(instance["slot"]), int(instance["header"]),
                              int(instance["sector"]), instance["sha256"]),
                             (472, stage, slot, 336 + slot * 150, sector, digest))

    def test_four_c_owners_and_preserved_entry_raw_extents(self):
        from tools.project.overlay_sources import c_segments

        for name in self.instances:
            module = self.modules[name]
            base = int(module["load_address"], 0)
            slot = (base - 0x8013B000) // 0x40000
            layout = ROOT / module["layout"]
            matching = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            expected = [{
                "address": f"0x{base + offset:X}", "size": f"0x{size:X}",
                "profile": PROFILE,
                "source": "src/overlays/french_model_variant/variant336_" + helper
                          + ("_slot1" if slot else "") + ".c",
            } for offset, helper, size, _ in HELPERS]
            self.assertEqual(matching, {"schema": 1, "functions": expected})
            self.assertEqual(
                [(row["source"], row["profile"]) for row in c_segments(ROOT, layout)],
                [(row["source"], PROFILE) for row in expected])
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            self.assertEqual(
                [(row["start"], row["vram"], row["subsegments"][0][:2]) for row in segments[:-1]],
                [(offset, base + offset, [offset, kind]) for offset, kind in (
                    (0, "data"), (4, "asm"), (0xA9C, "c"), (0x13D4, "c"),
                    (0x1860, "c"), (0x1B6C, "c"), (0x23B8, "data"))])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(row["address"], 0), int(row["size"], 0), row["status"]) for row in inventory],
                [(base + start, end - start, "unmatched_asm" if start == 4 else "matching_c")
                 for start, end in SPANS])
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x23B8, 0x2C48)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; "
                              f"// type:u8 size:0x{size:X} defined:true", symbols)

    def test_terminal_fingerprints_and_symbol_only_wrapper(self):
        with (ROOT / "notes/overlays/french-model-variant336-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertGreaterEqual(len(rows), 32)
        self.assertGreaterEqual(sum(row["result"] == "text_exact" for row in rows), 4)
        terminals = {(int(row["function_offset"], 0), int(row["slot"])): row
                     for row in rows if row["result"] == "matched"}
        self.assertEqual(set(terminals), {(offset, slot) for offset, *_ in HELPERS for slot in (0, 1)})
        for offset, helper, size, _ in HELPERS:
            for slot in (0, 1):
                source = SOURCE.with_name("variant336_" + helper + ("_slot1" if slot else "") + ".c")
                row = terminals[(offset, slot)]
                self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
                self.assertEqual((row["profile"], row["instruction_bytes"], row["different_words"]),
                                 (PROFILE, str(size), "0"))
            self.assertEqual(SOURCE.with_name(f"variant336_{helper}_slot1.c").read_text(),
                             '#include "../../types.h"\n\n'
                             f'#define func_{0x8013B000 + offset:X} func_{0x8017B000 + offset:X}\n'
                             f'#include "variant336_{helper}.c"\n')

    def test_measured_strand_behavior_without_submission(self):
        text = SOURCE.read_text()
        for expression in (
            "ModelVariantStrandWide *strand;", "work + 0x1AB0", "work + 0x1F10",
            "MODEL_VARIANT_WORD(work, 0x2000) >= 2", "j < 13", "i < 6",
            "b += 0x708", "r = (j << 10) / 12", "a = (i << 12) / 6",
            "(u32)rsin(b) >> 9", "line->attribute = 0x50000000",
            "line->g = 0x40", "line->r = 0", "line->b = 0x80",
            "MODEL_VARIANT_HALF(work, 0x1FF0)", "MODEL_VARIANT_HALF(work, 0x1FF2)",
            "(u32)MODEL_VARIANT_WORD(work, 0x1FD0) >> 1",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("RotTransPers4("), 1)
        self.assertNotIn("GsSort", text)
        self.assertNotIn("extern ", text)

    def test_paired_sheet_origins_and_unscaled_depth(self):
        text = SOURCE.with_name("variant336_sheets.c").read_text()
        for expression in (
            "work + 0xF00", "work + 0x1E3C", "MODEL_VARIANT_WORD(work, 0x1FEC)",
            "(i & 1) == 0", "node += 960", "i + 1 == 8",
            "MODEL_VARIANT_WORD(node, 0x2C8)", "MODEL_VARIANT_WORD(node, 0x2D8)",
            "MODEL_VARIANT_WORD(node, 0x2E8)", "MODEL_VARIANT_WORD(node, 0x2F0)",
            "sheet->size < 4096", "sheet->size < 8192",
            "depth >= 0 && flag >= 0", "GsSortPoly(poly, ot, depth)",
        ):
            self.assertIn(expression, text)
        self.assertNotIn("depth =", text.replace("depth = RotTransPers4(", "RotTransPers4("))
        self.assertEqual(text.count("ScaleMatrix("), 1)
        self.assertIn("ratan2(MODEL_VARIANT_WORD(work, 0x1FB4)", text)

    def test_ribbon_node_growth_and_terminal_projection(self):
        text = SOURCE.with_name("variant336_ribbon.c").read_text()
        for expression in (
            "MODEL_VARIANT_WORD(work, 0x2000) > 0", "i < 4", "k < 17",
            "ribbon->extent * 48 / 1024", "ribbon->origin.vy = -1024",
            "ribbon->end.vy = 0", "wave += 1100", "phase = i * 0x400",
            "ribbon->count += MODEL_VARIANT_WORD(work, 0x1FD0) * 2",
            "MODEL_VARIANT_WORD(work, 0x1FEC) = i * 2 + 3",
            "ribbon->extent -= MODEL_VARIANT_WORD(work, 0x1FD0) * 8",
            "MODEL_VARIANT_WORD(work, 0x2000) = 3",
            "MODEL_VARIANT_WORD(work, 0x1FD0) * 650",
            "MODEL_VARIANT_WORD(work, 0x1FD0) * 100",
            "&ribbon->a[15], &ribbon->a[k]", "&ribbon->flag[k]",
        ):
            self.assertIn(expression, text)
        self.assertNotIn("extern ", text)

    def test_streamer_endpoint_cases_and_unsigned_timing(self):
        text = SOURCE.with_name("variant336_streamers.c").read_text()
        for expression in (
            "reach = 0x80", "work + 0x13C0", "work + 0x1EC0",
            "MODEL_VARIANT_WORD(work, 0x1FFC) / 64", "streamer->width[k] = 2",
            "MODEL_VARIANT_WORD(work, 0x2000) == 0",
            "(u32)MODEL_VARIANT_WORD(work, 0x1FC8)",
            "MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1FD8), 0x10)",
            "MODEL_VARIANT_WORD(*(u8 *G32 *)(work + 0x1FD8), 0x14)",
            "m.t[1] = MODEL_VARIANT_WORD(work, 0x1F38)",
            "&streamer->a[15], &streamer->a[k]", "&streamer->flag[k]",
            "streamer->otz[k] >= 0 && streamer->flag[k] >= 0",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("streamer->ox[k] ="), 2)
        self.assertEqual(text.count("streamer->oy[k] ="), 2)
        self.assertNotIn("if (work)", text)
        self.assertNotIn("extern ", text)

    def test_target_compiled_canonical_layouts(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from elftools.elf.elffile import ELFFile
        from tools.project.build_baseline import TOOLCHAIN, compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles[PROFILE]["compiler"], f"{TOOLCHAIN}/mipsel-none-elf-as",
                     "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        checks = {"sizeof(ModelVariantStrandWide)": 132,
                  "sizeof(((ModelVariantStrandWide *)0)->point)": 104,
                  "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16, "sizeof(MATRIX)": 32,
                  "sizeof(GsCOORDINATE2)": 80, "sizeof(PSXLONG)": 4, "sizeof(GsLINE)": 16}
        for field, offset in (("attribute", 0), ("x0", 4), ("x1", 8), ("r", 12), ("g", 13), ("b", 14)):
            checks[f"(u32)&((GsLINE *)0)->{field}"] = offset
        checks.update({"sizeof(ModelVariantSheet)": 152, "sizeof(ModelVariant336Ribbon)": 960,
                       "sizeof(Variant321Streamer)": 888, "sizeof(POLY_GT4)": 52,
                       "sizeof(POLY_FT4)": 40, "sizeof(POLY_G4)": 36, "sizeof(u8 *G32)": 4})
        for typename, members in (
            ("ModelVariantSheet", (("v0", 0), ("v1", 32), ("v2", 64), ("v3", 96),
                                   ("outer", 128), ("inner", 132), ("size", 136))),
            ("ModelVariant336Ribbon", (("a", 0), ("sa", 0x88), ("angle", 0xCC),
                ("b", 0x110), ("sb", 0x198), ("width", 0x1DC), ("color", 0x220),
                ("origin", 0x2C8), ("end", 0x2D8), ("state", 0x2E8),
                ("count", 0x2EC), ("extent", 0x2F0), ("flag", 0x2F4),
                ("otz", 0x338), ("ox", 0x37C), ("oy", 0x39E))),
            ("Variant321Streamer", (("a", 0), ("sa", 0x88), ("angle", 0xCC),
                ("b", 0x110), ("sb", 0x198), ("width", 0x1DC), ("color", 0x264),
                ("flag", 0x2AC), ("otz", 0x2F0), ("ox", 0x334), ("oy", 0x356))),
        ):
            for field, offset in members:
                checks[f"(u32)&(({typename} *)0)->{field}"] = offset
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french336-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/model_variant/model_variant.h"\n'
                '#include "../../src/overlays/model_variant/variant321_streamers.h"\n'
                '#include "../../src/overlays/french_model_variant/variant336_ribbon.h"\n'
                'const u32 layouts[] = {' + ", ".join(checks) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            {"source": str(source), "object": "layout.o", "profile": PROFILE},
                            profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(struct.unpack(f"<{len(checks)}I", elf.get_section(symbol["st_shndx"]).data()),
                                 tuple(checks.values()))

    def test_retail_closed_spans_and_absent_local_strand_calls(self):
        from tools.project.overlay_function_inventory import walk_function

        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for stage, slot, sector, digest in IMAGES:
                base = 0x8013B000 + slot * 0x40000
                archive.seek(sector * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), digest)
                self.assertEqual(struct.unpack_from("<I", payload)[0], 336 + slot * 150)
                for start, end in SPANS:
                    result = walk_function(payload, base, start, end - start)
                    self.assertTrue(result["closed"])
                    self.assertEqual(result["visited"], set(range(start, end, 4)))
                    self.assertEqual(result["returns"], 1)
                    self.assertNotIn(base + 0x1860, result["calls"])
                    if start == 0x1860:
                        self.assertEqual(result["external"], {
                            0x80087CB8, 0x800875F8, 0x80086258, 0x80085558,
                            0x800866F8, 0x80086628, 0x80087958,
                        })
                self.assertEqual(struct.unpack_from("<I", payload, 0x1860)[0], 0x27BDFEF8)
                self.assertEqual(struct.unpack_from("<I", payload, 0x1B68)[0], 0x27BD0108)
                for offset, _, size, frame in HELPERS:
                    self.assertEqual(struct.unpack_from("<I", payload, offset)[0],
                                     0x27BD0000 | ((-frame) & 0xFFFF))
                    self.assertEqual(struct.unpack_from("<I", payload, offset + size - 4)[0],
                                     0x27BD0000 | frame)
