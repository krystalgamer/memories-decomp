/* The credits overlay's runtime: presentation setup, the per-frame line
 * update, glyph building, line drawing and line-group start, in image order. */
#include "../../types.h"
#include "../../game/main.h"
#include "../../psyq/libapi.h"
#include "../../psyq/strings.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/ordering_tables.h"
#include "credits.h"

/* Each row's text pointer is a literal: the compiler emits them to this
 * unit's .rodata, ahead of the palette rectangles, in reverse order of first
 * use with identical strings merged, as the retail module stores them. */
#define CREDITS_ENTRY(group, flags, text) \
    { (group), (flags), { 0, 0 }, (u8 *)(text) }

CreditsEntry gCredits_aEntries[] = {
#ifdef VERSION_JAPAN
    CREDITS_ENTRY(0, 0x01,
                  "\x83\x49\x83\x8A\x83\x57\x83\x69\x83\x8B\x83\x4C\x83\x83\x83\x89"
                  "\x83\x4E\x83\x5E\x81\x5B\x83\x66\x83\x55\x83\x43\x83\x93"),
    CREDITS_ENTRY(0, 0x01,
                  "\x83\x49\x83\x8A\x83\x57\x83\x69\x83\x8B\x83\x82\x83\x93\x83\x58"
                  "\x83\x5E\x81\x5B\x83\x66\x83\x55\x83\x43\x83\x93"),
    CREDITS_ENTRY(0, 0x01, "\x83\x56\x83\x69\x83\x8A\x83\x49\x8C\xB4\x88\xC4"),
    CREDITS_ENTRY(0, 0x00, 0),
    CREDITS_ENTRY(0, 0x00,
                  "\x81\x40\x81\x40\x8D\x82\x8B\xB4\x81\x40\x98\x61\x8A\xF3"),
    CREDITS_ENTRY(1, 0x01,
                  "\x8A\xE9\x89\xE6\x81\x45\x8E\x64\x97\x6C\x90\xDD\x8C\x76\x81\x45"
                  "\x91\x8D\x8A\xC4\x93\xC2"),
    CREDITS_ENTRY(1, 0x00, 0),
    CREDITS_ENTRY(1, 0x00,
                  "\x81\x40\x81\x40\x8B\x76\x95\xDB\x81\x40\x90\xB7\x8D\x91"),
    CREDITS_ENTRY(2, 0x01, "\x8B\x72\x96\x7B"),
    CREDITS_ENTRY(2, 0x00, 0),
    CREDITS_ENTRY(2, 0x00,
                  "\x81\x40\x81\x40\x8A\xA0\x95\x94\x81\x40\x89\xEB\x8F\xCD"),
    CREDITS_ENTRY(3, 0x01,
                  "\x83\x76\x83\x8D\x83\x4F\x83\x89\x83\x80\x8A\xC4\x93\xC2"),
    CREDITS_ENTRY(3, 0x00, 0),
    CREDITS_ENTRY(3, 0x00, "\x81\x40\x81\x40\x82\x66\x82\x74\x82\x6D"),
    CREDITS_ENTRY(4, 0x01,
                  "\x93\x47\x8E\x76\x8D\x6C\x83\x8D\x83\x57\x83\x62\x83\x4E"),
    CREDITS_ENTRY(4, 0x00, 0),
    CREDITS_ENTRY(4, 0x00,
                  "\x81\x40\x81\x40\x95\xBD\x93\x63\x81\x40\x89\x70\x8D\x4F"),
    CREDITS_ENTRY(4, 0x00,
                  "\x81\x40\x81\x40\x8B\x76\x95\xDB\x81\x40\x90\xB7\x8D\x91"),
    CREDITS_ENTRY(5, 0x01,
                  "\x82\x52\x82\x63\x81\x45\x82\x72\x82\x65\x82\x77\x8A\xC4\x93\xC2"),
    CREDITS_ENTRY(5, 0x00, 0),
    CREDITS_ENTRY(5, 0x00, "\x81\x40\x81\x40\x82\x69\x82\x74\x82\x6D"),
    CREDITS_ENTRY(6, 0x01, "\x82\x72\x82\x65\x82\x77\x83\x60\x81\x5B\x83\x80"),
    CREDITS_ENTRY(6, 0x00, 0),
    CREDITS_ENTRY(6, 0x00, "\x81\x40\x81\x40\x94\xC2\x91\x71\x81\x40\x90\xBD"),
    CREDITS_ENTRY(6, 0x00,
                  "\x81\x40\x81\x40\x82\x73\x82\x71\x82\x74\x82\x73\x82\x67"),
    CREDITS_ENTRY(6, 0x00,
                  "\x81\x40\x81\x40\x8B\xB4\x96\x7B\x81\x40\x91\xEC\x96\xE7"),
    CREDITS_ENTRY(6, 0x00,
                  "\x81\x40\x81\x40\x90\xEC\x96\x94\x81\x40\x91\x50\x93\x54"),
    CREDITS_ENTRY(7, 0x01, "\x94\xFC\x8F\x70\x8A\xC4\x93\xC2"),
    CREDITS_ENTRY(7, 0x00, 0),
    CREDITS_ENTRY(7, 0x00,
                  "\x81\x40\x81\x40\x90\x5F\x94\xF6\x81\x40\x90\x5E\x8E\x6A"),
    CREDITS_ENTRY(8, 0x01,
                  "\x82\x51\x82\x63\x83\x4F\x83\x89\x83\x74\x83\x42\x83\x62\x83\x4E"
                  "\x83\x58"),
    CREDITS_ENTRY(8, 0x40, 0),
    CREDITS_ENTRY(8, 0x42,
                  "\x81\x40\x81\x40\x83\x4A\x81\x5B\x83\x68\x8C\xB4\x89\xE6"),
    CREDITS_ENTRY(8, 0x40, 0),
    CREDITS_ENTRY(8, 0x40,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x98\x51\x89\x7A\x81\x40\x8D\x4B"
                  "\x8E\x69\x81\x69\x82\x50\x82\x55\x81\x6A"),
    CREDITS_ENTRY(8, 0x40,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\x6D\x95\x94\x81\x40\x8D\x84"),
    CREDITS_ENTRY(8, 0x30, 0),
    CREDITS_ENTRY(8, 0x32,
                  "\x81\x40\x81\x40\x83\x4A\x81\x5B\x83\x68\x92\x85\x8D\xCA"),
    CREDITS_ENTRY(8, 0x30, 0),
    CREDITS_ENTRY(8, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x94\xA8\x92\x86\x81\x40\x97\x46"
                  "\x8B\x49\x8E\x71"),
    CREDITS_ENTRY(8, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\xE5\x96\xEC\x81\x40\x93\x4E"
                  "\x96\xE7"),
    CREDITS_ENTRY(8, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\xCA\x92\x4A\x81\x40\x92\x6D"
                  "\x8E\x75"),
    CREDITS_ENTRY(8, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x5F\x94\xF6\x81\x40\x90\x5E"
                  "\x8E\x6A"),
    CREDITS_ENTRY(8, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x98\x51\x89\x7A\x81\x40\x8D\x4B"
                  "\x8E\x69\x81\x69\x82\x50\x82\x55\x81\x6A"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x22,
                  "\x81\x40\x81\x40\x83\x4C\x83\x83\x83\x93\x83\x79\x81\x5B\x83\x93"
                  "\x8C\xB4\x89\xE6"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\x6D\x95\x94\x81\x40\x8D\x84"),
    CREDITS_ENTRY(8, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x88\xE4\x8F\xE3\x81\x40\x8C\x92"
                  "\x93\xF1"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x22,
                  "\x81\x40\x81\x40\x83\x4C\x83\x83\x83\x93\x83\x79\x81\x5B\x83\x93"
                  "\x92\x85\x8D\xCA"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x94\xA8\x92\x86\x81\x40\x97\x46"
                  "\x8B\x49\x8E\x71"),
    CREDITS_ENTRY(8, 0x10, 0),
    CREDITS_ENTRY(8, 0x12,
                  "\x81\x40\x81\x40\x83\x51\x81\x5B\x83\x80\x83\x70\x81\x5B\x83\x63"
                  "\x83\x4F\x83\x89\x83\x74\x83\x42\x83\x62\x83\x4E\x83\x58\x81\x40"
                  "\x81\x40"),
    CREDITS_ENTRY(8, 0x10, 0),
    CREDITS_ENTRY(8, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x94\xA8\x92\x86\x81\x40\x97\x46"
                  "\x8B\x49\x8E\x71"),
    CREDITS_ENTRY(8, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x98\x51\x89\x7A\x81\x40\x8D\x4B"
                  "\x8E\x69\x81\x69\x82\x50\x82\x55\x81\x6A"),
    CREDITS_ENTRY(8, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x5F\x8E\x52\x81\x40\x89\xC1"
                  "\x93\xDE\x8E\x71"),
    CREDITS_ENTRY(9, 0x01,
                  "\x82\x52\x82\x63\x83\x4F\x83\x89\x83\x74\x83\x42\x83\x62\x83\x4E"
                  "\x83\x58"),
    CREDITS_ENTRY(9, 0x30, 0),
    CREDITS_ENTRY(9, 0x32,
                  "\x81\x40\x81\x40\x83\x4C\x83\x83\x83\x93\x83\x79\x81\x5B\x83\x93"
                  "\x94\x77\x8C\x69"),
    CREDITS_ENTRY(9, 0x30, 0),
    CREDITS_ENTRY(9, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\xCE\x8C\xB4\x81\x40\x88\xE8"
                  "\x98\x4E"),
    CREDITS_ENTRY(9, 0x20, 0),
    CREDITS_ENTRY(9, 0x22,
                  "\x81\x40\x83\x82\x83\x93\x83\x58\x83\x5E\x81\x5B\x83\x82\x83\x66"
                  "\x83\x8A\x83\x93\x83\x4F\x81\x45\x83\x82\x81\x5B\x83\x56\x83\x87"
                  "\x83\x93\x81\x40"),
    CREDITS_ENTRY(9, 0x20, 0),
    CREDITS_ENTRY(9, 0x20,
                  "\x81\x40\x81\x40\x8C\x46\x92\x4A\x81\x40\x92\xBC\x8E\xF7\x81\x40"
                  "\x81\x40\x81\x40\x96\x6D\x95\x94\x81\x40\x8D\x84"),
    CREDITS_ENTRY(9, 0x20,
                  "\x81\x40\x81\x40\x8D\xE2\x96\x7B\x81\x40\x8F\x5B\x8E\x71\x81\x40"
                  "\x81\x40\x81\x40\x88\xE4\x8F\xE3\x81\x40\x8C\x92\x93\xF1"),
    CREDITS_ENTRY(9, 0x20,
                  "\x81\x40\x81\x40\x96\xCA\x92\x4A\x81\x40\x92\x6D\x8E\x75\x81\x40"
                  "\x81\x40\x81\x40\x90\x5F\x94\xF6\x81\x40\x90\x5E\x8E\x6A"),
    CREDITS_ENTRY(9, 0x20,
                  "\x81\x40\x81\x40\x96\xE5\x96\xEC\x81\x40\x93\x4E\x96\xE7\x81\x40"
                  "\x81\x40\x81\x40\x90\x99\x96\x7B\x81\x40\x91\xA5\x94\x56"),
    CREDITS_ENTRY(9, 0x20,
                  "\x81\x40\x81\x40\x98\x51\x89\x7A\x81\x40\x8D\x4B\x8E\x69\x81\x69"
                  "\x82\x50\x82\x55\x81\x6A"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x12,
                  "\x81\x40\x81\x40\x82\x72\x82\x65\x82\x77\x91\x66\x8D\xDE"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\xE5\x96\xEC\x81\x40\x93\x4E"
                  "\x96\xE7"),
    CREDITS_ENTRY(9, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x5F\x94\xF6\x81\x40\x90\x5E"
                  "\x8E\x6A"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x12,
                  "\x81\x40\x81\x40\x83\x49\x81\x5B\x83\x76\x83\x6A\x83\x93\x83\x4F"
                  "\x83\x80\x81\x5B\x83\x72\x81\x5B"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x99\x96\x7B\x81\x40\x91\xA5"
                  "\x94\x56"),
    CREDITS_ENTRY(10, 0x01, "\x89\xB9\x8A\x79"),
    CREDITS_ENTRY(10, 0x00, 0),
    CREDITS_ENTRY(10, 0x00,
                  "\x81\x40\x81\x40\x90\xCE\x88\xE4\x81\x40\x92\xBC\x8E\x71"),
    CREDITS_ENTRY(10, 0x00,
                  "\x81\x40\x81\x40\x93\x63\x95\xD3\x81\x69\x82\xC8\x82\xD7\x82\xE2"
                  "\x82\xF1\x81\x6A\x97\x54\x8E\x8C\x81\x40\x81\x40"),
    CREDITS_ENTRY(10, 0x00,
                  "\x81\x40\x81\x40\x94\xF6\x8D\xE8\x81\x40\x98\x61\x88\xEA\x98\x59"),
    CREDITS_ENTRY(11, 0x01, "\x8C\xF8\x89\xCA\x89\xB9"),
    CREDITS_ENTRY(11, 0x00, 0),
    CREDITS_ENTRY(11, 0x00, "\x81\x40\x81\x40\x82\x60\x82\x71\x82\x78"),
    CREDITS_ENTRY(12, 0x01,
                  "\x83\x54\x83\x45\x83\x93\x83\x68\x83\x76\x83\x8D\x83\x4F\x83\x89"
                  "\x83\x80"),
    CREDITS_ENTRY(12, 0x00, 0),
    CREDITS_ENTRY(12, 0x00,
                  "\x81\x40\x81\x40\x8C\x70\x8A\xE2\x81\x40\x92\xBC\x8F\x5B"),
    CREDITS_ENTRY(13, 0x01,
                  "\x83\x7C\x83\x50\x83\x62\x83\x67\x83\x58\x83\x65\x81\x5B\x83\x56"
                  "\x83\x87\x83\x93\x81\x40\x81\x40"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x02,
                  "\x81\x40\x81\x40\x83\x76\x83\x8D\x83\x4F\x83\x89\x83\x80"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x00,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x82\x66\x82\x74\x82\x6D"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x02,
                  "\x81\x40\x81\x40\x83\x4F\x83\x89\x83\x74\x83\x42\x83\x62\x83\x4E"
                  "\x83\x58"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x00,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x5F\x8E\x52\x81\x40\x89\xC1"
                  "\x93\xDE\x8E\x71"),
    CREDITS_ENTRY(14, 0x01,
                  "\x83\x76\x83\x8D\x83\x5F\x83\x4E\x83\x67\x83\x66\x83\x55\x83\x43"
                  "\x83\x93"),
    CREDITS_ENTRY(14, 0x00, 0),
    CREDITS_ENTRY(14, 0x00,
                  "\x81\x40\x81\x40\x91\x9D\x93\x63\x81\x40\x96\xBE\x97\x59"),
    CREDITS_ENTRY(14, 0x00,
                  "\x81\x40\x81\x40\x97\xE9\x96\xD8\x81\x40\x8C\xAA\x8E\x71"),
    CREDITS_ENTRY(15, 0x01,
                  "\x83\x4A\x81\x5B\x83\x68\x83\x8B\x81\x5B\x83\x8B\x90\xA7\x8D\xEC"
                  "\x8B\xA6\x97\xCD"),
    CREDITS_ENTRY(15, 0x00, 0),
    CREDITS_ENTRY(15, 0x00,
                  "\x81\x40\x81\x40\x8E\x52\x93\x63\x81\x40\x90\x4D\x97\x6D"),
    CREDITS_ENTRY(15, 0x00,
                  "\x81\x40\x81\x40\x90\xE7\x93\x63\x81\x40\x91\xEC\x97\x9D"),
    CREDITS_ENTRY(16, 0x01,
                  "\x83\x82\x83\x93\x83\x58\x83\x5E\x81\x5B\x83\x82\x83\x66\x83\x8B"
                  "\x90\xA7\x8D\xEC\x8B\xA6\x97\xCD"),
    CREDITS_ENTRY(16, 0x00, 0),
    CREDITS_ENTRY(16, 0x02,
                  "\x8A\x94\x8E\xAE\x89\xEF\x8E\xD0\x83\x55\x83\x62\x83\x4E\x83\x58"
                  "\x81\x45\x83\x47\x83\x93\x83\x5E\x81\x5B\x83\x65\x83\x43\x83\x93"
                  "\x83\x81\x83\x93\x83\x67"),
    CREDITS_ENTRY(16, 0x00, 0),
    CREDITS_ENTRY(16, 0x00,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\xD8\x91\xBA\x81\x40\x95\xDB"),
    CREDITS_ENTRY(16, 0x00,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8D\x82\x96\xF6\x81\x40\x89\xC3"
                  "\x8C\xF5"),
    CREDITS_ENTRY(17, 0x01,
                  "\x83\x58\x83\x79\x83\x56\x83\x83\x83\x8B\x83\x54\x83\x93\x83\x4E"
                  "\x83\x58"),
    CREDITS_ENTRY(17, 0x50, 0),
    CREDITS_ENTRY(17, 0x52,
                  "\x81\x40\x81\x40\x8F\x57\x89\x70\x8E\xD0\x81\x40\x8F\x54\x8A\xA7"
                  "\x8F\xAD\x94\x4E\x83\x57\x83\x83\x83\x93\x83\x76\x95\xD2\x8F\x57"
                  "\x95\x94\x81\x40\x81\x40"),
    CREDITS_ENTRY(17, 0x50, 0),
    CREDITS_ENTRY(17, 0x50,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x92\xB9\x93\x88\x81\x40\x98\x61"
                  "\x95\x46"),
    CREDITS_ENTRY(17, 0x50,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8D\x82\x8B\xB4\x81\x40\x8F\x72"
                  "\x8F\xB9"),
    CREDITS_ENTRY(17, 0x50,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x95\x72\x8E\x71\x81\x40\x8B\x67"
                  "\x8B\x76"),
    CREDITS_ENTRY(17, 0x50,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x93\x88\x81\x40\x81\x40\x92\x71"
                  "\x94\x56"),
    CREDITS_ENTRY(17, 0x50,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8D\x82\x8B\xB4\x81\x40\x89\xEB"
                  "\x93\xDE"),
    CREDITS_ENTRY(17, 0x40, 0),
    CREDITS_ENTRY(17, 0x42,
                  "\x81\x40\x81\x40\x8F\x57\x89\x70\x8E\xD0\x81\x40\x82\x75\x83\x57"
                  "\x83\x83\x83\x93\x83\x76\x95\xD2\x8F\x57\x95\x94"),
    CREDITS_ENTRY(17, 0x40, 0),
    CREDITS_ENTRY(17, 0x40,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8B\x67\x91\x71\x81\x40\x89\x70"
                  "\x97\x59"),
    CREDITS_ENTRY(17, 0x40,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\xEC\x91\xBA\x81\x40\x95\x90"
                  "\x8E\x6D"),
    CREDITS_ENTRY(17, 0x40,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x92\xAC\x93\x63\x81\x40\x8F\x40"
                  "\x8E\xA1"),
    CREDITS_ENTRY(17, 0x30, 0),
    CREDITS_ENTRY(17, 0x32,
                  "\x81\x40\x81\x40\x8F\x57\x89\x70\x8E\xD0\x81\x40\x83\x52\x83\x7E"
                  "\x83\x62\x83\x4E\x83\x81\x83\x66\x83\x42\x83\x41\x95\xD2\x8F\x57"
                  "\x95\x94\x81\x40\x81\x40"),
    CREDITS_ENTRY(17, 0x30, 0),
    CREDITS_ENTRY(17, 0x30,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\xB4\x90\x85\x81\x40\x90\xBD"
                  "\x88\xEA"),
    CREDITS_ENTRY(17, 0x20, 0),
    CREDITS_ENTRY(17, 0x22,
                  "\x81\x40\x81\x40\x83\x52\x83\x69\x83\x7E\x8A\x94\x8E\xAE\x89\xEF"
                  "\x8E\xD0"),
    CREDITS_ENTRY(17, 0x20, 0),
    CREDITS_ENTRY(17, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x96\x6B\x8F\xE3\x81\x40\x88\xEA"
                  "\x8E\x4F"),
    CREDITS_ENTRY(17, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8E\x52\x93\x63\x81\x40\x90\xB3"
                  "\x8E\xF7"),
    CREDITS_ENTRY(17, 0x20,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8F\xC0\xE0\x56\x81\x40\x97\xB2"),
    CREDITS_ENTRY(17, 0x10, 0),
    CREDITS_ENTRY(17, 0x12,
                  "\x81\x40\x81\x40\x83\x52\x83\x69\x83\x7E\x8A\x94\x8E\xAE\x89\xEF"
                  "\x8E\xD0"),
    CREDITS_ENTRY(17, 0x10, 0),
    CREDITS_ENTRY(17, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x93\xFA\x8B\x67\x81\x40\x8D\x46"
                  "\x95\x76"),
    CREDITS_ENTRY(17, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x8A\x99\x93\x63\x81\x40\x8D\x5F"
                  "\x8F\xBA"),
    CREDITS_ENTRY(17, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x91\x8A\x8C\xB4\x81\x40\x9B\x92"
                  "\x96\xE7"),
    CREDITS_ENTRY(17, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x90\x85\x93\x87\x81\x40\x93\x8D"
                  "\x8E\x71"),
    CREDITS_ENTRY(17, 0x10,
                  "\x81\x40\x81\x40\x81\x40\x81\x40\x92\x86\x8C\xB4\x81\x40\x88\xE8"
                  "\x93\xDE\x8E\x71"),
    CREDITS_ENTRY(18, 0x01,
                  "\x83\x76\x83\x8D\x83\x66\x83\x85\x81\x5B\x83\x54\x81\x5B"),
    CREDITS_ENTRY(18, 0x00, 0),
    CREDITS_ENTRY(18, 0x00, "\x81\x40\x81\x40\x89\xBA\x91\xBA\x81\x40\x91\x8F"),
    CREDITS_ENTRY(19, 0x01,
                  "\x83\x47\x83\x4F\x83\x5B\x83\x4E\x83\x65\x83\x42\x83\x75\x83\x76"
                  "\x83\x8D\x83\x66\x83\x85\x81\x5B\x83\x54\x81\x5B\x81\x40\x81\x40"),
    CREDITS_ENTRY(19, 0x00, 0),
    CREDITS_ENTRY(19, 0x00,
                  "\x81\x40\x81\x40\x8B\x67\x89\xAA\x81\x40\x8A\xEE\x8D\x73"),
    CREDITS_ENTRY(99, 0x01, "\x90\xA7\x8D\xEC"),
    CREDITS_ENTRY(99, 0x00, 0),
    CREDITS_ENTRY(99, 0x00,
                  "\x81\x40\x81\x40\x82\x6A\x82\x62\x82\x64\x83\x57\x83\x83\x83\x70"
                  "\x83\x93\x81\x40\x82\x64\x82\x60\x82\x72\x82\x73\x81\x40\x81\x40"),
    CREDITS_ENTRY(-1, 0x00, 0),
#else
    CREDITS_ENTRY(0, 0x01, "Original_Character_Design"),
    CREDITS_ENTRY(0, 0x01, "Original_Monster_Design"),
    CREDITS_ENTRY(0, 0x01, "Scenario_Draft"),
    CREDITS_ENTRY(0, 0x00, 0),
    CREDITS_ENTRY(0, 0x00, "__Kazuki_TAKAHASHI"),
    CREDITS_ENTRY(1, 0x01, "Planning_&_Specification_Design"),
    CREDITS_ENTRY(1, 0x01, "General_Director"),
    CREDITS_ENTRY(1, 0x00, 0),
    CREDITS_ENTRY(1, 0x00, "__Morikuni_KUBO"),
    CREDITS_ENTRY(2, 0x01, "Scenario"),
    CREDITS_ENTRY(2, 0x00, 0),
    CREDITS_ENTRY(2, 0x00, "__Masaaki_KARUBE"),
    CREDITS_ENTRY(3, 0x01, "Program_Direction"),
    CREDITS_ENTRY(3, 0x00, 0),
    CREDITS_ENTRY(3, 0x00, "__GUN"),
    CREDITS_ENTRY(4, 0x01, "Thought_Logic_Design"),
    CREDITS_ENTRY(4, 0x00, 0),
    CREDITS_ENTRY(4, 0x00, "__Hidehiro_HIRATA"),
    CREDITS_ENTRY(4, 0x00, "__Morikuni_KUBO"),
    CREDITS_ENTRY(5, 0x01, "3D_&_VFX_Direction"),
    CREDITS_ENTRY(5, 0x00, 0),
    CREDITS_ENTRY(5, 0x00, "__JUN"),
    CREDITS_ENTRY(6, 0x01, "VFX_Team"),
    CREDITS_ENTRY(6, 0x00, 0),
    CREDITS_ENTRY(6, 0x00, "__Makoto_ITAKURA"),
    CREDITS_ENTRY(6, 0x00, "__TRUTH"),
    CREDITS_ENTRY(6, 0x00, "__Takuya_HASHIMOTO"),
    CREDITS_ENTRY(6, 0x00, "__UKIN"),
    CREDITS_ENTRY(7, 0x01, "Art_Direction"),
    CREDITS_ENTRY(7, 0x00, 0),
    CREDITS_ENTRY(7, 0x00, "__Masafumi_KAMIO"),
    CREDITS_ENTRY(8, 0x01, "2D_Graphics"),
    CREDITS_ENTRY(8, 0x40, 0),
    CREDITS_ENTRY(8, 0x42, "__Card_Draft"),
    CREDITS_ENTRY(8, 0x40, 0),
    CREDITS_ENTRY(8, 0x40, "____Koji_NAMIKOSHI(16)"),
    CREDITS_ENTRY(8, 0x40, "____Takeshi_URABE"),
    CREDITS_ENTRY(8, 0x30, 0),
    CREDITS_ENTRY(8, 0x32, "__Card_Painting"),
    CREDITS_ENTRY(8, 0x30, 0),
    CREDITS_ENTRY(8, 0x30, "____Yukiko_HATANAKA"),
    CREDITS_ENTRY(8, 0x30, "____Tetsuya_KADONO"),
    CREDITS_ENTRY(8, 0x30, "____Tomoji_OMOTANI"),
    CREDITS_ENTRY(8, 0x30, "____Masafumi_KAMIO"),
    CREDITS_ENTRY(8, 0x30, "____Koji_NAMIKOSHI(16)"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x22, "__Campaign_Part_Draft"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x20, "____Takeshi_URABE"),
    CREDITS_ENTRY(8, 0x20, "____Kenji_INOUE"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x22, "__Campaign_Part_Painting"),
    CREDITS_ENTRY(8, 0x20, 0),
    CREDITS_ENTRY(8, 0x20, "____Yukiko_HATANAKA"),
    CREDITS_ENTRY(8, 0x10, 0),
    CREDITS_ENTRY(8, 0x12, "__Game_Component_Graphics"),
    CREDITS_ENTRY(8, 0x10, 0),
    CREDITS_ENTRY(8, 0x10, "____Yukiko_HATANAKA"),
    CREDITS_ENTRY(8, 0x10, "____Koji_NAMIKOSHI(16)"),
    CREDITS_ENTRY(8, 0x10, "____Kanako_KOHYAMA"),
    CREDITS_ENTRY(9, 0x01, "3D_Graphics"),
    CREDITS_ENTRY(9, 0x30, 0),
    CREDITS_ENTRY(9, 0x32, "__Campaign_Part_Scenery"),
    CREDITS_ENTRY(9, 0x30, 0),
    CREDITS_ENTRY(9, 0x30, "____Ikuro_ISHIHARA"),
    CREDITS_ENTRY(9, 0x20, 0),
    CREDITS_ENTRY(9, 0x22, "_Monster_Modeling_&_Motion"),
    CREDITS_ENTRY(9, 0x20, 0),
    CREDITS_ENTRY(9, 0x20, "__Naoki_KUMAGAI______Takeshi_URABE"),
    CREDITS_ENTRY(9, 0x20, "__Mitsuko_SAKAMOTO___Kenji_INOUE"),
    CREDITS_ENTRY(9, 0x20, "__Tomoji_OMOTANI_____Masafumi_KAMIO"),
    CREDITS_ENTRY(9, 0x20, "__Tetsuya_KADONO_____Noriyuki_SUGIMOTO"),
    CREDITS_ENTRY(9, 0x20, "__Koji_NAMIKOSHI(16)"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x12, "__VFX_Materials_Design"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x10, "____Tetsuya_KADONO"),
    CREDITS_ENTRY(9, 0x10, "____Masafumi_KAMIO"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x12, "__Opening_Movie_Design"),
    CREDITS_ENTRY(9, 0x10, 0),
    CREDITS_ENTRY(9, 0x10, "____Yasuhiko_TANAKA"),
    CREDITS_ENTRY(10, 0x01, "Music"),
    CREDITS_ENTRY(10, 0x00, 0),
    CREDITS_ENTRY(10, 0x00, "__Naoko_ISHII"),
    CREDITS_ENTRY(10, 0x00, "__Hiroshi(nabeyan)TANABE"),
    CREDITS_ENTRY(10, 0x00, "__Waichiro_OZAKI"),
    CREDITS_ENTRY(11, 0x01, "Sound_Effects"),
    CREDITS_ENTRY(11, 0x00, 0),
    CREDITS_ENTRY(11, 0x00, "__ARY"),
    CREDITS_ENTRY(12, 0x01, "Sound_Program"),
    CREDITS_ENTRY(12, 0x00, 0),
    CREDITS_ENTRY(12, 0x00, "__Naomitsu_TSUGIIWA"),
    CREDITS_ENTRY(13, 0x01, "Localization_Team"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x02, "__Program"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x00, "____Takuya_HASHIMOTO"),
    CREDITS_ENTRY(13, 0x00, "____Shin'ichi_TASAKI"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x02, "__Graphics"),
    CREDITS_ENTRY(13, 0x00, 0),
    CREDITS_ENTRY(13, 0x00, "____Yasuhiko_TANAKA"),
    CREDITS_ENTRY(14, 0x01, "Product_Design"),
    CREDITS_ENTRY(14, 0x00, 0),
    CREDITS_ENTRY(14, 0x00, "__Akio_MASUDA"),
    CREDITS_ENTRY(14, 0x00, "__Shizuko_SUZUKI"),
    CREDITS_ENTRY(15, 0x01, "Card_Rule_Co-Design"),
    CREDITS_ENTRY(15, 0x00, 0),
    CREDITS_ENTRY(15, 0x00, "__Nobuhiro_YAMADA"),
    CREDITS_ENTRY(15, 0x00, "__Takuri_CHIDA"),
    CREDITS_ENTRY(16, 0x01, "Special_Thanks"),
    CREDITS_ENTRY(16, 0x50, 0),
    CREDITS_ENTRY(16, 0x50, "__Kazuhiko_TORISHIMA"),
    CREDITS_ENTRY(16, 0x50, "__Toshimasa_TAKAHASHI"),
    CREDITS_ENTRY(16, 0x50, "__Yoshihisa_HEISHI"),
    CREDITS_ENTRY(16, 0x50, "__Tomoyuki_SHIMA"),
    CREDITS_ENTRY(16, 0x50, "__Masana_TAKAHASHI"),
    CREDITS_ENTRY(16, 0x40, 0),
    CREDITS_ENTRY(16, 0x40, "__Hideo_YOSHIKURA"),
    CREDITS_ENTRY(16, 0x40, "__Takeshi_NOMURA"),
    CREDITS_ENTRY(16, 0x40, "__Muneharu_MACHIDA"),
    CREDITS_ENTRY(16, 0x30, 0),
    CREDITS_ENTRY(16, 0x30, "__Seiichi_SHIMIZU"),
    CREDITS_ENTRY(16, 0x20, 0),
    CREDITS_ENTRY(16, 0x20, "__Kazumi_KITAUE"),
    CREDITS_ENTRY(16, 0x20, "__Masaki_YAMADA"),
    CREDITS_ENTRY(16, 0x20, "__Takashi_NUMAZAWA"),
    CREDITS_ENTRY(16, 0x20, "__Tetsuya_HIYOSHI"),
    CREDITS_ENTRY(16, 0x10, 0),
    CREDITS_ENTRY(16, 0x10, "__Scott_DOLPH"),
    CREDITS_ENTRY(16, 0x10, 0),
    CREDITS_ENTRY(16, 0x10, "__Takao_HIYOSHI"),
    CREDITS_ENTRY(16, 0x10, "__Hiroaki_KAMATA"),
    CREDITS_ENTRY(16, 0x10, "__Masaya_AIHARA"),
    CREDITS_ENTRY(16, 0x10, "__Momoko_MIZUSHIMA"),
    CREDITS_ENTRY(16, 0x10, "__Kanako_NAKAHARA"),
    CREDITS_ENTRY(17, 0x01, "Special_Thanks"),
    CREDITS_ENTRY(17, 0x00, 0),
    CREDITS_ENTRY(17, 0x00, "__4KIDS_Entertainment"),
    CREDITS_ENTRY(17, 0x00, "__ASATSU-DK_Inc."),
    CREDITS_ENTRY(17, 0x00, "__XAX_Entertainment"),
    CREDITS_ENTRY(17, 0x00, "__INTAC"),
    CREDITS_ENTRY(18, 0x01, "Producer"),
    CREDITS_ENTRY(18, 0x00, 0),
    CREDITS_ENTRY(18, 0x00, "__Satoshi_SHIMOMURA"),
    CREDITS_ENTRY(19, 0x01, "Executive_Producer"),
    CREDITS_ENTRY(19, 0x00, 0),
    CREDITS_ENTRY(19, 0x00, "__Motoyuki_YOSHIOKA"),
    CREDITS_ENTRY(99, 0x01, "Created_by"),
    CREDITS_ENTRY(99, 0x00, 0),
    CREDITS_ENTRY(99, 0x00, "Konami_Computer_Entertainment_Japan"),
    CREDITS_ENTRY(99, 0x00, "________"),
    CREDITS_ENTRY(-1, 0x00, 0),
#endif
};

/* Palette rows for the three colour ramps; Japan uploads only the first. */
const RECT gCredits_ClutRect0 = { 0x200, 0xF6, 0x10, 1 };
#ifndef VERSION_JAPAN
const RECT gCredits_ClutRect1 = { 0x200, 0xF7, 0x10, 1 };
const RECT gCredits_ClutRect2 = { 0x200, 0xF8, 0x10, 1 };
#endif

/* The presentation state, zero-initialized and common (this unit builds with
 * gcc_2_8_1_g0_split_comm); each layout's credits_commons subsegment places it
 * after the entry table. */
u8 gCredits_bFinished;
CreditsLine gCredits_aLines[2];

#ifdef VERSION_JAPAN
/* The Japanese package draws its rows from glyph bitmaps in VRAM, so it
 * uploads one palette instead of the three colour ramps. */
void Credits_InitPresentation(void)
{
    RECT rect;
    u16 clut[16];
    s32 i;

    rect = gCredits_ClutRect0;
    clut[0] = 0;
    clut[1] = 0xFFFF;
    clut[2] = 0xC21F;
    clut[3] = 0xC3F0;
    clut[4] = 0xA108;
    clut[5] = 0x9088;
    clut[6] = 0x9104;
    for (i = 7; i < 16; i++) {
        clut[i] = 0;
    }
    while (IsIdleGPU(3) != 0) {
    }
    while (LoadImage2(&rect, (u32 *)clut) != 0) {
    }
    while (IsIdleGPU(3) != 0) {
    }
    for (i = 1; i >= 0; i--) {
        gCredits_aLines[i].entries = 0;
    }
    gCredits_bFinished = 0;
}
#else
void Credits_InitPresentation(void)
{
    RECT rect0;
    RECT rect1;
    RECT rect2;
    u16 clut0[16];
    u16 clut1[16];
    u16 clut2[16];
    s32 i;

    rect0 = gCredits_ClutRect0;
    rect1 = gCredits_ClutRect1;
    rect2 = gCredits_ClutRect2;
    clut0[0] = 0;
    clut0[1] = 0x8421;
    clut0[2] = 0x9084;
    clut0[3] = 0x9CE7;
    clut0[4] = 0xA529;
    clut0[5] = 0xAD6B;
    clut0[6] = 0xB5AD;
    clut0[7] = 0xBDEF;
    clut0[8] = 0xC631;
    clut0[9] = 0xCE73;
    clut0[10] = 0xD6B5;
    clut0[11] = 0xDEF7;
    clut0[12] = 0xE739;
    clut0[13] = 0xEF7B;
    clut0[14] = 0xF7BD;
    clut0[15] = 0xFFFF;
    clut1[0] = 0;
    clut1[1] = 0x8421;
    clut1[2] = 0x8844;
    clut1[3] = 0x8C67;
    clut1[4] = 0x9089;
    clut1[5] = 0x94AB;
    clut1[6] = 0x98CD;
    clut1[7] = 0x9CEF;
    clut1[8] = 0xA111;
    clut1[9] = 0xA533;
    clut1[10] = 0xA955;
    clut1[11] = 0xAD77;
    clut1[12] = 0xB199;
    clut1[13] = 0xB5BB;
    clut1[14] = 0xB9DD;
    clut1[15] = 0xBDFF;
    clut2[0] = 0;
    clut2[1] = 0x8421;
    clut2[2] = 0x8882;
    clut2[3] = 0x8CE3;
    clut2[4] = 0x9124;
    clut2[5] = 0x9565;
    clut2[6] = 0x99A6;
    clut2[7] = 0x9DE7;
    clut2[8] = 0xA228;
    clut2[9] = 0xA669;
    clut2[10] = 0xAAAA;
    clut2[11] = 0xAEEB;
    clut2[12] = 0xB32C;
    clut2[13] = 0xB76D;
    clut2[14] = 0xBBAE;
    clut2[15] = 0xBFEF;
    while (IsIdleGPU(3) != 0) {
    }
    while (LoadImage2(&rect0, (u32 *)clut0) != 0) {
    }
    while (LoadImage2(&rect1, (u32 *)clut1) != 0) {
    }
    while (LoadImage2(&rect2, (u32 *)clut2) != 0) {
    }
    while (IsIdleGPU(3) != 0) {
    }
    for (i = 1; i >= 0; i--) {
        gCredits_aLines[i].entries = 0;
    }
    gCredits_bFinished = 0;
}
#endif

/* The brightness ramp of a fading line: 0 below 0, 0x80 above 0x80. */
#define CLAMP(x) ((x) < 0 ? 0 : (x) > 0x80 ? 0x80 : (x))

/* Advances both credits lines by one frame. State 0 fades a line in, and
 * state 1 holds it; a line whose first row is group 0x63 sets gCredits_bFinished
 * instead. State 2 fades the line out. State 3 keeps it blank, then lays out
 * its next page, or moves to state 4 when no page is left. State 4 starts the
 * following group in the first free line and releases this one. Returns
 * gCredits_bFinished. */
s32 Credits_UpdateLines(void)
{
    CreditsLine *other;
    s32 i;
    s32 j;
    s32 fade_in;
    s32 level0;
    s32 level1;
    s32 level2;
    s32 level3;
    s32 n;
    s32 full_level;
    s32 fade_out;
    s32 dim3;
    s32 dim2;
    s32 dim1;
    s32 dim0;

    for (i = 0; i < 2; i++) {
        if (gCredits_aLines[i].entries == 0) {
            continue;
        }
        switch (gCredits_aLines[i].state) {
        case 0:
            fade_in = (0x80 - gCredits_aLines[i].timer) << 7;
            level0 = fade_in / 32;
            level1 = fade_in / 64;
            level2 = fade_in / 96;
            level3 = fade_in / 128;
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state++;
            }
            level0 = CLAMP(level0);
            level1 = CLAMP(level1);
            level2 = CLAMP(level2);
            level3 = CLAMP(level3);
            Credits_DrawLine(i, level0, level1, level2, level3);
            break;
        case 1:
            if (gCredits_aLines[i].entries->group == 0x63) {
                gCredits_bFinished = 1;
            } else {
                gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep;
                if (gCredits_aLines[i].timer <= 0) {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state++;
                    if (gCredits_aLines[i].title_held != 0) {
                        if (gCredits_aLines[i].page == 1) {
                            gCredits_aLines[i].title_held = 0;
                        }
                    } else {
                        gCredits_aLines[i].title_held++;
                    }
                }
            }
            Credits_DrawLine(i, 0x80, 0x80, 0x80, 0x80);
            break;
        case 2:
            full_level = 0x80;
            fade_out = (full_level - gCredits_aLines[i].timer) << 7;
            dim3 = fade_out / 32;
            dim2 = fade_out / 64;
            dim1 = fade_out / 96;
            dim0 = fade_out / 128;
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                if (gCredits_aLines[i].page >= 2) {
                    gCredits_aLines[i].timer = 0x3C;
                    gCredits_aLines[i].state++;
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            dim0 = CLAMP(dim0);
            dim1 = CLAMP(dim1);
            dim2 = CLAMP(dim2);
            dim3 = CLAMP(dim3);
            Credits_DrawLine(i, full_level - dim0, full_level - dim1, full_level - dim2, full_level - dim3);
            break;
        case 3:
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                gCredits_aLines[i].timer = 0x98;
                gCredits_aLines[i].state = 0;
                gCredits_aLines[i].page--;
                if (gCredits_aLines[i].page != 0) {
                    Credits_BuildLineGlyphs(i, gCredits_aLines[i].entries->group);
                } else {
                    gCredits_aLines[i].timer = 0x98;
                    gCredits_aLines[i].state = 4;
                }
            }
            Credits_DrawLine(i, 0, 0, 0, 0);
            break;
        case 4:
            gCredits_aLines[i].timer -= gGraphics_nBaseFrameStep * 2;
            if (gCredits_aLines[i].timer <= 0) {
                n = gCredits_aLines[i].end->group;
                /* A negative group enters the slot search at its exit; the
                   search hands the group to the first free line. */
                switch (n < 0) {
                case 0:
                    j = 0;
                    do {
                        other = gCredits_aLines;
                    } while (0);
                    for (;;) {
                        if (other->entries == 0) {
                            other->height = 0;
                            other->width = 0;
                            other->page = 0;
                            if (Credits_BuildLineGlyphs(j, n) != 0) {
                                other->x = 40;
                                other->y = 40;
                                if (other->width > 160 || other->height > 120) {
                                    other->x = (320 - other->width) / 2;
                                    other->y = (240 - other->height) / 2 - 10;
                                }
                                other->state = 0;
                                other->timer = 152;
                                other->title_held = 0;
                            }
                        } else {
                            j++;
                            other++;
                            if (j < 2) {
                                continue;
                            }
                        }
                default:
                        gCredits_aLines[i].entries = 0;
                        break;
                    }
                }
            }
            break;
        }
    }
    return gCredits_bFinished;
}

/* Collects the credits rows of `group` for line `index`: builds each row's
 * glyph bitmaps from the ROM font into a local buffer and records the line's
 * width, height and page. */
s32 Credits_BuildLineGlyphs(s32 index, s32 group)
{
    RECT rect;
    u16 buf[64];
    CreditsLine *line;
    CreditsEntry *entry;
    s32 count;
    u32 cur;
    s32 page_height;
    s32 keep_mask;
    u32 flags;
    u8 *text;
    s32 fg;
    s32 sh;
    u32 page;
    s32 width;
    u16 code;
    u8 *font;
    u16 *pixel_row;
    s32 k;
    s32 nibbles;
    u8 bits;
    s32 word; /* one name for every pixel_row[k] read-modify-write: its
               * single pseudo outranks `bits` in global-alloc, so the loads
               * take $a0 and `bits` $a1 */

    /* Allocator steering, not behaviour. The constant switch jumps over the
     * unreachable empty loop to `case 0`, and that jump survives until flow
     * deletes the loop, so the case label still splits the entry block when
     * reload runs and jump2 only removes the jump afterwards. With the label
     * there, reload cannot reuse $a0 for `index` and reloads it from its
     * stack slot ($s5), which is what retail does. */
    switch (0) {
        for (;;) {
        }
    case 0:
        entry = gCredits_aEntries;
        count = 0;
        cur = 0;
        page_height = 0;
        rect.x = index * 192 + 512;
        rect.y = 256;
        rect.w = 4;
        rect.h = 16;
        line = &gCredits_aLines[index];
        while (entry->group >= 0) {
            if (group < entry->group) {
                break;
            }
            if (entry->group == group) {
                flags = entry->flags;
                text = entry->text;
                fg = (flags & 0xF) + 1;
                sh = (flags & 0xF) + 4;
                keep_mask = 15;
                page = flags >> 4;
                if (line->entries == 0) {
                    line->entries = entry;
                    cur = page;
                }
                if (cur == page) {
                    page_height += 16;
                } else {
                    if (line->height < page_height) {
                        line->height = page_height;
                    }
                    cur = page;
                    page_height = 0;
                }
                if (text != 0) {
                    width = strlen(text) / 2 * 16;
                    if (line->width < width) {
                        line->width = width;
                    }
                    if (line->page == 0 || line->page == page) {
                        rect.x = index * 192 + 512 + count / 15 * 96;
                        rect.y = count % 15 * 16 + 256;
                        while (*text != 0) {
                            code = text[1] | (text[0] << 8);
                            font = (u8 *)Krom2RawAdd2(code);
                            pixel_row = buf;
                            k = 63;
                            do {
                                *pixel_row++ = 0;
                            } while (--k >= 0);
                            pixel_row = buf;
                            /* The wrappers weight the two glyph overrides so `font`
                               keeps its register ahead of the cleared buffer's
                               cursor. */
                            if (code == 0xE056) {
                                do {
                                    font = (u8 *)gCredits_abReplacementGlyphs[0];
                                } while (0);
                            }
                            if (code == 0x9B92) {
                                do {
                                    font = (u8 *)gCredits_abReplacementGlyphs[1];
                                } while (0);
                            }
                            for (k = 14; k != -1; k--) {
                                bits = font[0];
                                nibbles = 0;
                                if (bits & 0x80) nibbles = sh << 4;
                                if (bits & 0x40) nibbles |= sh << 8;
                                word = pixel_row[4];
                                pixel_row[4] = word | ((bits & 0x20) ? nibbles | sh << 12 : nibbles);
                                bits = font[0];
                                nibbles = 0;
                                if (bits & 0x10) nibbles = sh;
                                if (bits & 0x08) nibbles |= sh << 4;
                                if (bits & 0x04) nibbles |= sh << 8;
                                word = pixel_row[5];
                                pixel_row[5] = word | ((bits & 0x02) ? nibbles | sh << 12 : nibbles);
                                nibbles = 0;
                                if (font[0] & 0x01) nibbles = sh;
                                bits = font[1];
                                if (bits & 0x80) nibbles |= sh << 4;
                                if (bits & 0x40) nibbles |= sh << 8;
                                word = pixel_row[6];
                                pixel_row[6] = word | ((bits & 0x20) ? nibbles | sh << 12 : nibbles);
                                bits = font[1];
                                nibbles = 0;
                                if (bits & 0x10) nibbles = sh;
                                if (bits & 0x08) nibbles |= sh << 4;
                                if (bits & 0x04) nibbles |= sh << 8;
                                word = pixel_row[7];
                                pixel_row[7] = word | ((bits & 0x02) ? nibbles | sh << 12 : nibbles);

                                bits = font[0];
                                nibbles = 0;
                                if (!(bits & 0x80)) nibbles = keep_mask;
                                if (!(bits & 0x40)) nibbles |= keep_mask << 4;
                                if (!(bits & 0x20)) nibbles |= keep_mask << 8;
                                word = pixel_row[0];
                                pixel_row[0] = word & (!(bits & 0x10) ? nibbles | keep_mask << 12 : nibbles);
                                bits = font[0];
                                nibbles = 0;
                                if (!(bits & 0x08)) nibbles = keep_mask;
                                if (!(bits & 0x04)) nibbles |= keep_mask << 4;
                                if (!(bits & 0x02)) nibbles |= keep_mask << 8;
                                word = pixel_row[1];
                                pixel_row[1] = word & (!(bits & 0x01) ? nibbles | keep_mask << 12 : nibbles);
                                bits = font[1];
                                nibbles = 0;
                                if (!(bits & 0x80)) nibbles = keep_mask;
                                if (!(bits & 0x40)) nibbles |= keep_mask << 4;
                                if (!(bits & 0x20)) nibbles |= keep_mask << 8;
                                word = pixel_row[2];
                                pixel_row[2] = word & (!(bits & 0x10) ? nibbles | keep_mask << 12 : nibbles);
                                bits = font[1];
                                nibbles = 0;
                                if (!(bits & 0x08)) nibbles = keep_mask;
                                if (!(bits & 0x04)) nibbles |= keep_mask << 4;
                                if (!(bits & 0x02)) nibbles |= keep_mask << 8;
                                word = pixel_row[3];
                                pixel_row[3] = word & (!(bits & 0x01) ? nibbles | keep_mask << 12 : nibbles);

                                bits = font[0];
                                nibbles = 0;
                                if (bits & 0x80) nibbles = fg;
                                if (bits & 0x40) nibbles |= fg << 4;
                                if (bits & 0x20) nibbles |= fg << 8;
                                word = pixel_row[0];
                                pixel_row[0] = word | ((bits & 0x10) ? nibbles | fg << 12 : nibbles);
                                bits = font[0];
                                nibbles = 0;
                                if (bits & 0x08) nibbles = fg;
                                if (bits & 0x04) nibbles |= fg << 4;
                                if (bits & 0x02) nibbles |= fg << 8;
                                word = pixel_row[1];
                                pixel_row[1] = word | ((bits & 0x01) ? nibbles | fg << 12 : nibbles);
                                bits = font[1];
                                nibbles = 0;
                                if (bits & 0x80) nibbles = fg;
                                if (bits & 0x40) nibbles |= fg << 4;
                                if (bits & 0x20) nibbles |= fg << 8;
                                word = pixel_row[2];
                                pixel_row[2] = word | ((bits & 0x10) ? nibbles | fg << 12 : nibbles);
                                bits = font[1];
                                nibbles = 0;
                                if (bits & 0x08) nibbles = fg;
                                if (bits & 0x04) nibbles |= fg << 4;
                                if (bits & 0x02) nibbles |= fg << 8;
                                word = pixel_row[3];
                                pixel_row[3] = word | ((bits & 0x01) ? nibbles | fg << 12 : nibbles);
                                font += 2;
                                pixel_row += 4;
                            }
                            while (IsIdleGPU(3) != 0) {
                            }
    #ifdef VERSION_JAPAN
                            /* Japan uploads each cell for Credits_DrawLine. */
                            while (LoadImage2(&rect, (u32 *)buf) != 0) {
                            }
    #endif
                            while (IsIdleGPU(3) != 0) {
                            }
                            rect.x += 4;
                            text += 2;
                        }
                    }
                    if (line->page == 0) {
                        line->page = entry[1].flags >> 4;
                    }
                    count++;
                    if (count >= 30) {
                        break;
                    }
                }
            }
            entry++;
        }
        line->end = entry;
        if (line->height < page_height) {
            line->height = page_height;
        }
        if (line->page != 0) {
            line->height += 16;
        }
        return count;
    }
}

#ifdef VERSION_JAPAN
/* Draws one credits line from the glyph bitmaps Credits_BuildLineGlyphs
 * uploaded to VRAM: row `drawn` of the line sits in column drawn / 15, slot
 * drawn % 15 of the line's area, and each two-byte character is one 16x16
 * POLY_GT4 cell. Unlike the US package, both packets are submitted: `glyph`
 * with blend mode 1 and `shade` with blend mode 2. */
void Credits_DrawLine(s32 index, s32 c0, s32 c1, s32 c2, s32 c3)
{
    POLY_GT4 glyph;
    POLY_GT4 shade;
    CreditsLine *line;
    CreditsEntry *entry;
    s32 drawn;
    s32 full_level;
    s32 x;
    s32 y;
    s32 i;
    s32 r0;
    s32 r1;
    s32 r2;
    s32 r3;
    u32 group;
    s32 u;
    s32 v;
    s32 first;
    s32 ox;

    drawn = 0;
    line = &gCredits_aLines[index];
    entry = line->entries;
    x = line->x;
    y = line->y;
    full_level = 0x80;
    if (entry == 0) {
        return;
    }
    setPolyGT4(&glyph);
    setPolyGT4(&shade);
    setSemiTrans(&glyph, 1);
    setSemiTrans(&shade, 1);
    glyph.clut = 0x3DA0;
    shade.clut = 0x3C20;
    first = entry->group;
    do {
        group = entry->flags >> 4;
        if (drawn == 0 || line->page == group) {
            if (entry->text != 0) {
                r0 = c0;
                r1 = c1;
                r2 = c2;
                r3 = c3;
                if (line->title_held != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = full_level;
                }
                setRGB0(&glyph, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&glyph, r1 * 64 / full_level, r1 * 64 / full_level, r1 * 64 / full_level);
                setRGB2(&glyph, r2 * 64 / full_level, r2 * 64 / full_level, r2 * 64 / full_level);
                setRGB3(&glyph, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
                setRGB0(&shade, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&shade, r1 * 192 / full_level, r1 * 192 / full_level, r1 * 192 / full_level);
                setRGB2(&shade, r2 * 192 / full_level, r2 * 192 / full_level, r2 * 192 / full_level);
                setRGB3(&shade, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
                for (i = 0; i < strlen(entry->text) / 2; i++) {
                    ox = i * 4;
                    u = (drawn / 15 * 96 + ox) % 64 * 4;
                    v = drawn % 15 * 16 % 256;
                    setXY4(&glyph, x, y, x + 15, y, x, y + 15, x + 15, y + 15);
                    setUV4(&glyph, u, v, u + 15, v, u, v + 15, u + 15, v + 15);
                    setXY4(&shade, x, y, x + 15, y, x, y + 15, x + 15, y + 15);
                    setUV4(&shade, u, v, u + 15, v, u, v + 15, u + 15, v + 15);
                    glyph.tpage = getTPage(0, 1,
                                           index * 192 + 512 + drawn / 15 * 96 + ox,
                                           drawn % 15 * 16 + 256);
                    GsSortPoly(&glyph, gGraphics_apOrderingTables[2], 0);
                    shade.tpage = getTPage(0, 2,
                                           index * 192 + 512 + drawn / 15 * 96 + ox,
                                           drawn % 15 * 16 + 256);
                    GsSortPoly(&shade, gGraphics_apOrderingTables[2], 0);
                    x += 16;
                }
                x = line->x;
                drawn++;
            }
            y += 16;
        } else if (entry->text != 0) {
            drawn++;
        }
        entry++;
    } while (entry->group == first);
}
#else
/* Draws one credits line: walks its group and draws the first text-bearing
 * row plus any row whose paragraph matches the line's page. Each byte of the
 * row text is mapped into the 8x12 credits glyph atlas and submitted as one
 * semi-transparent POLY_GT4 with corner brightnesses c0..c3 scaled against
 * 0x80. A second `shade` packet is configured alongside but never submitted. */
void Credits_DrawLine(s32 index, s32 c0, s32 c1, s32 c2, s32 c3)
{
    POLY_GT4 glyph;
    POLY_GT4 shade;
    CreditsLine *line;
    CreditsEntry *entry;
    s32 drawn;
    s32 full_level;
    s32 x;
    s32 nx;
    s32 y;
    s32 i;
    u32 code;
    s32 u;
    s32 r0;
    s32 r1;
    s32 r2;
    s32 r3;
    u32 flags;
    u32 group;
    u32 kind;
    s32 clut;
    s32 v;
    s32 first;

    drawn = 0;
    line = &gCredits_aLines[index];
    entry = line->entries;
    x = line->x;
    y = line->y;
    full_level = 0x80;
    if (entry == 0) {
        return;
    }
    setPolyGT4(&glyph);
    setPolyGT4(&shade);
    setSemiTrans(&glyph, 1);
    setSemiTrans(&shade, 1);
    shade.clut = 0x3C20;
    first = entry->group;
    do {
        flags = entry->flags;
        group = flags >> 4;
        kind = flags & 0xF;
        if (kind == 2) {
            glyph.clut = 0x3E20;
        } else if (kind == 1) {
            glyph.clut = 0x3DE0;
        } else {
            glyph.clut = 0x3DA0;
        }
        if (drawn == 0 || line->page == group) {
            if (entry->text != 0) {
                r0 = c0;
                r1 = c1;
                r2 = c2;
                r3 = c3;
                if (line->title_held != 0 && line->page != 0 && drawn == 0) {
                    r0 = r1 = r2 = r3 = full_level;
                }
                setRGB0(&glyph, r0 * 128 / full_level, r0 * 128 / full_level, r0 * 128 / full_level);
                setRGB1(&glyph, r1 * 128 / full_level, r1 * 128 / full_level, r1 * 128 / full_level);
                setRGB2(&glyph, r2 * 128 / full_level, r2 * 128 / full_level, r2 * 128 / full_level);
                setRGB3(&glyph, r3 * 128 / full_level, r3 * 128 / full_level, r3 * 128 / full_level);
                setRGB0(&shade, r0 * 192 / full_level, r0 * 192 / full_level, r0 * 192 / full_level);
                setRGB1(&shade, r1 * 192 / full_level, r1 * 192 / full_level, r1 * 192 / full_level);
                setRGB2(&shade, r2 * 192 / full_level, r2 * 192 / full_level, r2 * 192 / full_level);
                setRGB3(&shade, r3 * 192 / full_level, r3 * 192 / full_level, r3 * 192 / full_level);
                for (i = 0; i < strlen(entry->text); i++) {
                    code = entry->text[i];
                    if (code >= 0x61) {
                        code += 0x20;
                    } else {
                        code += 0x1F;
                    }
                    /* One word load tests group and the flags' low nibble together. */
                    if ((*(u32 *)entry & 0xFFF) == 0x63) {
                        glyph.clut = (code & 0xFF) == 0x69 ? 0x3DE0 : 0x3DA0;
                    }
                    u = (code & 0xF) * 8;
                    v = (((code & 0xF0) >> 4) - 4) * 12;
                    nx = x + 8;
                    setXY4(&glyph, x, y, nx, y, x, y + 12, nx, y + 12);
                    setUV4(&glyph, u, v, u + 8, v, u, v + 12, u + 8, v + 12);
                    setXY4(&shade, x, y, nx, y, x, y + 12, nx, y + 12);
                    glyph.tpage = 10;
                    setUV4(&shade, u, v, u + 8, v, u, v + 12, u + 8, v + 12);
                    GsSortPoly(&glyph, gGraphics_apOrderingTables[2], 0);
                    x = nx;
                    shade.tpage = 10;
                }
                x = line->x;
                drawn++;
            }
            y += 16;
        } else if (entry->text != 0) {
            drawn++;
        }
        entry++;
    } while (entry->group == first);
}
#endif

void Credits_StartLineGroup(s32 group)
{
    CreditsLine *line;
    s32 i;

    i = 0;
    line = gCredits_aLines;
    /* The constant switch enters the search loop at its case label. Until
       flow deletes the dead empty loop, that jump comes from outside the
       loop, so loop.c leaves it alone (no hoisted 40), as with retail's goto
       loop; jump2 then deletes the jump. */
    switch (0) {
        for (;;) {
        }
        do {
    case 0:
            if (line->entries == 0) {
                line->height = 0;
                line->width = 0;
                do {
                    line->page = 0;
                    if (Credits_BuildLineGlyphs(i, group) != 0) {
                        line->x = 40;
                        line->y = 40;
                        if (line->width > 160 || line->height > 120) {
                            line->x = (320 - line->width) / 2;
                            line->y = (240 - line->height) / 2 - 10;
                        }
                        line->state = 0;
                        line->timer = 152;
                        line->title_held = 0;
                    }
                    return;
                } while (0);
            }
            i++;
            line++;
        } while (i < 2);
    }
}

u8 Credits_IsFinished(void)
{
    return gCredits_bFinished;
}
