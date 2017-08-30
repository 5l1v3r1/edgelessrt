#ifndef _OE_DEFS_H
#define _OE_DEFS_H

#if !defined(_MSC_VER) && !defined(__GNUC__)
# error "Unsupported platform"
#endif

#if defined(__GNUC__) && (__GNUC__ >= 4)
# define OE_PRINTF_FORMAT(N,M) __attribute__((format(printf, N, M)))
#else
# define OE_PRINTF_FORMAT(N,M) /* empty */
#endif

#define OE_DEPRECATED(MSG) __attribute__((deprecated(MSG)))

#ifdef _MSC_VER
# define OE_INLINE static __inline
#elif __GNUC__
# define OE_INLINE static __inline__
#endif

#if defined(__cplusplus)
# define OE_EXTERNC extern "C"
# define OE_EXTERNC_BEGIN extern "C" {
# define OE_EXTERNC_END }
#else
# define OE_EXTERNC
# define OE_EXTERNC_BEGIN
# define OE_EXTERNC_END
#endif

#ifdef __GNUC__
# define OE_EXPORT __attribute__((visibility("default")))
#else
# error "implement"
#endif

#ifdef __GNUC__
# define OE_PACKED __attribute__((packed))
#else
# error "pack unimplemented"
#endif

#ifdef __GNUC__
# define OE_ALIGNED(BYTES) __attribute__((aligned(BYTES)))
#else
# error "aligned unimplemented"
#endif

#define __OE_CONCAT(X,Y) X##Y
#define OE_CONCAT(X,Y) __OE_CONCAT(X,Y)

#define OE_UNUSED __attribute__((unused))

#define OE_CHECK_SIZE(N, M) \
    typedef unsigned char  \
    OE_CONCAT(__OE_CHECK_SIZE, __LINE__)[N==M?1:-1] OE_UNUSED

#define OE_STATIC_ASSERT(COND) \
    typedef unsigned char  \
    OE_CONCAT(__OE_STATIC_ASSERT, __LINE__)[COND?1:-1] OE_UNUSED

#define OE_TRACE printf("OE_TRACE: %s(%u): %s()\n", \
    __FILE__, __LINE__, __FUNCTION__)

OE_STATIC_ASSERT(sizeof(long) == sizeof(long long));

OE_STATIC_ASSERT(sizeof(long) == sizeof(void*));

#endif /* _OE_DEFS_H */