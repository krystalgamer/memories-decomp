import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import re
import sys
import unittest
from unittest import mock

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments
from verify_inputs import load_checksum_manifest


class FrenchMainMenuBanksTests(unittest.TestCase):
    config = "sles_03948"
    region = "france"
    prefix = "french"

    def modules(self):
        manifest = json.loads((ROOT / f"config/{self.config}/overlays.json").read_text())
        names = [f"{self.prefix}_main_menu"] + [
            f"{self.prefix}_main_menu_language_{n}" for n in range(1, 5)]
        modules = [module for module in manifest["modules"] if module["name"] in names]
        self.assertEqual([module["name"] for module in modules], names)
        return modules

    def test_physical_banks_follow_the_language_package_loader(self):
        checksum = load_checksum_manifest(ROOT / f"config/{self.config}/files.sha256")[
            f"game/{self.region}/DATA/SU.MRG"]
        for language, module in enumerate(self.modules()):
            with self.subTest(language=language):
                self.assertEqual(module["archive"], f"game/{self.region}/DATA/SU.MRG")
                self.assertEqual(module["archive_sha256"], checksum)
                self.assertEqual(module["sector_offset"], language * 136 + 64 + 32 + 2)
                self.assertEqual((module["sector_count"], int(module["load_address"], 0)),
                                 (16, 0x80180000))
                self.assertNotIn("duplicate_sector_offsets", module)
                self.assertEqual(module["linker_symbols"],
                                 f"config/{self.config}/overlays/main_menu_linker_symbols.txt")
        loader = (ROOT / "src/game/european/main_menu_load_package_stage.c").read_text()
        self.assertIn("#define MAIN_MENU_PACKAGE_FIRST_SECTOR (D_8009C02B * 0x88)", loader)
        stages = (ROOT / "src/game/main_menu_load_package_stage.c").read_text()
        for count in (64, 32, 2, 16):
            self.assertIn(f"object->phase_size = {count} * FILE_SECTOR_SIZE;", stages)
        self.assertIn("object->value_08 = D_8001002C;", stages)

    def test_all_banks_select_the_same_31_accepted_units(self):
        donor = ROOT / "config/sles_03948/overlays/main_menu_matching_c.json"
        expected = json.loads(donor.read_text())["functions"]
        self.assertEqual(len(expected), 31)
        self.assertEqual(sum(int(entry["size"], 0) for entry in expected), 18280)
        for module in self.modules():
            layout = ROOT / module["layout"]
            matching = layout.with_name(layout.stem + "_matching_c.json")
            self.assertEqual(json.loads(matching.read_text())["functions"], expected)
            segments = c_segments(ROOT, layout)
            self.assertEqual([segment["source"] for segment in segments],
                             [entry["source"] for entry in expected])
            self.assertEqual([segment["profile"] for segment in segments],
                             [entry["profile"] for entry in expected])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(len(rows), 31)
            cursor = 0x8018001C
            for row, entry in zip(rows, expected):
                self.assertEqual(int(row["address"], 0), cursor)
                self.assertEqual(int(row["size"], 0), int(entry["size"], 0))
                self.assertEqual(row["status"], "matching_c")
                cursor += int(row["size"], 0)
            self.assertEqual(cursor, 0x80184784)
            text = layout.read_text()
            self.assertIn(f"config/{self.config}/overlays/main_menu_symbols.txt", text)
            for offset, kind, stem in ((0, "data", "header"), (4, "rodata", "rodata"),
                                       (0x4784, "data", "data")):
                self.assertRegex(text, rf"\[0x{offset:X}, {kind}, overlays/{module['name']}/{stem}\]")

    def test_retail_images_keep_distinct_raw_data_and_identical_code(self):
        archive = ROOT / f"game/{self.region}/DATA/SU.MRG"
        if not archive.is_file():
            self.skipTest(f"{self.region} retail archive unavailable")
        code, hashes = None, set()
        with archive.open("rb") as handle:
            for module in self.modules():
                handle.seek(module["sector_offset"] * 2048)
                image = handle.read(module["sector_count"] * 2048)
                self.assertEqual(len(image), 32768)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                layout = (ROOT / module["layout"]).read_text()
                self.assertIn(f"sha1: {hashlib.sha1(image).hexdigest()}", layout)
                if code is None:
                    code = image[0x1C:0x4784]
                self.assertEqual(image[0x1C:0x4784], code)
                hashes.add(module["sha256"])
        self.assertEqual(len(hashes), 5)

    def test_absent_build_skips_without_optional_elf_dependency(self):
        with (
            mock.patch.dict(sys.modules, {"elftools": None, "elftools.elf.elffile": None}),
            mock.patch.object(Path, "is_file", return_value=False),
            self.assertRaisesRegex(unittest.SkipTest, f"Run make {self.prefix}-match-overlays first"),
        ):
            self.test_complete_production_images_and_selected_c_definitions_when_built()

    def test_built_images_skip_when_optional_elf_dependency_is_unavailable(self):
        with (
            mock.patch.object(Path, "is_file", return_value=True),
            mock.patch.object(importlib.util, "find_spec", return_value=None),
            mock.patch.dict(sys.modules, {"elftools": None, "elftools.elf.elffile": None}),
            self.assertRaisesRegex(unittest.SkipTest, "pyelftools unavailable"),
        ):
            self.test_complete_production_images_and_selected_c_definitions_when_built()

    def test_complete_production_images_and_selected_c_definitions_when_built(self):
        modules = self.modules()
        for module in modules:
            linked = ROOT / f"tmp/overlays/{module['name']}/build/{module['name']}.elf"
            if not linked.is_file():
                self.skipTest(f"Run make {self.prefix}-match-overlays first: {module['name']}")
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools unavailable")
        from elftools.elf.elffile import ELFFile

        for module in modules:
            name = module["name"]
            directory = ROOT / f"tmp/overlays/{name}"
            layout = ROOT / module["layout"]
            image = (ROOT / module["output"]).read_bytes()
            self.assertEqual((directory / f"build/{name}.bin").read_bytes(), image)
            self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
            link_script = (directory / f"{name}.ld").read_text()
            objects = {segment["source"]: directory / "build" / segment["object"]
                       for segment in c_segments(ROOT, layout)}
            entries = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            with (directory / f"build/{name}.elf").open("rb") as handle:
                elf = ELFFile(handle)
                table = elf.get_section_by_name(".symtab")
                for row, entry in zip(rows, entries):
                    address, size = int(row["address"], 0), int(row["size"], 0)
                    definition, = table.get_symbol_by_name(row["name"])
                    self.assertIsInstance(definition["st_shndx"], int)
                    self.assertEqual((definition["st_value"], definition["st_size"],
                                      definition["st_info"]["type"]), (address, size, "STT_FUNC"))
                    section = elf.get_section(definition["st_shndx"])
                    offset = address - section["sh_addr"]
                    self.assertTrue(section["sh_flags"] & 4)
                    self.assertEqual(section.data()[offset:offset + size],
                                     image[address - 0x80180000:address - 0x80180000 + size])
                    obj = objects[entry["source"]]
                    self.assertIn(f"{obj.relative_to(ROOT)}(.text);", link_script)
                    with obj.open("rb") as object_handle:
                        object_elf = ELFFile(object_handle)
                        own, = object_elf.get_section_by_name(".symtab").get_symbol_by_name(row["name"])
                        self.assertIsInstance(own["st_shndx"], int)
                        self.assertEqual((own["st_value"], own["st_size"],
                                          own["st_info"]["type"]), (0, size, "STT_FUNC"))
                        self.assertTrue(object_elf.get_section(own["st_shndx"])["sh_flags"] & 4)
            raw = re.findall(r"(\S+/(?:header\.data|rodata\.rodata|data\.data)\.o)\((\.\w+)\);",
                             link_script)
            sizes = []
            for object_path, section_name in raw:
                with (ROOT / object_path).open("rb") as handle:
                    section = ELFFile(handle).get_section_by_name(section_name)
                    if section is not None and section["sh_size"]:
                        self.assertFalse(section["sh_flags"] & 4)
                        sizes.append(section["sh_size"])
            self.assertEqual(sorted(sizes), [4, 24, 14460])


class SpanishMainMenuBanksTests(FrenchMainMenuBanksTests):
    config = "sles_03951"
    region = "spain"
    prefix = "spanish"


class ItalianMainMenuBanksTests(FrenchMainMenuBanksTests):
    config = "sles_03950"
    region = "italy"
    prefix = "italian"


class GermanMainMenuBanksTests(FrenchMainMenuBanksTests):
    config = "sles_03949"
    region = "germany"
    prefix = "german"


if __name__ == "__main__":
    unittest.main()
