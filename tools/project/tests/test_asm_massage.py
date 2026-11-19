from __future__ import annotations

from pathlib import Path
import sys
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import asm_massage as am
import asm_massage_deps as deps
import asm_massage_rules as rules


def parse(text: str, noreorder: bool = True) -> list[am.Instr]:
    """Instruction list of one function body; NOREORDER means the text spells
    out its delay slots."""
    mode = "\t.set\tnoreorder\n" if noreorder else ""
    source = "\t.ent\tf\nf:\n" + mode + text + "\n\t.end\tf\n"
    return am.parse_function(source, "f")


def body(instrs: list[am.Instr]) -> list[str]:
    return [i.text() for i in instrs]


def run(names: str, text: str, settings: dict | None = None) -> list[str]:
    context = rules.Context(settings or {})
    instrs = am.webify(parse(text))
    for name in names.split():
        instrs = rules.RULES[name].apply(instrs, context)
    return body(instrs)


class ParseTests(unittest.TestCase):
    def test_pseudo_instructions_expand_to_machine_form(self):
        self.assertEqual(body(parse(
            "li\t$2,1431633920\n"
            "li\t$3,0x12345678\n"
            "lw\t$24,gSym\n"
            "sw\t$5,gSym\n"
            "srl\t$4,$4,$25\n"
            "addu\t$16,$sp,4\n"
            "lwc2\t$0, 0( $2 );lwc2\t$1, 4( $2 )\n"
            "#nop\n"
            "j\t$31\n"
            "nop # DEBUG: branch/jump")), [
            "lui $v0, 0x5555",
            "lui $v1, 0x1234", "ori $v1, $v1, 0x5678",
            "lui $t8, %hi(gSym)", "lw $t8, %lo(gSym)($t8)",
            "lui $at, %hi(gSym)", "sw $a1, %lo(gSym)($at)",
            "srlv $a0, $a0, $t9",
            "addiu $s0, $sp, 0x4",
            "lwc2 $0, 0x0($v0)", "lwc2 $1, 0x4($v0)",
            "jr $ra", "nop",
        ])


    def test_more_pseudo_instructions_and_rejected_macros(self):
        self.assertEqual(body(parse("la\t$4,gSym\nnegu\t$2,$3\nnot\t$2,$3\nsltu\t$2,$3,5")), [
            "lui $a0, %hi(gSym)", "addiu $a0, $a0, %lo(gSym)",
            "subu $v0, $zero, $v1", "nor $v0, $v1, $zero", "sltiu $v0, $v1, 0x5"])
        with self.assertRaises(am.MassageError):
            parse("div\t$2,$3,$4")

    def test_reorder_branches_get_a_delay_slot_nop(self):
        # Raw GCC: reorder mode except its own filled .set noreorder blocks.
        out = body(parse(
            "bltz\t$2,$L1\n"
            ".set\tnoreorder\n.set\tnomacro\nbne\t$8,$0,$L1\naddu\t$7,$7,20\n"
            ".set\tmacro\n.set\treorder\n$L1:\nj\t$31", noreorder=False))
        self.assertEqual(out, ["bltz $v0, $L1", "nop", "bne $t0, $zero, $L1",
                               "addiu $a3, $a3, 0x14", "$L1:", "jr $ra", "nop"])


class WebTests(unittest.TestCase):
    def test_reused_register_splits_but_loop_variable_does_not(self):
        out = body(am.webify(parse(
            "lw\t$2,0($4)\nsw\t$2,4($4)\nlw\t$2,8($4)\n"
            "$L1:\nsw\t$2,12($4)\naddu\t$2,$2,1\nbne\t$2,$0,$L1\nnop\n"
            "j\t$31\nnop")))
        self.assertEqual(out[:3], ["lw $v0.1, 0x0($a0.0)", "sw $v0.1, 0x4($a0.0)",
                                   "lw $v0.2, 0x8($a0.0)"])
        self.assertEqual(out[4], "sw $v0.2, 0xC($a0.0)")
        self.assertEqual(out[5], "addiu $v0.2, $v0.2, 0x1")

    def test_chains_share_one_map_entry(self):
        instrs = am.webify(parse(
            "lhu\t$2,0($4)\nsll\t$2,$2,3\naddu\t$2,$2,$5\nlwc2\t$0, 0( $2 )\nj\t$31\nnop"))
        chains = am.web_chains(instrs)
        self.assertEqual({chains["$v0.1"], chains["$v0.2"], chains["$v0.3"]}, {"$v0.1"})


