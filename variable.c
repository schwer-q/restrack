/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
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

}
