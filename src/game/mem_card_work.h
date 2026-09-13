#ifndef MEMORIES_DECOMP_MEM_CARD_WORK_H
#define MEMORIES_DECOMP_MEM_CARD_WORK_H

#include "../types.h"

#define MEM_CARD_WORK_SLOT_CAPACITY 2
#define MEM_CARD_WORK_SLOT_SIZE 0x268
#define MEM_CARD_WORK_ROOT_SIZE 0x1C
#define MEM_CARD_WORK_CALLBACK_COUNT 5
#define MEM_CARD_WORK_FRAME_SIZE 128
#define MEM_CARD_WORK_READ_ADDRESS 0x80200000
#define MEM_CARD_WORK_FRAME_ADDRESS 0x80210000
#define MEM_CARD_WORK_WRITE_ADDRESS 0x80300000
#define MEM_CARD_WORK_WRITE_BODY_ADDRESS 0x80300200
#define MEM_CARD_WORK_FRAME_BYTES \
    (*(u8 (*)[MEM_CARD_WORK_FRAME_SIZE])MEM_CARD_WORK_FRAME_ADDRESS)

typedef struct {
    u8 *cursor;
    u8 pad_04[8];
    u8 free_blocks;
    u8 entry_index;
    u8 pad_0E[0x25A];
} MemCardWorkSlot;

typedef struct {
    u8 pad_00[0x10];
    u8 blocks;
    u8 pad_11;
    s16 remaining;
    u16 offset;
    s16 chunk;
    u16 field_18;
    u8 text_index;
    u8 pad_1B;
} MemCardWorkRoot;

/* A bounded view of the existing image backing, not a new allocation.
 * Two complete slots fit before the separate directory at 0x800EFBC0.
 * The only observed selector writer initializes zero; the byte type does
 * not make arbitrary selectors safe. See notes/memory-card-work-controller.md. */
typedef struct {
    MemCardWorkRoot root;
    MemCardWorkSlot slots[MEM_CARD_WORK_SLOT_CAPACITY];
} MemCardWorkArea;

typedef char MemCardWorkSlot_size_must_match[
    sizeof(MemCardWorkSlot) == MEM_CARD_WORK_SLOT_SIZE ? 1 : -1
];
typedef char MemCardWorkRoot_size_must_match[
    sizeof(MemCardWorkRoot) == MEM_CARD_WORK_ROOT_SIZE ? 1 : -1
];
typedef char MemCardWorkArea_size_must_be_0x4EC[
    sizeof(MemCardWorkArea) == 0x4EC ? 1 : -1
];
typedef char MemCardWorkSlot_offsets_must_match[
    (u32)&((MemCardWorkSlot *)0)->free_blocks == 0xC &&
    (u32)&((MemCardWorkSlot *)0)->entry_index == 0xD ? 1 : -1
];
typedef char MemCardWorkRoot_offsets_must_match[
    (u32)&((MemCardWorkRoot *)0)->blocks == 0x10 &&
    (u32)&((MemCardWorkRoot *)0)->remaining == 0x12 &&
    (u32)&((MemCardWorkRoot *)0)->offset == 0x14 &&
    (u32)&((MemCardWorkRoot *)0)->chunk == 0x16 &&
    (u32)&((MemCardWorkRoot *)0)->text_index == 0x1A ? 1 : -1
];

extern MemCardWorkArea D_800EF6D0;
extern u16 D_8009B3CC;
extern u8 D_8009B3CF;
extern u8 D_8009B3DD;
extern u8 D_8009B3D5;
extern s32 D_8009B3C8;
extern s32 D_8009B3E4;

/* Measured two-argument caller view. The five stored callbacks currently
 * consume the root through their own MenuRecord view and ignore argument 2.
 * The low-nibble selector is not evidence of a sixteen-entry table. */
extern void (*D_80090F88[MEM_CARD_WORK_CALLBACK_COUNT])(
    MemCardWorkRoot *root, MemCardWorkSlot *slot
);

/* CPU-addressed directory frame: all six scalar identities below are
 * interior bytes of this same 128-byte range, never separate storage.
 * Existing absolute linker symbols supply those identities. No RAM-mirror
 * equivalence with other addresses is assumed by this view. */
extern u8 D_80210000[MEM_CARD_WORK_FRAME_SIZE];
extern u8 D_8021007A __attribute__((section(".data")));
extern u8 D_8021007B __attribute__((section(".data")));
extern u8 D_8021007C __attribute__((section(".data")));
extern u8 D_8021007D __attribute__((section(".data")));
extern u8 D_8021007E __attribute__((section(".data")));
extern u8 D_8021007F __attribute__((section(".data")));

extern char D_8009AF6C[];
extern char D_80010378[];
extern char D_80010398[];
extern char D_800103A4[];

void func_8003DC1C(void);

#endif