class RuleTests(unittest.TestCase):
    def test_dmpsx_markers_become_cop2_words(self):
        self.assertEqual(run("dmpsx-native", ".word 0x000000bf\n.word 0x000011bf"),
                         [".word 0x4A280030", ".word 0x4B58002D"])
        with self.assertRaises(am.MassageError):
            run("dmpsx-native", ".word 0x0000003f")

    def test_gte_reads_skip_the_stack(self):
        out = run("gte-direct-read dce",
                  "addu\t$3,$sp,4\n"
                  ".word 0x000000bf\n"
                  "cfc2\t$12, $31;nop;sw\t$12, 0( $sp )\n"
                  "lw\t$8,0($sp)\n"
                  "swc2\t$7, 0( $3 )\n"
                  "sw\t$8,0($5)\n"
                  "lw\t$4,4($sp)\n"
                  "sw\t$4,4($5)\n"
                  "j\t$31\nnop")
        self.assertEqual(out, [".word 0x000000BF", "cfc2 $t0.1, $31", "nop",
                               "mfc2 $a0.1, $7", "sw $t0.1, 0x0($a1.0)",
                               "sw $a0.1, 0x4($a1.0)", "jr $ra", "nop"])

    def test_address_folds_into_offset(self):
        self.assertEqual(run("fold-address", "addu\t$8,$5,12\nswc2\t$12, 0( $8 )\nj\t$31\nnop"),
                         ["swc2 $12, 0xC($a1.0)", "jr $ra", "nop"])

    def test_biased_pointer_is_unbiased(self):
        out = run("undo-strength-reduction",
                  "addu\t$7,$6,18\n$L1:\nlhu\t$9,-4($7)\nsw\t$9,0($5)\n"
                  "addu\t$8,$8,-1\nbne\t$8,$0,$L1\naddu\t$7,$7,20\nj\t$31\nnop")
        self.assertEqual(out[:2], ["$L1:", "lhu $t1.1, 0xE($a2.0)"])
        self.assertEqual(out[5], "addiu $a2.0, $a2.0, 0x14")

    def test_second_pointer_copy_merges(self):
        out = run("undo-strength-reduction",
                  "addu\t$6,$5,3\n$L1:\nsw\t$24,1($6)\naddu\t$6,$6,32\n"
                  "sw\t$24,0($5)\naddu\t$5,$5,32\n"
                  "bne\t$8,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[:4], ["$L1:", "sw $t8.0, 0x4($a1.0)", "sw $t8.0, 0x0($a1.0)",
                                   "addiu $a1.0, $a1.0, 0x20"])

    def test_halfwords_become_word_loads_and_shifts(self):
        out = run("halfword-words",
                  "lhu\t$8,14($7)\nsll\t$8,$8,3\nlwc2\t$0, 0( $8 )\n"
                  "lhu\t$3,16($7)\nsll\t$3,$3,3\nlwc2\t$2, 0( $3 )\n"
                  "lhu\t$4,18($7)\nsw\t$4,8($9)\nsw\t$8,0($9)\nsw\t$3,4($9)\nj\t$31\nnop")
        self.assertEqual(out[:2], ["lw %word1, 0xC($a3.0)", "lw %word2, 0x10($a3.0)"])
        self.assertEqual(out[2:4], ["srl $t0.1, %word1, 16", "sll $t0.2, $t0.1, 3"])
        self.assertEqual(out[5:7], ["sll $v1.1, %word2, 16", "srl $v1.2, $v1.1, 13"])
        self.assertEqual(out[8], "srl $a0.1, %word2, 16")

    def test_setlen_addprim_becomes_single_tag_build(self):
        out = run("tag-build",
                  "li\t$10,0x00ffffff\nli\t$13,-16777216\n$L1:\n"
                  "li\t$8,7\nsb\t$8,3($5)\n"
                  "lw\t$3,0($5)\nlw\t$8,0($4)\n"
                  "and\t$3,$3,$13\nand\t$8,$8,$10\nor\t$3,$3,$8\nsw\t$3,0($5)\n"
                  "and\t$3,$5,$10\nlw\t$8,0($4)\naddu\t$5,$5,32\n"
                  "and\t$8,$8,$13\nor\t$8,$8,$3\nsw\t$8,0($4)\n"
                  "bne\t$5,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out, [
            "$L1:",
            "lw %tag1, 0x0($a0.0)", "sll %tag1, %tag1, 8", "ori %tag1, %tag1, 0x7",
            "srl $at, %tag1, 8", "sll %tag1, %tag1, 24", "or %tag1, %tag1, $at",
            "sw %tag1, 0x0($a1.0)", "sll %tag1, $a1.0, 8", "srl %tag1, %tag1, 8",
            "sw %tag1, 0x0($a0.0)", "addiu $a1.0, $a1.0, 0x20",
            "bne $a1.0, $zero, $L1", "nop", "jr $ra", "nop",
        ])

    def test_adjacent_globals_compose_into_one_word_load(self):
        out = run("word-compose",
                  "lbu\t$3,gModel_bBlendRateActive\nlbu\t$4,gModel_bCurrentDrawMode\n"
                  "lhu\t$8,gModel_wDrawTintRedGreen\nsll\t$3,$3,8\nor\t$4,$4,$3\n"
                  "sll\t$8,$8,16\nor\t$4,$4,$8\nsw\t$4,0($5)\nj\t$31\nnop")
        self.assertEqual(out, ["lui $a0.4, %hi(gModel_bCurrentDrawMode)",
                               "lw $a0.4, %lo(gModel_bCurrentDrawMode)($a0.4)",
                               "sw $a0.4, 0x0($a1.0)", "jr $ra", "nop"])

    def test_loop_invariant_operand_goes_last(self):
        out = run("invariant-last",
                  "$L1:\nsll\t$2,$3,3\naddu\t$2,$9,$2\nlwc2\t$0, 0( $2 )\n"
                  "addu\t$3,$3,1\nbne\t$3,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[2], "addu $v0.2, $v0.1, $t1.0")

    def test_struct_halfwords_compose_into_one_word_load(self):
        out = run("word-compose",
                  "lhu\t$3,4($5)\nlhu\t$8,6($5)\nsll\t$8,$8,16\nor\t$7,$3,$8\n"
                  "mtc2\t$7, $17\nj\t$31\nnop")
        self.assertEqual(out[:2], ["lw $a3.1, 0x4($a1.0)", "mtc2 $a3.1, $17"])

    def test_unaligned_struct_copy_becomes_word_copy_and_is_reused(self):
        out = run("aligned-word-copy reuse-load",
                  "lwl\t$19,3($5)\nlwr\t$19,0($5)\nswl\t$19,15($6)\nswr\t$19,12($6)\n"
                  "lwl\t$19,3($5)\nlwr\t$19,0($5)\nswl\t$19,27($6)\nswr\t$19,24($6)\n"
                  "j\t$31\nnop")
        self.assertEqual(len(out), 5)
        self.assertEqual(out[0][:3], "lw ")
        self.assertEqual([x.split(", ")[1] for x in out[1:3]], ["0xC($a2.0)", "0x18($a2.0)"])
        self.assertEqual(out[1].split(",")[0], out[2].split(",")[0])

    def test_sz_writes_follow_their_own_test(self):
        out = run("gte-load-early",
                  "lw\t$7,4($5)\nbeq\t$7,$14,$L6\nsw\t$8,0($6)\n"
                  "lw\t$9,4($11)\nbeq\t$9,$14,$L6\nnop\n"
                  "mtc2\t$7, $17;mtc2\t$9, $18\n$L6:\nj\t$31\nnop")
        self.assertEqual(out[3], "mtc2 $a3.1, $17")
        self.assertEqual(out[6], "mtc2 $t1.1, $18")  # the empty slot of its test

    def test_store_fills_an_empty_branch_slot(self):
        self.assertEqual(run("store-delay-slot",
                             "sw\t$8,12($6)\nbeq\t$7,$14,$L6\nnop\n$L6:\nj\t$31\nnop")[:2],
                         ["beq $a3.0, $t6.0, $L6", "sw $t0.0, 0xC($a2.0)"])

    def test_frameless_parks_saved_registers_in_the_argument_block(self):
        instrs = am.rename(am.webify(parse(
            "subu\t$sp,$sp,8\nsw\t$16,0($sp)\nlw\t$7,0($4)\nlhu\t$24,2($7)\n"
            "beq\t$24,$0,$L2\nnop\nli\t$16,5\nli\t$18,6\nsw\t$16,0($7)\nsw\t$18,4($7)\n"
            "$L2:\nlw\t$2,0($4)\nlw\t$16,0($sp)\nj\t$31\naddu\t$sp,$sp,8")), {})
        out = body(rules.RULES["frameless"].apply(instrs, rules.Context({"frameless": "$a0 0x20"})))
        self.assertEqual(out[:4], ["lw $a3, 0x0($a0)", "lhu $t8, 0x2($a3)",
                                   "sw $s0, 0x20($a0)", "sw $s2, 0x24($a0)"])
        self.assertEqual(out[-5:], ["lw $s0, 0x20($a0)", "lw $s2, 0x24($a0)",
                                    "lw $v0, 0x0($a0)", "jr $ra", "nop"])


class PolygonRuleTests(unittest.TestCase):
    def test_gte_wait_nops_survive(self):
        self.assertEqual(run("strip-nops", "lw\t$8,0($4)\nnop\nmtc2\t$8, $12\n"
                             "nop;nop;.word 0x0000117f\nj\t$31\nnop")[:5],
                         ["lw $t0.1, 0x0($a0.0)", "mtc2 $t0.1, $12", "nop", "nop", ".word 0x0000117F"])

    def test_spilled_invariant_is_read_from_its_register(self):
        out = run("unspill", "lw\t$16,0($4)\nsw\t$16,8($sp)\n$L1:\nlw\t$9,8($sp)\n"
                  "sw\t$9,0($5)\nbne\t$5,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[:3], ["lw $s0.1, 0x0($a0.0)", "$L1:", "sw $s0.1, 0x0($a1.0)"])

    def test_spill_reload_into_a_merge_becomes_a_copy(self):
        out = run("unspill", "lw\t$16,0($4)\nsw\t$16,8($sp)\nbeq\t$5,$0,$L1\nnop\n"
                  "lw\t$12,gSym\nj\t$L2\nnop\n$L1:\nlw\t$12,8($sp)\n$L2:\n"
                  "sw\t$12,0($5)\nj\t$31\nnop")
        self.assertIn("addu $t4.2, $s0.1, $zero", out)
        self.assertFalse(any("($sp)" in line for line in out))

    def test_reloaded_store_is_forwarded_around_the_loop(self):
        # As tag-build leaves it: one temporary for the whole sequence.
        instrs = parse("$L1:\nlw\t$13,0($2)\nsll\t$13,$13,8\nsw\t$13,0($25)\n"
                       "sll\t$13,$25,8\nsw\t$13,0($2)\nj\t$L1\nnop")
        out = body(rules.RULES["loop-load-forwarding"].apply(instrs, rules.Context({})))
        self.assertEqual(out[:2], ["lw $t5, 0x0($v0)", "$L1:"])

    def test_large_constants_are_built_in_at_at_their_use(self):
        out = run("at-constants", "li\t$10,0xff9fffff\n$L1:\nand\t$9,$9,$10\n"
                  "sw\t$9,0($5)\nbne\t$5,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[:4], ["$L1:", "lui $at, 0xFF9F", "ori $at, $at, 0xFFFF",
                                   "and $t1.0, $t1.0, $at"])

    def test_zero_extension_uses_shifts_across_a_delay_slot(self):
        out = run("zero-extend-shifts", "beq\t$5,$0,$L1\nandi\t$8,$8,0xffff\n"
                  "sw\t$8,0($6)\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:4], ["beq $a1.0, $zero, $L1", "sll $t0.1, $t0.0, 16",
                                   "srl $t0.1, $t0.1, 16", "sw $t0.1, 0x0($a2.0)"])

    def test_move_from_zero_is_li_zero(self):
        self.assertEqual(run("zero-as-addiu", "move\t$3,$0\nsw\t$3,0($4)\nj\t$31\nnop")[0],
                         "addiu $v1.1, $zero, 0x0")

    def test_pointer_increment_sinks_past_its_accesses(self):
        out = run("increment-sink", "$L1:\naddu\t$25,$25,40\nsw\t$12,4($25)\nlw\t$3,-32($25)\n"
                  "sw\t$3,8($25)\nbne\t$25,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[1:5], ["sw $t4.0, 0x2C($t9.0)", "lw $v1.1, 0x8($t9.0)",
                                    "sw $v1.1, 0x30($t9.0)", "addiu $t9.0, $t9.0, 0x28"])

    def test_loop_compare_constant_is_set_inside_the_loop(self):
        out = run("loop-constant", "beq\t$5,$0,$L0\nli\t$3,-1\n$L0:\n$L1:\n"
                  "lw\t$2,4($6)\nbeq\t$2,$3,$L2\nnop\naddu\t$6,$6,8\n$L2:\n"
                  "bne\t$6,$0,$L1\nnop\nj\t$31\nnop")
        self.assertEqual(out[:5], ["beq $a1.0, $zero, $L0", "nop", "$L0:", "$L1:",
                                   "addiu $v1.1, $zero, -0x1"])

    def test_record_words_pack_two_lone_high_halves(self):
        out = run("halfword-words",
                  "lhu\t$8,14($7)\nsll\t$8,$8,3\nlwc2\t$0, 0( $8 )\n"
                  "lhu\t$9,18($7)\nlwc2\t$1, 0( $9 )\nlhu\t$10,22($7)\nlwc2\t$2, 0( $10 )\n"
                  "lhu\t$11,26($7)\nlwc2\t$3, 0( $11 )\nlhu\t$12,10($7)\nsll\t$12,$12,3\n"
                  "sw\t$12,0($5)\nj\t$31\nnop")
        self.assertEqual(out[:2], ["lw %word1, 0xC($a3.0)", "lwr %word1, 0xA($a3.0)"])
        self.assertIn("sll $t4.1, %word1, 16", out)
        self.assertIn("srl $t4.2, $t4.1, 13", out)


