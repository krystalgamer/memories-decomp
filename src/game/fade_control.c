#include "../types.h"

#include "fade.h"

extern u8 D_8009B141;
extern void func_80015780(void);
extern void func_800157DC(void);
extern void func_8001581C(void);
extern void func_80015998(void);
extern void func_8001572C(void);
extern void Fade_InitOut(void);
extern void Fade_StartOut(void);
extern void func_80015944(void);
extern void func_80015870(void);

void func_800159D8(void)
{
    func_80015780();
    func_80015998();
}

void func_80015A00(void)
{
    func_800157DC();
    func_80015998();
}

void func_80015A28(void)
{
    func_8001581C();
    func_80015998();
}

void func_80015A50(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_8001572C();
    func_80015998();
}

void func_80015A94(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_8001572C();
    func_80015998();
}

void func_80015AD8(void)
{
    Fade_InitOut();
    func_80015998();
}

void Fade_WaitOut(void)
{
    Fade_StartOut();
    func_80015998();
}

void func_80015B28(void)
{
    func_80015944();
    func_80015998();
}

void func_80015B50(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_80015870();
    func_80015998();
}

void func_80015B94(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_80015870();
    func_80015998();
}

void func_80015BD8(s32 value, s32 flags)
{
    FadeTransitionState *state = &D_800E9EC8;

    state->target_level = value;
    state->flags = flags | 0x80;
}

void func_80015BF0(s32 value)
{
    FadeTransitionState *state = &D_800E9EC8;

    state->level = value;
    state->target_level = value;
    state->flags = 0x80;
}

void func_80015C0C(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_8001572C();
}

void func_80015C48(void)
{
    FadeTransitionState *state;

    func_80015780();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_8001572C();
}

void func_80015C84(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 2;
    func_80015870();
}

void func_80015CC0(void)
{
    FadeTransitionState *state;

    Fade_InitOut();
    state = &D_800E9EC8;
    state->flags |= 6;
    func_80015870();
}

void func_80015CFC(void)
{
    D_8009B141 = 1;
}

void func_80015D0C(void)
{
    D_8009B141 = 0;
}
