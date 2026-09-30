/*
 * File:convert.h
 */
/*
 * $PSLibId: Run-time Library Release 4.6$
 */
#ifndef _CONVERT_H
#define _CONVERT_H

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
extern int atoi(const char *);
extern PSXLONG atol(const char *);
extern PSXLONG strtol(const char *,char**, int);
extern unsigned PSXLONG strtoul(const char *, char **, int);
extern PSXLONG labs(PSXLONG);

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif
