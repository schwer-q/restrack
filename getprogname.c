/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 *
 */

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "compat.h"

#if !defined(HAVE_GETPROGNAME)

extern const char *program_invocation_name;

const char *
getprogname(void)
{
	return ((const char *)program_invocation_name);
}

#endif	/* HAVE_GETPROGNAME */
