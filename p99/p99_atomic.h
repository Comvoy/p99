/* This may look like nonsense, but it really is -*- mode: C -*-             */
/*                                                                           */
/* Except of parts copied from previous work and as explicitly stated below, */
/* the author and copyright holder for this work is                          */
/* (C) copyright  2011-2012 Jens Gustedt, INRIA, France                      */
/*                                                                           */
/* This file is free software; it is part of the P99 project.                */
/* You can redistribute it and/or modify it under the terms of the QPL as    */
/* given in the file LICENSE. It is distributed without any warranty;        */
/* without even the implied warranty of merchantability or fitness for a     */
/* particular purpose.                                                       */
/*                                                                           */
#ifndef P99_ATOMIC_H
#define P99_ATOMIC_H 1

#include "p99_enum.h"
#include "p99_generic.h"
#include "p99_block.h"

/**
 ** @addtogroup atomic C11 atomic operations
 **
 ** This is a stub implementation of C11 atomic types and operations.
 **
 ** This uses gcc extensions
 ** - all those that are needed for ::P99_GENERIC
 ** - @c __typeof__ to declare @c typedef or local variables of a specific type
 ** - block expressions with <code>({ ... })</code>
 **
 ** We also use @c __sync_lock_test_and_set and other similar builtins
 ** if they are available. If not, @c __sync_lock_test_and_set and @c
 ** __sync_lock_release are implemented in assembler (for 4 byte
 ** integers) and all other operations are synthesized with an
 ** ::atomic_flag that is used as a spinlock.
 **
 ** @see _Atomic
 ** @see P99_DECLARE_ATOMIC
 **
 ** @{
 **/


/**
 ** @addtogroup atomic_stub Stub replacements of C11 atomic operations
 **
 ** This is a collection stub implementation of C11 atomic types and
 ** operations for the case that gcc doesn't implement them.
 **
 ** These use the @c __asm__ gcc extension.
 **
 ** The only operations that are implemented by these stubs are @c
 ** lock_test_and_set and @c lock_release operations. These are the
 ** minimal set of operations that are needed to comply to the
 ** standard.
 **
 ** @{
 **/

#ifdef P99_TEST_ATOMIC
# undef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_1
# undef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_2
# undef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_4
# undef __GCC_HAVE_SYNC_COMPARE_AND_SWAP_8
#endif

#if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)

p99_inline
uint32_t p00_sync_lock_test_and_set(uint32_t volatile *p00_objp) {
  return __sync_lock_test_and_set(p00_objp, 1);
}

p99_inline
void p00_sync_lock_release(uint32_t volatile *p00_objp) {
  __sync_lock_release(p00_objp);
}

p99_inline
void p00_mfence(void) {
  __sync_synchronize();
}


#elif defined(__arm__)
# include "p99_atomic_arm.h"
#elif defined(__x86_64__) || defined(__i386__)
# include "p99_atomic_x86.h"
#else
# warning "no support for atomic operations detected for this platform" \
# define P00_NO_ATOMICS
#endif

/**
 ** @}
 **/

#ifndef P00_NO_ATOMICS

/**
 ** @addtogroup atomic_macros
 **
 ** Most of the macros defined here just serve the purpose of
 ** detecting the lock-free property for specific integer data
 ** types. Names are used as they are foreseen in the standard, e.g
 ** ::ATOMIC_INT_LOCK_FREE that handles the case for @c int and @c
 ** unsigned types.
 **
 ** This information can be provided externally. If it isn't, some
 ** heuristic is used to detect these features. Currently the only
 ** mechanism that is implemented is the one of gcc.
 **
 ** From this information then is constructed a list of atomic integer
 ** types, ::P99_ATOMIC_LOCK_FREE_TYPES, that is used inside the
 ** macros for type generic programming.
 **
 ** The only type/functions that is always necessary is ::atomic_flag
 ** that is used for all other atomic types.
 **
 ** @{
 **/


#ifndef ATOMIC_INT8_LOCK_FREE
#  if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1)
#   define ATOMIC_INT8_LOCK_FREE 2
#   define P00_TYPE_LIST_ELEM1 , (1, uint8_t)
#  else
#   define ATOMIC_INT8_LOCK_FREE 0
#   define P00_TYPE_LIST_ELEM1
#  endif
#endif
#ifndef ATOMIC_INT16_LOCK_FREE
#  if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2)
#   define ATOMIC_INT16_LOCK_FREE 2
#   define P00_TYPE_LIST_ELEM2 , (2, uint16_t)
#  else
#   define ATOMIC_INT16_LOCK_FREE 0
#   define P00_TYPE_LIST_ELEM2
#  endif
#endif
#ifndef ATOMIC_INT32_LOCK_FREE
#  if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4)
#   define ATOMIC_INT32_LOCK_FREE 2
#   define P00_TYPE_LIST_ELEM4 , (4, uint32_t)
#  else
#   define ATOMIC_INT32_LOCK_FREE 0
#   define P00_TYPE_LIST_ELEM4
#  endif
#endif
#ifndef ATOMIC_INT64_LOCK_FREE
#  if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8)
#   define ATOMIC_INT64_LOCK_FREE 2
#   define P00_TYPE_LIST_ELEM8 , (8, uint64_t)
#  else
#   define ATOMIC_INT64_LOCK_FREE 0
#   define P00_TYPE_LIST_ELEM8
#  endif
#endif
#ifndef ATOMIC_INT128_LOCK_FREE
#  if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_16)
#   define ATOMIC_INT128_LOCK_FREE 2
#   define P00_TYPE_LIST_ELEM16 , (16, p99x_uint128)
#  else
#   define ATOMIC_INT128_LOCK_FREE 0
#   define P00_TYPE_LIST_ELEM16
#  endif
#endif


#ifndef ATOMIC_BOOL_LOCK_FREE
# if defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && (UINT_MAX <= UINT32_MAX)
#define ATOMIC_BOOL_LOCK_FREE 2
# else
#define ATOMIC_BOOL_LOCK_FREE 0
# endif
#endif
#ifndef ATOMIC_CHAR_LOCK_FREE
# define ATOMIC_CHAR_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#endif
#if USHRT_MAX == UINT8_MAX
# define ATOMIC_SHORT_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif USHRT_MAX == UINT16_MAX
# define ATOMIC_SHORT_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif USHRT_MAX == UINT32_MAX
# define ATOMIC_SHORT_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif USHRT_MAX == UINT64_MAX
# define ATOMIC_SHORT_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif USHRT_MAX == UINT128_MAX
# define ATOMIC_SHORT_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_SHORT_LOCK_FREE 0
#endif
#if UINT_MAX == UINT8_MAX
# define ATOMIC_INT_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif UINT_MAX == UINT16_MAX
# define ATOMIC_INT_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif UINT_MAX == UINT32_MAX
# define ATOMIC_INT_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif UINT_MAX == UINT64_MAX
# define ATOMIC_INT_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif UINT_MAX == UINT128_MAX
# define ATOMIC_INT_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_INT_LOCK_FREE 0
#endif
#if ULONG_MAX == UINT8_MAX
# define ATOMIC_LONG_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif ULONG_MAX == UINT16_MAX
# define ATOMIC_LONG_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif ULONG_MAX == UINT32_MAX
# define ATOMIC_LONG_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif ULONG_MAX == UINT64_MAX
# define ATOMIC_LONG_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif ULONG_MAX == UINT128_MAX
# define ATOMIC_LONG_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_LONG_LOCK_FREE 0
#endif
#if ULLONG_MAX == UINT8_MAX
# define ATOMIC_LLONG_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif ULLONG_MAX == UINT16_MAX
# define ATOMIC_LLONG_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif ULLONG_MAX == UINT32_MAX
# define ATOMIC_LLONG_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif ULLONG_MAX == UINT64_MAX
# define ATOMIC_LLONG_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif ULLONG_MAX == UINT128_MAX
# define ATOMIC_LLONG_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_LLONG_LOCK_FREE 0
#endif
#if UINTPTR_MAX == UINT8_MAX
# define ATOMIC_INTPTR_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif UINTPTR_MAX == UINT16_MAX
# define ATOMIC_INTPTR_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif UINTPTR_MAX == UINT32_MAX
# define ATOMIC_INTPTR_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif UINTPTR_MAX == UINT64_MAX
# define ATOMIC_INTPTR_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif UINTPTR_MAX == UINT128_MAX
# define ATOMIC_INTPTR_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_INTPTR_LOCK_FREE 0
#endif
#ifndef ATOMIC_POINTER_LOCK_FREE
#define ATOMIC_POINTER_LOCK_FREE ATOMIC_INTPTR_LOCK_FREE
#endif
#if ATOMIC_INTPTR_LOCK_FREE == 2
#if UINTPTR_MAX == UINT8_MAX
# define P00_UINT_TYPE_LIST                                    \
  (1, uintptr_t)                                               \
  P00_TYPE_LIST_ELEM2                                          \
  P00_TYPE_LIST_ELEM4                                          \
  P00_TYPE_LIST_ELEM8                                          \
  P00_TYPE_LIST_ELEM16
