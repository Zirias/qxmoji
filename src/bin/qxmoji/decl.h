#ifndef QXMOJI_DECL_H
#define QXMOJI_DECL_H

#define QXMOJIVERSTR "0.1"

#undef qxmoji___cdecl
#undef SOEXPORT
#undef SOLOCAL
#undef DECLEXPORT

#define ATTR_ACCESS(x)
#define ATTR_ALLOCSZ(x)
#define ATTR_CONST
#define ATTR_FALLTHROUGH
#define ATTR_FORMAT(x)
#define ATTR_MALLOC
#define ATTR_NONNULL(x)
#define ATTR_NORETURN
#define ATTR_RETNONNULL
#define ATTR_PURE
#define CMETHOD

#ifdef __cplusplus
#  define qxmoji___cdecl extern "C"
#  define DECLDATA
#  define C_CLASS_DECL(t) struct t
#else
#  define qxmoji___cdecl
#  define DECLDATA extern
#  define C_CLASS_DECL(t) typedef struct t t
#endif

#if defined __has_attribute
#  if __has_attribute (access)
#    undef ATTR_ACCESS
#    define ATTR_ACCESS(x) __attribute__ ((access x))
#  endif
#  if __has_attribute (alloc_size)
#    undef ATTR_ALLOCSZ
#    define ATTR_ALLOCSZ(x) __attribute__ ((alloc_size x))
#  endif
#  if __has_attribute (const)
#    undef ATTR_CONST
#    define ATTR_CONST __attribute__ ((const))
#  endif
#  if __has_attribute (fallthrough)
#    undef ATTR_FALLTHROUGH
#    define ATTR_FALLTHROUGH __attribute__ ((fallthrough))
#  endif
#  if __has_attribute (format)
#    undef ATTR_FORMAT
#    define ATTR_FORMAT(x) __attribute__ ((format x))
#  endif
#  if __has_attribute (malloc)
#    undef ATTR_MALLOC
#    define ATTR_MALLOC __attribute__ ((malloc))
#  endif
#  if __has_attribute (nonnull)
#    undef ATTR_NONNULL
#    undef CMETHOD
#    define ATTR_NONNULL(x) __attribute__ ((nonnull x))
#    define CMETHOD __attribute__ ((nonnull (1)))
#  endif
#  if __has_attribute (noreturn)
#    undef ATTR_NORETURN
#    define ATTR_NORETURN __attribute__ ((noreturn))
#  endif
#  if __has_attribute (returns_nonnull)
#    undef ATTR_RETNONNULL
#    define ATTR_RETNONNULL __attribute__ ((returns_nonnull))
#  endif
#  if __has_attribute (pure)
#    undef ATTR_PURE
#    define ATTR_PURE __attribute__ ((pure))
#  endif
#  if __has_attribute (visibility)
#    define SOEXPORT qxmoji___cdecl __attribute__((visibility("default")))
#    define SOLOCAL __attribute__((visibility("hidden")))
#  else
#    define SOEXPORT qxmoji___cdecl
#    define SOLOCAL
#  endif
#endif
#define DECLEXPORT qxmoji___cdecl

#if defined(__clang__)
#  define qxmoji___compiler clang
#elif defined(__GNUC__)
#  define qxmoji___compiler GCC
#endif
#ifdef qxmoji___compiler
#  define qxmoji___pragma(x) _Pragma(#x)
#  define qxmoji___diagprag1(x,y) qxmoji___pragma(x diagnostic y)
#  define qxmoji___diagprag(x) qxmoji___diagprag1(qxmoji___compiler, x)
#  define qxmoji___suppress1(x) qxmoji___diagprag(ignored x)
#  define qxmoji___suppress(x) qxmoji___suppress1(#x)
#  define SUPPRESS(x) qxmoji___diagprag(push) qxmoji___suppress(-W##x)
#  define ENDSUPPRESS qxmoji___diagprag(pop)
#else
#  define SUPPRESS(x)
#  define ENDSUPPRESS
#endif

#endif
