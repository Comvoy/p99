/* This may look like nonsense, but it really is -*- mode: C -*-             */
/*                                                                           */
/* Except of parts copied from previous work and as explicitly stated below, */
/* the author and copyright holder for this work is                          */
/* all rights reserved,  2011 Jens Gustedt, INRIA, France                    */
/*                                                                           */
/* This file is part of the P99 project. You received this file as as        */
/* part of a confidential agreement and you may generally not                */
/* redistribute it and/or modify it, unless under the terms as given in      */
/* the file LICENSE.  It is distributed without any warranty; without        */
/* even the implied warranty of merchantability or fitness for a             */
/* particular purpose.                                                       */
/*                                                                           */
/**
 ** @brief Instantiation of the atomic operations for the different
 ** integer types.
 **
 ** @warning This file is included several times. Don't protect it by include guards.
 **/


P99_INSTANTIATE(_Bool, ORWL_MANG(atomic_compare_exchange_weak), void volatile*object, void*restrict expected, ORWL_AT desired);
P99_INSTANTIATE(void, ORWL_MANG(atomic_store), void volatile*object, ORWL_AT desired);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_fetch_add), void volatile*object, ORWL_AT operand);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_fetch_sub), void volatile*object, ORWL_AT operand);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_fetch_or), void volatile*object, ORWL_AT operand);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_fetch_xor), void volatile*object, ORWL_AT operand);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_fetch_and), void volatile*object, ORWL_AT operand);
P99_INSTANTIATE(ORWL_RT, ORWL_MANG(atomic_load), void volatile*object);