#elif UINTPTR_MAX == UINT16_MAX
# define P00_UINT_TYPE_LIST                                    \
  (2, uintptr_t)                                               \
  P00_TYPE_LIST_ELEM1                                          \
  P00_TYPE_LIST_ELEM4                                          \
  P00_TYPE_LIST_ELEM8                                          \
  P00_TYPE_LIST_ELEM16
#elif UINTPTR_MAX == UINT32_MAX
# define P00_UINT_TYPE_LIST                                    \
  (4, uintptr_t)                                               \
  P00_TYPE_LIST_ELEM1                                          \
  P00_TYPE_LIST_ELEM2                                          \
  P00_TYPE_LIST_ELEM8                                          \
  P00_TYPE_LIST_ELEM16
#elif UINTPTR_MAX == UINT64_MAX
# define P00_UINT_TYPE_LIST                                    \
  (8, uintptr_t)                                               \
  P00_TYPE_LIST_ELEM1                                          \
  P00_TYPE_LIST_ELEM2                                          \
  P00_TYPE_LIST_ELEM4                                          \
  P00_TYPE_LIST_ELEM16
#elif UINTPTR_MAX == UINT128_MAX
# define P00_UINT_TYPE_LIST                                    \
  (16, uintptr_t)                                              \
  P00_TYPE_LIST_ELEM1                                          \
  P00_TYPE_LIST_ELEM2                                          \
  P00_TYPE_LIST_ELEM4                                          \
  P00_TYPE_LIST_ELEM8
#else
# define P00_UINT_TYPE_LIST                                    \
  (1, uint8_t)                                                 \
  P00_TYPE_LIST_ELEM2                                          \
  P00_TYPE_LIST_ELEM4                                          \
  P00_TYPE_LIST_ELEM8                                          \
  P00_TYPE_LIST_ELEM16
#endif
#endif
#if UINT_LEAST16_MAX == UINT16_MAX
# define ATOMIC_CHAR16_T_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif UINT_LEAST16_MAX == UINT32_MAX
# define ATOMIC_CHAR16_T_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif UINT_LEAST16_MAX == UINT64_MAX
# define ATOMIC_CHAR16_T_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif UINT_LEAST16_MAX == UINT128_MAX
# define ATOMIC_CHAR16_T_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_CHAR16_T_LOCK_FREE 0
#endif
#if UINT_LEAST32_MAX == UINT32_MAX
# define ATOMIC_CHAR32_T_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif UINT_LEAST32_MAX == UINT64_MAX
# define ATOMIC_CHAR32_T_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif UINT_LEAST32_MAX == UINT128_MAX
# define ATOMIC_CHAR32_T_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_CHAR32_T_LOCK_FREE 0
#endif
#if WCHAR_MAX == UINT8_MAX || (!WCHAR_MIN && (WCHAR_MAX == INT8_MAX))
# define ATOMIC_WCHAR_LOCK_FREE ATOMIC_INT8_LOCK_FREE
#elif WCHAR_MAX == UINT16_MAX || (!WCHAR_MIN && (WCHAR_MAX == INT16_MAX))
# define ATOMIC_WCHAR_LOCK_FREE ATOMIC_INT16_LOCK_FREE
#elif WCHAR_MAX == UINT32_MAX || (!WCHAR_MIN && (WCHAR_MAX == INT32_MAX))
# define ATOMIC_WCHAR_LOCK_FREE ATOMIC_INT32_LOCK_FREE
#elif WCHAR_MAX == UINT64_MAX || (!WCHAR_MIN && (WCHAR_MAX == INT64_MAX))
# define ATOMIC_WCHAR_LOCK_FREE ATOMIC_INT64_LOCK_FREE
#elif WCHAR_MAX == UINT128_MAX || (!WCHAR_MIN && (WCHAR_MAX == INT128_MAX))
# define ATOMIC_WCHAR_LOCK_FREE ATOMIC_INT128_LOCK_FREE
#else
# define ATOMIC_WCHAR_LOCK_FREE 0
#endif

/**
 ** @brief A default unsigned integer type for type @a T
 **
 ** @return The result is an unsigned integer type of the same width
 ** as @a T, if such an integer type exists. Otherwise, the type is @c
 ** uintptr_t, that is an unsigned integer type that should be
 ** compatible with address arithmetic.
 **
 ** If @a T has the same width the result is @c uintptr_t. Otherwise,
 ** the result type is chosen among @c uint8_t, @c uint16_t, @c
 ** uint32_t or @c uint64_t if any of them fits the width. If none
 ** does, we fall back to @c uintptr_t.
 **
 ** As a consequence, if @a T is an integer type and the
 ** implementation has another integer type of the same width, this is
 ** not necessarily the integer type "corresponding" to @a T. E.g if
 ** @a T is <code>long long</code> and has the same width as @c long,
 ** the result type could well be <code>unsigned long</code> instead
 ** of <code>unsigned long long</code>.
 **
 ** @remark Uses the @c __typeof__ extension of the gcc-family of
 ** compilers.
 **/
P00_DOCUMENT_TYPE_ARGUMENT(P99_UINT_DEFAULT, 0)
#if defined(P00_UINT_TYPE_LIST) || defined(P00_DOXYGEN)
# define P99_UINT_DEFAULT(T)                                                      \
__typeof__(P99_GENERIC_SIZE_LIT(sizeof(T), (uintptr_t){ 0 }, P00_UINT_TYPE_LIST))
#else
# define P00_UINT_TYPE_LIST
# define P99_UINT_DEFAULT(T) uintptr_t
#endif

/**
 ** @brief Initialize a variable of type ::atomic_flag
 ** @memberof atomic_flag
 **/
#define ATOMIC_FLAG_INIT P99_ENC_INIT(0)

/**
 ** @brief Initialize a variable of an atomic type.
 ** @memberof atomic_int
 **/
