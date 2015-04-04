/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 */

#ifndef __RTRACK_H
#define __RTRACK_H

#include <clang-c/Index.h>

typedef struct rtrack rtrack_t;
typedef struct scope scope_t;

struct rtrack {
	char			*filename;
	CXIndex			index;
	CXTranslationUnit	tu;

	int	scopelvl;
	scope_t	*scope;
};

struct scope {
	int		level;
	CXCursor	parent;

	scope_t		*next;
};

rtrack_t	*rtrack_new(const char *filename);
void		rtrack_analyse(rtrack_t *rtrack);
void		rtrack_free(rtrack_t *rtrack);

void	scope_register(rtrack_t *rtrack, CXCursor parent);
void	scope_unregister(rtrack_t *rtrack, CXCursor parent);
int	scope_unscoped(rtrack_t *rtrack, CXCursor parent);

#endif	/* __RTRACK_H */
