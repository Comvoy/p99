/* This may look like nonsense, but it really is -*- mode: C -*-             */
/*                                                                           */
/* Except of parts copied from previous work and as explicitly stated below, */
/* the author and copyright holder for this work is                          */
/* (C) copyright  2011 Jens Gustedt, INRIA, France                           */
/*                                                                           */
/* This file is free software; it is part of the P99 project.                */
/* You can redistribute it and/or modify it under the terms of the QPL as    */
/* given in the file LICENSE. It is distributed without any warranty;        */
/* without even the implied warranty of merchantability or fitness for a     */
/* particular purpose.                                                       */
/*                                                                           */

/**
 ** @file
 ** @brief conformance test for C99
 **
 ** This file only tests the "compiler" part of C, that is without any
 ** include files. Any C implementation, whether hosted or
 ** freestanding should comply to this.
 **
 ** To convince your compiler to compile this you have perhaps to
 ** provide some additional parameters on the command line. E.g for
 ** the gcc family of compilers (including third party pretenders) you
 ** usually have to give "-std=c99" to switch to C99 mode. But other
 ** parameters may be in order, consult your compiler documentation.
 **
 ** This file is split into several parts that hopefully are
 ** self explaining. Each of the parts has a macro @c SKIP_... that
 ** lets you skip a test.
 **/


#ifndef SKIP_VA_ARGS_MACRO
# define FIRST(X, ...) X
# if FIRST(0, something)
#  error "The preprocessor lacks variable argument list support. Run test with -DSKIP_VA_ARGS_MACRO to be able to see the other tests."
# endif
#endif


#ifndef SKIP_BOOL
enum { hasTrue = (_Bool)127, hasFalse = (_Bool)0 };
_Bool has_Bool[1];
_Bool has_Bool[hasTrue] = { hasFalse };
#endif

#ifndef SKIP_VLA
double has_VLA_function(unsigned n, double A[n][n]) {
  double ret = 0.0;
  unsigned i, j;
  for (i = 0; i < n; ++i)
    for (j = 0; j < n; ++j)
      ret += A[i][j];
  return ret;
}
double has_VLA(unsigned n) {
  double VLA[n][n];
  return has_VLA_function(n, VLA);
}


#endif

#ifndef SKIP_INLINE
typedef void (*func)(void);

/* This alone should not result in a generation of a symbol
   "has_undefined_symbol1". It should even not have an entry as
   "undefined" in the symbol table, but such an entry would be
   tolerable. */
inline
void
has_undefined_symbol1(void) {
  /* empty */
}

/* This alone should not result in a generation of a symbol
   "undefined_symbol2" but insert an "undefined" symbol to such a
   function in the symbol table. */
inline
void
has_undefined_symbol2(void) {
  /* empty */
}
func undefined_symbol2_tester[] = { has_undefined_symbol2 };

/* This should result in a generation of a symbol has_mandatory_symbol1 */
inline
void
has_mandatory_symbol1(void) {
  /* empty */
}
extern inline
void
has_mandatory_symbol1(void);

/* This should result in a generation of a symbol has_mandatory_symbol2 */
inline
void
has_mandatory_symbol2(void) {
  /* empty */
}
func mandatory_symbol2_tester[] = { has_mandatory_symbol2 };
extern inline
void
has_mandatory_symbol2(void);

/* This should result in a generation of a symbol has_mandatory_symbol3 */
inline
void
has_mandatory_symbol3(void) {
  /* empty */
}
void
has_mandatory_symbol3(void);

/* This should result in a generation of a symbol has_mandatory_symbol4 */
inline
void
has_mandatory_symbol4(void) {
  /* empty */
}
func mandatory_symbol4_tester[] = { has_mandatory_symbol4 };
extern inline
void
has_mandatory_symbol4(void);

#endif

#ifndef SKIP_HEXDOUBLE
enum { hasHexdouble = (0x1P2 > 10) };
double has_hexdouble[hasHexdouble];
#endif

#ifndef SKIP_COMPOUND
unsigned has_compound_literal(void) {
  return (unsigned){ 0 };
}
#endif

#ifndef SKIP_INITIALIZERS
unsigned has_designated_array_initializer[4] = { [3] = 1u };
unsigned A1[] = { [3] = 1u };
unsigned has_length_from_initializer[sizeof(has_designated_array_initializer) == sizeof(A1)];
struct {
  unsigned first;
  double second;
} has_designated_struct_initializer = { .second = 1, .first = 2 };
#endif

#define GLUE2(A, B) A ## B
#define CONCAT2(A, B) GLUE2(A, B)
#define STRINGIFY(A) STRINGIFY_(A)
#define STRINGIFY_(A) #A

#ifndef SKIP_TOKEN_CONCAT
double has_concat_of_floats_1E = CONCAT2(1E, 3);
double has_concat_of_floats_1Ep = CONCAT2(1E+, 3);
/* This one should iteratively compose a double value from left to
   right. */
# ifndef SKIP_TOKEN_CONCAT_ITERATIVE
double has_concat_of_floats_iterative = CONCAT2(CONCAT2(CONCAT2(1, E), +), 3);
# endif
/* These ones are tricky. The terms inside the STRINGIFY should lead to
   valid preprocessing tokens but which are invalid tokens for the
   following phases. */
char const has_concat_of_floats_1Ep3Em[] = STRINGIFY(CONCAT2(CONCAT2(1E+, 3E-), 3));
# ifndef SKIP_TOKEN_CONCAT_HASH_HASH
char const has_concat_of_hash_hash[] = STRINGIFY(CONCAT2(#, #));
# endif
#endif

#ifndef SKIP_EXPANDS

/* expand arguments after assigment, but before call */
# define COMMA ,
# define GLUE1(X) GLUE2(X)
unsigned has_determines_macro_arguments_first[GLUE1(0 COMMA 1)];

/* Expand args before ## concatenation */
# define EATEAT 0
# define EAT 1
enum { CONCAT2(eat, GLUE2(EAT, EAT)) = GLUE2(EAT, EAT),
       CONCAT2(eat, CONCAT2(EAT, EAT)) = CONCAT2(EAT, EAT)};
unsigned has_expands_args_before_concatenation[eat11] = { eat0 };
#endif

#ifndef SKIP_TRAILING_COMMA
enum { enumConstant, } has_enum_trailing_commas;
unsigned has_initializer_trailing_commas[] = { 0, };
#endif

#ifndef SKIP_FLEXIBLE
typedef struct {
  unsigned len;
  double arr[];
} flexible;
typedef union {
  flexible flex;
  char buffer[sizeof(flexible) + 10*sizeof(double)];
} flex10;
flex10 has_flexible_array = { .flex.len = 10 };
#endif

#ifndef SKIP_RESTRICT
char restrict_buffer[25];
char *restrict has_restrict_keyword = restrict_buffer;
#endif

#ifndef SKIP_STATIC_PARAMETER
void has_static_parameter(double A[static 10]){
 /* empty */
}
#endif
#ifndef SKIP_CONST_PARAMETER
void has_const_parameter(double A[const 10]){
 /* empty */
}
#endif
#ifndef SKIP_VOLATILE_PARAMETER
void has_volatile_parameter(double A[volatile 10]){
 /* empty */
}
#endif
#ifndef SKIP_RESTRICT_PARAMETER
# ifndef SKIP_RESTRICT
void has_restrict_parameter(double A[restrict 10]){
 /* empty */
}
# endif
#endif

#ifndef SKIP_COMMENTS
enum { hasCppComment = -2
       + 3
       //* for C99 all of this is a comment to the end of the line */ 3
       /* for C89 this resolves in division by 3 */
};
unsigned has_cpp_comment[hasCppComment];
#endif

#ifndef SKIP_MIXED
unsigned has_mixed_declaration(void) {
  /* infinite recursion, but who cares */
  has_mixed_declaration();
  unsigned a = 10;
  return a;
}
#endif

#ifndef SKIP_FOR_DECLARATION
unsigned has_for_declaration(void) {
  unsigned a = 10;
  for (unsigned i = 0; i < a; ++i) {
    a -= i;
  }
  switch (0)
    for (unsigned var; 0;)
    default:
      switch((var = 0), 0)
      default: {
        return var;
      }
  return a;
}
#endif

#ifndef SKIP_IDEM
unsigned const const has_idempotent_const = 1;
unsigned volatile volatile has_idempotent_volatile = 1;
# ifndef SKIP_RESTRICT
unsigned *restrict restrict has_idempotent_restrict = 0;
# endif
#endif

#ifndef SKIP_PRAGMA
# pragma STDC FP_CONTRACT ON
# define PRAGMA(MESS) _Pragma(# MESS)
PRAGMA(STDC FP_CONTRACT OFF)
#endif

#ifndef SKIP_FUNC
void has_func_macro(char const* where[]) {
  *where = __func__;
 }
#endif

#ifndef SKIP_LONG_LONG
long long has_long_long = 0;
unsigned long long has_ullong_max[-1 + 2*((0ULL - 1) >= 18446744073709551615ULL)];
#endif

#ifndef SKIP_LONG_DOUBLE
long double has_long_double = 1.0L;
#endif

#ifndef SKIP_PREPRO_ARITH
# if (0 - 1) >= 0
#  error "this should be a negative value"
# endif
/* Unsigned arithmetic should be done in the type that corresponds to
   uintmax_t and it should use modulo arithmetic. */
# if ~0U < 0
#  error "this should be a large positive value"
# endif
# if ~0U != (0U - 1)
#  error "the preprocessor should do modulo arithmetic on unsigned values"
# endif
# if (~0U < 65535)
#  error "this should be a large positive value, at least UINT_MAX >= 2^{16} - 1"
# endif
# if (~0U < 4294967295)
#  error "this should be a large positive value, at least ULONG_MAX >= 2^{32} - 1"
# endif
# if (~0U < 18446744073709551615ULL)
#  error "this should be a large positive value, at least ULLONG_MAX >= 2^{64} - 1"
# endif
unsigned has_preprocessor_uintmax;
#endif

#ifndef SKIP_UNIVERSAL
int has_hex_character = L'\x0401';
int has_universal_character_4 = L'\u2118';
int has_universal_character_8 = L'\U00000401';
int const* has_universal_string_4 = L"\u2018\u03A7\u2060X\u2019";
int const* has_universal_string_8 = L"\U00002018\U000003A7\U00002060X\U00002019";
int has_\u03BA\u03B1\u03B8\u03BF\u03BB\u03B9\u03BA\u03CC\u03C2_\u03c7\u03B1\u03C1\u03B1\u03BA\u03C4\u03AE\u03C1 = 1;
#endif


/* Have checks for digraphs and trigraphs at the end, since they may
   mix up the pretty printing. */

#ifndef SKIP_DIGRAPH

/* check for the four "language" digraphs */
double has_punctuation_digraph <::> = <% 0 %>;

%: define HAS_HASH_DIGRAPH 1
enum { hasHashDigraph = !!HAS_HASH_DIGRAPH };
%: define DIGRAPH_STRINGIFY(X) %:X
char const has_digraph_stringify[] = DIGRAPH_STRINGIFY(digraph);
# if !defined(HAS_HASH_DIGRAPH)
#  error "The preprocessor lacks digraph %: support. Run test with -DSKIP_DIGRAPH to be able to see the other tests."
# endif

%: define HAS_HASH_HASH_DIGRAPH_(a, b) a %:%: b
%: define HAS_HASH_HASH_DIGRAPH HAS_HASH_HASH_DIGRAPH_(0, 1)
enum { hasHashHashDigraph = !!HAS_HASH_HASH_DIGRAPH};
/* This one here and the one in the trigraph section should be merged
   into one symbol. */
double has_hash_hash_interpretedCorrectly[hasHashHashDigraph];
# if !defined(HAS_HASH_HASH_DIGRAPH) || (HAS_HASH_HASH_DIGRAPH != 1)
#  error "The preprocessor lacks quadgraph %:%: support. Run test with -DSKIP_DIGRAPH to be able to see the other tests."
# endif

#endif


#ifndef SKIP_TRIGRAPH

/* Check for the eight "language" trigraphs. If this works and you
   run this through your preprocessor phase (usually with -E) this
   should, all of a sudden look like valid C code. */
int has_punctuation_trigraph ??(??) = ??< (0 ??' 1), (0 ??! 1), ??-0,  '??/0' ??>;
/* don't get confused by the syntax highlighting the ??' aint't too bad for that */

??= define HAS_HASH_TRIGRAPH 1
enum { hasHashTrigraph = !!HAS_HASH_TRIGRAPH };
??= define TRIGRAPH_STRINGIFY(X) ??=X
char const has_trigraph_stringfy[] = TRIGRAPH_STRINGIFY(trigraph);
# if !defined(HAS_HASH_TRIGRAPH)
#  error "The preprocessor lacks trigraph ??= support. Run test with -DSKIP_TRIGRAPH to be able to see the other tests."
# endif

??= define HAS_HASH_HASH_TRIGRAPH_(a, b) a ??=??= b
??= define HAS_HASH_HASH_TRIGRAPH HAS_HASH_HASH_TRIGRAPH_(0, 1)
enum { hasHashHashTrigraph = !!HAS_HASH_HASH_TRIGRAPH};
/* This one here and the one in the digraph section should be merged
   into one symbol. */
double has_hash_hash_interpretedCorrectly[hasHashHashTrigraph];
# if !defined(HAS_HASH_HASH_TRIGRAPH) || (HAS_HASH_HASH_TRIGRAPH != 1)
#  error "The preprocessor lacks hexgraph ??=??= support. Run test with -DSKIP_TRIGRAPH to be able to see the other tests."
# endif

#endif