#define ATOMIC_VAR_INIT(V) { .p00_xval = { .p00_type_member = (V), }, }

/**
 ** @brief A list of types that are supposed to have lock-free atomic
 ** operations.
 **
 ** This list is constructed from the knowledge given via the
 ** lock-free macros for the standard types.
 **
 ** @see ATOMIC_BOOL_LOCK_FREE
 ** @see ATOMIC_CHAR_LOCK_FREE
 ** @see ATOMIC_SHORT_LOCK_FREE
 ** @see ATOMIC_INT_LOCK_FREE
 ** @see ATOMIC_LONG_LOCK_FREE
 ** @see ATOMIC_LLONG_LOCK_FREE
 **/
#ifndef P99_ATOMIC_LOCK_FREE_TYPES
# define P99_ATOMIC_LOCK_FREE_TYPES P00_ATOMIC_LOCK_FREE_TYPES6_

#if ATOMIC_BOOL_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES0 _Bool
#endif
#if ATOMIC_CHAR_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES1 char, signed char, unsigned char
#endif
#if ATOMIC_SHORT_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES2 short, unsigned short
#endif
#if ATOMIC_INT_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES3 int, unsigned int
#endif
#if ATOMIC_LONG_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES4 long, unsigned long
#endif
#if ATOMIC_LLONG_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES5 long long, unsigned long long
#endif
#if ATOMIC_POINTER_LOCK_FREE == 2
# define P00_ATOMIC_LOCK_FREE_TYPES6 char*, signed char*, unsigned char*
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES0
# ifdef P00_ATOMIC_LOCK_FREE_TYPES1
#  define P00_ATOMIC_LOCK_FREE_TYPES1_ P00_ATOMIC_LOCK_FREE_TYPES0, P00_ATOMIC_LOCK_FREE_TYPES1
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES1_ P00_ATOMIC_LOCK_FREE_TYPES0
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES1
#  define P00_ATOMIC_LOCK_FREE_TYPES1_ P00_ATOMIC_LOCK_FREE_TYPES1
# endif
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES1_
# ifdef P00_ATOMIC_LOCK_FREE_TYPES2
#  define P00_ATOMIC_LOCK_FREE_TYPES2_ P00_ATOMIC_LOCK_FREE_TYPES1_, P00_ATOMIC_LOCK_FREE_TYPES2
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES2_ P00_ATOMIC_LOCK_FREE_TYPES1_
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES2
#  define P00_ATOMIC_LOCK_FREE_TYPES2_ P00_ATOMIC_LOCK_FREE_TYPES2
# endif
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES2_
# ifdef P00_ATOMIC_LOCK_FREE_TYPES3
#  define P00_ATOMIC_LOCK_FREE_TYPES3_ P00_ATOMIC_LOCK_FREE_TYPES2_, P00_ATOMIC_LOCK_FREE_TYPES3
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES3_ P00_ATOMIC_LOCK_FREE_TYPES2_
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES3
#  define P00_ATOMIC_LOCK_FREE_TYPES3_ P00_ATOMIC_LOCK_FREE_TYPES3
# endif
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES3_
# ifdef P00_ATOMIC_LOCK_FREE_TYPES4
#  define P00_ATOMIC_LOCK_FREE_TYPES4_ P00_ATOMIC_LOCK_FREE_TYPES3_, P00_ATOMIC_LOCK_FREE_TYPES4
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES4_ P00_ATOMIC_LOCK_FREE_TYPES3_
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES4
#  define P00_ATOMIC_LOCK_FREE_TYPES4_ P00_ATOMIC_LOCK_FREE_TYPES4
# endif
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES4_
# ifdef P00_ATOMIC_LOCK_FREE_TYPES5
#  define P00_ATOMIC_LOCK_FREE_TYPES5_ P00_ATOMIC_LOCK_FREE_TYPES4_, P00_ATOMIC_LOCK_FREE_TYPES5
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES5_ P00_ATOMIC_LOCK_FREE_TYPES4_
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES5
#  define P00_ATOMIC_LOCK_FREE_TYPES5_ P00_ATOMIC_LOCK_FREE_TYPES5
# endif
#endif

#ifdef P00_ATOMIC_LOCK_FREE_TYPES5_
# ifdef P00_ATOMIC_LOCK_FREE_TYPES6
#  define P00_ATOMIC_LOCK_FREE_TYPES6_ P00_ATOMIC_LOCK_FREE_TYPES5_, P00_ATOMIC_LOCK_FREE_TYPES6
# else
#  define P00_ATOMIC_LOCK_FREE_TYPES6_ P00_ATOMIC_LOCK_FREE_TYPES5_
# endif
#else
# ifdef P00_ATOMIC_LOCK_FREE_TYPES6
#  define P00_ATOMIC_LOCK_FREE_TYPES6_ P00_ATOMIC_LOCK_FREE_TYPES6
# endif
#endif
#endif

#ifndef P00_ATOMIC_LOCK_FREE_TYPES6_
# define P00_ATOMIC_LOCK_FREE_TYPES6_
#endif

/**
 ** @}
 **/


/**
 ** @addtogroup atomic_enum
 ** @{
 **/

P99_DECLARE_ENUM(memory_order,
                 memory_order_relaxed,
                 memory_order_consume,
                 memory_order_acquire,
                 memory_order_release,
                 memory_order_acq_rel,
                 memory_order_seq_cst
                );


/**
 ** @}
 **/

/**
 ** @addtogroup atomic_types
 ** @{
 **/

/**
 ** @brief The "minimal" type for which atomic operations must be
 ** defined.
 **
 ** This only has very basic operations that can set and clear the
 ** flag. In particular it has no operation that returns the state of
 ** the flag and that would not modify it eventually. So this is a
 ** kind of Heisenberg flag, you can't measure it without modifying
 ** it. This can be used as a spinlock to force the flag to be set,
 ** perform a desired operation and then clear the flag, again.
 **
 ** There are extensions ::atomic_flag_lock, ::atomic_flag_trylock and
 ** ::atomic_flag_unlock that perform these spinlock type operations
 ** with only the necessary memory consistency.
 **/
P99_ENC_DECLARE(uint32_t volatile, atomic_flag);

#define P00_AT(OBJP) ((OBJP)->p00_xval.p00_type_member)
#define P00_AO(OBJP) ((OBJP)->p00_xval.p00_operator_member)
#define P00_AM(OBJP) ((OBJP)->p00_xval.p00_memory_member)

P00_DOCUMENT_IDENTIFIER_ARGUMENT(P00_DECLARE_ATOMIC_TYPE, 0)
P00_DOCUMENT_TYPE_ARGUMENT(P00_DECLARE_ATOMIC_TYPE, 1)
P00_DOCUMENT_IDENTIFIER_ARGUMENT(P00_DECLARE_ATOMIC_TYPE, 2)
#define P00_DECLARE_ATOMIC_TYPE(TAGGER, TI, T, NAME)                   \
TAGGER P99_PASTE3(NAME, _, TAGGER) {                                   \
  atomic_flag p00_lock;                                                \
  union {                                                              \
    TI p00_operator_member;                                            \
    P99_UINT_DEFAULT(T) p00_memory_member;                             \
    T p00_type_member;                                                 \
  } p00_xval;                                                          \
};                                                                     \
typedef TAGGER P99_PASTE3(NAME, _, TAGGER) P99_PASTE3(NAME, _, TAGGER)


