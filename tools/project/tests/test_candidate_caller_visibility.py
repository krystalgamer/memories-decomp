"""Callers of the #3859 mixed -G candidates must see a declaration.

GCC 2.8.1 accepts a call with no visible prototype and treats the callee as
`int f()`. The build stays byte-exact, and the ownership checks
(unmatched_contracts.py, the candidate contract fingerprints) only ask
whether a declaration exists somewhere in the tree, so a caller that loses
its include goes unnoticed. That happened when #3878 moved these prototypes
to unmatched.h: five callers kept including the old headers and silently fell
back to implicit declarations.

Each pair below is compiled with its recorded profile's front-end flags and
-Wimplicit-function-declaration, and must report no implicit declaration of
the callee. The negative control removes a caller's unmatched.h include while
the global prototype stays in place, and requires the compiler to report the
implicit call, so this test fails for exactly the regression it guards.
"""

from __future__ import annotations

import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
CONFIG = REPOSITORY / "config/slus_01411"
COMPILER = "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
IMPLICIT = re.compile(r"implicit declaration of function `([^']+)'")
FRONT_END_FLAG = re.compile(r"^-(?:D|U|I|G|m|O|f(?!no-builtin$))")

# (caller, callee, include that provides the callee's declaration)
PAIRS = [
    ("src/candidates/func_800283F4.c", "func_80029164", '#include "../game/duel_effect_resource_setup.h"'),
    ("src/candidates/func_8004A2F8.c", "SD_SetVoiceVolume", '#include "../unmatched.h"'),
    ("src/candidates/func_8004B734.c", "func_8004AAFC", '#include "../game/sound.h"'),
    ("src/candidates/password/func_8016A37C.c", "func_80029164", '#include "../../game/duel_effect_resource_setup.h"'),
    ("src/game/func_80049BAC.c", "func_8004A518", '#include "../unmatched.h"'),
    ("src/game/fade_update.c", "Fade_StepBands", '#include "fade.h"'),
    ("src/candidates/func_8004A518.c", "func_8004A764", '#include "../game/sound.h"'),
    ("src/game/func_8004AAFC.c", "func_8004A43C", '#include "sound.h"'),
    ("src/candidates/func_80024E58.c", "SD_SEPlayFull", '#include "../game/sound.h"'),
    ("src/candidates/func_80024E58.c", "func_80040410", '#include "../game/display_object_config.h"'),
    ("src/candidates/func_80028B08.c", "func_80042188", '#include "../game/display_object_list_renderers.h"'),
    ("src/candidates/func_80041068.c", "func_80042188", '#include "../game/display_object_list_renderers.h"'),
    ("src/game/func_80040DD8.c", "func_80042188", '#include "display_object_list_renderers.h"'),
]


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


def implicit_calls(path: Path, profile: dict[str, object], include_dir: Path) -> set[str]:
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
            f"-I{include_dir}",
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
    return set(IMPLICIT.findall(completed.stderr))


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
        # The regression the review reproduced: func_80049BAC.c without its
        # unmatched.h include, with func_8004A518's prototype still there.
        unmatched = (REPOSITORY / "src/unmatched.h").read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "unmatched.h" not in include:
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(unmatched, rf"(?m)^(?![ \t]*/?\*)[^\n]*\b{callee}\s*\(")
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                (REPOSITORY / "tmp").mkdir(exist_ok=True)
                scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
                try:
                    # Nested so the probe's own relative includes cannot
                    # resolve beside it; -I points them at the real directory.
                    probe = scratch / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    probe.write_text(text.replace(include + "\n", "", 1), encoding="utf-8")
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                finally:
                    shutil.rmtree(scratch, ignore_errors=True)
                self.assertIn(callee, found)


if __name__ == "__main__":
    unittest.main()
