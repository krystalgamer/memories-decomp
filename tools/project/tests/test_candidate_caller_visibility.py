"""Callers must see their canonical function declarations.

GCC 2.8.1 accepts a call with no visible prototype and treats the callee as
`int f()`. The build stays byte-exact, and the ownership checks
(unmatched_contracts.py, the candidate contract fingerprints) only ask
whether a declaration exists somewhere in the tree, so a caller that loses
its include goes unnoticed. That happened when #3878 moved these prototypes
to unmatched.h: five callers kept including the old headers and silently fell
back to implicit declarations.

Each pair below is compiled with its recorded profile's front-end flags and
-Wimplicit-function-declaration, and must report no implicit declaration of
the callee. The negative control removes a caller's declaration-providing
include while the prototype stays in its owning header, and requires the
compiler to report the implicit call, so this test fails for exactly the
regression it guards.
The shared CD position-conversion declarations are covered at every caller too.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))
import candidate_builds

CONFIG = REPOSITORY / "config/slus_01411"
COMPILER = "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
IMPLICIT = re.compile(r"implicit declaration of function `([^']+)'")
FRONT_END_FLAG = re.compile(r"^-(?:D|U|I|G|m|O|f(?!no-builtin$))")

# (caller, callee, include that provides the callee's declaration)
PAIRS = [
    ("src/candidates/func_800283F4.c", "func_80029164", '#include "../game/duel_effect_resource_setup.h"'),
    ("src/game/sound_spatialization.c", "SD_SetVoiceVolume", '#include "../unmatched.h"'),
    ("src/game/sound_secondary_commands.c", "func_8004AAFC", '#include "sound.h"'),
    ("src/candidates/password/func_8016A37C.c", "func_80029164", '#include "../../game/duel_effect_resource_setup.h"'),
    ("src/game/func_8004A6D8.c", "func_8004A518", '#include "sound.h"'),
    ("src/game/fade_update.c", "Fade_StepBands", '#include "fade.h"'),
    ("src/game/sound_voice_setup.c", "SD_ResetVoiceEnvelope", '#include "sound.h"'),
    ("src/game/func_8004AAFC.c", "func_8004A43C", '#include "sound.h"'),
    ("src/game/func_8001B938.c", "func_8004036C", '#include "display_object_core.h"'),
    ("src/game/debug_menu_campaign_entry.c", "func_8004036C", '#include "display_object_core.h"'),
    ("src/game/func_800218F0.c", "func_800400AC", '#include "display_object_core.h"'),
    ("src/game/func_800218F0.c", "func_800404CC", '#include "display_object_config.h"'),
    ("src/game/func_800262D4.c", "func_8004036C", '#include "display_object_core.h"'),
    ("src/game/func_80024E58.c", "SD_SEPlayFull", '#include "sound.h"'),
    ("src/game/func_80024E58.c", "func_80040410", '#include "display_object_config.h"'),
    ("src/game/file_stream.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/game/func_80014294.c", "CdIntToPos_8007E600", '#include "file_cd_helpers.h"'),
    ("src/game/func_80014294.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/game/movie_stream_requests.c", "CdIntToPos_8007E600", '#include "file_cd_helpers.h"'),
    ("src/game/movie_stream_requests.c", "CdPosToInt_8007E710", '#include "file_cd_helpers.h"'),
    ("src/candidates/func_80028B08.c", "func_80042188", '#include "../game/display_object_packet_submit.h"'),
    ("src/game/func_80040DD8.c", "func_80042188", '#include "display_object_packet_submit.h"'),
    ("src/game/func_80041068.c", "func_80042188", '#include "display_object_packet_submit.h"'),
    ("src/game/model_load_step.c", "func_8004CB0C", '#include "model_slot_setup.h"'),
    ("src/game/model_intro_controller.c", "func_80056828", '#include "model_load_step.h"'),
    ("src/game/func_80050584.c", "func_80056828", '#include "model_load_step.h"'),
    ("src/game/func_800534B8.c", "func_80056828", '#include "model_load_step.h"'),
    ("src/game/func_8004CB0C.c", "func_8005A3D0", '#include "../game/model_parent_search.h"'),
    ("src/game/func_80024200.c", "func_800235C0", '#include "duel_field_display_objects.h"'),
    ("src/game/duel_field_display_objects.c", "func_80018150", '#include "duel_card_object_helpers.h"'),
    ("src/game/func_80031874.c", "func_80031784", '#include "func_80031784.h"'),
    ("src/game/func_80031874.c", "func_800316F0", '#include "duel_card_stat_display.h"'),
    ("src/game/func_80031874.c", "Text_EncodeDecimalNoPadding", '#include "text_encode_decimal_no_padding.h"'),
    ("src/game/func_8002A3CC.c", "func_8002A2F4", '#include "func_8002A2F4.h"'),
    ("src/game/func_8002BD0C.c", "func_8002A2F4", '#include "func_8002A2F4.h"'),
]

PACKET_SUBMIT_CANDIDATES = (
    "src/candidates/func_80028B08.c",
)


def profiles_by_source() -> dict[str, str]:
    result: dict[str, str] = {}
    manifests = [CONFIG / "matching_c.json"]
    manifests += sorted((CONFIG / "overlays").glob("*_matching_c.json"))
    for manifest in manifests:
        for entry in json.loads(manifest.read_text(encoding="utf-8"))["functions"]:
            result[entry["source"]] = entry["profile"]
    for entry in json.loads((CONFIG / "candidates.json").read_text(encoding="utf-8"))["candidates"]:
        result[entry["source"]] = entry["profile"]
    return result


def compiler_diagnostics(
    path: Path,
    profile: dict[str, object],
    include_dir: Path,
    *,
    warnings_as_errors: bool = False,
) -> str:
    flags = [str(f) for f in profile["compiler_flags"] if FRONT_END_FLAG.match(str(f))]  # type: ignore[index]
    completed = subprocess.run(
        [
            str(REPOSITORY / str(profile["compiler"])),
            # -S stops before the assembler, which rejects -G on its own
            # command line; the output is discarded.
            "-S",
            "-o",
            os.devnull,
            "-Wimplicit-function-declaration",
            *(["-Werror"] if warnings_as_errors else []),
            f"-I{include_dir}",
            f"-I{REPOSITORY / 'src'}",
            *flags,
            str(path),
        ],
        cwd=REPOSITORY,
        capture_output=True,
        text=True,
        errors="replace",
    )
    if completed.returncode != 0:
        raise AssertionError(f"{path} does not compile:\n{completed.stderr}")
    return completed.stderr


def implicit_calls(
    path: Path,
    profile: dict[str, object],
    include_dir: Path,
    *,
    warnings_as_errors: bool = False,
) -> set[str]:
    return set(IMPLICIT.findall(compiler_diagnostics(
        path, profile, include_dir, warnings_as_errors=warnings_as_errors,
    )))


@unittest.skipUnless((REPOSITORY / COMPILER).is_file(), "needs the GCC 2.8.1 toolchain")
class CandidateCallerVisibilityTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.profiles = json.loads(
            (CONFIG / "compiler_profiles.json").read_text(encoding="utf-8")
        )["profiles"]
        cls.sources = profiles_by_source()

    def profile(self, source: str) -> dict[str, object]:
        self.assertIn(source, self.sources, f"{source} is not a built source")
        return self.profiles[self.sources[source]]

    def test_callers_see_their_callees(self) -> None:
        for source, callee, _ in PAIRS:
            with self.subTest(source=source, callee=callee):
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertRegex(text, rf"\b{callee}\s*\(", "the call is gone")
                found = implicit_calls(
                    REPOSITORY / source,
                    self.profile(source),
                    (REPOSITORY / source).parent,
                )
                self.assertNotIn(callee, found)

    def test_lost_include_is_caught_while_the_prototype_still_exists(self) -> None:
        # Keep each selected prototype in its owner while hiding that owner
        # from the caller, reproducing the lost-include regression.
        unmatched = (REPOSITORY / "src/unmatched.h").read_text(encoding="utf-8")
        sound = (REPOSITORY / "src/game/sound.h").read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "unmatched.h" in include:
                declarations = unmatched
            elif callee == "func_8004A518":
                declarations = sound
            else:
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(
                    declarations,
                    rf"(?m)^(?![ \t]*/?\*)[^\n]*\b{callee}\s*\(",
                )
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                (REPOSITORY / "tmp").mkdir(exist_ok=True)
                scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
                try:
                    # Nested so ordinary relative includes still resolve
                    # through -I, while this probe can supply a shadow owner
                    # with only the selected prototype removed.
                    probe = scratch / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    include_path = re.search(r'"([^"]+)"', include)
                    self.assertIsNotNone(include_path)
                    shadow = (probe.parent / include_path.group(1)).resolve()
                    shadow.parent.mkdir(parents=True, exist_ok=True)
                    without_callee, count = re.subn(
                        rf"(?m)^[^\n]*\b{callee}\s*\([^;]*;\s*$",
                        "",
                        declarations,
                        count=1,
                    )
                    self.assertEqual(count, 1)
                    shadow.write_text(without_callee, encoding="utf-8")
                    probe.write_text(text, encoding="utf-8")
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                finally:
                    shutil.rmtree(scratch, ignore_errors=True)
                self.assertIn(callee, found)

    def test_lost_cd_header_is_caught_while_sdk_types_remain_visible(self) -> None:
        header = (REPOSITORY / "src/game/file_cd_helpers.h").read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "file_cd_helpers.h" not in include:
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(header, rf"\b{callee}\s*\(")
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    probe.write_text(
                        text.replace(include, '#include "../psyq/libcd.h"', 1),
                        encoding="utf-8",
                    )
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                self.assertIn(callee, found)

    def test_lost_shared_owner_header_is_caught(self) -> None:
        for source, callee, include in PAIRS:
            if not any(
                owner in include
                for owner in (
                    "display_object_packet_submit.h",
                    "func_8002A2F4.h",
                    "model_slot_setup.h",
                    "model_load_step.h",
                    "duel_field_display_objects.h",
                    "duel_card_object_helpers.h",
                    "func_80031784.h",
                    "duel_card_stat_display.h",
                    "text_encode_decimal_no_padding.h",
                )
            ):
                continue
            with self.subTest(source=source):
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    probe.write_text(
                        text.replace(include + "\n", "", 1),
                        encoding="utf-8",
                    )
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                self.assertIn(callee, found)

    def test_packet_submit_contract_tracks_declaration_changes(self) -> None:
        entries = {
            entry["source"]: entry
            for entry in json.loads(
                (CONFIG / "candidates.json").read_text(encoding="utf-8")
            )["candidates"]
        }
        index = candidate_builds.canonical_declaration_index({"func_80042188"})
        declarations = index["func_80042188"]
        expected = candidate_builds.canonical_symbol_contract_hash(
            "func_80042188", declarations
        )

        for source in PACKET_SUBMIT_CANDIDATES:
            with self.subTest(source=source):
                self.assertEqual(
                    entries[source]["canonical_contracts"]["func_80042188"],
                    expected,
                )

        changed = [
            (path, statement.replace("s32 mode", "u32 mode", 1))
            for path, statement in declarations
        ]
        changed_hash = candidate_builds.canonical_symbol_contract_hash(
            "func_80042188", changed
        )
        self.assertNotEqual(expected, changed_hash)

    def test_model_dispatch_uses_shared_owner_view_after_promotion(self) -> None:
        candidates = json.loads(
            (CONFIG / "candidates.json").read_text(encoding="utf-8")
        )["candidates"]
        self.assertFalse(any(candidate["address"] == "0x80056828" for candidate in candidates))
        entry = next(
            candidate
            for candidate in json.loads(
                (CONFIG / "matching_c.json").read_text(encoding="utf-8")
            )["functions"]
            if candidate["address"] == "0x80056828"
        )
        source = "src/game/model_load_step.c"
        self.assertEqual(entry["source"], source)
        self.assertEqual(entry["profile"], "gcc_2_8_1_g8_split")
        declarations = candidate_builds.canonical_declaration_index(
            {"func_8004CB0C"}
        )["func_8004CB0C"]
        self.assertEqual(
            {statement for _, statement in declarations},
            {
                "void func_8004CB0C(s32 slot, s32 arg1, s32 arg2, s32 arg3);",
                "void func_8004CB0C(void);",
            },
        )
        expected = candidate_builds.canonical_symbol_contract_hash(
            "func_8004CB0C", declarations
        )
        path = REPOSITORY / source
        text = path.read_text(encoding="utf-8")
        selector = "#define MODEL_SLOT_SETUP_EXPLICIT_TRANSFER_ARGS\n"
        self.assertEqual(text.count(selector), 1)
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
            probe = Path(directory) / "a/b/c" / path.name
            probe.parent.mkdir(parents=True)
            probe.write_text(text.replace(selector, "", 1), encoding="utf-8")
            with self.assertRaisesRegex(AssertionError, "too many arguments to function"):
                compiler_diagnostics(probe, self.profile(source), path.parent)

        changed = [
            (path, statement.replace("s32 arg3", "u32 arg3", 1))
            for path, statement in declarations
        ]
        changed_hash = candidate_builds.canonical_symbol_contract_hash(
            "func_8004CB0C", changed
        )
        self.assertNotEqual(expected, changed_hash)

    def test_candidate_sprite_records_cannot_be_swapped(self) -> None:
        source = "src/candidates/func_80028B08.c"
        path = REPOSITORY / source
        profile = self.profile(source)
        original = path.read_text(encoding="utf-8")
        call = "func_80042188(PRM, CTX, arg1, arg, EXT);"
        swapped = "func_80042188(PRM, EXT, arg1, arg, CTX);"
        self.assertIn(call, original)
        base_diagnostics = compiler_diagnostics(path, profile, path.parent)
        self.assertNotRegex(
            base_diagnostics,
            r"passing arg [25] of `func_80042188' from incompatible pointer type",
        )

        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
        try:
            probe = scratch / "a/b/c" / path.name
            probe.parent.mkdir(parents=True)
            probe.write_text(original.replace(call, swapped, 1), encoding="utf-8")
            diagnostics = compiler_diagnostics(probe, profile, path.parent)
        finally:
            shutil.rmtree(scratch, ignore_errors=True)
        self.assertRegex(
            diagnostics,
            r"passing arg 2 of `func_80042188' from incompatible pointer type",
        )
        self.assertRegex(
            diagnostics,
            r"passing arg 5 of `func_80042188' from incompatible pointer type",
        )

    def test_parent_search_caller_needs_owning_header(self) -> None:
        source = "src/game/func_8004CB0C.c"
        include = '#include "../game/model_parent_search.h"\n'
        text = (REPOSITORY / source).read_text(encoding="utf-8")
        self.assertIn(include, text)
        self.assertNotRegex(text, r"extern[^\n]*\bfunc_8005A3D0\b")
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "a/b/c/caller.c"
            path.parent.mkdir(parents=True)
            path.write_text(text.replace(include, "", 1), encoding="utf-8")
            found = implicit_calls(
                path, self.profile(source), (REPOSITORY / source).parent
            )
        self.assertIn("func_8005A3D0", found)

    def test_parent_search_has_one_pointer_view(self) -> None:
        definition = "src/game/func_8005A3D0.c"
        self.assertIn(
            '#include "model_parent_search.h"',
            (REPOSITORY / definition).read_text(encoding="utf-8"),
        )
        views = [("ModelSlot *, void *", True), ("u8 *, GsCOORDUNIT *", False)]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "view.c"
            for parameters, accepted in views:
                with self.subTest(parameters=parameters):
                    path.write_text(
                        '#include "model_parent_search.h"\n'
                        + f"s32 (*view)({parameters}) = func_8005A3D0;\n",
                        encoding="utf-8",
                    )
                    if accepted:
                        self.assertEqual(
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            ),
                            set(),
                        )
                    else:
                        with self.assertRaisesRegex(AssertionError, "incompatible"):
                            implicit_calls(
                                path, self.profile(definition),
                                REPOSITORY / "src/game",
                                warnings_as_errors=True,
                            )


if __name__ == "__main__":
    unittest.main()
