#ifndef _DEBUG_H
#define _DEBUG_H

#if DEBUG
#include <stdio.h>
#include <time.h>

#ifndef _MSC_VER

# define dbg_msg(level, s, args...) \
	do { fprintf(stderr, "[%ld %10s]  "s"\n", time(0), level, ##args) ; fflush(stderr); } while(0)

#else /* _MSC_VER */

# define dbg_msg(level, s, ...) \
	do { fprintf(stderr, "[%ld %10s]  "s"\n", time(0), level, __VA_ARGS__) ; fflush(stderr); } while(0)

#endif /* _MSC_VER */

#else

#ifndef _MSC_VER
# define dbg_msg(level, s, args...)
#else /* _MSC_VER */
# define dbg_msg(level, s, ...)
#endif /* _MSC_VER */

#endif /* DEBUG */

#endif /* _DEBUG_H */
