import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import unittest

from elftools.elf.elffile import ELFFile

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from progress import load_french_overlay_inventories
import register_model_images as registration

CONFIG = ROOT / "config/sles_03948"
LINKER_SYMBOLS = "config/sles_03948/overlays/model_return_two_linker_symbols.txt"
SOURCE = "src/overlays/spanish_model_primary/return_two"


class FrenchModelReturnTwoModuleTests(unittest.TestCase):
    """Every other French return-two primary reuses the accepted C entry."""

    def setUp(self):
        self.manifest = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        self.modules = {row["name"]: row for row in self.manifest
                        if row.get("linker_symbols") == LINKER_SYMBOLS
                        and row["name"].startswith("french_model_image_model_")}
        with (ROOT / "notes/overlays/french-model-return-two-modules.csv").open() as handle:
            self.rows = list(csv.DictReader(handle))
        with (ROOT / "notes/overlays/spanish-model-return-two-instances.csv").open() as handle:
            self.instances = list(csv.DictReader(handle))

    def slot(self, module):
        return (int(module["load_address"], 0) - 0x8013A000) // 0x40000

    def test_modules_cover_every_unrepresented_ledger_instance_once(self):
        self.assertEqual(len(self.modules), 1188)
        self.assertEqual(len(self.rows), 1220)
        expected = {(int(row[f"slot{slot}_sector"]), row[f"slot{slot}_sha256"])
                    for row in self.instances for slot in (0, 1)} - {
            (220, self.instances[0]["slot0_sha256"]), (222, self.instances[0]["slot1_sha256"])}
        self.assertEqual({(int(row["sector"]), row["sha256"]) for row in self.rows}, expected)
        mapped = registration.registered_keys(self.manifest)
        for row in self.rows:
            module = mapped[("game/france/DATA/MODEL.MRG", int(row["sector"]), 2,
                             0x8013A000 + int(row["slot"]) * 0x40000)]
            self.assertEqual(module["name"], row["module"])
            self.assertEqual(module["sha256"], row["sha256"])
        self.assertEqual({row["module"] for row in self.rows}, set(self.modules))

    def test_layouts_match_the_accepted_representatives(self):
        counts = load_french_overlay_inventories(ROOT)
        templates = {slot: (CONFIG / f"overlays/model_return_two_slot{slot}.yaml").read_text()
                     for slot in (0, 1)}
        for name, module in self.modules.items():
            slot, layout = self.slot(module), ROOT / module["layout"]
            text = layout.read_text()
            sha1 = re.search(r"^sha1: ([0-9a-f]{40})$", text, re.M)[1]
            expected = templates[slot].replace(f"french_model_return_two_slot{slot}", name)
            self.assertEqual(text, re.sub(r"^sha1: [0-9a-f]{40}$", f"sha1: {sha1}", expected,
                                          count=1, flags=re.M))
            address = 0x8013A004 + slot * 0x40000
            source = SOURCE + ("_slot1" if slot else "") + ".c"
            self.assertEqual(json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text()),
                             {"schema": 1, "functions": [{"address": f"0x{address:X}", "size": "0x8",
                              "profile": "gcc_2_8_1_g0_split", "source": source}]})
            self.assertEqual([segment["source"] for segment in c_segments(ROOT, layout)], [source])
            self.assertEqual(counts[layout.stem]["function_count"], 1)
            self.assertEqual(counts[layout.stem]["matching_c_function_count"], 1)
            self.assertEqual(counts[layout.stem]["matching_c_bytes"], 8)

    def test_built_images_have_exact_bytes_and_owners(self):
        for name, module in self.modules.items():
            directory = ROOT / "tmp/overlays" / name
            binary, elf_path = directory / f"build/{name}.bin", directory / f"build/{name}.elf"
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            with self.subTest(module=name):
                self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
                self.assertEqual(binary.read_bytes(), (ROOT / module["output"]).read_bytes())
                slot = self.slot(module)
                base = 0x8013A000 + slot * 0x40000
                objects = {path.relative_to(directory / "build").as_posix(): path
                           for path in (directory / "build").rglob("*.o")}
                c_object = SOURCE + ("_slot1" if slot else "") + ".o"
                self.assertEqual(set(objects), {
                    c_object,
                    f"tmp/overlays/{name}/asm/data/overlays/{name}/module_header.data.o",
                    f"tmp/overlays/{name}/asm/data/overlays/{name}/unclassified_tail.data.o"})
                definers = {}
                for relative, path in objects.items():
                    with path.open("rb") as handle:
                        for symbol in ELFFile(handle).get_section_by_name(".symtab").iter_symbols():
                            if isinstance(symbol["st_shndx"], int) and symbol.name.startswith(("func_", "D_")) \
                                    and not symbol.name.endswith(".NON_MATCHING"):
                                definers.setdefault(symbol.name, []).append((relative, symbol["st_size"]))
                self.assertEqual(definers, {
                    f"func_{base + 4:X}": [(c_object, 8)],
                    f"D_{base:X}": [(f"tmp/overlays/{name}/asm/data/overlays/{name}/module_header.data.o", 4)],
                    f"D_{base + 12:X}": [(f"tmp/overlays/{name}/asm/data/overlays/{name}/unclassified_tail.data.o", 0xFF4)]})
                with elf_path.open("rb") as handle:
                    linked, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(f"func_{base + 4:X}")
                    self.assertEqual((linked["st_value"], linked["st_size"], linked["st_info"]["type"]),
                                     (base + 4, 8, "STT_FUNC"))


if __name__ == "__main__":
    unittest.main()
