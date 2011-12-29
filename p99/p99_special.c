

/** @file
 ** @brief This file just contains automatically produced information for doxygen.
 **
 ** It will usually be overwritten for any new release.
 **/

/**
 ** @page safety Safety of macro programming and pitfalls
 **
 ** Often we may hear arguments that function like macros obfuscate
 ** the code, are dangerous, not maintainable, in short: the big
 ** evil.
 **
 ** I am convinced that this is just one of the other urban
 ** legends. Macros as such are not less safe than, say, the use of
 ** pointers or of a high quality sharpened kitchen knife.  And they
 ** belong to C as the <code>for</code> or the <code>++</code>
 ** operator.
 **
 ** Macros can make your code easier readable, better maintainable,
 ** more precise and more efficient than you ever would do by writing
 ** code "directly" without macros. It is all about how you write
 ** them, if you do that yourself, and how you use them.
 **
 ** As one example look at the following code:
 ** @code
 ** if (P99_ARE_ORDERED(<, a, b, c, d, e, f)) {
 **   // all are in order, do something special
 ** } else {
 **   // handle the base case
 ** }
 ** @endcode
 **  - This is readable: you have all the variables that are to be
 **    checked by the condition in one glance.
 **  - This is maintainable: just change a variable name in one place,
 **    deleted one or add one without problems.
 **  - This is precise and describes exactly what it does.
 **  - This is efficient since its replacement is exactly the
 **    expression that is needed for that task:
 ** @code
 ** ((a) < (b)) && ((b) < (c)) && ((c) < (d)) && ((d) < (e)) && ((e) < (f))
 ** @endcode
 ** which by itself is not very readable, difficult to maintain and
 ** not very explicit in its intentions.
 **
 ** In the example a part of the efficiency also comes from the fact
 ** that it is <em>not a function</em>. This is e.g interesting if
 ** some of the variables in the list are of integer type and the
 ** others of floating type: promotions from integer type to floating
 ** type will only be performed where necessary in a comparison of two
 ** adjacent variables. In such a way we can profit of the exact
 ** comparison for integer types and can avoid the problems of
 ** rounding an integer to the next representable double. (Think of
 ** comparing <code>e = UINT64_MAX - 1</code> and <code>e =
 ** UINT64_MAX</code>.)
 **
 ** But, macros have pitfalls, in particular <em>one</em> important
 ** pitfall: you don't see from a macro call if the arguments are
 ** evaluated multiple times or not. So if you have the habit to
 ** program with side effects, you really have to be careful.
 **
 ** The simplest really is to avoid that, don't have expressions with
 ** side effects such as <code>++</code> as arguments to macros or
 ** functions. Really, don't do that.
 **
 ** If you are programming macros, you have to be more careful since
 ** you can't assume that everybody knows what she or he is doing. For
 ** P99 we an automatic suite of scripts that tests if any of the
 ** macros that start with "P99_" evaluate their arguments multiple
 ** times. This is a bit tricky, special care has to be taken for
 ** macros that use the ternary operator <code>?:</code> and the @c
 ** sizeof operator:
 **
 **  - For the ternary operator, P99 has a lot where the condition is
 **    constant and one branch is only evaluated for its type.
 **  - <code>sizeof</code> is special because its argument is also
 **    mostly evaluated for its type and not for its value. There is
 **    one exception for that rule, variable length arrays, VLA.
 **
 ** The mentioned scripts help us detect these and other special cases
 ** and the documentation of the corresponding P99 macros is then
 ** annotated with warnings and remarks that document the special
 ** behavior of these macros.
 **
 ** @see P99_AARG(TYPE, NAME, DIM, VAR) for argument <code>TYPE</code>, <code>DIM</code>.
 ** @see P99_AASSIGN(TARGET, SOURCE, N) for argument <code>TARGET</code>, <code>SOURCE</code>.
 ** @see P99_ACALL(ARR, N, TYPE) for arguments <code>ARR</code>, <code>N</code>, <code>TYPE</code>.
 ** @see P99_ACCESSORS(X, N) for argument <code>X</code>, <code>N</code>.
 ** @see P99_ACOPY(TYPE, N, ...) for argument <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_ALEN(ARR, N) for argument <code>ARR</code>, <code>N</code>.
 ** @see P99_ALENS(ARR, N) for argument <code>ARR</code>, <code>N</code>.
 ** @see P99_ARE_EQ(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_ARE_ORDERED(OP, ...) for arguments <code>OP</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>.
 ** @see P99_BIGFUNC(FUNC, M, ...) for argument <code>M</code>.
 ** @see P99_BIGOP(OP, M, ...) for argument <code>M</code>.
 ** @see P99_CALLOC(T, N) for argument <code>T</code>.
 ** @see P99_CALL_DEFARG(NAME, M, ...) for argument <code>M</code>.
 ** @see P99_CALL_DEFARG_LIST(NAME, M, ...) for argument <code>M</code>.
 ** @see P99_CALL_VA_ARG(NAME, M, T, ...) for argument <code>M</code>.
 ** @see P99_CASERANGE(START, ...) for argument <code>START</code>, <code>__VA_ARG__[0]</code>.
 ** @see P99_CDIM(NAME, ...) for argument <code>NAME</code>.
 ** @see P99_CHOOSE5(xT, cc, cs, ci, cl, cll) for argument <code>xT</code>.
 ** @see P99_DECLARE_DELETE(...) for argument <code>__VA_ARG__[0]</code>.
 ** @see P99_DECLARE_ENUM(T, ...) for arguments <code>T</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_DECLARE_ENUM_GETNAME(T, ...) for arguments <code>T</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_DECLARE_STRUCT(NAME) for argument <code>NAME</code>.
 ** @see P99_DECLARE_UNION(NAME) for argument <code>NAME</code>.
 ** @see P99_DEC_DOUBLE(...) for arguments <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>, <code>__VA_ARG__[4]</code>.
 ** @see P99_DEFINE_UNION(NAME, ...) for arguments <code>NAME</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_DERIVED_TYPES(T) for argument <code>T</code>.
 ** @see P99_DESIGNATED(VAR, ...) for arguments <code>VAR</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_DO(TYPE, VAR, ...) for argument <code>TYPE</code>.
 ** @see P99_FCALLOC(T, F, N) for arguments <code>T</code>, <code>F</code>, <code>N</code>.
 ** @see P99_FHEAD(T, F, P) for argument <code>T</code>.
 ** @see P99_FMALLOC(T, F, N) for arguments <code>T</code>, <code>F</code>, <code>N</code>.
 ** @see P99_FOR(NAME, N, OP, FUNC, ...) for arguments <code>NAME</code>, <code>N</code>, <code>FUNC</code>.
 ** @see P99_FORALL(NAME, ...) for argument <code>NAME</code>.
 ** @see P99_FREALLOC(P, T, F, N) for arguments <code>T</code>, <code>F</code>, <code>N</code>.
 ** @see P99_FSIZEOF(T, F, N) for arguments <code>T</code>, <code>F</code>, <code>N</code>.
 ** @see P99_GUARDED_BLOCK(T, NAME, INITIAL, BEFORE, AFTER) for arguments <code>T</code>, <code>NAME</code>, <code>AFTER</code>.
 ** @see P99_HEX_DOUBLE(...) for arguments <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>, <code>__VA_ARG__[4]</code>.
 ** @see P99_HTONL(X) for argument <code>X</code>, <code>1</code>.
 ** @see P99_HTONS(X) for argument <code>X</code>, <code>1</code>.
 ** @see P99_INVARIANT(EXPR) for argument <code>EXPR</code>.
 ** @see P99_IPOW(N, X) for argument <code>N</code>, <code>X</code>.
 ** @see P99_ISSIGNED(T) for argument <code>T</code>.
 ** @see P99_IS_INF(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_IS_MAX(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_IS_MIN(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_IS_ONE(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_IS_SUP(FIRST, ...) for argument <code>FIRST</code>.
 ** @see P99_JOIN(...) for arguments <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_LCOPY(TYPE, VAR, ...) for arguments <code>TYPE</code>, <code>VAR</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_LITERAL(...) for arguments <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>, <code>__VA_ARG__[3]</code>.
 ** @see P99_MAXOF(0) for argument <code>1</code>.
 ** @see P99_MEMSET(TA, SO, N) for argument <code>SO</code>.
 ** @see P99_MEMZERO(T, TA, N) for argument <code>T</code>.
 ** @see P99_MINOF(A, B) for argument <code>A</code>, <code>B</code>.
 ** @see P99_NAME(N, NAME) for argument <code>N</code>.
 ** @see P99_NTOHL(X) for argument <code>X</code>.
 ** @see P99_NTOHS(X) for argument <code>X</code>.
 ** @see P99_PARALLEL_DO(TYPE, VAR, ...) for argument <code>TYPE</code>.
 ** @see P99_PARALLEL_FORALL(NAME, ...) for argument <code>NAME</code>.
 ** @see P99_PLAIN_TYPE(T) for argument <code>T</code>.
 ** @see P99_POINTER_TYPE(T) for argument <code>T</code>.
 ** @see P99_POSS(N) for argument <code>N</code>.
 ** @see P99_PRAGMA_DO(PRAG, TYPE, VAR, ...) for argument <code>TYPE</code>.
 ** @see P99_PRI(xT, F, LEN) for argument <code>xT</code>.
 ** @see P99_PROTOTYPE(RT, NAME, ...) for argument <code>RT</code>, <code>__VA_ARG__[0]</code>.
 ** @see P99_PZERO(X, N) for argument <code>X</code>.
 ** @see P99_REP(...) for arguments <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_REPEAT(MACRO, N) for argument <code>MACRO</code>, <code>N</code>.
 ** @see P99_SEP(MACRO, ...) for argument <code>MACRO</code>.
 ** @see P99_SEQ(MACRO, ...) for argument <code>MACRO</code>.
 ** @see P99_SER(MACRO, ...) for argument <code>MACRO</code>.
 ** @see P99_SIGNED_REPRESENTATION(T) for argument <code>T</code>.
 ** @see P99_STRDUP(...) for arguments <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_STRUCT_LITERAL(TYPE, ...) for arguments <code>TYPE</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_STRUCT_TYPE0(TYPE, ...) for arguments <code>TYPE</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>, <code>__VA_ARG__[3]</code>.
 ** @see P99_STRUCT_TYPES(TYPE, ...) for arguments <code>TYPE</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>, <code>__VA_ARG__[3]</code>.
 ** @see P99_STRUCT_UNUSE(TYPE, VAR, ...) for arguments <code>TYPE</code>, <code>VAR</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_STRUCT_USE(TYPE, VAR, ...) for arguments <code>TYPE</code>, <code>VAR</code>, <code>__VA_ARG__[0]</code>, <code>__VA_ARG__[1]</code>, <code>__VA_ARG__[2]</code>.
 ** @see P99_SWAP(_0, _1) for argument <code>_0</code>, <code>_1</code>.
 ** @see P99_TMAX(T) for argument <code>T</code>.
 ** @see P99_TMIN(T) for argument <code>T</code>.
 ** @see P99_TOKJOIN(TOK, ...) for argument <code>TOK</code>.
 ** @see P99_TO_UNSIGNED(T, MACRO) for argument <code>T</code>, <code>MACRO</code>.
 ** @see P99_TZERO(X) for argument <code>X</code>.
 ** @see P99_UNROLL(MACRO, N) for argument <code>MACRO</code>, <code>N</code>.
 ** @see P99_UT_MAX(T) for argument <code>T</code>.
 ** @see P99_VASSIGNS(NAME, ...) for argument <code>NAME</code>.
 **/
