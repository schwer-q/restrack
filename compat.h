/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 *
 */

#ifndef __COMPAT_H
#define __COMPAT_H

#include <sys/types.h>

#if !defined(HAVE_GETPROGNAME)
const char	*getprogname(void);
#endif	/* HAVE_GETPROGNAME */

#if !defined(HAVE_STRLCPY)
size_t	strlcpy(char * __restrict dst, const char * __restrict src, size_t siz);
#endif	/* HAVE_STRLCPY */

#endif	/* __COMPAT_H */