class DirectPolygonRuleTests(unittest.TestCase):
    def test_gte_load_of_a_stored_variable_becomes_mtc2(self):
        out = run("gte-direct-write", "lw\t$15,gSym\nsw\t$15,8($sp)\n"
                  "lwc2\t$6, 0( $sp )\nj\t$31\nnop".replace("0( $sp )", "8( $sp )"))
        self.assertIn("mtc2 $t7.2, $6", out)
        self.assertFalse(any("$sp" in line for line in out))

    def test_cast_byte_load_is_a_signed_load(self):
        out = run("sign-extend-load", "lbu\t$8,gSym\nbeq\t$5,$0,$L1\nnop\n"
                  "sll\t$3,$8,24\nsra\t$9,$3,24\nsw\t$9,0($5)\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:2], ["lui $t0.1, %hi(gSym)", "lb $t1.1, %lo(gSym)($t0.1)"])
        self.assertFalse(any(line.startswith(("sll", "sra")) for line in out))

    def test_invariant_compare_constant_is_built_in_at(self):
        loop = ("lb\t$22,gSym\nli\t$3,1\n$L1:\nlw\t$2,4($6)\nbeq\t{},$3,$L2\nnop\n"
                "addu\t$6,$6,8\n$L2:\nbne\t$6,$0,$L1\nnop\nj\t$31\nnop")
        out = run("at-branch-constants", loop.format("$22"))
        self.assertIn("addiu $at, $zero, 0x1", out)
        self.assertIn("beq $s6.2, $at, $L2", out)
        # A constant the loop compares fresh loads with keeps its register.
        self.assertNotIn("addiu $at, $zero, 0x1", run("at-branch-constants", loop.format("$2")))

    def test_loop_carried_spill_slot_becomes_a_register(self):
        out = run("unspill", "sw\t$5,48($sp)\n$L1:\nlw\t$24,48($sp)\naddu\t$24,$24,28\n"
                  "sw\t$24,48($sp)\nbne\t$24,$0,$L1\nnop\nj\t$31\nnop")
        self.assertFalse(any("($sp)" in line for line in out))
        self.assertIn("addu %slot1, $a1.0, $zero", out)

    def test_constant_in_a_delay_slot_leaves_a_nop(self):
        # GCC's preheader constant feeds an AND it put in a branch slot; the
        # AND leaves the slot (the taken path does not read it).
        out = run("at-constants", "li\t$10,0xff9fffff\nbeq\t$5,$0,$L1\nand\t$9,$9,$10\n"
                  "sw\t$9,0($5)\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:5], ["beq $a1.0, $zero, $L1", "nop", "lui $at, 0xFF9F",
                                   "ori $at, $at, 0xFFFF", "and $t1.1, $t1.0, $at"])

    def test_jump_slot_takes_the_flag_reset(self):
        out = physical("fill-jump-slot", "li\t$3,0\nsw\t$12,4($25)\naddu\t$25,$25,8\n"
                       "j\t$L1\nnop\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:4], ["sw $t4, 0x4($t9)", "addiu $t9, $t9, 0x8", "j $L1",
                                   "addiu $v1, $zero, 0x0"])


