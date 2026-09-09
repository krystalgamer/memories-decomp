#ifndef MEMORIES_DECOMP_LIBRARY_MARK_OWNED_CARDS_H
#define MEMORIES_DECOMP_LIBRARY_MARK_OWNED_CARDS_H

#include "../types.h"

/* Walks the save's card table and marks every owned card in the Library
 * screen's own state, which is what makes the grid draw them as obtained. */
void Library_MarkOwnedCards(void);

#endif
