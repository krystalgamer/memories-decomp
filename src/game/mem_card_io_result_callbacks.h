#ifndef MEMORIES_DECOMP_MEM_CARD_IO_RESULT_CALLBACKS_H
#define MEMORIES_DECOMP_MEM_CARD_IO_RESULT_CALLBACKS_H

PSXLONG MemCard_SetIOResultCompleteCB(void);
PSXLONG MemCard_SetIOResultTimeoutCB(void);
PSXLONG MemCard_SetIOResultErrorCB(void);
PSXLONG MemCard_SetIOResultNewCardCB(void);

#endif