class DirectPolygonDependenceTests(unittest.TestCase):
    def test_accesses_across_a_pointer_increment_are_rebased(self):
        for later, overlap in ((8, True), (4, False)):
            program = f"sw\t$8,40($25)\naddu\t$25,$25,32\nsw\t$9,{later}($25)\nj\t$31\nnop"
            prog = deps.Program(am.rename(am.webify(parse(program)), {}))
            first, _, second = prog.order[:3]
            self.assertEqual(deps.may_alias(prog, first, second), overlap)

    def test_inserted_register_copy_is_proven(self):
        program = "lw\t$16,gSym\nsw\t$16,0($5)\nj\t$31\nnop"
        before = am.rename(am.webify(parse(program)), {})
        texts = [i.text() for i in before]
        patch = am.make_residual(texts, texts[:2] + ["addu $t4, $s0, $zero",
                                                     "sw $t4, 0x0($a1)"] + texts[3:])
        report = deps.check(before, am.apply_residual(before, patch))
        self.assertEqual((report.copies, report.flags), (1, []))


def physical(names: str, text: str) -> list[str]:
    instrs = am.rename(am.webify(parse(text)), {})
    for name in names.split():
        instrs = rules.RULES[name].apply(instrs, rules.Context({}))
    return body(instrs)


