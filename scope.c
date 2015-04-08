/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
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
	scope_t *head, *scope;

	scope = xcalloc(1, sizeof(scope_t));
	scope->parent = parent;

	if (!rtrack->scopes) {
		scope->level = 1;
	        rtrack->scopes = scope;
	}
	else {
		head = rtrack->scopes;
		while (head->next)
			head = head->next;
		scope->level = head->level + 1;
		head->next = scope;
	}
	rtrack->scopelvl = scope->level;
}

void
scope_unregister(rtrack_t *rtrack, CXCursor parent)
{
	scope_t *scope, *prev = NULL;

	for (scope = rtrack->scopes; scope; /* void */) {
		if (clang_equalCursors(parent, scope->parent)) {
			if (scope->next)
				scope_unregister(rtrack, scope->next->parent);

			rtrack->scopelvl = scope->level - 1;
			free(scope);
			if (!prev)
				rtrack->scopes = NULL;
			else
				prev->next = NULL;

			printf("=> scope has changed: "
			       "destroyed scope \033[01;31m#%d\033[00m\n",
			       rtrack->scopelvl+1);

			return;
		}
		prev = scope;
		scope = scope->next;
	}
}

int
scope_unscoped(rtrack_t *rtrack, CXCursor parent)
{
	scope_t *scope;

	printf("*** unscope ***\n");
	for (scope = rtrack->scopes; scope; /* void */) {
		if (clang_equalCursors(parent, scope->parent))
			return (1);
		scope = scope->next;
	}
	return (0);
}
