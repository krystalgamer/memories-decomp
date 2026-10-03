.include "macro.inc"
.set noat
.set noreorder
.section .text, "ax"
nonmatching func_8013BDB0, 0x59C

glabel func_8013BDB0
    /* DB0 8013BDB0 C8FEBD27 */  addiu      $sp, $sp, -0x138
    /* DB4 8013BDB4 1401B1AF */  sw         $s1, 0x114($sp)
    /* DB8 8013BDB8 21888000 */  addu       $s1, $a0, $zero
    /* DBC 8013BDBC 2401B5AF */  sw         $s5, 0x124($sp)
    /* DC0 8013BDC0 21A82002 */  addu       $s5, $s1, $zero
    /* DC4 8013BDC4 1001B0AF */  sw         $s0, 0x110($sp)
    /* DC8 8013BDC8 902BB026 */  addiu      $s0, $s5, 0x2B90
    /* DCC 8013BDCC 3401BFAF */  sw         $ra, 0x134($sp)
    /* DD0 8013BDD0 3001BEAF */  sw         $fp, 0x130($sp)
    /* DD4 8013BDD4 2C01B7AF */  sw         $s7, 0x12C($sp)
    /* DD8 8013BDD8 2801B6AF */  sw         $s6, 0x128($sp)
    /* DDC 8013BDDC 2001B4AF */  sw         $s4, 0x120($sp)
    /* DE0 8013BDE0 1C01B3AF */  sw         $s3, 0x11C($sp)
    /* DE4 8013BDE4 C463010C */  jal        func_80058F10
    /* DE8 8013BDE8 1801B2AF */   sw        $s2, 0x118($sp)
    /* DEC 8013BDEC 5800BE27 */  addiu      $fp, $sp, 0x58
    /* DF0 8013BDF0 E400A2AF */  sw         $v0, 0xE4($sp)
    /* DF4 8013BDF4 E800A0A7 */  sh         $zero, 0xE8($sp)
  .L8013BDF8:
    /* DF8 8013BDF8 E800A897 */  lhu        $t0, 0xE8($sp)
    /* DFC 8013BDFC 00000000 */  nop
    /* E00 8013BE00 00140800 */  sll        $v0, $t0, 16
    /* E04 8013BE04 03940200 */  sra        $s2, $v0, 16
    /* E08 8013BE08 40101200 */  sll        $v0, $s2, 1
    /* E0C 8013BE0C 21105500 */  addu       $v0, $v0, $s5
    /* E10 8013BE10 7A304484 */  lh         $a0, 0x307A($v0)
    /* E14 8013BE14 70304584 */  lh         $a1, 0x3070($v0)
    /* E18 8013BE18 6826020C */  jal        ratan2
    /* E1C 8013BE1C 2188A002 */   addu      $s1, $s5, $zero
    /* E20 8013BE20 00084224 */  addiu      $v0, $v0, 0x800
    /* E24 8013BE24 F000A2AF */  sw         $v0, 0xF0($sp)
    /* E28 8013BE28 21B00000 */  addu       $s6, $zero, $zero
    /* E2C 8013BE2C F400B2AF */  sw         $s2, 0xF4($sp)
    /* E30 8013BE30 40111200 */  sll        $v0, $s2, 5
    /* E34 8013BE34 21102202 */  addu       $v0, $s1, $v0
    /* E38 8013BE38 F800A2AF */  sw         $v0, 0xF8($sp)
  .L8013BE3C:
    /* E3C 8013BE3C F400A88F */  lw         $t0, 0xF4($sp)
    /* E40 8013BE40 21980000 */  addu       $s3, $zero, $zero
    /* E44 8013BE44 00110800 */  sll        $v0, $t0, 4
    /* E48 8013BE48 2110A202 */  addu       $v0, $s5, $v0
    /* E4C 8013BE4C 01000825 */  addiu      $t0, $t0, 0x1
    /* E50 8013BE50 FC00A2AF */  sw         $v0, 0xFC($sp)
    /* E54 8013BE54 00141600 */  sll        $v0, $s6, 16
    /* E58 8013BE58 03140200 */  sra        $v0, $v0, 16
    /* E5C 8013BE5C 0401A8AF */  sw         $t0, 0x104($sp)
    /* E60 8013BE60 01004824 */  addiu      $t0, $v0, 0x1
    /* E64 8013BE64 0001A2AF */  sw         $v0, 0x100($sp)
    /* E68 8013BE68 0801A8AF */  sw         $t0, 0x108($sp)
    /* E6C 8013BE6C 00141300 */  sll        $v0, $s3, 16
  .L8013BE70:
    /* E70 8013BE70 03BC0200 */  sra        $s7, $v0, 16
    /* E74 8013BE74 80901700 */  sll        $s2, $s7, 2
    /* E78 8013BE78 21A05102 */  addu       $s4, $s2, $s1
    /* E7C 8013BE7C 6400848E */  lw         $a0, 0x64($s4)
    /* E80 8013BE80 00000000 */  nop
    /* E84 8013BE84 02008104 */  bgez       $a0, .L8013BE90
    /* E88 8013BE88 00000000 */   nop
    /* E8C 8013BE8C 21200000 */  addu       $a0, $zero, $zero
  .L8013BE90:
    /* E90 8013BE90 F000A897 */  lhu        $t0, 0xF0($sp)
    /* E94 8013BE94 00000000 */  nop
    /* E98 8013BE98 2400A8A7 */  sh         $t0, 0x24($sp)
    /* E9C 8013BE9C FC00A88F */  lw         $t0, 0xFC($sp)
    /* EA0 8013BEA0 2000A0A7 */  sh         $zero, 0x20($sp)
    /* EA4 8013BEA4 2200A0A7 */  sh         $zero, 0x22($sp)
    /* EA8 8013BEA8 2030028D */  lw         $v0, 0x3020($t0)
    /* EAC 8013BEAC 00000000 */  nop
    /* EB0 8013BEB0 18004400 */  mult       $v0, $a0
    /* EB4 8013BEB4 12100000 */  mflo       $v0
    /* EB8 8013BEB8 02004104 */  bgez       $v0, .L8013BEC4
    /* EBC 8013BEBC 00000000 */   nop
    /* EC0 8013BEC0 FF034224 */  addiu      $v0, $v0, 0x3FF
  .L8013BEC4:
    /* EC4 8013BEC4 F800A88F */  lw         $t0, 0xF8($sp)
    /* EC8 8013BEC8 83120200 */  sra        $v0, $v0, 10
    /* ECC 8013BECC 942F038D */  lw         $v1, 0x2F94($t0)
    /* ED0 8013BED0 FC00A88F */  lw         $t0, 0xFC($sp)
    /* ED4 8013BED4 21186200 */  addu       $v1, $v1, $v0
    /* ED8 8013BED8 4C00A3AF */  sw         $v1, 0x4C($sp)
    /* EDC 8013BEDC 2430028D */  lw         $v0, 0x3024($t0)
    /* EE0 8013BEE0 00000000 */  nop
    /* EE4 8013BEE4 18004400 */  mult       $v0, $a0
    /* EE8 8013BEE8 12100000 */  mflo       $v0
    /* EEC 8013BEEC 02004104 */  bgez       $v0, .L8013BEF8
    /* EF0 8013BEF0 00000000 */   nop
    /* EF4 8013BEF4 FF034224 */  addiu      $v0, $v0, 0x3FF
  .L8013BEF8:
    /* EF8 8013BEF8 F800A88F */  lw         $t0, 0xF8($sp)
    /* EFC 8013BEFC 83120200 */  sra        $v0, $v0, 10
    /* F00 8013BF00 982F038D */  lw         $v1, 0x2F98($t0)
    /* F04 8013BF04 FC00A88F */  lw         $t0, 0xFC($sp)
    /* F08 8013BF08 21186200 */  addu       $v1, $v1, $v0
    /* F0C 8013BF0C 5000A3AF */  sw         $v1, 0x50($sp)
    /* F10 8013BF10 2830028D */  lw         $v0, 0x3028($t0)
    /* F14 8013BF14 00000000 */  nop
    /* F18 8013BF18 18004400 */  mult       $v0, $a0
    /* F1C 8013BF1C 12300000 */  mflo       $a2
    /* F20 8013BF20 0200C104 */  bgez       $a2, .L8013BF2C
    /* F24 8013BF24 00000000 */   nop
    /* F28 8013BF28 FF03C624 */  addiu      $a2, $a2, 0x3FF
  .L8013BF2C:
    /* F2C 8013BF2C 2000A427 */  addiu      $a0, $sp, 0x20
    /* F30 8013BF30 F800A88F */  lw         $t0, 0xF8($sp)
    /* F34 8013BF34 3800A527 */  addiu      $a1, $sp, 0x38
    /* F38 8013BF38 9C2F038D */  lw         $v1, 0x2F9C($t0)
    /* F3C 8013BF3C 00100224 */  addiu      $v0, $zero, 0x1000
    /* F40 8013BF40 2800A2AF */  sw         $v0, 0x28($sp)
    /* F44 8013BF44 2C00A2AF */  sw         $v0, 0x2C($sp)
    /* F48 8013BF48 3000A2AF */  sw         $v0, 0x30($sp)
    /* F4C 8013BF4C 83120600 */  sra        $v0, $a2, 10
    /* F50 8013BF50 21186200 */  addu       $v1, $v1, $v0
    /* F54 8013BF54 4C1F020C */  jal        RotMatrix
    /* F58 8013BF58 5400A3AF */   sw        $v1, 0x54($sp)
    /* F5C 8013BF5C 3800A28F */  lw         $v0, 0x38($sp)
    /* F60 8013BF60 3C00A38F */  lw         $v1, 0x3C($sp)
    /* F64 8013BF64 4000A48F */  lw         $a0, 0x40($sp)
    /* F68 8013BF68 4400A58F */  lw         $a1, 0x44($sp)
    /* F6C 8013BF6C 7C00A2AF */  sw         $v0, 0x7C($sp)
    /* F70 8013BF70 8000A3AF */  sw         $v1, 0x80($sp)
    /* F74 8013BF74 8400A4AF */  sw         $a0, 0x84($sp)
    /* F78 8013BF78 8800A5AF */  sw         $a1, 0x88($sp)
    /* F7C 8013BF7C 4800A28F */  lw         $v0, 0x48($sp)
    /* F80 8013BF80 4C00A38F */  lw         $v1, 0x4C($sp)
    /* F84 8013BF84 5000A48F */  lw         $a0, 0x50($sp)
    /* F88 8013BF88 5400A58F */  lw         $a1, 0x54($sp)
    /* F8C 8013BF8C 8C00A2AF */  sw         $v0, 0x8C($sp)
    /* F90 8013BF90 9000A3AF */  sw         $v1, 0x90($sp)
    /* F94 8013BF94 9400A4AF */  sw         $a0, 0x94($sp)
    /* F98 8013BF98 9800A5AF */  sw         $a1, 0x98($sp)
    /* F9C 8013BF9C 7800A427 */  addiu      $a0, $sp, 0x78
    /* FA0 8013BFA0 2128C003 */  addu       $a1, $fp, $zero
    /* FA4 8013BFA4 C000A0AF */  sw         $zero, 0xC0($sp)
    /* FA8 8013BFA8 B418020C */  jal        GsGetLs
    /* FAC 8013BFAC 7800A0AF */   sw        $zero, 0x78($sp)
    /* FB0 8013BFB0 7415020C */  jal        GsSetLsMatrix
    /* FB4 8013BFB4 2120C003 */   addu      $a0, $fp, $zero
    /* FB8 8013BFB8 C81C020C */  jal        ReadRotMatrix
    /* FBC 8013BFBC 2120C003 */   addu      $a0, $fp, $zero
    /* FC0 8013BFC0 2000A427 */  addiu      $a0, $sp, 0x20
    /* FC4 8013BFC4 4C1F020C */  jal        RotMatrix
    /* FC8 8013BFC8 2128C003 */   addu      $a1, $fp, $zero
    /* FCC 8013BFCC 2120C003 */  addu       $a0, $fp, $zero
    /* FD0 8013BFD0 9C1D020C */  jal        ScaleMatrix
    /* FD4 8013BFD4 2800A527 */   addiu     $a1, $sp, 0x28
    /* FD8 8013BFD8 EC1D020C */  jal        SetRotMatrix
    /* FDC 8013BFDC 2120C003 */   addu      $a0, $fp, $zero
    /* FE0 8013BFE0 C0301700 */  sll        $a2, $s7, 3
    /* FE4 8013BFE4 21202602 */  addu       $a0, $s1, $a2
    /* FE8 8013BFE8 1000C524 */  addiu      $a1, $a2, 0x10
    /* FEC 8013BFEC 21282502 */  addu       $a1, $s1, $a1
    /* FF0 8013BFF0 2000C624 */  addiu      $a2, $a2, 0x20
    /* FF4 8013BFF4 21302602 */  addu       $a2, $s1, $a2
    /* FF8 8013BFF8 38004226 */  addiu      $v0, $s2, 0x38
    /* FFC 8013BFFC 21102202 */  addu       $v0, $s1, $v0
    /* 1000 8013C000 1000A2AF */  sw         $v0, 0x10($sp)
    /* 1004 8013C004 40004226 */  addiu      $v0, $s2, 0x40
    /* 1008 8013C008 21102202 */  addu       $v0, $s1, $v0
    /* 100C 8013C00C 1400A2AF */  sw         $v0, 0x14($sp)
    /* 1010 8013C010 E000A227 */  addiu      $v0, $sp, 0xE0
    /* 1014 8013C014 1800A2AF */  sw         $v0, 0x18($sp)
    /* 1018 8013C018 C800A227 */  addiu      $v0, $sp, 0xC8
    /* 101C 8013C01C 30004726 */  addiu      $a3, $s2, 0x30
    /* 1020 8013C020 0001A88F */  lw         $t0, 0x100($sp)
    /* 1024 8013C024 21382702 */  addu       $a3, $s1, $a3
    /* 1028 8013C028 C0180800 */  sll        $v1, $t0, 3
    /* 102C 8013C02C 21104300 */  addu       $v0, $v0, $v1
    /* 1030 8013C030 21105200 */  addu       $v0, $v0, $s2
    /* 1034 8013C034 441E020C */  jal        RotTransPers3
    /* 1038 8013C038 1C00A2AF */   sw        $v0, 0x1C($sp)
    /* 103C 8013C03C 6400838E */  lw         $v1, 0x64($s4)
    /* 1040 8013C040 6C0082AE */  sw         $v0, 0x6C($s4)
    /* 1044 8013C044 00046228 */  slti       $v0, $v1, 0x400
    /* 1048 8013C048 2C004010 */  beqz       $v0, .L8013C0FC
    /* 104C 8013C04C 01006226 */   addiu     $v0, $s3, 0x1
    /* 1050 8013C050 0401A88F */  lw         $t0, 0x104($sp)
    /* 1054 8013C054 05000224 */  addiu      $v0, $zero, 0x5
    /* 1058 8013C058 28000215 */  bne        $t0, $v0, .L8013C0FC
    /* 105C 8013C05C 01006226 */   addiu     $v0, $s3, 0x1
    /* 1060 8013C060 A830A28E */  lw         $v0, 0x30A8($s5)
    /* 1064 8013C064 00000000 */  nop
    /* 1068 8013C068 80110200 */  sll        $v0, $v0, 6
    /* 106C 8013C06C 21106200 */  addu       $v0, $v1, $v0
    /* 1070 8013C070 640082AE */  sw         $v0, 0x64($s4)
    /* 1074 8013C074 00044228 */  slti       $v0, $v0, 0x400
    /* 1078 8013C078 20004014 */  bnez       $v0, .L8013C0FC
    /* 107C 8013C07C 01006226 */   addiu     $v0, $s3, 0x1
    /* 1080 8013C080 00040224 */  addiu      $v0, $zero, 0x400
    /* 1084 8013C084 640082AE */  sw         $v0, 0x64($s4)
    /* 1088 8013C088 E830A28E */  lw         $v0, 0x30E8($s5)
    /* 108C 8013C08C 00000000 */  nop
    /* 1090 8013C090 02004228 */  slti       $v0, $v0, 0x2
    /* 1094 8013C094 02004010 */  beqz       $v0, .L8013C0A0
    /* 1098 8013C098 02000224 */   addiu     $v0, $zero, 0x2
    /* 109C 8013C09C E830A2AE */  sw         $v0, 0x30E8($s5)
  .L8013C0A0:
    /* 10A0 8013C0A0 0801A88F */  lw         $t0, 0x108($sp)
    /* 10A4 8013C0A4 03000224 */  addiu      $v0, $zero, 0x3
    /* 10A8 8013C0A8 14000215 */  bne        $t0, $v0, .L8013C0FC
    /* 10AC 8013C0AC 01006226 */   addiu     $v0, $s3, 0x1
    /* 10B0 8013C0B0 01000224 */  addiu      $v0, $zero, 0x1
    /* 10B4 8013C0B4 1100E216 */  bne        $s7, $v0, .L8013C0FC
    /* 10B8 8013C0B8 01006226 */   addiu     $v0, $s3, 0x1
    /* 10BC 8013C0BC D830A38E */  lw         $v1, 0x30D8($s5)
    /* 10C0 8013C0C0 00000000 */  nop
    /* 10C4 8013C0C4 03006228 */  slti       $v0, $v1, 0x3
    /* 10C8 8013C0C8 04004010 */  beqz       $v0, .L8013C0DC
    /* 10CC 8013C0CC 01006224 */   addiu     $v0, $v1, 0x1
    /* 10D0 8013C0D0 D830A2AE */  sw         $v0, 0x30D8($s5)
    /* 10D4 8013C0D4 3EF00408 */  j          .L8013C0F8
    /* 10D8 8013C0D8 DC30B7AE */   sw        $s7, 0x30DC($s5)
  .L8013C0DC:
    /* 10DC 8013C0DC E830A38E */  lw         $v1, 0x30E8($s5)
    /* 10E0 8013C0E0 02000224 */  addiu      $v0, $zero, 0x2
    /* 10E4 8013C0E4 05006214 */  bne        $v1, $v0, .L8013C0FC
    /* 10E8 8013C0E8 01006226 */   addiu     $v0, $s3, 0x1
    /* 10EC 8013C0EC 0801A88F */  lw         $t0, 0x108($sp)
    /* 10F0 8013C0F0 00000000 */  nop
    /* 10F4 8013C0F4 E830A8AE */  sw         $t0, 0x30E8($s5)
  .L8013C0F8:
    /* 10F8 8013C0F8 01006226 */  addiu      $v0, $s3, 0x1
  .L8013C0FC:
    /* 10FC 8013C0FC 21984000 */  addu       $s3, $v0, $zero
    /* 1100 8013C100 00140200 */  sll        $v0, $v0, 16
    /* 1104 8013C104 03140200 */  sra        $v0, $v0, 16
    /* 1108 8013C108 02004228 */  slti       $v0, $v0, 0x2
    /* 110C 8013C10C 58FF4014 */  bnez       $v0, .L8013BE70
    /* 1110 8013C110 00141300 */   sll       $v0, $s3, 16
    /* 1114 8013C114 0100C226 */  addiu      $v0, $s6, 0x1
    /* 1118 8013C118 21B04000 */  addu       $s6, $v0, $zero
    /* 111C 8013C11C 00140200 */  sll        $v0, $v0, 16
    /* 1120 8013C120 03140200 */  sra        $v0, $v0, 16
    /* 1124 8013C124 03004228 */  slti       $v0, $v0, 0x3
    /* 1128 8013C128 44FF4014 */  bnez       $v0, .L8013BE3C
    /* 112C 8013C12C 74003126 */   addiu     $s1, $s1, 0x74
    /* 1130 8013C130 DC30A38E */  lw         $v1, 0x30DC($s5)
    /* 1134 8013C134 01000224 */  addiu      $v0, $zero, 0x1
    /* 1138 8013C138 25006214 */  bne        $v1, $v0, .L8013C1D0
    /* 113C 8013C13C 2188A002 */   addu      $s1, $s5, $zero
    /* 1140 8013C140 E800A897 */  lhu        $t0, 0xE8($sp)
    /* 1144 8013C144 00000000 */  nop
    /* 1148 8013C148 00140800 */  sll        $v0, $t0, 16
    /* 114C 8013C14C 03140200 */  sra        $v0, $v0, 16
    /* 1150 8013C150 01004224 */  addiu      $v0, $v0, 0x1
    /* 1154 8013C154 05000324 */  addiu      $v1, $zero, 0x5
    /* 1158 8013C158 1E004314 */  bne        $v0, $v1, .L8013C1D4
    /* 115C 8013C15C 21B00000 */   addu      $s6, $zero, $zero
    /* 1160 8013C160 2188A002 */  addu       $s1, $s5, $zero
  .L8013C164:
    /* 1164 8013C164 21980000 */  addu       $s3, $zero, $zero
    /* 1168 8013C168 00141600 */  sll        $v0, $s6, 16
    /* 116C 8013C16C C3210200 */  sra        $a0, $v0, 7
    /* 1170 8013C170 00141300 */  sll        $v0, $s3, 16
  .L8013C174:
    /* 1174 8013C174 03140200 */  sra        $v0, $v0, 16
    /* 1178 8013C178 80180200 */  sll        $v1, $v0, 2
    /* 117C 8013C17C 21187100 */  addu       $v1, $v1, $s1
    /* 1180 8013C180 00120200 */  sll        $v0, $v0, 8
    /* 1184 8013C184 23100200 */  negu       $v0, $v0
    /* 1188 8013C188 23104400 */  subu       $v0, $v0, $a0
    /* 118C 8013C18C 640062AC */  sw         $v0, 0x64($v1)
    /* 1190 8013C190 01006226 */  addiu      $v0, $s3, 0x1
    /* 1194 8013C194 21984000 */  addu       $s3, $v0, $zero
    /* 1198 8013C198 00140200 */  sll        $v0, $v0, 16
    /* 119C 8013C19C 03140200 */  sra        $v0, $v0, 16
    /* 11A0 8013C1A0 02004228 */  slti       $v0, $v0, 0x2
    /* 11A4 8013C1A4 F3FF4014 */  bnez       $v0, .L8013C174
    /* 11A8 8013C1A8 00141300 */   sll       $v0, $s3, 16
    /* 11AC 8013C1AC 0100C226 */  addiu      $v0, $s6, 0x1
    /* 11B0 8013C1B0 21B04000 */  addu       $s6, $v0, $zero
    /* 11B4 8013C1B4 00140200 */  sll        $v0, $v0, 16
    /* 11B8 8013C1B8 03140200 */  sra        $v0, $v0, 16
    /* 11BC 8013C1BC 03004228 */  slti       $v0, $v0, 0x3
    /* 11C0 8013C1C0 E8FF4014 */  bnez       $v0, .L8013C164
    /* 11C4 8013C1C4 74003126 */   addiu     $s1, $s1, 0x74
    /* 11C8 8013C1C8 DC30A0AE */  sw         $zero, 0x30DC($s5)
    /* 11CC 8013C1CC 2188A002 */  addu       $s1, $s5, $zero
  .L8013C1D0:
    /* 11D0 8013C1D0 21B00000 */  addu       $s6, $zero, $zero
  .L8013C1D4:
    /* 11D4 8013C1D4 10001224 */  addiu      $s2, $zero, 0x10
    /* 11D8 8013C1D8 2000B727 */  addiu      $s7, $sp, 0x20
  .L8013C1DC:
    /* 11DC 8013C1DC 21980000 */  addu       $s3, $zero, $zero
    /* 11E0 8013C1E0 00141600 */  sll        $v0, $s6, 16
    /* 11E4 8013C1E4 43130200 */  sra        $v0, $v0, 13
    /* 11E8 8013C1E8 21105700 */  addu       $v0, $v0, $s7
    /* 11EC 8013C1EC A8005424 */  addiu      $s4, $v0, 0xA8
    /* 11F0 8013C1F0 FF000824 */  addiu      $t0, $zero, 0xFF
    /* 11F4 8013C1F4 040008A2 */  sb         $t0, 0x4($s0)
    /* 11F8 8013C1F8 050008A2 */  sb         $t0, 0x5($s0)
    /* 11FC 8013C1FC 060008A2 */  sb         $t0, 0x6($s0)
    /* 1200 8013C200 100012A2 */  sb         $s2, 0x10($s0)
    /* 1204 8013C204 110012A2 */  sb         $s2, 0x11($s0)
    /* 1208 8013C208 120012A2 */  sb         $s2, 0x12($s0)
    /* 120C 8013C20C 1C0008A2 */  sb         $t0, 0x1C($s0)
    /* 1210 8013C210 1D0008A2 */  sb         $t0, 0x1D($s0)
    /* 1214 8013C214 1E0008A2 */  sb         $t0, 0x1E($s0)
    /* 1218 8013C218 280012A2 */  sb         $s2, 0x28($s0)
    /* 121C 8013C21C 290012A2 */  sb         $s2, 0x29($s0)
    /* 1220 8013C220 2A0012A2 */  sb         $s2, 0x2A($s0)
    /* 1224 8013C224 00141300 */  sll        $v0, $s3, 16
  .L8013C228:
    /* 1228 8013C228 83230200 */  sra        $a0, $v0, 14
    /* 122C 8013C22C 21189100 */  addu       $v1, $a0, $s1
    /* 1230 8013C230 30006294 */  lhu        $v0, 0x30($v1)
    /* 1234 8013C234 00000000 */  nop
    /* 1238 8013C238 080002A6 */  sh         $v0, 0x8($s0)
    /* 123C 8013C23C 32006284 */  lh         $v0, 0x32($v1)
    /* 1240 8013C240 00000000 */  nop
    /* 1244 8013C244 0A0002A6 */  sh         $v0, 0xA($s0)
    /* 1248 8013C248 34006294 */  lhu        $v0, 0x34($v1)
    /* 124C 8013C24C 00000000 */  nop
    /* 1250 8013C250 140002A6 */  sh         $v0, 0x14($s0)
    /* 1254 8013C254 36006284 */  lh         $v0, 0x36($v1)
    /* 1258 8013C258 00000000 */  nop
    /* 125C 8013C25C 160002A6 */  sh         $v0, 0x16($s0)
    /* 1260 8013C260 40006294 */  lhu        $v0, 0x40($v1)
    /* 1264 8013C264 00000000 */  nop
    /* 1268 8013C268 200002A6 */  sh         $v0, 0x20($s0)
    /* 126C 8013C26C 42006284 */  lh         $v0, 0x42($v1)
    /* 1270 8013C270 00000000 */  nop
    /* 1274 8013C274 220002A6 */  sh         $v0, 0x22($s0)
    /* 1278 8013C278 44006294 */  lhu        $v0, 0x44($v1)
    /* 127C 8013C27C 00000000 */  nop
    /* 1280 8013C280 2C0002A6 */  sh         $v0, 0x2C($s0)
    /* 1284 8013C284 46006284 */  lh         $v0, 0x46($v1)
    /* 1288 8013C288 00000000 */  nop
    /* 128C 8013C28C 2E0002A6 */  sh         $v0, 0x2E($s0)
    /* 1290 8013C290 6C00628C */  lw         $v0, 0x6C($v1)
    /* 1294 8013C294 00000000 */  nop
    /* 1298 8013C298 0B004004 */  bltz       $v0, .L8013C2C8
    /* 129C 8013C29C 01006226 */   addiu     $v0, $s3, 0x1
    /* 12A0 8013C2A0 21109400 */  addu       $v0, $a0, $s4
    /* 12A4 8013C2A4 0000428C */  lw         $v0, 0x0($v0)
    /* 12A8 8013C2A8 00000000 */  nop
    /* 12AC 8013C2AC 06004004 */  bltz       $v0, .L8013C2C8
    /* 12B0 8013C2B0 01006226 */   addiu     $v0, $s3, 0x1
    /* 12B4 8013C2B4 6C006694 */  lhu        $a2, 0x6C($v1)
    /* 12B8 8013C2B8 E400A58F */  lw         $a1, 0xE4($sp)
    /* 12BC 8013C2BC C810020C */  jal        GsSortPoly
    /* 12C0 8013C2C0 21200002 */   addu      $a0, $s0, $zero
    /* 12C4 8013C2C4 01006226 */  addiu      $v0, $s3, 0x1
  .L8013C2C8:
    /* 12C8 8013C2C8 21984000 */  addu       $s3, $v0, $zero
    /* 12CC 8013C2CC 00140200 */  sll        $v0, $v0, 16
    /* 12D0 8013C2D0 D5FF4018 */  blez       $v0, .L8013C228
    /* 12D4 8013C2D4 00141300 */   sll       $v0, $s3, 16
    /* 12D8 8013C2D8 0100C226 */  addiu      $v0, $s6, 0x1
    /* 12DC 8013C2DC 21B04000 */  addu       $s6, $v0, $zero
    /* 12E0 8013C2E0 00140200 */  sll        $v0, $v0, 16
    /* 12E4 8013C2E4 03140200 */  sra        $v0, $v0, 16
    /* 12E8 8013C2E8 03004228 */  slti       $v0, $v0, 0x3
    /* 12EC 8013C2EC BBFF4014 */  bnez       $v0, .L8013C1DC
    /* 12F0 8013C2F0 74003126 */   addiu     $s1, $s1, 0x74
    /* 12F4 8013C2F4 E800A897 */  lhu        $t0, 0xE8($sp)
    /* 12F8 8013C2F8 00000000 */  nop
    /* 12FC 8013C2FC 01000225 */  addiu      $v0, $t0, 0x1
    /* 1300 8013C300 E800A2A7 */  sh         $v0, 0xE8($sp)
    /* 1304 8013C304 00140200 */  sll        $v0, $v0, 16
    /* 1308 8013C308 03140200 */  sra        $v0, $v0, 16
    /* 130C 8013C30C 05004228 */  slti       $v0, $v0, 0x5
    /* 1310 8013C310 B9FE4014 */  bnez       $v0, .L8013BDF8
    /* 1314 8013C314 00000000 */   nop
    /* 1318 8013C318 3401BF8F */  lw         $ra, 0x134($sp)
    /* 131C 8013C31C 3001BE8F */  lw         $fp, 0x130($sp)
    /* 1320 8013C320 2C01B78F */  lw         $s7, 0x12C($sp)
    /* 1324 8013C324 2801B68F */  lw         $s6, 0x128($sp)
    /* 1328 8013C328 2401B58F */  lw         $s5, 0x124($sp)
    /* 132C 8013C32C 2001B48F */  lw         $s4, 0x120($sp)
    /* 1330 8013C330 1C01B38F */  lw         $s3, 0x11C($sp)
    /* 1334 8013C334 1801B28F */  lw         $s2, 0x118($sp)
    /* 1338 8013C338 1401B18F */  lw         $s1, 0x114($sp)
    /* 133C 8013C33C 1001B08F */  lw         $s0, 0x110($sp)
    /* 1340 8013C340 3801BD27 */  addiu      $sp, $sp, 0x138
    /* 1344 8013C344 0800E003 */  jr         $ra
    /* 1348 8013C348 00000000 */   nop
endlabel func_8013BDB0