class ScheduleTests(unittest.TestCase):
    def test_guard_chain_hoists_the_guard_load_and_sinks_the_slot(self):
        out = physical("guard-chain",
                       "lhu\t$24,2($4)\nbeq\t$24,$0,$L9\naddu\t$7,$5,$6\n"
                       "lw\t$13,gSym\nli\t$3,1\nbeq\t$13,$3,$L9\nlui\t$10,0x5555\n"
                       "ori\t$10,$10,0x5555\nsw\t$10,0($7)\n$L9:\nj\t$31\nnop")
        self.assertEqual(out[:9], [
            "lhu $t8, 0x2($a0)", "lui $t5, %hi(gSym)", "lw $t5, %lo(gSym)($t5)",
            "beq $t8, $zero, $L9", "nop", "addiu $v1, $zero, 0x1",
            "beq $t5, $v1, $L9", "addu $a3, $a1, $a2", "lui $t2, 0x5555"])

    def test_preheader_puts_constants_after_templates(self):
        out = physical("preheader-order",
                       "lui\t$10,0x5555\nori\t$10,$10,0x5555\nlw\t$11,gA\nli\t$12,-1\n"
                       "lw\t$8,24($4)\n$L1:\nsw\t$10,0($8)\nsw\t$11,4($8)\n"
                       "beq\t$9,$12,$L2\nnop\naddu\t$8,$8,8\n"
                       "bne\t$8,$0,$L1\nnop\n$L2:\nj\t$31\nnop")
        self.assertEqual(out[:7], [
            "lui $t3, %hi(gA)", "lw $t3, %lo(gA)($t3)", "lui $t2, 0x5555",
            "ori $t2, $t2, 0x5555", "addiu $t4, $zero, -0x1", "lw $t0, 0x18($a0)", "$L1:"])

    def test_load_delay_is_filled_from_above(self):
        out = physical("fill-load-delay",
                       "addu\t$13,$11,$5\n.word 0x4A280030\ncfc2\t$2,$31\n"
                       "bltz\t$2,$L1\nnop\nlwc2\t$0, 0( $13 )\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:4], [".word 0x4A280030", "cfc2 $v0, $31",
                                   "addu $t5, $t3, $a1", "bltz $v0, $L1"])


    def test_empty_branch_slot_takes_an_independent_alu_instruction(self):
        out = physical("fill-branch-slot", "addu\t$10,$10,$6\nmfc2\t$2, $24\naddu\t$11,$11,$6\n"
                       "blez\t$2,$L1\nnop\nlw\t$8,0($10)\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[:5], ["addu $t2, $t2, $a2", "mfc2 $v0, $24", "addu $t3, $t3, $a2",
                                   "blez $v0, $L1", "nop"])
        out = physical("fill-branch-slot", "mfc2\t$2, $24\naddu\t$10,$10,$6\naddu\t$11,$11,$6\n"
                       "blez\t$2,$L1\nnop\nlw\t$8,0($10)\n$L1:\nj\t$31\nnop")
        self.assertEqual(out[2:5], ["blez $v0, $L1", "addu $t3, $t3, $a2", "lw $t0, 0x0($t2)"])


