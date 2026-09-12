#ifndef MEMORIES_DECOMP_LIBRARY_RUNTIME_H
#define MEMORIES_DECOMP_LIBRARY_RUNTIME_H

#include "../types.h"
#include "file_transfer.h"

typedef struct {
    s32 values[4];
} LibraryViewQuad;

typedef char LibraryViewQuad_size_must_be_0x10[
    sizeof(LibraryViewQuad) == 0x10 ? 1 : -1
];

/* The Library state base. func_8002BAB4 dispatches on
 * `D_800EA1E8[0] & 0xF`; func_8002BFCC also needs this byte declaration while
 * reaching motion fields and storage beyond the typed record's known end.
 * func_8002A3CC.h documents that wider evidence and keeps the incompatible
 * LibraryMotionState declaration isolated.
 *
 * src/candidates/func_80029EC4.c is a third reader of this byte spelling: it
 * walks the table with func_80029EB0(D_800EA1E8, index) and had been declaring
 * it privately in the same words. */
extern u8 D_800EA1E8[];

/* Signed coordinate views in the Library overlay work area. The dispatcher
 * compares the copied slot coordinate at 0x80181012 with its reference at
 * 0x80181002 while converging the shared view state. */
extern s16 D_80181002 __attribute__((section(".data")));
extern s16 D_80181012 __attribute__((section(".data")));

/* The dispatcher's handlers for states 0 and 3: the first sets the state
 * byte it is handed to 1, the second does nothing. */
void func_8002BAA0(u8 *state);
void func_8002BAAC(u8 *state);

void func_8002BAB4(void);

/* Package-transfer phase callback installed by func_8002BFCC before it
 * initializes the Library screen state and hands control to func_8002BAB4. */
void func_8002BD0C(FileTransferDescriptor *object, s32 mode);

/* Walks the save's card table and marks every owned card in the Library
 * screen's own state, which is what makes the grid draw them as obtained. */
void Library_MarkOwnedCards(void);

/* The Library screen's entry: sets the screen up, installs func_8002BD0C as
 * the package-transfer phase callback and hands control to func_8002BAB4. */
void func_8002BFCC(void);

#endif
