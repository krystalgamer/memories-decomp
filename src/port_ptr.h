#ifndef YUGIOH_PORT_PTR_H
#define YUGIOH_PORT_PTR_H

/* Guest-width pointer annotations for native 64-bit builds of this source.
 *
 * Nothing in this repository defines MEMORIES_PC, so for the console and
 * for every build and check here the macros expand to what they replace:
 * G32 to nothing, CALL32(type, f) to f and PSXLONG to long. The
 * preprocessed tokens, and therefore the objects, are unchanged.
 *
 * A native 64-bit port (MEMORIES_PC, clang for x86-64 or AArch64, built
 * with -fms-extensions) keeps the game's data at its retail addresses, so
 * structures and pinned globals must keep their 32-bit layout:
 *
 *   G32     follows the '*' of a pointer the game stores in memory: a
 *           structure or union member, or a global pinned to a retail
 *           address (`u8 *G32 data;`, `void (*G32 callback)(void);`).
 *           With a pointer typedef it follows the typedef name
 *           (`Callback G32 update;`). A local that walks such storage is
 *           `T *G32 *p`. It becomes clang's `__ptr32 __uptr`, a 4-byte
 *           pointer that is zero-extended on load. Pointers the game only
 *           holds in registers, locals and parameters stay plain.
 *   CALL32  wraps the callee of a call through a G32 function pointer:
 *           `CALL32(type, pointer)(args)`, where type is the plain function
 *           pointer type. LLVM cannot lower a call through a 32-bit
 *           pointer, so the port casts it to a native one first.
 *   PSXLONG spells the Psy-Q `long`, which is 32 bits on the console but
 *           64 bits on LP64 hosts (Linux, Android). It is `long` here and
 *           `int` in a native LP64 build.
 */

#if defined(MEMORIES_PC) && defined(__clang__) && \
    (defined(__x86_64__) || defined(__aarch64__))
#define G32 __ptr32 __uptr
#define CALL32(type, pointer) ((type)(pointer))
#else
#define G32
#define CALL32(type, pointer) pointer
#endif

#if defined(MEMORIES_PC) && defined(__LP64__)
#define PSXLONG int
#else
#define PSXLONG long
#endif

#endif