class DependenceTests(unittest.TestCase):
    PROGRAM = ("lw\t$8,0($4)\nlw\t$9,4($4)\naddu\t$10,$8,$9\nsw\t$10,0($5)\n"
               "lw\t$11,4($5)\nlw\t$12,0($6)\nsw\t$11,8($7)\nsw\t$12,12($7)\nj\t$31\nnop")
    REST = [6, 7, 8, 9]

    def reorder(self, order: list[int]):
        before = am.rename(am.webify(parse(self.PROGRAM)), {})
        texts = [i.text() for i in before]
        patch = am.make_residual(texts, [texts[k] for k in order + self.REST])
        return deps.check(before, am.apply_residual(before, patch))

    def test_independent_moves_are_proven(self):
        report = self.reorder([1, 0, 2, 3, 4, 5])
        self.assertEqual((report.moves, report.flags), (1, []))

    def test_breaking_a_register_dependence_is_flagged(self):
        self.assertTrue(any(f.startswith("RAW") for f in self.reorder([0, 2, 1, 3, 4, 5]).flags))

    def test_memory_order_follows_possible_aliasing(self):
        # sw 0($a1) and lw 4($a1) are disjoint; lw 0($a2) may be the same word.
        self.assertEqual(self.reorder([0, 1, 2, 4, 3, 5]).flags, [])
        self.assertIn("memory", " ".join(self.reorder([0, 1, 2, 5, 3, 4]).flags))