/**
 ** @brief declare an atomic type that will have lock-free operations
 **
 ** Normally it shouldn't be necessary to use this macro in user code
 ** for integral data types; the lock-free types should be detected
 ** automatically.
 **/
P00_DOCUMENT_TYPE_ARGUMENT(P99_DECLARE_ATOMIC_LOCK_FREE, 0)
P00_DOCUMENT_IDENTIFIER_ARGUMENT(P99_DECLARE_ATOMIC_LOCK_FREE, 1)
#ifdef P00_DOXYGEN
# define P99_DECLARE_ATOMIC_LOCK_FREE(T, NAME)                                                                         \
/** @brief Atomic access to a value of type <code>T</code> @see atomic_int for the possible operations on this type */ \
typedef P99_PASTE3(p99_, NAME, _union) NAME
#else
#define P99_DECLARE_ATOMIC_LOCK_FREE(T, NAME)                          \
P00_DECLARE_ATOMIC_TYPE(union, T, T, P99_PASTE2(p99_, NAME));          \
P00_DECLARE_ATOMIC_TYPE(struct, uintptr_t, T, P99_PASTE2(p99_, NAME)); \
typedef P99_PASTE3(p99_, NAME, _union) NAME
#endif


#define P00_DECLARE_ATOMIC2(T, ...)                                                                                    \
P00_DECLARE_ATOMIC_TYPE(union, T, T, P99_PASTE(p99_, __VA_ARGS__));                                                    \
P00_DECLARE_ATOMIC_TYPE(struct, uintptr_t, T, P99_PASTE(p99_, __VA_ARGS__));                                           \
/** @brief Atomic access to a value of type <code>T</code> @see atomic_int for the possible operations on this type */ \
typedef P99_PASTE(p99_, __VA_ARGS__, _struct) __VA_ARGS__

/**
 ** @brief declare an atomic type that will use lock operations to
 ** guarantee atomicity
 **
 ** Normally it shouldn't be necessary to use this macro in user code
 ** for integral data types; the lock-free types should be detected
 ** automatically.
 **
 ** This is used per default for integral types that are detected not
 ** to support atomic operations directly and for floating point
 ** types.
 **
 ** Use this macro to protect your own particular data type.
 **
 ** @param T base type for the atomic type
 **
 ** @param NAME the name of the newly derived type. Usually for type
 ** @c foo you should use something like atomic_foo.
 **/
P00_DOCUMENT_TYPE_ARGUMENT(P99_DECLARE_ATOMIC, 0)
P00_DOCUMENT_IDENTIFIER_ARGUMENT(P99_DECLARE_ATOMIC_LOCK_FREE, 1)
#ifdef P00_DOXYGEN
#define P99_DECLARE_ATOMIC(T, NAME)
#else
#define P99_DECLARE_ATOMIC(...)                                                                                        \
/** @brief Atomic access to a value of type <code>T</code> @see atomic_int for the possible operations on this type */ \
P99_IF_EQ_1(P99_NARG(__VA_ARGS__))                                                                                     \
(P00_DECLARE_ATOMIC2(__VA_ARGS__, P99_PASTE2(atomic_, __VA_ARGS__)))                                                   \
(P00_DECLARE_ATOMIC2(__VA_ARGS__))
#endif

#define P00_DECLARE_ATOMIC(T, ...)                                                                                     \
/** @brief Atomic access to a value of type <code>T</code> @see atomic_int for the possible operations on this type */ \
typedef P00_ATOMIC_STRUCT(T, __VA_ARGS__) __VA_ARGS__

#define P00_DECLARE_ATOMIC_CHOICE(MAC, T, NAME)                \
P99_IF_EQ_2(MAC)                                               \
(P99_DECLARE_ATOMIC_LOCK_FREE(T, NAME))                        \
(P99_DECLARE_ATOMIC(T, NAME))


/**
 ** @brief Atomic access to a value of type <code>int</code>
 **
 ** See the "public member function" section of this description for
 ** the operations that support this type
 **/
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_INT_LOCK_FREE, int, atomic_int);

/**
 ** @brief Atomic access to a value of type <code>bool</code>
 ** @see atomic_load
 ** @see atomic_store
 ** @see atomic_compare_exchange_weak
 ** @see ATOMIC_VAR_INIT
 ** @see atomic_init
 **
 **/
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_BOOL_LOCK_FREE, _Bool, atomic_bool);


P00_DECLARE_ATOMIC_CHOICE(ATOMIC_CHAR_LOCK_FREE, char, atomic_char);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_CHAR_LOCK_FREE, signed char, atomic_schar);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_CHAR_LOCK_FREE, unsigned char, atomic_uchar);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_SHORT_LOCK_FREE, short, atomic_short);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_SHORT_LOCK_FREE, unsigned short, atomic_ushort);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_INT_LOCK_FREE, unsigned int, atomic_uint);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_LONG_LOCK_FREE, long, atomic_long);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_LONG_LOCK_FREE, unsigned long, atomic_ulong);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_LLONG_LOCK_FREE, long long, atomic_llong);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_LONG_LOCK_FREE, unsigned long long, atomic_ullong);

P00_DECLARE_ATOMIC_CHOICE(ATOMIC_POINTER_LOCK_FREE, char*, atomic_char_ptr);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_POINTER_LOCK_FREE, signed char*, atomic_schar_ptr);
P00_DECLARE_ATOMIC_CHOICE(ATOMIC_POINTER_LOCK_FREE, unsigned char*, atomic_uchar_ptr);

P99_DECLARE_ATOMIC(float, atomic_float);
P99_DECLARE_ATOMIC(double, atomic_double);
P99_DECLARE_ATOMIC(long double, atomic_ldouble);

#ifndef __STDC_NO_COMPLEX__
P99_DECLARE_ATOMIC(float _Complex, atomic_cfloat);
P99_DECLARE_ATOMIC(double _Complex, atomic_cdouble);
P99_DECLARE_ATOMIC(long double _Complex, atomic_cldouble);
#define P00_ATOMIC_TYPES                                       \
  (_Bool, atomic_bool*),                                       \
  (char, atomic_char*),                                        \
  (signed char, atomic_schar*),                                \
  (unsigned char, atomic_uchar*),                              \
  (short int, atomic_short*),                                  \
  (unsigned short int, atomic_ushort*),                        \
  (int, atomic_int*),                                          \
  (unsigned int, atomic_uint*),                                \
  (long int, atomic_long*),                                    \
  (unsigned long int, atomic_ulong*),                          \
  (long long int, atomic_llong*),                              \
  (unsigned long long int, atomic_ullong*),                    \
  (float, atomic_float*),                                      \
  (double, atomic_double*),                                    \
  (long double, atomic_ldouble*),                              \
  (float _Complex, atomic_cfloat*),                            \
  (double _Complex, atomic_cdouble*),                          \
  (long double _Complex, atomic_cldouble*)
#else
#define P00_ATOMIC_TYPES                                       \
  (_Bool, atomic_bool*),                                       \
  (char, atomic_char*),                                        \
  (signed char, atomic_schar*),                                \
  (unsigned char, atomic_uchar*),                              \
  (short int, atomic_short*),                                  \
  (unsigned short int, atomic_ushort*),                        \
  (int, atomic_int*),                                          \
  (unsigned int, atomic_uint*),                                \
  (long int, atomic_long*),                                    \
  (unsigned long int, atomic_ulong*),                          \
  (long long int, atomic_llong*),                              \
  (unsigned long long int, atomic_ullong*),                    \
  (float, atomic_float*),                                      \
  (double, atomic_double*),                                    \
  (long double, atomic_ldouble*)
