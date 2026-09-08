#include "../types.h"
#include "sound.h"

typedef struct {
    u32 pos;
    u32 pos_saved;
    u8 pad08[0x10];
    u16 field_18;
    u16 field_1A;
    u32 field_1C;
    u32 field_20;
    u8 field_24;
    u8 field_25;
    u8 loop_count;
    u8 pad27[2];
    u8 field_29;
    u8 field_2A;
    u8 pad2B;
} SDSequenceTrack;

extern void func_8004B374(s32, s32, s32);
extern void func_8004ADE8(s32, s32, s32);
extern void func_8004B49C(s32, s32, s32);
extern void func_8004B6E8(s32, s32);
extern void func_8004B70C(s32, s32, s32);

#define SEQ_U32(off) (*(u32 *)((u8 *)D_8009B458 + (off)))
#define SEQ_U16(off) (*(u16 *)((u8 *)D_8009B458 + (off)))

/* Sequence channel event dispatcher: routes a note off (0x80), note on
   (0x90, a zero velocity is a note off), controller (0xB0), program (0xC0)
   or pitch (0xE0) message to the secondary-object handlers. Controller
   0x63 with value 0x14 saves every track's position and registers (or
   just this track's when the sequence has no track table) for a loop, and
   value 0x1E counts the loop down and restores them; controller 6 while
   the channel is in mode 0x14 sets the loop count on every track (or on
   this track alone, without forwarding) before the controller is passed
   on. */
void func_8004C114(SDSequenceTrack *p, s32 status, u8 d1, u8 d2) {
    SDSequenceTrack *t;
    u8 ch;
    s32 n;
    u8 *q;
    s32 i;
    u8 v;

    ch = status & SD_SEQUENCE_CHANNEL_MASK;
    switch (status & SD_SEQUENCE_STATUS_MASK) {
    case SD_SEQUENCE_NOTE_OFF:
        func_8004B374(ch, d1, d2);
        break;
    case SD_SEQUENCE_NOTE_ON:
        if (d2 != 0) {
            func_8004ADE8(ch, d1, d2);
        } else {
            func_8004B374(ch, d1, 0);
        }
        break;
    case SD_SEQUENCE_CONTROL_CHANGE:
        if (d1 == SD_SEQUENCE_CONTROL_MODE) {
            if (d2 == SD_SEQUENCE_LOOP_START) {
                u8 *seq;

                seq = (u8 *)D_8009B458;
                if (*(u16 *)(seq + 0x7F8) != 0) {
                    if (((SDSecondaryState *)seq)->field_07FA != 0) {
                        i = 0;
                        do {
                            t = (SDSequenceTrack *)(seq + 0x518 + i * SD_SEQUENCE_TRACK_RECORD_SIZE);
                            t->field_25 = t->field_24;
                            t->field_2A = t->field_29;
                            t->loop_count = d2;
                            t->pos_saved = t->pos;
                            t->field_20 = t->field_1C;
                            t->field_1A = t->field_18;
                            seq = (u8 *)D_8009B458;
                            i++;
                        } while (i < ((SDSecondaryState *)seq)->field_07FA);
                    }
                    SEQ_U32(0x7F4) = SEQ_U32(0x7F0);
                } else {
                    p->loop_count = SD_SEQUENCE_LOOP_UNCOUNTED;
                    p->pos_saved = p->pos;
                }
            } else if (d2 == SD_SEQUENCE_LOOP_END) {
                v = p->loop_count;
                if (v != 0) {
                    u8 *seq;

                    if (v < SD_SEQUENCE_LOOP_UNCOUNTED) {
                        p->loop_count = v - 1;
                    }
                    seq = (u8 *)D_8009B458;
                    if (*(u16 *)(seq + 0x7F8) != 0) {
                        if (((SDSecondaryState *)seq)->field_07FA != 0) {
                            i = 0;
                            do {
                                t = (SDSequenceTrack *)(seq + 0x518 + i * SD_SEQUENCE_TRACK_RECORD_SIZE);
                                t->field_24 = t->field_25;
                                t->field_29 = t->field_2A;
                                t->pos = t->pos_saved;
                                t->field_1C = t->field_20;
                                t->field_18 = t->field_1A;
                                seq = (u8 *)D_8009B458;
                                i++;
                            } while (i < ((SDSecondaryState *)seq)->field_07FA);
                        }
                        SEQ_U32(0x7F0) = SEQ_U32(0x7F4);
                    } else {
                        p->pos = p->pos_saved;
                    }
                }
            }
        }
        if (d1 == SD_SEQUENCE_CONTROL_DATA_ENTRY) {
            u8 *seq;

            seq = (u8 *)D_8009B458;
            n = ch * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
            if (*(seq + n + 0x12) == SD_SEQUENCE_LOOP_START) {
                if (*(u16 *)(seq + 0x7F8) != 0) {
                    if (((SDSecondaryState *)seq)->field_07FA != 0) {
                        i = 0;
                        do {
                            q = seq + i * SD_SEQUENCE_TRACK_RECORD_SIZE;
                            q[0x53E] = d2;
                            seq = (u8 *)D_8009B458;
                            i++;
                        } while (i < ((SDSecondaryState *)seq)->field_07FA);
                    }
                } else {
                    p->loop_count = d2;
                    break;
                }
            }
        }
        func_8004B49C(ch, d1, d2);
        break;
    case SD_SEQUENCE_PROGRAM_CHANGE:
        func_8004B6E8(ch, d1);
        break;
    case SD_SEQUENCE_PITCH_BEND:
        func_8004B70C(ch, d1, d2);
        break;
    }
}
