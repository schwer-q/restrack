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
