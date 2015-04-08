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

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include "rtrack.h"
#include "xalloc.h"

void
variable_register(rtrack_t *rtrack, CXCursor cursor)
{
	CXString name;
	CXString typename;
	scope_t *scope;
	variable_t *var, *tmp;

	var = xcalloc(1, sizeof(variable_t));
	name = clang_getCursorSpelling(cursor);
	var->type = clang_getCursorType(cursor);
	typename = clang_getTypeSpelling(var->type);

	var->name = xstrdup(clang_getCString(name));
	var->typename = xstrdup(clang_getCString(typename));

	clang_disposeString(name);
	clang_disposeString(typename);

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;

	if (!scope->variables)
	        scope->variables = var;
	else {
		for (tmp = scope->variables; tmp->next; /* void */)
			tmp = tmp->next;
		tmp->next = var;
		var->prev = tmp;
	}
}


void
variable_unregister(rtrack_t *rtrack, CXCursor cursor)
{
	scope_t *scope;
	variable_t *var;

	for (scope = rtrack->scopes; scope->next; /* void */)
		scope = scope->next;

	for (var = scope->variables;
	     clang_equalCursors(var->cursor, cursor); /* void */)
		var = var->next;
	if (var->next)
		variable_unregister(rtrack, var->next->cursor);

	free(var->name);
	free(var->typename);
	var->prev->next = NULL;
	free(var);
}
