import csv
import hashlib
from pathlib import Path
import re
import struct
import unittest
from unittest.mock import patch

from tools.project.tests import test_french_options as french
from tools.project.progress import load_spanish_overlay_inventories

ROOT = french.ROOT


class SpanishOptionsTests(french.FrenchOptionsTests):
    region = "spain"
    region_name = "Spanish"
    module_name = "spanish_options"
    config_name = "sles_03951"
    executable_name = "SLES_039.51"
    boot_region = "spanish"
    build_target = "spanish-match"
    sdk_object = "asm/generated/spanish_80073c4c.o"
    sdk_prefix = "func_spanish_"
    dependencies = {
        **french.FrenchOptionsTests.dependencies,
        "grid": {"D_80169080", "D_80169140", "D_80169148",
                 "SetPolyGT4", "GsSortPoly", "GsSortFastSprite"},
    }
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    resident_raw_owners = (
        ("spanish_raw_800101d8", 0x800101D8, (("D_800101D8", 0x800101D8, 4),)),
        ("spanish_raw_8009c398", 0x8009C398, (
            ("D_8009C02B", 0x8009C44B, 1), ("D_8009B118", 0x8009C4B0, 4),
            ("D_8009B0F4", 0x8009C460, 4), ("D_8009B134", 0x8009C484, 4),
        )),
        ("image_after_viewport", 0x8009C4C4, (
            ("gText_abColorSlots", 0x801BF98C, 5), ("D_800E9D70", 0x8009C838, 16),
            ("gLibrary_aCardArtRecord", 0x801DC000, 0x2600), ("D_801AF000", 0x801AF000, 0x1000),
            ("gFade_State", 0x800EB248, 40),
            ("gInput_wPad1Pressed", 0x8009C72C, 2), ("gInput_wPad1Repeat", 0x8009C728, 2),
            ("gSD_bOutputType", 0x8009C784, 1),
        )),
    )

    def assert_attempt_history(self):
        with (ROOT / "notes/overlays/spanish-options-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual(len(rows), len(self.helpers) + 1)
        step = [row for row in rows if int(row["function_offset"], 0) == 0xF70]
        self.assertEqual(len(step), 2)
        self.assertEqual(step[0]["fingerprint"],
                         "13fcb2c33cd9a463d83bb028b2b39086d71ae018f34f4de2e00e72aae77d2d2d")
        self.assertEqual([(row["result"], row["instruction_bytes"], row["different_words"])
                          for row in step], [("matched", "192", "0")] * 2)
        latest = {int(row["function_offset"], 0): row for row in rows}
        self.assertEqual(set(latest), {offset for offset, _, _ in self.helpers})
        for offset, size, stem in self.helpers:
            row = latest[offset]
            self.assertEqual(int(row["function_offset"], 0), offset)
            self.assertEqual((row["module"], row["profile"], row["result"]),
                             (self.module_name, self.profiles.get(stem, "gcc_2_8_1_g0_split"), "matched"))
            self.assertEqual((int(row["instruction_bytes"]), row["different_words"]), (size, "0"))
            source = ROOT / f"src/overlays/pal_options/{stem}.c"
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())

    def test_suffix_closed_leaf_bodies_and_unproven_callers(self):
        from tools.project.overlay_function_inventory import walk_function

        image = self.retail_image()
        for start, size in ((0x1D78, 452), (0x1F3C, 56)):
            cfg = walk_function(image, 0x80168000, start, size)
            self.assertTrue(cfg["closed"])
            self.assertFalse(cfg["external"])
            self.assertFalse(cfg["indirect_calls"])
        executable = ROOT / f"game/{self.region}/{self.executable_name}"
        if not executable.is_file():
            self.skipTest("Legal Spanish executable required for resident caller scan")
        resident = executable.read_bytes()
        self.assertEqual(hashlib.sha256(resident).hexdigest(),
                         "b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790")
        for blob in (image, resident):
            words = struct.unpack(f"<{len(blob) // 4}I", blob)
            jumps = {0x80000000 | ((word & 0x3FFFFFF) << 2)
                     for word in words if word >> 26 in (2, 3)}
            for address in (0x80169D78, 0x80169F3C):
                self.assertNotIn(address, jumps)
                self.assertNotIn(address, words)

    def test_spanish_bindings_are_independently_pinned(self):
        bindings = {
            name: int(value, 0) for name, value in re.findall(
                r"^(\w+) = (0x[0-9A-F]+);$", (ROOT / self.module()["linker_symbols"]).read_text(), re.M)
        }
        self.assertEqual(bindings, {
            "gText_abColorSlots": 0x801BF98C, "D_8009C02B": 0x8009C44B,
            "DisplayObject_UpdateResourceVariant": 0x80040748,
            "func_80043BC8": 0x80043DC8, "func_80043B7C": 0x80043D7C,
            "D_800E9D70": 0x8009C838, "D_8009B118": 0x8009C4B0,
            "StoreImage": 0x8007FF70, "LoadImage": 0x8007FF10, "DrawSync": 0x8007FC64,
            "Fade_StartIn": 0x800156F8, "Fade_StartOut": 0x80015820,
            "SD_BGMFadeOut": 0x80040258, "gFade_State": 0x800EB248,
            "D_8009B0F4": 0x8009C460, "D_8009B134": 0x8009C484,
            "gInput_wPad1Pressed": 0x8009C72C, "gInput_wPad1Repeat": 0x8009C728,
            "gSD_bOutputType": 0x8009C784, "SD_SetOutputType": 0x80047430,
            "SD_SEPlayFull": 0x80040204, "DisplayObject_SetResourceVariant": 0x80040734,
            "rcos": 0x800866F8,
            "GsSortPoly": 0x800842A8, "D_801AF000": 0x801AF000,
            "SetPolyGT4": 0x80082EE8, "GsSortFastSprite": 0x80084978,
            "DisplayObject_FindFreeGeneralSlot": 0x80040350,
            "DisplayObject_AcquireSlot": 0x800403D0,
            "DisplayObject_ConfigureSpriteAtPosition": 0x80040800,
            "DisplayObject_ConfigureSpriteAtPositionWithResource": 0x80042BD8,
            "DisplayObject_SetDepthOffset": 0x80042C1C, "SD_BGMPlay": 0x8004022C,
        })

    def test_missing_spanish_inputs_skip_before_open(self):
        module = self.module()
        with patch.object(self, "module", return_value=module), \
                patch.object(Path, "is_file", return_value=False), \
                patch.object(Path, "open", side_effect=AssertionError("opened unavailable input")):
            with self.assertRaisesRegex(unittest.SkipTest, "Spanish WA"):
                self.retail_image()
            with self.assertRaisesRegex(unittest.SkipTest, "Spanish executable"):
                self.test_resident_load_pointer_and_options_entrypoints()

    def test_third_input_pointer_and_byte_selectors(self):
        image = self.retail_image()
        for offset, word in (
            (0x940, 0x0C0100D4), (0x948, 0x00402021), (0x94C, 0x0C0100F4),
            (0x954, 0x00408021), (0x994, 0x3C028017), (0x998, 0xAC509078),
            (0xB6C, 0xA0500068), (0xB78, 0x90850069),
            (0xB90, 0xA0500068), (0xB9C, 0x90850069), (0xE6C, 0x0C05A293),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))

    def test_wave_table_store_extents_and_signed_division(self):
        image = self.retail_image()
        for offset, word in (
            (0xC38, 0x24420100), (0xC3C, 0xAC629144),
            (0xC6C, 0x0C0219BE), (0xC70, 0x32440FFF),
            (0xC88, 0x24420FFF), (0xC8C, 0x00021B03),
            (0xC94, 0xAC830000), (0xCD4, 0xAC820000),
            (0xCE4, 0x2A220009), (0xCEC, 0x26100004),
            (0xCF0, 0x26B50024), (0xCF8, 0x2A820005), (0xD00, 0x26D60024),
        ):
            self.assertEqual(struct.unpack_from("<I", image, offset)[0], word, hex(offset))
        for offset in (0x1080, 0x1148):
            self.assertEqual(dict(self.data_owners)[offset], 5 * 9 * 4)


if __name__ == "__main__":
    unittest.main()