#endif


P00_DOCUMENT_TYPE_ARGUMENT(P99_ATOMIC_INHERIT, 0)
#define P99_ATOMIC_INHERIT(T)                                                               \
(*P99_GENERIC_LIT                                                                           \
 ((T){ 0 },                                                                                 \
  P99_GENERIC_SIZE_LIT                                                                      \
  (sizeof(T)+1,                                                                             \
   (struct P99_PASTE3(p99_atomic_, T, _struct)*){ 0 },                                      \
   (1, union P99_PASTE3(p99_atomic_, T, _union)*)                                           \
   P99_IF_EQ_2(ATOMIC_INT8_LOCK_FREE)(,(2, union P99_PASTE3(p99_atomic_, T, _union)*))()    \
   P99_IF_EQ_2(ATOMIC_INT16_LOCK_FREE)(,(3, union P99_PASTE3(p99_atomic_, T, _union)*))()   \
   P99_IF_EQ_2(ATOMIC_INT32_LOCK_FREE)(,(5, union P99_PASTE3(p99_atomic_, T, _union)*))()   \
   P99_IF_EQ_2(ATOMIC_INT64_LOCK_FREE)(,(9, union P99_PASTE3(p99_atomic_, T, _union)*))()   \
   P99_IF_EQ_2(ATOMIC_INT128_LOCK_FREE)(,(17, union P99_PASTE3(p99_atomic_, T, _union)*))() \
   ),                                                                                       \
  P00_ATOMIC_TYPES))

/**
 ** @brief refer to an atomic type of base type T
 **
 ** @param T a @b type name
 **
 ** This implements only part of the @c _Atomic keyword of C11. First
 ** it implements only the part described in 6.7.2.4, atomic type
 ** specifiers. I does not cover type qualifiers.
 **
 ** Second for data types that are not predefined integer or
 ** floatingpoint types, the underlying realization of the atomic type
 ** must have been previously declared with ::P99_DECLARE_ATOMIC.
 **
 ** Besides the atomic integer types that are listed in the standard
 ** we provide 6 atomic types for all real and complex floating point
 ** types, as long as they exist.
 **
 ** For the standard predefined integer types there are different
 ** atomic types as they are listed in the standard. The atomic types
 ** for the floating point types (as there are no names listed in the
 ** standard) are chosen arbitrarily with an @c atomic_ prefix. Don't
 ** rely on a particular naming. Whenever ::_Atomic is called with a
 ** typename @a T that resolves to one of the predefined arithmetic
 ** types (without @c enum and pointers) the same type as for the
 ** original type results: e.g if @c unsigned is 32 bit wide (and @c
 ** long is 64)
 **
 ** @code
 ** _Atomic(uint32_t) a;
 ** _Atomic(unsigned) b;
 ** @endcode
 **
 ** is equivalent to
 **
 ** @code
 ** atomic_uint a;
 ** atomic_uint b;
 ** @endcode
 **
 ** @remark These types don't work with the usual operators such as @c
 ** +=, and a variable of such an atomic type doesn't evaluate to an
 ** rvalue of the base type.
 **
 ** @remark These variables are guaranteed to be in a valid state when
 ** they are @c 0 initialized by the default initializer, either
 ** explicitly or when declared with static storage location. The
 ** initial value of the base type corresponds to its @c 0 initialized
 ** value.
 **
 ** @see ATOMIC_VAR_INIT for an initializer macro
 ** @see atomic_init for a initializer function
 ** @see atomic_load to return an rvalue of the underlying base type @a T
 ** @see atomic_store to store a value that is compatible with @a T
 ** @see atomic_compare_exchange_weak to store a value conditionally
 **
 ** With the exception of the floating point types, these latter three
 ** operations are lock-free if a lock-free low-level function is
 ** implemented for an unsigned integer type that has the same
 ** width. This should usually be the case for all integer types,
 ** including @c enum, and pointer types.
 **
 ** If the underlying operations are defined for @a T the following
 ** generic functions (macros) can be used with an atomic type:
 **
 ** @see atomic_fetch_add to add a value to the object as @c += would on @a T. This should be
 ** lock-free for all integer types (see above) and is @em not
 ** lock-free for floating point types.
 **
 ** @see atomic_fetch_sub to subtract a value as @c -= would on @a T. See ::atomic_fetch_add
 ** for constraints.
 **
 ** @see atomic_fetch_or "or" a value in place as @c |= would on @a T
 **
 ** @see atomic_fetch_and "and" a value in place as @c &= would on @a T
 **
 ** @see atomic_fetch_xor "exclusive or" a value in place as @c ^= would on @a T
 **
 ** @warning Don't assign atomic variables through the @c =
 ** operator. This will most probably not do what you expect:
 ** - This would copy state information from the right side to the
 **   left side.
 ** - Neither the read access on the right side nor the write access
 **   on the left side would be atomic.
 **
 ** @warning Don't use atomic variables as function arguments.
 ** - If you are only interested in the value evaluate by ::atomic_load.
 ** - Otherwise pass it through a pointer to atomic type.
 **
 ** @ingroup C11_keywords
 **/
#ifdef P00_DOXYGEN
# define _Atomic(T) P99_PASTE2(atomic_, T)
#else
# define _Atomic(T) __typeof__(P99_ATOMIC_INHERIT(T))
#endif

typedef _Atomic(char16_t) atomic_char16_t;
typedef _Atomic(char32_t) atomic_char32_t;
typedef _Atomic(wchar_t) atomic_wchar_t;
typedef _Atomic(int_least8_t) atomic_int_least8_t;
typedef _Atomic(uint_least8_t) atomic_uint_least8_t;
typedef _Atomic(int_least16_t) atomic_int_least16_t;
typedef _Atomic(uint_least16_t) atomic_uint_least16_t;
typedef _Atomic(int_least32_t) atomic_int_least32_t;
typedef _Atomic(uint_least32_t) atomic_uint_least32_t;
typedef _Atomic(int_least64_t) atomic_int_least64_t;
typedef _Atomic(uint_least64_t) atomic_uint_least64_t;
typedef _Atomic(int_fast8_t) atomic_int_fast8_t;
typedef _Atomic(uint_fast8_t) atomic_uint_fast8_t;
typedef _Atomic(int_fast16_t) atomic_int_fast16_t;
typedef _Atomic(uint_fast16_t) atomic_uint_fast16_t;
typedef _Atomic(int_fast32_t) atomic_int_fast32_t;
typedef _Atomic(uint_fast32_t) atomic_uint_fast32_t;
typedef _Atomic(int_fast64_t) atomic_int_fast64_t;
typedef _Atomic(uint_fast64_t) atomic_uint_fast64_t;
typedef _Atomic(intptr_t) atomic_intptr_t;
typedef _Atomic(uintptr_t) atomic_uintptr_t;
typedef _Atomic(size_t) atomic_size_t;
typedef _Atomic(ptrdiff_t) atomic_ptrdiff_t;
typedef _Atomic(intmax_t) atomic_intmax_t;
typedef _Atomic(uintmax_t) atomic_uintmax_t;

/**
 ** @}
 **/

/**
 ** @addtogroup fences Memory Fences
 **
 ** This is only rudimentary support for fences. Basically all fences
 ** but with argument ::memory_order_relaxed perform a full memory
 ** barrier.
 ** @{
 **/

