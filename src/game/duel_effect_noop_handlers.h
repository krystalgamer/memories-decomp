#ifndef MEMORIES_DECOMP_DUEL_EFFECT_NOOP_HANDLERS_H
#define MEMORIES_DECOMP_DUEL_EFFECT_NOOP_HANDLERS_H

/* Two empty handlers that exist to fill slots in the duel-effect dispatch
   tables. Both bodies are empty; they are distinct symbols because the tables
   reference them at different indices. */
void func_800289AC(void);
void func_800289B4(void);

#endif
