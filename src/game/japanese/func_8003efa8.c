#include "../../types.h"
#include "../data_transfer_request.h"
#include "../mem_card.h"

/* Japanese only, with no US counterpart: opens the memory-card dialog at
   step 3 for the 0x8000-byte buffer at 0x80140000, under the file name the
   Japanese image keeps at 0x8001032C ("BISLPMP86398-YUGIOH"). */
void func_8003EFA8(void)
{
    MemCardDialog_Request((u8 *)0x80140000, 0x8000, D_8001032C, 3);
}