p99_inline
void atomic_thread_fence(memory_order p00_ord) {
  switch (p00_ord) {
  case memory_order_relaxed: break;
  default: p00_mfence(); break;
  }
}

/**
 ** @remark In the current implementation a signal fence and a thread
 ** fence are the same full memory barrier.
 **/
#define atomic_signal_fence atomic_thread_fence

/**
 ** @}
 **/

/**
 ** @brief Unconditionally set @a *object to @c true and return the
 ** previous value
 **
 ** @memberof atomic_flag
 **
 ** @param object the object that will be set
 **
 ** @param order fine tune the set operation for a specific memory
 ** order. The current implementation only uses this to enforce a
 ** complete memory barrier where necessary.
 **/
p99_inline
_Bool atomic_flag_test_and_set_explicit(volatile atomic_flag *p00_objp, memory_order p00_ord) {
  _Bool p00_ret;
  switch (p00_ord) {
    /* This case doesn't require any guarantee. */
  case memory_order_relaxed:
    p00_ret = P99_ENCP(p00_objp);
    P99_ENCP(p00_objp) = 1;
    break;
    /* For these three the acquire semantics are not sufficient. */
  case memory_order_release: ;
  case memory_order_acq_rel: ;
  case memory_order_seq_cst:
    atomic_thread_fence(p00_ord);
    p00_ret = p00_sync_lock_test_and_set(&P99_ENCP(p00_objp));
    break;
  default:
    p00_ret = p00_sync_lock_test_and_set(&P99_ENCP(p00_objp));
    break;
  }
  return p00_ret;
}

/**
 ** @brief Unconditionally set @a *object to @c true and return the
 ** previous value
 **
 ** @memberof atomic_flag
 **/
p99_inline
_Bool atomic_flag_test_and_set(volatile atomic_flag *p00_objp) {
  return atomic_flag_test_and_set_explicit(p00_objp, memory_order_seq_cst);
}

/**
 ** @brief Unconditionally set @a *p00_objp to @c false
 **
 ** @memberof atomic_flag
 **
 ** @param p00_objp the object that will be set
 **
 ** @param p00_ord fine tune the set operation for a specific memory
 ** order. The current implementation only uses this to enforce a
 ** complete memory barrier where necessary.
 **/
p99_inline
void atomic_flag_clear_explicit(volatile atomic_flag *p00_objp, memory_order p00_ord) {
  switch(p00_ord) {
    /* This case doesn't require any guarantee. */
  case memory_order_relaxed:
    P99_ENCP(p00_objp) = 0;
    break;
    /* For these three the release semantics are not sufficient. */
  case memory_order_acquire: ;
  case memory_order_acq_rel: ;
  case memory_order_seq_cst:
    p00_sync_lock_release(&P99_ENCP(p00_objp));
    atomic_thread_fence(p00_ord);
    break;
  default:
    p00_sync_lock_release(&P99_ENCP(p00_objp));
    break;
  }
}

/**
 ** @brief Unconditionally set @a *p00_objp to @c false
 **
 ** @memberof atomic_flag
 **/
p99_inline
void atomic_flag_clear(volatile atomic_flag *p00_objp) {
  atomic_flag_clear_explicit(p00_objp, memory_order_seq_cst);
}

/**
 ** @brief extension: spin on @a p00_objp setting the flag until the state before was "clear"
 **
 ** This interprets an ::atomic_flag as a spinlock. State "clear"
 ** means unlocked and state "set" means locked.
 **
 ** This operation only guarantees acquire consistency.
 **
 ** @memberof atomic_flag
 **/
p99_inline
void atomic_flag_lock(volatile atomic_flag *p00_objp) {
  while (atomic_flag_test_and_set_explicit(p00_objp, memory_order_acquire));
}

/**
 ** @brief extension: set the flag and return true if we are the first
 ** to do so
 **
 ** This interprets an ::atomic_flag as a spinlock. State "clear"
 ** means unlocked and state "set" means locked.
 **
 ** This operation only guarantees acquire consistency.
 **
 ** @memberof atomic_flag
 **/
p99_inline
_Bool atomic_flag_trylock(volatile atomic_flag *p00_objp) {
  return !atomic_flag_test_and_set_explicit(p00_objp, memory_order_acquire);
}

/**
 ** @brief extension: clear the flag unconditionally
 **
 ** This operation only guarantees release consistency.
 **
 ** @memberof atomic_flag
 **/
p99_inline
void atomic_flag_unlock(volatile atomic_flag *p00_objp) {
  atomic_flag_clear_explicit(p00_objp, memory_order_release);
}

/**
 ** @brief Protect the following block or statement as a critical
 ** section of the program by using @a FLAGP as a spinlock.
 **
 ** @param FLAGP is an expression that evaluates to a pointer to
 ** ::atomic_flag. The pointee is used as a spinlock, so this is an
 ** active wait.
 **
 ** @remark @a FLAGP is only evaluated once at the beginning, so it
 ** would be safe to change it in the depending block or statement.
 **
 ** @warning Such a section should only contain a handful of
 ** statements.
 **
 ** @warning Such a section should not contain preliminary exits such
 ** as @c goto, @c break, @c return, @c longjmp, or ::P99_UNWIND etc.
 **
 ** Such a critical section is only protected against threads that try
 ** to enter this same critical section. Threads may well be
 ** simultaneously be in different critical sections.
 **
 ** @see P99_MUTUAL_EXCLUDE that is more suited for larger sections.
 **/
#define P99_SPIN_EXCLUDE(FLAGP)                                             \
P00_BLK_START                                                               \
P00_BLK_DECL(register atomic_flag volatile*const, P99_FILEID(flg), (FLAGP)) \
P00_BLK_BEFAFT(atomic_flag_lock(P99_FILEID(flg)),                           \
               atomic_flag_unlock(P99_FILEID(flg)))                         \
P00_BLK_END

/**
 ** @brief return true if @a OBJP points to a lock-free object
 **
 ** @remark in this implementation this is a compile time expression,
 ** but this is nothing you should build upon for future
 ** implementations of the C11 standard. For us this property is
 ** important such that two different versions of the atomic
 ** operations can be chosen at compile time.
 **
 ** @remark Uses the @c __typeof__ extension of the gcc-compiler
 ** family.
 **
 ** @memberof atomic_int
 **/
#define atomic_is_lock_free(OBJP) (!offsetof(__typeof__(*OBJP), p00_xval))

#define P00_ATOMIC_TERN(OBJP, VAL, ALT) P99_TYPED_TERN(atomic_is_lock_free(OBJP), (VAL), (ALT))


/**
 ** @brief Initialize the object behind @a OBJP with value @a VAL
 **
 ** @a VAL and the base type of @a OBJP must be assignment compatible.
 **
 ** @warning this should only be used in a context that is known to
 ** have no race condition
 **
 ** @memberof atomic_int
 **/
#define atomic_init(OBJP, VAL)                                 \
(void)({                                                       \
    P99_MAC_ARGS((p00_objp, OBJP), (p00_val, VAL));            \
    /* To take care of the atomic_flag and padding bytes. */   \
    memset(p00_objp, 0, sizeof *p00_objp);                     \
    P00_AT(p00_objp) = p00_val;                                \
  })

/**
 ** @brief Store @a VAL into the object behind @a OBJP.
 **
 ** @a VAL and the base type of @a OBJP must be assignment compatible.
 **
 ** @remark this can be used in a context that is known to have a race
 ** condition
 **
 ** @memberof atomic_int
 **/
