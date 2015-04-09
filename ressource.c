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
#include <clang-c/CXString.h>

#include "rtrack.h"
#include "xalloc.h"

static const char *assign_func[] = {
	"calloc",
	"fopen",
	"malloc",
	"mmap",
	"open",
	"realloc",
	"socket",
	"strdup",
	"xcalloc",
	"xmalloc",
	"xopen",
	"xrealloc",
	"xstrdup",
	NULL
};

static const char *release_func[] = {
	"close",
	"fclose",
	"free",
	"munmap",
	NULL
};

void
ressouce_assign(rtrack_t *rtrack, CXCursor varcurs, CXCursor rescurs)
{
	variable_t *var;
	CXString funcname;

	for (var = rtrack->scopes->variables; var->next; /* void */) {
		if (clang_equalCursors(var->cursor, varcurs))
			break;
		var = var->next;
	}

	var->ressource = xcalloc(1, sizeof(ressource_t));
	var->ressource->assign = rescurs;
	funcname = clang_getCursorSpelling(rescurs);

	printf("=> ressource assigned (%s) to %s\n",
	       clang_getCString(funcname), var->name);
	clang_disposeString(funcname);
}

void
ressouce_release(rtrack_t *rtrack, CXCursor varcurs, CXCursor rescurs)
{
	variable_t *var;
	CXString funcname;

	for (var = rtrack->scopes->variables; var->next; /* void */) {
		if (clang_equalCursors(var->cursor, varcurs))
			break;
		var = var->next;
	}

	var->ressource = xcalloc(1, sizeof(ressource_t));
	var->ressource->release = rescurs;
	funcname = clang_getCursorSpelling(rescurs);

	printf("=> ressource released (%s) from %s\n",
	       clang_getCString(funcname), var->name);
	clang_disposeString(funcname);
}

int
ressource_is_assign(CXCursor cursor)
{
	CXString funcname;
	char *name;
	int idx;

	funcname = clang_getCursorSpelling(cursor);
	name = clang_getCString(funcname);

	for (idx = 0; assign_func[idx]; ++idx)
		if (!strcmp(name, assign_func[idx]))
			break;
	if (!assign_func[idx])
		return (0);

	clang_disposeString(cursor);
	return (1);
}

int
ressource_is_release(CXCursor cursor)
{
	CXString funcname;
	char *name;
	int idx;

	funcname = clang_getCursorSpelling(cursor);
	name = clang_getCString(funcname);

	for (idx = 0; release_func[idx]; ++idx)
		if (!strcmp(name, release_func[idx]))
			break;
	if (!release_func[idx])
		return (0);

	clang_disposeString(cursor);
	return (1);
}
