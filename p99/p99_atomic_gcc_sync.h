/* This may look like nonsense, but it really is -*- mode: C -*-              */
/*                                                                            */
/* Except for parts copied from previous work and as explicitly stated below, */
/* the author and copyright holder for this work is                           */
/* (C) copyright  2012, 2014 Jens Gustedt, INRIA, France                      */
/*                                                                            */
/* This file is free software; it is part of the P99 project.                 */
/* You can redistribute it and/or modify it under the terms of the QPL as     */
/* given in the file LICENSE. It is distributed without any warranty;         */
/* without even the implied warranty of merchantability or fitness for a      */
/* particular purpose.                                                        */
/*                                                                            */
#ifndef P99_ATOMIC_GCC_SYNC_H
#define P99_ATOMIC_GCC_SYNC_H 1

#ifndef P99_ATOMIC_H
# warning "never include this file directly, use p99_atomic.h, instead"
#endif

#include "p99_args.h"

/**
 ** @file
 **
 ** @brief Implement some of the atomic operations as fallback to gcc'
 ** @c __sync builtins.
 **/


#define P00_ATOMIC_EXCHANGE_DECLARE(T, N)                             \
p99_inline                                                            \
T P99_PASTE(p00_atomic_exchange_, N)(T volatile * p00_p, T p00_des) { \
  T p00_ret = *p00_p;                                                 \
  for (;;) {                                                          \
    T p00_val = __sync_val_compare_and_swap(p00_p, p00_ret, p00_des); \
    if (P99_LIKELY(p00_val == p00_ret)) break;                        \
    p00_ret = p00_val;                                                \
  }                                                                   \
  return p00_ret;                                                     \
}                                                                     \
P99_MACRO_END(P00_ATOMIC_EXCHANGE_DECLARE)

P00_ATOMIC_EXCHANGE_DECLARE(uint8_t, 1);
P00_ATOMIC_EXCHANGE_DECLARE(uint16_t, 2);
P00_ATOMIC_EXCHANGE_DECLARE(uint32_t, 4);
P00_ATOMIC_EXCHANGE_DECLARE(uint64_t, 8);

#define p00_mfence(...) __sync_synchronize()

#define p00_sync_lock_release_(OBJ, ORD, ...)                           \
p99_extension ({                                                        \
  p00_atomic_flag* p00_obj = (OBJ);                                     \
  /* __sync_lock_release only has release consistency */                \
  /* the fence must come before so nothing can be reordered after */    \
  switch (ORD) {                                                        \
  case memory_order_consume: ;                                          \
  case memory_order_acquire: ;                                          \
  case memory_order_acq_rel: ;                                          \
  case memory_order_seq_cst: ;                                          \
    p00_mfence();                                                       \
  }                                                                     \
  __sync_lock_release(p00_obj);                                         \
 })

#define p00_sync_lock_test_and_set_(OBJ, ORD, ...)                      \
p99_extension ({                                                        \
  p00_atomic_flag* p00_obj = (OBJ);                                     \
  /* __sync_lock_test_and_set only has acquire consistency */           \
  int ret = __sync_lock_test_and_set(p00_obj, 1);                       \
  /* the fence must come after so nothing can be reordered before */    \
  switch (ORD) {                                                        \
  case memory_order_consume: ;                                          \
  case memory_order_release: ;                                          \
  case memory_order_acq_rel: ;                                          \
  case memory_order_seq_cst: ;                                          \
    p00_mfence();                                                       \
  }                                                                     \
  ret = ret;                                                            \
 })

#define p00_sync_lock_release(...) p00_sync_lock_release_(__VA_ARGS__, memory_order_seq_cst, )
#define p00_sync_lock_test_and_set(...) p00_sync_lock_test_and_set_(__VA_ARGS__, memory_order_seq_cst, )


#endif