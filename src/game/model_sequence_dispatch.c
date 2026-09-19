#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/libhmd.h"
#include "../psyq/stdio.h"
#include "../psyq/exit.h"

#include "model_handler_state.h"
#include "model_packet_handlers.h"

/*
 * All 1844 bytes match under the existing uniform gcc_2_8_1_g0 profile.
 * The shared scanner inlines into the four endpoint walks. Direction-local
 * index captures preserve one read; separate duration paths retain their
 * distinct stores; the count precedes the handler table in the spill order.
 *
 * The single-pass endpoint checks preserve GCC 2.8.1's reference weighting.
 * Flattening them exchanges the context and cached-sentinel registers in
 * fifteen words. The ModelAnimParams tail keeps its byte-addressed form but
 * derives its base from GsARGUNIT_ANIM::header_size. No register bindings or
 * instruction assembly are used.
 */
static __inline__ u32 scan_command(GsSEQ *track, u32 *commands, u32 index)
{
    s32 step;
    u32 word;
    u32 condition;
    u32 action;

    step = track->speed < 0 ? -1 : 1;
    while ((word = commands[(u16)index]) & 0x80000000) {
        if ((word & 0xC0000000) == 0x80000000) {
            condition = (word & 0x7F0000) >> 16;
            if (condition == 0 || track->sid == condition) {
                action = (word & 0x3F800000) >> 23;
                if (condition != 0 || action != 0) {
                    track->sid = action;
                }
                index = word;
                continue;
            }
        } else if ((word & 0xC0000000) == 0xC0000000) {
            condition = (word & 0x7F0000) >> 16;
            action = (word & 0x3F800000) >> 23;
            if (action == 1 && (condition == 0 || track->sid == condition)) {
                return 0xFFFF;
            }
        } else {
            printf(D_800117EC, D_80011814, 0x57);
            exit(1);
        }
        index += step;
    }
    return index;
}

u32 *func_8005C7BC(GsARGUNIT_ANIM *ctx)
{
    GsSEQ *cursor;
    GsSEQ *track;
    s32 count;
    u32 *handlers;
    u32 *commands;
    u32 *parameters;
    ModelAnimParams *slots;
    s32 (*handler)(GsARGUNIT_ANIM *);
    s32 i;
    s32 direction;
    u32 active;
    u32 found;
    u32 value;
    u32 duration;
    u8 *dispatch_record;
    s32 result;
    u32 *end;

    cursor = (GsSEQ *)((u8 *)ctx->primp + 4);
    handlers = (u32 *)ctx->htop;
    commands = (u32 *)ctx->ctop;
    count = ((u16 *)ctx->primp)[1];
    parameters = (u32 *)ctx->ptop;
    for (i = 0; i < count; i++) {
        active = cursor->aframe;
        track = cursor;
        if ((u16)active != 0) {
            if ((u16)active != 0xFFFF) {
                cursor->aframe = active - 1;
            }
            direction = cursor->speed < 0 ? -1 : 1;
            if (cursor->rframe == 0x7000) {
                direction = 1;
                cursor->rframe = 0;
            } else if (cursor->rframe == 0x6000) {
                direction = -1;
                cursor->rframe = 0;
            }
retry:
            if (track->rframe == 0 || track->tframe == 0) {
                track->traveling = 0;
                if (direction == 1) {
                    u32 old_index;
                    if (commands[track->ti] & 0x80000000) {
                        found = scan_command(track, commands, track->ti + 1);
                        track->ti = found;
                        do {
                            if ((u16)found == 0xFFFF) goto stopped;
                        } while (0);
                    }
                    old_index = track->ti;
                    track->ci = old_index;
                    found = scan_command(track, commands, old_index + direction);
                    track->ti = found;
                } else {
                    u32 old_index;
                    if (commands[track->ci] & 0x80000000) {
                        found = scan_command(track, commands, track->ci + direction);
                        track->ci = found;
                        do {
                            if ((u16)found == 0xFFFF) goto stopped;
                        } while (0);
                    }
                    old_index = track->ci;
                    track->ti = old_index;
                    found = scan_command(track, commands, old_index + direction);
                    track->ci = found;
                }
                if ((u16)found == 0xFFFF) {
stopped:
                    track->aframe = 0;
                    goto next_track;
                }
                if (direction == 1) {
                    duration = ((u8 *)&commands[track->ti])[2] << 4;
                    track->tframe = duration;
                    track->rframe = duration - (u16)track->rframe;
                } else {
                    track->tframe = ((u8 *)&commands[track->ti])[2] << 4;
                }
            }
            dispatch_record = (u8 *)(
                (u32)&((u32 *)0)[track->ti] + (u32)commands);
            slots = (ModelAnimParams *)(
                (u8 *)ctx +
                (u32)&((GsARGUNIT_ANIM *)0)->header_size +
                (ctx->header_size << 2)
            );
            value = dispatch_record[3] & 0x7F;
            handler = (s32 (*)(GsARGUNIT_ANIM *))handlers[value + 1];
            slots->seq = cursor;
            slots->source =
                (ModelKeyframe *)(parameters + (u16)commands[track->ci]);
            slots->target =
                (ModelKeyframe *)(parameters + (u16)commands[track->ti]);
            if (track->ii != 0xFFFF) {
                slots->out =
                    (ModelKeyframe *)(parameters + (u16)commands[track->ii]);
            } else {
                slots->out = 0;
            }
            if (track->ci != track->ti) {
                result = handler(ctx);
                if (result == 1) {
                    track->rframe = 0;
                    goto retry;
                }
                if (((u8 *)&commands[track->ti])[2] == 0) {
                    goto retry;
                }
                track->rframe = (u16)track->rframe - track->speed;
                if (direction == 1 && track->rframe < 0) {
                    track->rframe = 0x7000;
                } else if (direction == -1 && track->rframe >= track->tframe) {
                    track->rframe = 0x6000;
                }
            }
        }
next_track:
        cursor = (GsSEQ *)(
            (u8 *)cursor + (u32)&((u32 *)0)[cursor->size]);
    }
    end = (u32 *)ctx->primp;
    return end + *(u16 *)end;
}
