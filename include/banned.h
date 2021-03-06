#pragma once

/*
 * Header borrowed from the git git repository
 * https://github.com/git/git/blob/be7935ed8bff19f481b033d0d242c5d5f239ed50/banned.h
 */ 

#define HAVE_VARIADIC_MACROS

/*
 * This header lists functions that have been banned from our code base,
 * because they're too easy to misuse (and even if used correctly,
 * complicate audits). Including this header turns them into compile-time
 * errors.
 */

#define BANNED(func) sorry_##func##_is_a_banned_function

#undef strcpy
#define strcpy(x, y) BANNED(strcpy) // NOLINT
#undef strcat
#define strcat(x, y) BANNED(strcat) // NOLINT
#undef strncpy
#define strncpy(x, y, n) BANNED(strncpy)
#undef strncat
#define strncat(x, y, n) BANNED(strncat)

#undef sprintf
#undef vsprintf
#ifdef HAVE_VARIADIC_MACROS
#define sprintf(...) BANNED(sprintf) // NOLINT
#define vsprintf(...) BANNED(vsprintf)
#else
#define sprintf(buf, fmt, arg) BANNED(sprintf) // NOLINT
#define vsprintf(buf, fmt, arg) BANNED(vsprintf)
#endif

#undef gmtime
#define gmtime(t) BANNED(gmtime)
#undef localtime
#define localtime(t) BANNED(localtime)
#undef ctime
#define ctime(t) BANNED(ctime)
#undef ctime_r
#define ctime_r(t, buf) BANNED(ctime_r)
#undef asctime
#define asctime(t) BANNED(asctime)
#undef asctime_r
#define asctime_r(t, buf) BANNED(asctime_r)
