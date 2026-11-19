#ifndef GAME_FILE_STAGING_BUFFER_H
#define GAME_FILE_STAGING_BUFFER_H

#include "../types.h"
#include "file_constants.h"
#include "card_list_rows.h"

/* Three sectors: the duel's stage 8 loads exactly 3 * FILE_SECTOR_SIZE
 * bytes here, and the field AI script that stage 9 loads next,
 * gAiScript_abFieldScript, sits at +0x1800 in every layout (NA/Europe
 * 0x801A9800, Japan 0x801B9800). */
#define FILE_STAGING_BUFFER_SECTORS 3
#define FILE_STAGING_BUFFER_SIZE (FILE_STAGING_BUFFER_SECTORS * FILE_SECTOR_SIZE)

/* The password shop's view: while that screen runs the buffer holds one
 * record per card id, the card's price in starchips and then its eight-digit
 * password packed four bits a digit. Password_LookupCardID walks the
 * passwords from card 1 (shop_cards[1]) until a price of -1 ends the
 * table. */
typedef struct {
    u32 price;
    s32 password;
} PasswordShopCard;

/* The shared package staging buffer at 0x801A8000 (JP 0x801B8000). Unrelated
 * subsystems load different files into it at different times, so each user
 * reads it through the view of the file it loaded: file-transfer stages and
 * the model sound-bank loader store byte addresses into it, the script
 * engine reads a package that opens with a table of u16 byte offsets indexed
 * by script number, the trade screen keeps its two card-list row sets here,
 * and the password shop its price table. */
typedef union {
    u8 bytes[FILE_STAGING_BUFFER_SIZE];
    u16 script_offsets[FILE_STAGING_BUFFER_SIZE / sizeof(u16)];
    CardListRowSet card_list_rows[CARD_LIST_ROW_SET_COUNT];
    PasswordShopCard shop_cards[FILE_STAGING_BUFFER_SIZE / sizeof(PasswordShopCard)];
} FileStagingBuffer;

extern FileStagingBuffer gFile_StagingBuffer;

#endif