class JoinMergeTests(unittest.TestCase):
    """GCC copies a pure instruction into every way into a label; the
    original computes it once after the label."""

    def run_merge(self, middle: str):
        text = ("beq\t$5,$0,$L2\nsrl\t$16,$4,24\n" + middle + "srl\t$16,$4,24\n"
                "$L2:\nsll\t$16,$16,24\nsw\t$16,0($7)\nj\t$31\nnop")
        webs = am.webify(parse(text))
        before = am.rename(webs, {})
        texts = [i.text() for i in before]
        merged = [t for t in texts if not t.startswith("srl")]
        merged[1:1] = ["nop"]  # the emptied delay slot
        merged.insert(merged.index("$L2:") + 1, "srl $s0, $a0, 24")
        report = deps.check(before, am.apply_residual(before, am.make_residual(texts, merged)),
                            webs=webs)
        return report

    def test_identical_copies_merge_after_the_join(self):
        report = self.run_merge("addu\t$6,$6,1\n")
        self.assertEqual((len(report.merged), report.flags), (1, []))

    def test_copies_reading_different_values_are_refused(self):
        report = self.run_merge("addu\t$4,$4,1\n")
        self.assertEqual(report.merged, [])
        self.assertIn("operands differ", report.refused[0])
        self.assertTrue(report.flags)

    def test_copy_read_on_the_not_taken_path_is_refused(self):
        report = self.run_merge("addu\t$6,$16,1\n")
        self.assertEqual(report.merged, [])
        self.assertIn("read on a not-taken path", report.refused[0])
        self.assertTrue(report.flags)


class ContractTests(unittest.TestCase):
    def test_only_stock_psyq_macros_reach_the_hardware(self):
        self.assertEqual(am.check_source_contract(
            '#include "../psyq/inline_c.h"\nvoid f(void) { gte_rtpt_b(); }\n'), [])
        self.assertEqual(len(am.check_source_contract(
            '#include "handwritten_asm.h"\nregister int x asm("$16");\n')), 3)


