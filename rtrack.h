/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef __RTRACK_H
#define __RTRACK_H

#include <clang-c/Index.h>

typedef struct rtrack rtrack_t;
typedef struct scope scope_t;
typedef struct variable variable_t;

struct rtrack {
	char			*filename;
	CXIndex			index;
	CXTranslationUnit	tu;

	int		scopelvl;
	scope_t		*scopes;
};

struct scope {
	int		level;
	CXCursor	parent;
	variable_t	*variables;

	scope_t		*next;
};

struct variable {
	char		*name;
	char		*typename;

	CXCursor	cursor;
	CXType		type;

	variable_t	*prev;
	variable_t	*next;
};


rtrack_t	*rtrack_new(const char *filename);
void		rtrack_analyse(rtrack_t *rtrack);
void		rtrack_free(rtrack_t *rtrack);

void	scope_register(rtrack_t *rtrack, CXCursor parent);
void	scope_unregister(rtrack_t *rtrack, CXCursor parent);
int	scope_unscoped(rtrack_t *rtrack, CXCursor parent);

#endif	/* __RTRACK_H */
