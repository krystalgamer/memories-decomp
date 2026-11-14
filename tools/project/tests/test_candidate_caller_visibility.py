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
    ("src/game/sound_effect_request.c", "SD_AllocateSoundEffectVoice", '#include "sound_voice_allocator.h"'),
    ("src/game/sound_effect_request.c", "SD_EnqueueCdSePlay", '#include "sound_output_state.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_KeyOffVoiceGroup", '#include "sound_voice_selection.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_StartSoundEffectVoice", '#include "sound_voice_selection.h"'),
    ("src/game/sd_allocate_sound_effect_voice.c", "SD_EnqueueCdSePlay", '#include "sound_output_state.h"'),
    ("src/game/display_object_system.c", "DisplayObject_ProjectPackedRotationFacing", '#include "display_object_system.h"'),
    ("src/game/duel_effect_states.c", "CardPreview_RequestResourceTransfer", '#include "card_preview.h"'),
    ("src/game/duel_effect_states.c", "CardPreview_CreateResourceObjects", '#include "card_preview.h"'),
    ("src/game/library.c", "CardPreview_RequestResourceTransfer", '#include "card_preview.h"'),
    ("src/game/library.c", "CardPreview_CreateResourceObjects", '#include "card_preview.h"'),
    ("src/game/duel_scene.c", "CardPreview_RequestResourceTransfer", '#include "card_preview.h"'),
    ("src/game/duel_scene.c", "CardPreview_CreateResourceObjects", '#include "card_preview.h"'),
    ("src/overlays/password/shop.c", "CardPreview_CreateResourceObjects", '#include "../../game/card_preview.h"'),
    ("src/game/sound_spatialization.c", "SD_SetVoiceVolume", '#include "sound.h"'),
    ("src/game/sound_secondary_commands.c", "SD_UpdateSecondaryObjectVoices", '#include "sound.h"'),
    ("src/overlays/password/shop.c", "CardPreview_RequestResourceTransfer", '#include "../../game/card_preview.h"'),
    ("src/game/sound_secondary_playback.c", "SD_ResetSecondaryPlayback", '#include "sound.h"'),
    ("src/game/fade_runtime.c", "Fade_StepBands", '#include "fade.h"'),
    ("src/game/sound_voice_setup.c", "SD_ResetVoiceEnvelope", '#include "sound.h"'),
    ("src/game/sd_update_secondary_object_voices.c", "SD_UpdateSecondaryObjectPitch", '#include "sound.h"'),
    ("src/game/duel_scene.c", "DisplayObject_ReleaseIfPresent", '#include "display_object_system.h"'),
    ("src/game/debug_menu.c", "DisplayObject_ReleaseIfPresent", '#include "display_object_system.h"'),
    ("src/game/duel_scene.c", "DisplayObject_AcquireSlot", '#include "display_object_system.h"'),
    ("src/game/duel_scene.c", "DisplayObject_ConfigureSpriteAtPosition", '#include "display_object_system.h"'),
    ("src/game/duel_scene.c", "Duel_GetOrCreateActionReadyMarker", '#include "duel_scene.h"'),
    ("src/game/duel_scene.c", "SD_SEPlayFull", '#include "sound.h"'),
    ("src/game/duel_scene.c", "Duel_UpdateCardUseConfirmation", '#include "duel_scene.h"'),
    (
        "src/game/duel_scene.c",
        "DisplayObject_SetResourceVariant",
        '#include "display_object_system.h"',
    ),
    ("src/game/file_transfer_runtime.c", "CdIntToPos_8007E600", '#include "movie_pipeline.h"'),
    ("src/game/file_transfer_runtime.c", "CdPosToInt_8007E710", '#include "movie_pipeline.h"'),
    ("src/game/movie_pipeline.c", "CdIntToPos_8007E600", '#include "movie_pipeline.h"'),
    ("src/game/movie_pipeline.c", "CdPosToInt_8007E710", '#include "movie_pipeline.h"'),
    ("src/game/card_preview.c", "DisplayObject_SubmitPacket", '#include "display_object_system.h"'),
    ("src/game/display_object_system.c", "DisplayObject_SubmitPacket", '#include "display_object_system.h"'),
    ("src/game/model.c", "Model_InitSlot", '#include "model_slot_control.h"'),
    ("src/game/model.c", "Model_AdvanceSlotLoad", '#include "model.h"'),
    ("src/game/model_init_slot.c", "Model_FindBranchingChildCoord", '#include "../game/model_scene_runtime.h"'),
    ("src/game/build_deck.c", "Text_EncodeDecimalNoPadding", '#include "text_encode_decimal_digits.h"'),
    ("src/game/main_mode_runners.c", "BuildDeck_InitScreen", '#include "build_deck.h"'),
    ("src/game/main_mode_runners.c", "BuildDeck_InitScreen", '#include "../game/build_deck.h"'),
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


# Folded owners also carry the unit's types and are reached transitively, so
# the shared-owner probe shadows them instead of dropping their include.
FOLDED_SHADOW_OWNERS = (
    "build_deck.h",
    "display_object_system.h",
    "duel_scene.h",
    "model.h",
)


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
        sound = (REPOSITORY / "src/game/sound.h").read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if callee == "SD_ResetSecondaryPlayback":
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
        header = (
            REPOSITORY / "src/psyq/libcd_abi_variants.h"
        ).read_text(encoding="utf-8")
        for source, callee, include in PAIRS:
            if "movie_pipeline.h" not in include:
                continue
            # The helpers now live in movie_pipeline.c's own header, which
            # that unit cannot lose; only outside callers are probed.
            if source == "src/game/movie_pipeline.c":
                continue
            with self.subTest(source=source, callee=callee):
                self.assertRegex(header, rf"\b{callee}\s*\(")
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    probe.write_text(
                        text.replace(include, '#include "../psyq/libcd.h"'),
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
                    "build_deck.h",
                    "display_object_system.h",
                    "card_preview.h",
                    "card_preview.h",
                    "model_slot_control.h",
                    "model.h",
                    "duel_scene.h",
                    "duel_scene.h",
                    "duel_scene.h",
                    "duel_scene.h",
                    "build_deck.h",
                    "text_encode_decimal_digits.h",
                )
            ):
                continue
            with self.subTest(source=source):
                text = (REPOSITORY / source).read_text(encoding="utf-8")
                self.assertIn(include + "\n", text)
                replacement = ""
                if source == "src/game/card_preview.c" and "display_object_system.h" in include:
                    # Preserve the real layout, not the lost function declaration.
                    owner = (REPOSITORY / "src/game/display_object_system.h").read_text()
                    layout = re.search(r"typedef struct \{[^}]*\} DisplayObjectPacketOrigin;", owner)
                    self.assertIsNotNone(layout)
                    replacement = (
                        '#include "../psyq/libgte.h"\n#include "../psyq/libgpu.h"\n'
                        + layout.group() + "\n"
                    )
                if "duel_scene.h" in include:
                    # Retain actual data types, not the lost function declarations.
                    replacement = (
                        '#include "duel_scene.h"\n'
                    )
                shadow = None
                shadow_name = "model_slot_control.h"
                folded = any(owner in include for owner in FOLDED_SHADOW_OWNERS)
                if folded:
                    # A callee this unit defines above its first call needs no
                    # prototype, so losing the owner header cannot hide it.
                    definition = re.search(
                        rf"(?m)^[^\n;#]*\b{callee}\s*\([^;]*\)\s*\{{", text
                    )
                    calls = [
                        m.start() for m in re.finditer(rf"\b{callee}\s*\(", text)
                        if not definition
                        or not definition.start() <= m.start() < definition.end()
                    ]
                    if definition and calls and definition.start() < calls[0]:
                        continue
                    # Keep the owner's types; lose only the callee's
                    # prototype, and include the shadow first so its guard
                    # wins over any transitive include of the real owner.
                    shadow_name = re.search(r'"([^"]+)"', include).group(1)
                    owner = (REPOSITORY / Path(source).parent / shadow_name).read_text()
                    shadow, count = re.subn(
                        rf"(?m)^(?![ \t]*(?:/?\*|//))[^\n]*\b{callee}\s*\([^;{{}}]*;\s*$",
                        "",
                        owner,
                        count=1,
                    )
                    self.assertEqual(count, 1)
                    replacement = ""
                elif text.count(include + "\n") > 1:
                    # A grouped unit repeats the owner include; removing one
                    # copy hides nothing, so shadow the owner with a copy
                    # that loses only the callee's declaration.
                    shadow_name = re.search(r'"([^"]+)"', include).group(1)
                    owner = (REPOSITORY / Path(source).parent / shadow_name).read_text()
                    shadow, count = re.subn(
                        rf"(?m)^[^\n]*\b{callee}\s*\([^;]*;\s*$", "", owner, count=1
                    )
                    self.assertEqual(count, 1)
                    replacement = include + "\n"
                elif "model_slot_control.h" in include:
                    # The unit needs the rest of its own header; shadow it
                    # with a copy that loses only the callee's declaration.
                    owner = (REPOSITORY / "src/game/model_slot_control.h").read_text()
                    prototype = (
                        "void Model_InitSlot(s32 slot, u8 *hmd, s32 size, s32 flags);\n"
                    )
                    self.assertEqual(owner.count(prototype), 1)
                    shadow = owner.replace(prototype, "", 1)
                    replacement = include + "\n"
                with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as directory:
                    probe = Path(directory) / "a/b/c" / Path(source).name
                    probe.parent.mkdir(parents=True)
                    if shadow is not None:
                        shadow_path = probe.parent / shadow_name
                        shadow_path.parent.mkdir(parents=True, exist_ok=True)
                        shadow_path.write_text(shadow, encoding="utf-8")
                    probe_text = text.replace(include + "\n", replacement, 1)
                    if folded:
                        head, sep, rest = probe_text.partition("#include")
                        line, nl, rest = rest.partition("\n")
                        probe_text = head + sep + line + nl + include + "\n" + rest
                    probe.write_text(probe_text, encoding="utf-8")
                    found = implicit_calls(
                        probe, self.profile(source), (REPOSITORY / source).parent
                    )
                self.assertIn(callee, found)

    def test_packet_submit_contract_tracks_declaration_changes(self) -> None:
        index = candidate_builds.canonical_declaration_index({"DisplayObject_SubmitPacket"})
        declarations = index["DisplayObject_SubmitPacket"]
        self.assertTrue(declarations)
        expected = candidate_builds.canonical_symbol_contract_hash(
            "DisplayObject_SubmitPacket", declarations
        )
        for entry in json.loads(
            (CONFIG / "candidates.json").read_text(encoding="utf-8")
        )["candidates"]:
            contract = entry.get("canonical_contracts", {}).get(
                "DisplayObject_SubmitPacket"
            )
            if contract is not None and entry.get("module") is None:
                with self.subTest(source=entry["source"]):
                    self.assertEqual(contract, expected)

        changed = [
            (path, statement.replace("s32 mode", "u32 mode", 1))
            for path, statement in declarations
        ]
        changed_hash = candidate_builds.canonical_symbol_contract_hash(
            "DisplayObject_SubmitPacket", changed
        )
        self.assertNotEqual(expected, changed_hash)

    def test_candidate_sprite_records_cannot_be_swapped(self) -> None:
        source = "src/game/card_preview.c"
        path = REPOSITORY / source
        profile = self.profile(source)
        original = path.read_text(encoding="utf-8")
        call = (
            "PRM, CARD_PANEL_PACKET_BYTES(packet), (GsOT *)ordering_table, arg,\n"
            "        CARD_PANEL_ORIGIN_BYTES(origin));"
        )
        swapped = (
            "PRM, CARD_PANEL_PACKET_BYTES(origin), (GsOT *)ordering_table, arg,\n"
            "        CARD_PANEL_ORIGIN_BYTES(packet));"
        )
        self.assertIn(call, original)
        base_diagnostics = compiler_diagnostics(path, profile, path.parent)
        self.assertNotRegex(
            base_diagnostics,
            r"(has no member named|passing arg [25] of `DisplayObject_SubmitPacket')",
        )

        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        scratch = Path(tempfile.mkdtemp(dir=REPOSITORY / "tmp"))
        try:
            probe = scratch / "a/b/c" / path.name
            probe.parent.mkdir(parents=True)
            probe.write_text(original.replace(call, swapped, 1), encoding="utf-8")
            with self.assertRaises(AssertionError) as failure:
                compiler_diagnostics(probe, profile, path.parent)
            diagnostics = str(failure.exception)
        finally:
            shutil.rmtree(scratch, ignore_errors=True)
        self.assertRegex(diagnostics, r"has no member named `tag'")
        self.assertRegex(diagnostics, r"has no member named `x'")

    def test_parent_search_caller_needs_owning_header(self) -> None:
        source = "src/game/model_init_slot.c"
        include = '#include "../game/model_scene_runtime.h"\n'
        text = (REPOSITORY / source).read_text(encoding="utf-8")
        self.assertIn(include, text)
        self.assertNotRegex(text, r"extern[^\n]*\bModel_FindBranchingChildCoord\b")
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "a/b/c/caller.c"
            path.parent.mkdir(parents=True)
            path.write_text(text.replace(include, "", 1), encoding="utf-8")
            found = implicit_calls(
                path, self.profile(source), (REPOSITORY / source).parent
            )
        self.assertIn("Model_FindBranchingChildCoord", found)

    def test_cursor_motion_object_and_direction_views(self) -> None:
        definition = "src/game/duel_scene.c"
        ambient = "(DuelCursorProcessGridDirectionAmbient)"
        views = [
            ("", "DuelSelectionRecord *, s32", True),
            ("", "DuelSelectionRecord *", False),
            ("", "DisplayObject *, s32", False),
            (ambient, "DuelSelectionRecord *", True),
            (ambient, "DuelSelectionRecord *, s32", False),
            (ambient, "DisplayObject *", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "cursor.c"
            for prefix, parameters, accepted in views:
                with self.subTest(prefix=prefix, parameters=parameters):
                    path.write_text(
                        '#include "duel_scene.h"\n'
                        + '#include "duel_scene.h"\n'
                        + "DisplayObject **object_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->cursor_object; }\n"
                        + "s8 *column_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->col; }\n"
                        + "s8 *row_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->row; }\n"
                        + "u8 *status_view(DuelSelectionRecord *cursor) "
                        + "{ return &cursor->status; }\n"
                        + "void (*held_input)(DuelSelectionRecord *) = DuelCursor_ProcessHeldGridDirection;\n"
                        + f"void (*step)({parameters}) = {prefix}DuelCursor_ProcessGridDirection;\n",
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

    def test_active_selection_global_has_one_record_view(self) -> None:
        definition = "src/game/duel_scene.c"
        views = [
            ("DuelSelectionRecord", True),
            ("DuelHandSlot", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "selection.c"
            for record, accepted in views:
                with self.subTest(record=record):
                    path.write_text(
                        '#include "duel_scene.h"\n'
                        + "struct DisplayObject **display_parent_slot("
                        + "DuelSelectionRecord *record) "
                        + "{ return &record->info_panel; }\n"
                        + f"{record} **active_record(void) "
                        + "{ return &gDuel_pActiveSelectionRecord; }\n"
                        + "DuelHandSlot **hand_slot(DuelSelectionRecord *record) "
                        + "{ return &record->hand; }\n",
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

    def test_hand_slot_has_canonical_display_object_pointers(self) -> None:
        definition = "src/game/duel_scene.c"
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "hand.c"
            for member in ("object", "order_marker"):
                for pointee, accepted in (("struct DisplayObject", True), ("u8", False)):
                    with self.subTest(member=member, pointee=pointee):
                        path.write_text(
                            '#include "duel_scene.h"\n'
                            + f"{pointee} **slot_view(DuelHandSlot *slot) "
                            + f"{{ return &slot->{member}; }}\n",
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

    def test_parent_search_has_one_pointer_view(self) -> None:
        definition = "src/game/model_scene_runtime.c"
        self.assertIn(
            '#include "model_scene_runtime.h"',
            (REPOSITORY / definition).read_text(encoding="utf-8"),
        )
        views = [
            ("ModelSlot *, GsCOORDUNIT *", True),
            ("ModelSlot *, void *", False),
            ("u8 *, GsCOORDUNIT *", False),
        ]
        with tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp") as temporary:
            path = Path(temporary) / "view.c"
            for parameters, accepted in views:
                with self.subTest(parameters=parameters):
                    path.write_text(
                        '#include "model_scene_runtime.h"\n'
                        + f"s32 (*view)({parameters}) = Model_FindBranchingChildCoord;\n",
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