class HazardAndResidualTests(unittest.TestCase):
    def test_load_delay_nops_are_rederived(self):
        instrs = parse("lw\t$2,0($4)\nbltz\t$2,$L1\nnop\naddu\t$3,$3,1\n$L1:\nj\t$31\nnop")
        self.assertEqual(body(rules.insert_hazard_nops(instrs))[:3],
                         ["lw $v0, 0x0($a0)", "nop", "bltz $v0, $L1"])

    def test_residual_is_strict(self):
        instrs = parse("addu\t$2,$4,1\naddu\t$3,$4,2\nj\t$31\nnop")
        patch = am.make_residual(body(instrs), ["addiu $v1, $a0, 0x2", "addiu $v0, $a0, 0x1",
                                                "jr $ra", "nop"])
        self.assertEqual(body(am.apply_residual(instrs, patch))[:2],
                         ["addiu $v1, $a0, 0x2", "addiu $v0, $a0, 0x1"])
        with self.assertRaises(am.MassageError):
            am.apply_residual(parse("addu\t$2,$4,7\naddu\t$3,$4,2\nj\t$31\nnop"), patch)


class UnitTests(unittest.TestCase):
    """A translation unit of several massaged functions (the build path)."""

    GCC = (
        "\t.text\n\t.align\t2\n\t.globl\tf\n\t.ent\tf\nf:\n"
        "\tbeq\t$4,$0,$L2\n\taddu\t$2,$4,1\n$L2:\n\tj\t$31\n\t.end\tf\n"
        "\t.align\t2\n\t.globl\tg\n\t.ent\tg\ng:\n"
        "\tbeq\t$4,$0,$L3\n\taddu\t$2,$4,2\n$L3:\n\tj\t$31\n\t.end\tg\n"
    )

    def edit(self, function: str) -> am.EditFile:
        return am.EditFile(Path(f"{function}.massage"), {
            "function": function, "source": "x.c", "address": "0x0", "size": "0x0",
            "rules": "strip-nops"}, {}, [])

    def test_each_function_gets_its_own_label_prefix(self):
        text = am.massage_unit(self.GCC, {name: self.edit(name) for name in "gf"})
        self.assertIn("\tbeq $a0, $zero, .Lf_1\n", text)
        self.assertIn(".Lg_1:", text)
        self.assertLess(text.index("\t.ent\tf"), text.index("\t.ent\tg"))
        self.assertTrue(text.startswith("\t.set\tnoreorder\n\t.set\tnomacro\n"))

    def test_edits_must_cover_the_unit_exactly(self):
        with self.assertRaisesRegex(am.MassageError, "no edit for g"):
            am.massage_unit(self.GCC, {"f": self.edit("f")})
        with self.assertRaisesRegex(am.MassageError, "no function for h"):
            am.massage_unit(self.GCC, {name: self.edit(name) for name in "fgh"})

    def test_units_with_data_are_refused(self):
        edits = {name: self.edit(name) for name in "fg"}
        for data in ("\t.rdata\n", "\t.word\t1\n"):
            with self.assertRaisesRegex(am.MassageError, "only code"):
                am.massage_unit(data + self.GCC, edits)
        # A GTE command word inside a function is code.
        text = am.massage_unit(self.GCC.replace("\tj\t$31\n\t.end\tf", "\t.word\t0x0000003f\n\tj\t$31\n\t.end\tf"), edits)
        self.assertIn(".word 0x0000003F", text)

    def test_massaged_c_may_use_only_inline_c_asm(self):
        import matching_source_contracts as contracts

        allowed = contracts.inline_c_shapes(REPOSITORY)
        stock = '__asm__ volatile ( "lwc2 $0, 0( %0 );" "lwc2 $1, 4( %0 )" : : "r"( p ) ) ;'
        own = '__asm__ volatile ( "lwc2 $0, 0( %0 )" : : "r"( p ) ) ;'
        self.assertIn(contracts.asm_shape(contracts.asm_statements(stock)[0]), allowed)
        self.assertNotIn(contracts.asm_shape(contracts.asm_statements(own)[0]), allowed)


if __name__ == "__main__":
    unittest.main()
