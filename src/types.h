#ifndef YUGIOH_TYPES_H
#define YUGIOH_TYPES_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;

/* Storage the retail image reserves but nothing in it names or references.
 * PADDING_VARIABLE(n) declares n bytes as u8 D_PADDING_<line>[n], so fillers
 * read as padding rather than as unidentified variables. Write it as
 * `static PADDING_VARIABLE(n);`: the name repeats across units. Zero-filled
 * storage is placed by byte count, so it lands where the scalar or array it
 * replaces did; initialized arrays are word-aligned, so initialized padding
 * at an unaligned offset keeps a scalar spelling. */
#define PADDING_CONCAT_(a, b) a##b
#define PADDING_CONCAT(a, b) PADDING_CONCAT_(a, b)
#define PADDING_VARIABLE(x) u8 PADDING_CONCAT(D_PADDING_, __LINE__)[x]

#endif
