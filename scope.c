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

#include <stdio.h>
#include <stdlib.h>

#include <clang-c/Index.h>

#include "rtrack.h"
#include "xalloc.h"

void
scope_register(rtrack_t *rtrack, CXCursor parent)
{
	scope_t *scope, *tmp;

	scope = xcalloc(1, sizeof(scope_t));
	scope->parent = parent;

	if (!rtrack->scopes) {
		scope->level = 1;
	        rtrack->scopes = scope;
	}
	else {
		for (tmp = rtrack->scopes; tmp->next; /* void */)
			tmp = tmp->next;
		scope->level = tmp->level + 1;
	        tmp->next = scope;
		scope->prev = tmp;
	}
	rtrack->scopelvl = scope->level;
}

void
scope_unregister(rtrack_t *rtrack, CXCursor parent)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope; /* void */) {
		if (clang_equalCursors(parent, scope->parent)) {
			if (scope->next)
				scope_unregister(rtrack, scope->next->parent);

			rtrack->scopelvl = scope->level - 1;
			if (scope->variables)
				variable_unregister(rtrack, scope,
						    scope->variables->cursor);
			if (!scope->prev)
				rtrack->scopes = NULL;
			else
				scope->prev->next = NULL;
			free(scope);

			printf("=> scope has changed: "
			       "destroyed scope \033[01;31m#%d\033[00m\n",
			       rtrack->scopelvl+1);

			return;
		}
		scope = scope->next;
	}
}

int
scope_unscoped(rtrack_t *rtrack, CXCursor parent)
{
	scope_t *scope;

	/* printf("*** unscope ***\n"); */
	for (scope = rtrack->scopes; scope; /* void */) {
		if (clang_equalCursors(parent, scope->parent))
			return (1);
		scope = scope->next;
	}
	return (0);
}

void
scope_set_call(rtrack_t *rtrack, int status)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	scope->calling = status;
	/* scope->caller = clang_getNullCursor(); */
}

int
scope_is_calling(rtrack_t *rtrack)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	return (scope->calling);
}

void
scope_set_assign(rtrack_t *rtrack, int status)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	scope->assigning = status;
	/* scope->caller = clang_getNullCursor(); */
}

int
scope_is_assign(rtrack_t *rtrack)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	return (scope->assigning);
}

void
scope_set_binop(rtrack_t *rtrack, int status)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	scope->binop = status;
	/* scope->caller = clang_getNullCursor(); */
}

int
scope_is_binop(rtrack_t *rtrack)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	return (scope->binop);
}

void
scope_returning(rtrack_t *rtrack)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	scope->returning = 1;
	printf("=> scope \033[01;31m#%d\033[00m is returning\n",
	       scope->level);
}

int
scope_is_returning(rtrack_t *rtrack)
{
	scope_t *scope;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;
	return (scope->returning);
}
