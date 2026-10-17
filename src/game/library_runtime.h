#ifndef MEMORIES_DECOMP_LIBRARY_RUNTIME_H
#define MEMORIES_DECOMP_LIBRARY_RUNTIME_H

#include "../types.h"
#include "file_transfer.h"
#include "library_state.h"

typedef struct {
    s32 values[4];
} LibraryViewQuad;

typedef char LibraryViewQuad_size_must_be_0x10[
    sizeof(LibraryViewQuad) == 0x10 ? 1 : -1
];

/* Signed coordinate views in the Library overlay work area. The dispatcher
 * compares the copied slot coordinate at 0x80181012 with its reference at
 * 0x80181002 while converging the shared view state. */
extern s16 D_80181002 __attribute__((section(".data")));
extern s16 D_80181012 __attribute__((section(".data")));

/* Library screen state 2, the card view: opens the view, slides the model
 * and text box in, rotates the model light with the view angle and walks the
 * close sequence back to the grid. Library_UpdateScreen passes the owned LibraryState;
 * internal byte/scalar views retain the retail access and scheduling shape. */
void Library_UpdateCardView(LibraryState *state);

/* Selects grid state 1 with a whole-byte write, not grid setup or a reset. */
void Library_EnterGridState(LibraryState *state);
/* The dispatcher's unchanged empty state-3 handler. */
void func_8002BAAC(u8 *state);

void Library_UpdateScreen(void);

/* Package-transfer phase callback installed by Library_InitializeScreen before it
 * initializes the Library screen state and hands control to Library_UpdateScreen. */
void Library_LoadPackageStage(FileTransferDescriptor *object, s32 mode);

/* Walks the save's card table and marks every owned card in the Library
 * screen's own state, which is what makes the grid draw them as obtained. */
void Library_MarkOwnedCards(void);

/* The Library screen's entry: sets the screen up, installs Library_LoadPackageStage as
 * the package-transfer phase callback and hands control to Library_UpdateScreen. */
void Library_InitializeScreen(void);

#endif
