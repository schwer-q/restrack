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


// TODO :  search fopen open malloc etc ...

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include "compat.h"
#include "rtrack.h"
#include "xalloc.h"

static enum CXChildVisitResult    visitor(CXCursor cursor,
                    CXCursor parent,
                    CXClientData data);
static char    *visitor_getfilename(CXCursor cursor);

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
    scope_unregister(this, cursor);
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

    if (scope_unscoped(this, parent)) {
        printf("=> scope has unscoped\n");
        scope_unregister(this, parent);
    }

    switch (clang_getCursorKind(cursor)) {
    case CXCursor_BinaryOperator: {
	    scope_set_binop(this, 1);
	    break;
    }

    case CXCursor_CompoundStmt: {
        scope_register(this, parent);
        printf("=> scope has changed: new scope \033[01;31m#%d\033[00m\n", this->scopelvl);
        break;
    }

    case CXCursor_DeclRefExpr: {
	    if (scope_is_assign(this)) {
		    ressouce_assign(this, this->lastvar, cursor);
		    this->lastvar = clang_getNullCursor();
		    scope_set_assign(this, 0);
	    }
	    else if (scope_is_binop(this)) {
		    this->lastvar = cursor;
		    scope_set_binop(this, 0);
	    }
	    else if (scope_is_calling(this)) {
		    /* We are in a CallExpr */
		    if (clang_Cursor_isNull(this->lastfunc)) {
			    /* lastfunc has not been set yet. The cursor point to the called function */
			    this->lastfunc = cursor;
		    }
		    else {
			    ressouce_release(this, cursor, this->lastfunc);
			    this->lastfunc = clang_getNullCursor();
			    scope_set_call(this, 0);
		    }

	    }
	    else if (scope_is_returning(this)) {
		    /*
		     * XXX: Nothing...
		     */
	    }


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

    case CXCursor_CallExpr:
    {
        CXString name = clang_getCursorSpelling(cursor);
        CXType type = clang_getCursorType(cursor);
        CXString typename = clang_getTypeSpelling(type);

        printf("=> found call declaration: \033[01;36m%s\033[00m \033[32m'%s'\033[00m\n",
               clang_getCString(name), clang_getCString(typename));
	clang_disposeString(name);
        clang_disposeString(typename);

	if (ressource_is_assign(cursor)) {
		printf("==> assign function call\n");
		scope_set_assign(this, 1);
		this->lastfunc = clang_getNullCursor();
	}
	else if (ressource_is_release(cursor)) {
		printf("==> release function call\n");
		scope_set_call(this, 1);
		this->lastfunc = clang_getNullCursor();
	}
	else
		return (CXChildVisit_Continue);
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
        variable_register(this, cursor);
        break;
    }

    case CXCursor_ReturnStmt: {
	    scope_returning(this);
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
