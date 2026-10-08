import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import unittest

from tools.project.overlay_sources import c_segments
from tools.project.tests import test_french_model_variant373 as family373

ROOT = Path(__file__).resolve().parents[3]
SECTORS = {33576, 33586, 105356, 105366}
STEM = "src/overlays/french_model_variant/variant474_funnels"
PROFILE = "gcc_2_8_1_g0_split"


class FrenchModelVariant474FunnelsTests(unittest.TestCase):
    def setUp(self):
        modules = json.loads((ROOT / "config/sles_03948/overlays.json").read_text())["modules"]
        self.modules = [m for m in modules if m["sector_offset"] in SECTORS]

    def test_four_registrations_and_preserved_neighbours(self):
        self.assertEqual(len(self.modules), 4)
        self.assertEqual({int(m["name"].split("_")[3]) for m in self.modules}, {121, 431})
        for module in self.modules:
            with self.subTest(module=module["name"]):
                base = int(module["load_address"], 0)
                source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
                layout = ROOT / module["layout"]
                functions = json.loads(layout.with_name(
                    layout.stem + "_matching_c.json").read_text())["functions"]
                self.assertEqual(len(functions), 5)
                self.assertIn({"address": f"0x{base + 0x1738:X}", "size": "0x688",
                               "profile": PROFILE, "source": source}, functions)
                self.assertEqual({int(f["address"], 0) - base for f in functions},
                                 {0x1738, 0x1DC0, 0x2240, 0x2BD8, 0x30D0})
                for function in functions:
                    if int(function["address"], 0) != base + 0x1738:
                        self.assertTrue(function["source"].startswith(
                            "src/overlays/spanish_model_variant/variant474_"))
                with layout.with_name(layout.stem + "_functions.csv").open() as stream:
                    rows = list(csv.DictReader(stream))
                row, = [r for r in rows if int(r["address"], 0) == base + 0x1738]
                self.assertEqual((row["status"], row["size"]), ("matching_c", "0x688"))
                self.assertIn("direct-entry reachable", row["notes"])
                self.assertIn("first recovery of this body in any release", row["notes"])
                self.assertEqual(
                    {int(r["address"], 0) - base for r in rows if r["status"] == "unmatched_asm"},
                    {4, 0xE38},
                )
                self.assertIn(f"[0x38BC, data, overlays/{module['name']}/unclassified_tail]",
                              layout.read_text())
                self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])

    def test_plain_source_and_terminal_fingerprints(self):
        source = ROOT / (STEM + ".c")
        wrapper = ROOT / (STEM + "_slot1.c")
        header = ROOT / (STEM + ".h")
        body = source.read_text()
        self.assertNotRegex(body, r"\b(?:extern|asm|__asm__|register|volatile|do)\b")
        self.assertIn("trigger->size > 900", body)
        self.assertIn("trigger->size > 1100", body)
        self.assertIn("funnel->size > 8192", body)
        self.assertIn("flicker = 1024;", body)
        self.assertIn("bias = flicker;", body)
        self.assertEqual(wrapper.read_text(),
                         '#include "../../types.h"\n'
                         '#define func_8013C738 func_8017C738\n'
                         '#include "variant474_funnels.c"\n')
        with (ROOT / "notes/overlays/french-model-variant474-funnels-attempts.csv").open() as stream:
            rows = list(csv.DictReader(stream))
        terminals = [r for r in rows if r["result"] == "matched"]
        self.assertEqual(len(terminals), 4)
        self.assertEqual({r["module"] for r in terminals}, {m["name"] for m in self.modules})
        for row in terminals:
            path = wrapper if row["slot"] == "1" else source
            self.assertEqual(row["fingerprint"], hashlib.sha256(path.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"],
                             hashlib.sha256(header.read_bytes()).hexdigest())
            self.assertEqual((row["function_offset"], row["profile"],
                              row["instruction_bytes"], row["different_words"]),
                             ("0x1738", PROFILE, "1672", "0"))

    def test_target_compiled_layout(self):
        checks = {"sizeof(Funnel474)": 0x118, "sizeof(Funnels474State)": 0x1C38,
                  "sizeof(SVECTOR)": 8, "sizeof(CVECTOR)": 4, "sizeof(POLY_GT4)": 52,
                  "(u32)&((Funnels474State *)0)->trigger.size": 0x494}
        for name, fields in (
            ("Funnel474", (("inner", 0), ("outer", 0x88), ("size", 0x110))),
            ("Funnels474State", (("funnels", 0), ("trigger", 0x230), ("quad", 0x1964),
                                  ("target", 0x1B5C), ("direction", 0x1B64),
                                  ("frame", 0x1B90), ("step", 0x1B9C),
                                  ("inner_color", 0x1C00), ("outer_color", 0x1C04),
                                  ("spin", 0x1C08), ("side", 0x1C34))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({name} *)0)->{field}"] = value
        family373.FrenchModelVariant373Tests.assert_target_layout(
            self, checks, "variant474_funnels.h")

    def test_entry_call_preserves_context_argument(self):
        archive = ROOT / "game/france/DATA/MODEL.MRG"
        if not archive.exists():
            self.skipTest("Legal French MODEL input required")
        with archive.open("rb") as stream:
            for module in self.modules:
                stream.seek(module["sector_offset"] * 2048)
                data = stream.read(0x5000)
                base = int(module["load_address"], 0)
                call, delay = struct.unpack_from("<II", data, 0xC24)
                self.assertEqual(call >> 26, 3)
                self.assertEqual(0x80000000 | ((call & 0x3FFFFFF) << 2), base + 0x1738)
                self.assertEqual(delay, 0x02402021)

    def test_complete_images_and_actual_c_owners(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            name = module["name"]
            directory = ROOT / "tmp/overlays" / name
            binary = directory / "build" / f"{name}.bin"
            if not binary.exists():
                self.skipTest("Build French overlays before checking owners")
            with self.subTest(module=name):
                data = binary.read_bytes()
                self.assertEqual(len(data), 0x5000)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                base = int(module["load_address"], 0)
                symbol = f"func_{base + 0x1738:X}"
                segment, = [s for s in c_segments(ROOT, ROOT / module["layout"])
                            if s["source"].startswith(STEM)]
                for path, value, section in (
                    (directory / "build" / segment["object"], 0, ".text"),
                    (directory / "build" / f"{name}.elf", base + 0x1738, ".helper_1738"),
                ):
                    with path.open("rb") as stream:
                        elf = ELFFile(stream)
                        own, = elf.get_section_by_name(".symtab").get_symbol_by_name(symbol)
                        self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                         (value, 0x688, "STT_FUNC"))
                        self.assertEqual(elf.get_section(own["st_shndx"]).name, section)
