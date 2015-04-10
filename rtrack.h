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

typedef struct ressource ressource_t;
typedef struct scope scope_t;
typedef struct variable variable_t;

struct rtrack {
	char			*filename;
	CXIndex			index;
	CXTranslationUnit	tu;

	int		scopelvl; /* current scope level */
	scope_t		*scopes;  /* scope hold in this tu */

	CXCursor	lastfunc;
	CXCursor	lastvar;
};

struct ressource {
	CXCursor	assign;	/* cursor that assigned the ressource */
	CXCursor	release; /* cursor that released the ressource */
};

struct scope {
	int		level;	/* scope level id */
	CXCursor	parent;	/* cursor parent to this scope */
	variable_t	*variables; /* variables declared in this scope */

	int		calling;   /* set to 1 if in callexpr */
	int		assigning; /* set to 1 if in callexpr */

	int		binop;
	int		returning; /* set to 1 if returning */

	scope_t		*prev;
	scope_t		*next;
};

struct variable {
	char		*name;	/* variable name */
	char		*typename; /* type name */

	CXCursor	cursor;	/* cursor reference */
	CXType		type;	/* type information */

	ressource_t	*ressource;

	variable_t	*prev;
	variable_t	*next;
};


rtrack_t *rtrack_new(const char *filename);
void	rtrack_analyse(rtrack_t *rtrack);
void	rtrack_free(rtrack_t *rtrack);

void	ressouce_assign(rtrack_t *rtrack, CXCursor varcurs, CXCursor rescurs);
void	ressouce_release(rtrack_t *rtrack, CXCursor varcurs, CXCursor rescurs);
int	ressource_is_assign(CXCursor cursor);
int	ressource_is_release(CXCursor cursor);

void	scope_register(rtrack_t *rtrack, CXCursor parent);
void	scope_unregister(rtrack_t *rtrack, CXCursor parent);
int	scope_unscoped(rtrack_t *rtrack, CXCursor parent);
void	scope_set_call(rtrack_t *rtrack, int status);
int	scope_is_calling(rtrack_t *rtrack);
void	scope_set_binop(rtrack_t *rtrack, int status);
int	scope_is_binop(rtrack_t *rtrack);
void	scope_set_assign(rtrack_t *rtrack, int status);
int	scope_is_assign(rtrack_t *rtrack);
void	scope_returning(rtrack_t *rtrack);
int	scope_is_returning(rtrack_t *rtrack);

void	variable_register(rtrack_t *rtrack, CXCursor cursor);
void	variable_unregister(rtrack_t *rtrack, scope_t *scope, CXCursor cursor);
int	variable_is_ressource(rtrack_t *rtrack, CXCursor cursor);
variable_t *variable_find(rtrack_t *rtrack, const char *vname);

#endif	/* __RTRACK_H */