#define atomic_store(OBJP, DESIRED)                                                              \
({                                                                                               \
  P99_MACRO_PVAR(p00_objp, (OBJP), volatile);                                                    \
  P99_MACRO_VAR(p00_des, (DESIRED));                                                             \
  if (!atomic_is_lock_free(p00_objp))                                                            \
    P99_SPIN_EXCLUDE(&p00_objp->p00_lock)                                                        \
      P00_AT(p00_objp) = p00_des;                                                                \
  P99_IF_EMPTY(P99_ATOMIC_LOCK_FREE_TYPES)                                                       \
    ()                                                                                           \
    (else {                                                                                      \
      union {                                                                                    \
        __typeof__(P00_AT(p00_objp)) p00_t;                                                      \
        __typeof__(P00_AM(p00_objp)) p00_m;                                                      \
      } p00_desm = { .p00_t = p00_des };                                                         \
      P99_MACRO_VAR(p00_prem, P00_AM(p00_objp));                                                 \
      for (;;) {                                                                                 \
        P99_MACRO_VAR(p00_valm,                                                                  \
                      __sync_val_compare_and_swap(&P00_AM(p00_objp), p00_prem, p00_desm.p00_m)); \
        if (p00_valm == p00_prem) break;                                                         \
        p00_prem = p00_valm;                                                                     \
      }                                                                                          \
    })                                                                                           \
 })

/**
 ** @brief Return the value of the object behind @a OBJP.
 **
 ** @remark this can be used in a context that is known to have a race
 ** condition
 **
 ** @memberof atomic_int
 **/
#define atomic_load(OBJP)                                                     \
({                                                                            \
  P99_MACRO_PVAR(p00_objp, (OBJP), volatile);                                 \
  union {                                                                     \
    __typeof__(P00_AT(p00_objp)) p00_t;                                       \
    __typeof__(P00_AM(p00_objp)) p00_m;                                       \
  } p00_ret;                                                                  \
  if (!atomic_is_lock_free(p00_objp))                                         \
    P99_SPIN_EXCLUDE(&p00_objp->p00_lock)                                     \
      p00_ret.p00_t = P00_AT(p00_objp);                                       \
  P99_IF_EMPTY(P99_ATOMIC_LOCK_FREE_TYPES)                                    \
    (else p00_ret.p00_t = P00_AT(p00_objp);)                                  \
    (else {                                                                   \
      p00_ret.p00_m =                                                         \
        P00_ATOMIC_TERN(p00_objp,                                             \
                        __sync_val_compare_and_swap(&P00_AM(p00_objp), 0, 0), \
                        P00_AM(p00_objp));                                    \
    })                                                                        \
    /* assign to itself to be sure that the result is an rvalue */            \
    p00_ret.p00_t = p00_ret.p00_t;                                            \
 })

#define P00_CVT(EXP) ((void const*)(((struct { void const volatile* a; }){ .a = (EXP) }).a))

/**
 ** @brief Atomically compare @a *OBJP to @a *EXPECTED and set it to
 ** @a DESIRED if they are equal.
 **
 ** @return true if both values agree and @c false otherwise.  If the
 ** two values are not the same, the current value is returned in @a
 ** *EXPECTED, additionally.
 **
 ** The base type of @a OBJP and @a *EXPECTED must be compatible, that
 ** is they must be the same if all qualifiers are taken out.
 **
 ** @a DESIRED must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** @memberof atomic_int
 **/
#define atomic_compare_exchange_weak(OBJP, EXPECTED, DESIRED)                                   \
({                                                                                              \
  P99_MACRO_PVAR(p00_objp, (OBJP), volatile);                                                   \
  P99_MACRO_PVAR(p00_exp, (EXPECTED), volatile);                                                \
  P99_MACRO_VAR(p00_des, DESIRED);                                                              \
  _Bool p00_ret = false;                                                                        \
  if (!atomic_is_lock_free(p00_objp)) {                                                         \
    P99_SPIN_EXCLUDE(&p00_objp->p00_lock) {                                                     \
      p00_ret = !memcmp(P00_CVT(p00_exp), P00_CVT(&P00_AT(p00_objp)), sizeof P00_AT(p00_objp)); \
      /* Both, *EXPECTED and DESIRED must be assignment compatible                              \
         with the base type */                                                                  \
      if (p00_ret) P00_AT(p00_objp) = p00_des;                                                  \
      else *p00_exp = P00_AT(p00_objp);                                                         \
    }                                                                                           \
  }                                                                                             \
  P99_IF_EMPTY(P99_ATOMIC_LOCK_FREE_TYPES)                                                      \
    (else p00_ret = false;)                                                                     \
    (else {                                                                                     \
      __typeof__(P00_AM(p00_objp)) volatile* p00_expm                                           \
        = (__typeof__(P00_AM(p00_objp))*)P00_ATOMIC_TERN(p00_objp, p00_exp, 0);                 \
      union {                                                                                   \
        __typeof__(P00_AT(p00_objp)) p00_t;                                                     \
        __typeof__(P00_AM(p00_objp)) p00_m;                                                     \
      } p00_desm = { .p00_t = p00_des };                                                        \
      __typeof__(P00_AM(p00_objp)) p00_valm                                                     \
        = P00_ATOMIC_TERN                                                                       \
        (p00_objp,                                                                              \
         __sync_val_compare_and_swap(&P00_AM(p00_objp), *p00_expm, p00_desm.p00_m),             \
         0);                                                                                    \
      p00_ret = (*p00_expm == p00_valm);                                                        \
      if (!p00_ret) *p00_expm = p00_valm;                                                       \
    })                                                                                          \
  p00_ret;                                                                                      \
 })

/**
 ** @brief Atomically swap the value of the atomic variable @a AOP
 ** with the contents of @a BOP.
 **
 ** @remark @a BOP must be a pointer to the base type of @a AOP
 **/
#define atomic_swap(AOP, BOP)                                           \
({                                                                      \
  P99_MACRO_PVAR(p00_aop, (AOP), volatile);                             \
  P99_MACRO_PVAR(p00_bop, (BOP));                                       \
  __typeof__(*p00_bop) p00_des = *p00_bop;                              \
  while (P99_UNLIKELY(!atomic_compare_exchange_weak(p00_aop, p00_bop, p00_des))) \
    P99_NOP;                                                            \
 })


#define P00_FETCH_OP(OBJP, OPERAND, BUILTIN, OPERATOR)                 \
({                                                                     \
  P99_MACRO_PVAR(p00_objp, (OBJP), volatile);                          \
  P99_MACRO_VAR(p00_op, OPERAND);                                      \
  __typeof__(P00_AT(p00_objp)) p00_ret;                                \
  if (!atomic_is_lock_free(p00_objp)) {                                \
    P99_SPIN_EXCLUDE(&p00_objp->p00_lock) {                            \
      p00_ret = P00_AT(p00_objp);                                      \
      P00_AT(p00_objp) OPERATOR p00_op;                                \
    }                                                                  \
  }                                                                    \
  P99_IF_EMPTY(P99_ATOMIC_LOCK_FREE_TYPES)                             \
    (else p00_ret = P00_AT(p00_objp);)                                 \
    (else {                                                            \
      p00_ret =                                                        \
        P00_ATOMIC_TERN(p00_objp,                                      \
                        BUILTIN(&P00_AO(p00_objp),                     \
                                P00_ATOMIC_TERN(p00_objp, p00_op, 0)), \
                        P00_AT(p00_objp));                             \
    })                                                                 \
    p00_ret;                                                           \
 })

