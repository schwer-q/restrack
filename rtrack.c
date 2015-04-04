/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include "rtrack.h"
#include "xalloc.h"

static enum CXChildVisitResult	visitor(CXCursor cursor,
					CXCursor parent,
					CXClientData data);
static char	*visitor_getfilename(CXCursor cursor);

rtrack_t *
rtrack_new(const char *filename)
{
	rtrack_t *this;

	this = xcalloc(1, sizeof(rtrack_t));
	this->filename = realpath(filename, NULL);
	this->index = clang_createIndex(1, 1);
	this->tu = clang_createTranslationUnitFromSourceFile(this->index,
							     this->filename,
							     0, NULL, 0, 0);

	return (this);
}

void
rtrack_free(rtrack_t *this)
{
	clang_disposeTranslationUnit(this->tu);
	clang_disposeIndex(this->index);
	free(this->filename);
}

void
rtrack_analyse(rtrack_t *this)
{
	CXCursor cursor;

	printf("analyzing %s...\n", this->filename);
	cursor = clang_getTranslationUnitCursor(this->tu);
	clang_visitChildren(cursor, visitor, (CXClientData)this);
}

static enum CXChildVisitResult
visitor(CXCursor cursor, CXCursor parent, CXClientData data)
{
	rtrack_t *this = (rtrack_t*)data;
	char *filename;

	/* (void)parent; */
	if ((filename = visitor_getfilename(cursor))) {
		if (strcmp(filename, this->filename))
			return (CXChildVisit_Continue);
		printf("%s\n", filename);
	}

	/*
	 * XXX: check parent whith scopes' parent
	 */
	if (scope_unscoped(this, parent)) {
		printf("=> scope has unscoped\n");
		scope_unregister(this, parent);
	}

	switch (clang_getCursorKind(cursor)) {
	case CXCursor_CompoundStmt: {
		scope_register(this, parent);
		printf("=> scope has changed: new scope \033[01;31m#%d\033[00m\n", this->scopelvl);
		break;
	}

	case CXCursor_FunctionDecl: {
		CXString name = clang_getCursorSpelling(cursor);
		printf("=> found function declaration: %s\n", clang_getCString(name));
		clang_disposeString(name);

		scope_register(this, parent);
		printf("=> scope has changed: new scope \033[01;31m#%d\033[00m\n", this->scopelvl);
		break;
	}

	case CXCursor_ParmDecl:
	case CXCursor_VarDecl:
	{
		CXString name = clang_getCursorSpelling(cursor);
		CXType type = clang_getCursorType(cursor);
		CXString typename = clang_getTypeSpelling(type);

		printf("=> found variable declaration: \033[01;36m%s\033[00m \033[32m'%s'\033[00m\n",
		       clang_getCString(name), clang_getCString(typename));
		clang_disposeString(name);
		clang_disposeString(typename);
		break;
	}

	default:
		break;
	}

	return (CXChildVisit_Recurse);
}

static char *
visitor_getfilename(CXCursor cursor)
{
	CXFile _file;
	CXSourceLocation _location;
	CXString _filename;
	static char filename[PATH_MAX];

	bzero(filename, sizeof(filename));

	_location = clang_getCursorLocation(cursor);
	clang_getFileLocation(_location, &_file, 0, 0, 0);
	if (!_file)
		return (NULL);
	_filename = clang_getFileName(_file);

	(void)strlcpy(filename, clang_getCString(_filename), PATH_MAX);
	clang_disposeString(_filename);
	return (filename);
}
