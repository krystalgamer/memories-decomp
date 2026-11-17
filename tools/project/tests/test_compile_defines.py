from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import build_baseline
import compile_defines
import overlay_extract
from compile_defines import DefineError


class DefineValidationTests(unittest.TestCase):
    def test_valid_defines_keep_their_order(self) -> None:
        self.assertEqual(
            compile_defines.validate_defines(
                ["VERSION_EUROPE", "_HEIGHT=0x100"], "entry"
            ),
            ("VERSION_EUROPE", "_HEIGHT=0x100"),
        )
        self.assertEqual(compile_defines.validate_defines(None, "entry"), ())
        self.assertEqual(
            compile_defines.define_flags(("A", "B=1")), ["-DA", "-DB=1"]
        )

    def test_invalid_names_and_duplicates_are_rejected(self) -> None:
        for value in (
            "VERSION_EUROPE",
            ["version_europe"],
            ["1ABC"],
            ["A B"],
            ["A="],
            ["A=1 2"],
            ["-DA"],
            [1],
            ["A", "A=1"],
        ):
            with self.subTest(value=value):
                with self.assertRaises(DefineError):
                    compile_defines.validate_defines(value, "entry")
        with self.assertRaisesRegex(DefineError, "duplicate define VERSION_EUROPE"):
            compile_defines.combine_defines(
                ("VERSION_EUROPE",), ("VERSION_EUROPE",), "entry"
            )


class WorkspaceTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="compile-defines-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def write(self, relative: str, text: str) -> Path:
        path = self.root / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path


class CompileCommandTests(WorkspaceTests):
    PROFILES = {
        "g0": {
            "compiler": "tools/gcc",
            "compiler_flags": ["-O2", "-G0"],
            "maspsx_flags": [],
            "aspsx_version": "2.81",
            "data_limit": 0,
        }
    }

    def setUp(self) -> None:
        super().setUp()
        self.write("src/example.c", "int Example;\n")
        self.write("tools/gcc", "")
        self.write("tools/vendor/maspsx/maspsx.py", "")
        self.commands: list[list[str]] = []

        def stop(root: Path, command: list[str]) -> None:
            self.commands.append(command)
            raise build_baseline.BuildError("stop after the compiler command")

        patcher = patch.object(build_baseline, "run", side_effect=stop)
        patcher.start()
        self.addCleanup(patcher.stop)

    def compile(self, segment: dict[str, object]) -> None:
        build_baseline.compile_c(
            self.root,
            self.root / "as",
            {"source": "src/example.c", "object": "example.o", "profile": "g0",
             **segment},
            self.PROFILES,
            object_directory="obj",
            asm_directory="asm",
        )

    def compiler_command(self, segment: dict[str, object]) -> list[str]:
        with self.assertRaisesRegex(build_baseline.BuildError, "stop after"):
            self.compile(segment)
        return self.commands[0]

    def test_defines_follow_the_profile_flags(self) -> None:
        command = self.compiler_command({"defines": ["VERSION_EUROPE", "HEIGHT=256"]})
        self.assertEqual(
            command[1:7],
            ["-S", "-O2", "-G0", "-DVERSION_EUROPE", "-DHEIGHT=256", "-o"],
        )

    def test_no_defines_leave_the_command_unchanged(self) -> None:
        self.assertEqual(self.compiler_command({})[1:5], ["-S", "-O2", "-G0", "-o"])

    def test_invalid_define_is_rejected_before_compiling(self) -> None:
        with self.assertRaisesRegex(build_baseline.BuildError, "invalid define"):
            self.compile({"defines": ["bad name"]})
        self.assertEqual(self.commands, [])


class ManifestCheckTests(WorkspaceTests):
    def manifests(self, region: object, entry: object, data: object = None) -> None:
        self.write(
            "config/sles_03947/matching_c.json",
            json.dumps({"schema": 1, "defines": region, "functions": [
                {"address": "0x80010000", "source": "src/game/a.c", "defines": entry}
            ]}),
        )
        if data is not None:
            self.write(
                "config/sles_03947/overlays/m_data_c.json",
                json.dumps({"schema": 1, "units": [
                    {"source": "src/overlays/m/a.c", "defines": data}
                ]}),
            )

    def test_valid_manifests_pass(self) -> None:
        self.manifests([], ["VERSION_EUROPE"], ["VERSION_EUROPE=1"])
        overlay_extract.verify_manifest_defines(self.root)

    def test_invalid_or_repeated_defines_are_rejected(self) -> None:
        for region, entry, data in (
            (["bad"], [], None),
            ([], ["VERSION EUROPE"], None),
            (["VERSION_EUROPE"], ["VERSION_EUROPE"], None),
            ([], [], ["lower"]),
        ):
            with self.subTest(region=region, entry=entry, data=data):
                self.manifests(region, entry, data)
                with self.assertRaises(overlay_extract.OverlayError):
                    overlay_extract.verify_manifest_defines(self.root)

    def test_top_level_defines_belong_to_the_region_manifest(self) -> None:
        self.manifests([], [])
        self.write(
            "config/sles_03947/overlays/m_matching_c.json",
            json.dumps({"schema": 1, "defines": [], "functions": []}),
        )
        with self.assertRaisesRegex(overlay_extract.OverlayError, "top-level"):
            overlay_extract.verify_manifest_defines(self.root)



class ObjectOverrideCheckTests(WorkspaceTests):
    def manifest(
        self,
        *entries: dict[str, object],
        split: str = "options:\n  create_c_files: false\n",
    ) -> None:
        self.write("src/game/a.c", "void A(void) {}\n")
        self.write("src/game/b.c", "void B(void) {}\n")
        self.write("config/sles_03947/split.yaml", split)
        self.write(
            "config/sles_03947/matching_c.json",
            json.dumps({"schema": 1, "functions": [
                {"address": "0x80010000", "source": "src/game/a.c"}, *entries
            ]}),
        )

    def test_valid_override_passes(self) -> None:
        self.manifest(
            {"address": "0x80010010", "source": "src/game/a.c", "object": "game/a_second"},
            {"address": "0x80010020", "source": "src/game/a.c", "object": "game/a_second"},
        )
        overlay_extract.verify_object_overrides(self.root)

    def test_invalid_overrides_are_rejected(self) -> None:
        for entry, split, message in (
            ({"object": "game/a_second"}, "options: {}\n", "create_c_files"),
            ({"object": "game/b"}, None, "existing source"),
            ({"object": "game/a"}, None, "repeats the name"),
            ({"object": "game/../a"}, None, "invalid object name"),
            ({"object": "game/a two"}, None, "invalid object name"),
        ):
            with self.subTest(entry=entry):
                self.manifest(
                    {"address": "0x80010010", "source": "src/game/a.c", **entry},
                    **({"split": split} if split is not None else {}),
                )
                with self.assertRaisesRegex(overlay_extract.OverlayError, message):
                    overlay_extract.verify_object_overrides(self.root)

    def test_an_object_is_built_from_one_source(self) -> None:
        self.manifest(
            {"address": "0x80010010", "source": "src/game/a.c", "object": "game/x"},
            {"address": "0x80010020", "source": "src/game/b.c", "object": "game/x"},
        )
        with self.assertRaisesRegex(overlay_extract.OverlayError, "also built from"):
            overlay_extract.verify_object_overrides(self.root)

    def test_overrides_belong_to_resident_manifests(self) -> None:
        self.manifest()
        self.write(
            "config/sles_03947/overlays/m_matching_c.json",
            json.dumps({"schema": 1, "functions": [
                {"address": "0x80100000", "source": "src/overlays/m/a.c",
                 "object": "overlays/m/x"}
            ]}),
        )
        with self.assertRaisesRegex(overlay_extract.OverlayError, "resident"):
            overlay_extract.verify_object_overrides(self.root)

if __name__ == "__main__":
    unittest.main()