/**
 ** @brief Atomically add @a OPERAND to @a *OBJP.
 **
 ** @return the current value hidden in @a OBJP before the operation.
 **
 ** @a OPERAND must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** The base type of @a OBJP must have an operator @c +=.
 **
 ** @memberof atomic_int
 **/
#define atomic_fetch_add(OBJP, OPERAND) P00_FETCH_OP((OBJP), (OPERAND), __sync_fetch_and_add, +=)


/**
 ** @brief Atomically subtract @a OPERAND from @a *OBJP.
 **
 ** @return the current value hidden in @a OBJP before the operation.
 **
 ** @a OPERAND must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** The base type of @a OBJP must have an operator @c -=.
 **
 ** @memberof atomic_int
 **/
#define atomic_fetch_sub(OBJP, OPERAND) P00_FETCH_OP((OBJP), (OPERAND), __sync_fetch_and_sub, -=)

/**
 ** @brief Atomically do a bitwise or operation between @a OPERAND and
 ** @a *OBJP.
 **
 ** @return the current value hidden in @a OBJP before the operation.
 **
 ** @a OPERAND must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** The base type of @a OBJP must have an operator @c |=.
 **
 ** @memberof atomic_int
 **/
#define atomic_fetch_or(OBJP, OPERAND) P00_FETCH_OP((OBJP), (OPERAND), __sync_fetch_and_or, |=)

/**
 ** @brief Atomically do a bitwise and operation between @a OPERAND
 ** and @a *OBJP.
 **
 ** @return the current value hidden in @a OBJP before the operation.
 **
 ** @a OPERAND must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** The base type of @a OBJP must have an operator @c &=.
 **
 ** @memberof atomic_int
 **/
#define atomic_fetch_and(OBJP, OPERAND) P00_FETCH_OP((OBJP), (OPERAND), __sync_fetch_and_and, &=)

/**
 ** @brief Atomically do a bitwise xor operation between @a OPERAND
 ** and @a *OBJP.
 **
 ** @return the current value hidden in @a OBJP before the operation.
 **
 ** @a OPERAND must be assignment compatible with the base type of @a
 ** OBJP.
 **
 ** The base type of @a OBJP must have an operator @c ^=.
 **
 ** @see atomic_int
 ** @see _Atomic
 **/
#define atomic_fetch_xor(OBJP, OPERAND) P00_FETCH_OP((OBJP), (OPERAND), __sync_fetch_and_xor, ^=)

#define atomic_fetch_add_conditional(OBJP, OPERAND)                       \
({                                                                        \
  P99_MACRO_PVAR(p00_objp, (OBJP), volatile);                             \
  P99_MACRO_VAR(p00_op, (OPERAND));                                       \
  P99_MACRO_VAR(p00_ret, atomic_load(p00_objp), volatile);                \
  while (p00_ret) {                                                       \
    P99_MACRO_VAR(p00_des, p00_ret + p00_op);                             \
    if (atomic_compare_exchange_weak(p00_objp, &p00_ret, p00_des)) break; \
  }                                                                       \
  p00_ret;                                                                \
 })

/**
 ** @brief Protect the following block or statement as a critical
 ** section of the program.
 **
 ** Internally this uses a <code>static</code> ::atomic_flag as a
 ** spinlock, so this is an active wait.
 **
 ** @warning Such a section should only contain a handful of
 ** statements.
 **
 ** @warning Such a section should not contain preliminary exits such
 ** as @c goto, @c break, @c return, @c longjmp, or ::P99_UNWIND etc.
 **
 ** Such a critical section is only protected against threads that try
 ** to enter this same critical section. Threads may well be
 ** simultaneously be in different critical sections.
 **
 ** @see P99_SPIN_EXCLUDE to protect several critical sections against
 ** each other.
 **
 ** @see P99_MUTUAL_EXCLUDE that is more suited for larger sections.
 **/
P99_BLOCK_DOCUMENT
#define P99_CRITICAL                                                 \
P00_BLK_START                                                        \
P00_BLK_DECL_STATIC(atomic_flag, P99_LINEID(crit), ATOMIC_FLAG_INIT) \
P99_SPIN_EXCLUDE(P99_LINEID(crit))


/**
 ** @brief Return a pointer to the top element of an atomic LIFO @a L
 ** @see P99_LIFO_PUSH
 ** @see P99_LIFO_POP
 **/
P00_DOCUMENT_PERMITTED_ARGUMENT(P99_LIFO_TOP, 0)
#define P99_LIFO_TOP(L)  atomic_load(L)

/**
 ** @brief Push element @a EL into an atomic LIFO @a L
 ** @see P99_LIFO_TOP
 ** @see P99_LIFO_POP
 **/
P00_DOCUMENT_PERMITTED_ARGUMENT(P99_LIFO_PUSH, 0)
P00_DOCUMENT_PERMITTED_ARGUMENT(P99_LIFO_PUSH, 1)
#define P99_LIFO_PUSH(L, EL)                    \
({                                              \
  P99_MACRO_VAR(p00_l, (L));                    \
  P99_MACRO_VAR(p00_el, (EL));                  \
  p00_el->p99_lifo = p00_el;                    \
  atomic_swap(p00_l, &p00_el->p99_lifo);        \
 })

/**
 ** @brief Pop the top element from an atomic LIFO @a L
 **
 ** This implements a generic interface to an atomic LIFO (Last In -
 ** Last Out) data structure. To use it you just have do some
 ** preparatory declarations and add a @c p99_lifo field to your data
 ** structure:
 **
 ** @code
 ** P99_DECLARE_STRUCT(myData);
 ** P99_POINTER_TYPE(myData);
 ** P99_DECLARE_ATOMIC(myData_ptr);
 **
 ** struct myData {
 **   ...
 **   myData_ptr p99_lifo;
 **   ...
 ** };
 ** extern _Atomic(myData_ptr) head;
 ** @endcode
 **
 ** Now @c head can be used as the head of a LIFO:
 **
 ** @code
 ** myData_ptr el = P99_NEW(myData_ptr, \/\* your initializer arguments \*\/);
 ** P99_FIFO_PUSH(&head, el);
 ** ...
 ** for (myData_ptr el = P99_FIFO_POP(&head);
 **      el;
 **      el = P99_FIFO_POP(&head)) {
 **        // do something with el and then
 **        free(el);
 ** }
 ** @endcode
 **
 ** @see P99_LIFO_PUSH
 ** @see P99_LIFO_TOP
 **/
P00_DOCUMENT_PERMITTED_ARGUMENT(P99_LIFO_POP, 0)
#define P99_LIFO_POP(L)                                                 \
({                                                                      \
  P99_MACRO_VAR(p00_l, (L));                                            \
  __typeof__(P99_LIFO_TOP(p00_l)) p00_el = P99_LIFO_TOP(p00_l);         \
  if (P99_LIKELY(p00_el)) {                                             \
    while (P99_UNLIKELY(!atomic_compare_exchange_weak(p00_l, &p00_el, p00_el->p99_lifo))) P99_NOP; \
    p00_el->p99_lifo = 0;                                               \
  }                                                                     \
  p00_el;                                                               \
 })


/**
 ** @}
 **/

# endif /* P00_NO_ATOMICS */
#undef P00_NO_ATOMICS

#endif
