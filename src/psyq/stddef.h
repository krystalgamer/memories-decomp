/*
 * File:stddef.h
 */
/*
 * $PSLibId: Run-time Library Release 4.6$
 */

#ifndef _STDDEF_H
#define _STDDEF_H

#include "../types.h"

#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned int size_t;  /* result type of the sizeof operator (ANSI) */
#endif

#ifndef _WCHAR_T
#define _WCHAR_T
typedef unsigned long wchar_t;		 /* type of a wide character */
#endif

#ifndef WEOF
#define WEOF    0xffffffff
#endif

#ifndef NULL
#define NULL 0		       /* null pointer constant */
#endif

#endif /* _STDDEF_H */
