#ifndef MEMORIES_DECOMP_FUNC_80059208_H
#define MEMORIES_DECOMP_FUNC_80059208_H

/* Returns &D_8009B478, the camera pitch triple model_update_view_metrics.h
   declares as u16. The return type is void * because that is what the
   definition returns, and it is why the three callers could each spell the
   result differently without anything complaining: void * converts to
   whichever pointer each of them wanted. */
void *func_80059208(void);

#endif
