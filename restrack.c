/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 */

#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <clang-c/Index.h>
#include <clang-c/CXString.h>

#include "queue.h"
#include "restrack.h"

static enum CXChildVisitResult visitor(CXCursor cursor, CXCursor parent, CXClientData data);
static inline enum CXChildVisitResult visitor_assign(CXCursor cursor, CXCursor parent, CXClientData data);
static inline enum CXChildVisitResult visitor_call(CXCursor cursor, CXCursor parent, CXClientData data);
static inline enum CXChildVisitResult visitor_return(CXCursor cursor, CXCursor parent, CXClientData data);

static void analyse(restrack_t *restrack);

static function_t *function_create(CXCursor cursor);
static void function_destroy(function_t *function);
static void function_enter(restrack_t *restrack, CXCursor cursor);
static void function_exit(restrack_t *restrack);

static scope_t *scope_create(scope_t *parent);
static void scope_destroy(scope_t *scope);
static void scope_enter(restrack_t *restrack);
static void scope_exit(restrack_t *restrack);
static variable_t *scope_getVariable(restrack_t *restrack, CXCursor cursor);

static variable_t *variable_create(CXCursor cursor);
static void variable_destroy(variable_t *variable);
static void variable_register(restrack_t *restrack, CXCursor cursor);

static const char *allocators[] = {
	"calloc",
	"fopen",
	"malloc",
	"mmap",
	"open",
	"realloc",
	"socket",
	"strdup",
	"xcalloc",
	"xfopen",
	"xmalloc",
	"xopen",
	"xrealloc",
	"xstrdup",
	NULL
};

static const char *deallocators[] = {
	"close",
	"fclose",
	"free",
	"munmap",
	NULL
};

static int isFunc = 0;

int
main(int argc, char **argv)
{
	CXCursor cursor;
	CXIndex index;
	CXTranslationUnit tu;
	int idx;
	restrack_t *restrack;

	index = clang_createIndex(1, 1);
	for (idx = 1; idx < argc; ++idx) {
		printf("parsing %s...\n", argv[idx]);
		tu = clang_createTranslationUnitFromSourceFile(index, argv[idx],
							       0, NULL, 0, 0);
		cursor = clang_getTranslationUnitCursor(tu);
		restrack = calloc(1, sizeof(restrack_t));
		STAILQ_INIT(&(restrack->scopes));
		scope_enter(restrack);
		STAILQ_INIT(&(restrack->functions));

		clang_visitChildren(cursor, visitor, (CXClientData)restrack);

		scope_exit(restrack);
		analyse(restrack);
		/* XXX: analysis */
		printf("\nhum...\n"
		       "you feel to me a pretty bad coder...\n"
		       "I have sent all your data to the UNIX black hole...\n");
		/* printf("take an oxygen mask and go to /dev/null\n"); */

		free(restrack);
		clang_disposeTranslationUnit(tu);
	}
	clang_disposeIndex(index);
	return (0);
}

static void
analyse(restrack_t *restrack)
{
	CXFile file;
	scope_t *scope;
	unsigned int column, line, offset;
	variable_t *var;

	STAILQ_FOREACH(scope, &(restrack->scopes), childs) {
		SLIST_FOREACH(var, &(scope->variables), others) {
			if (var->allocated == 1 &&
			    var->deallocated == 0 &&
			    var->returned == 0) {
				clang_getSpellingLocation(clang_getCursorLocation(var->cursor),
							  &file, &line, &column, &offset);
				printf("==> %s:%u:%u: \033[01;36m%s\033[00m: "
				       "\033[01;31mstill allocated when it was destroyed\033[00m\n",
				       clang_getCString(clang_getFileName(file)), line, column,
				       clang_getCString(clang_getCursorSpelling(var->cursor)));
			}
		}
	}
}

static enum CXChildVisitResult
visitor(CXCursor cursor, CXCursor parent __attribute__((unused)), CXClientData data)
{
	restrack_t *restrack;
	CXCursor origin;
	variable_t *var;

	restrack = (restrack_t *)data;
	switch (clang_getCursorKind(cursor)) {
	case CXCursor_BinaryOperator: {
		clang_visitChildren(cursor, visitor_assign, (CXClientData)restrack);
		return (CXChildVisit_Continue);
		break;
	}

	case CXCursor_CallExpr: {
		printf("=> CallExpr\n");
		isFunc = 1;
		clang_visitChildren(cursor, visitor_call, (CXClientData)restrack);
		return (CXChildVisit_Continue);
		break;
	}

	case CXCursor_CompoundStmt: {
		scope_enter(restrack);
		clang_visitChildren(cursor, visitor, (CXClientData)restrack);
		scope_exit(restrack);
		return (CXChildVisit_Continue);
		break;
	}

	case CXCursor_DeclRefExpr: {
		origin = clang_getCursorReferenced(cursor);

		if (restrack->current->returning == 1) {
			function_t *func;

			func = restrack->current_function;
			if (clang_getCursorKind(origin) == CXCursor_VarDecl) {
				var = scope_getVariable(restrack, origin);
				if (var->allocated == 1)
					func->allocator = 1;
				var->returned = 1;
			}
		}
		break;
	}

	case CXCursor_FunctionDecl: {
		function_enter(restrack, cursor);
		clang_visitChildren(cursor, visitor, (CXClientData)restrack);
		function_exit(restrack);

		return (CXChildVisit_Continue);
		break;
	}

	case CXCursor_IntegerLiteral: {
		printf("=> IntegerLiteral\n");
		break;
	}

	case CXCursor_ParmDecl: {
		variable_register(restrack, cursor);
		break;
	}

	case CXCursor_ReturnStmt: {
		printf("=> ReturnStmt\n");
		restrack->current->returning = 1;
		clang_visitChildren(cursor, visitor_return, (CXClientData)restrack);
		return (CXChildVisit_Continue);
		break;
	}

	case CXCursor_VarDecl: {
		variable_register(restrack, cursor);
		break;
	}

	default:
		break;
	}

	return (CXChildVisit_Recurse);
}

static inline enum CXChildVisitResult
visitor_assign(CXCursor cursor, CXCursor parent __attribute__((unused)), CXClientData data)
{
	/* static CXCursor rvalue = clang_getNullCursor(); */
	CXCursor origin;
	CXString name;
	function_t *function;
	int idx;
	restrack_t *restrack;
	static CXCursor lvalue;
	static int islvalue = 1;
	variable_t *var, *var1;

	restrack = (restrack_t *)data;
	switch (clang_getCursorKind(cursor)) {
	case CXCursor_BinaryOperator: {
		printf("hum...\n"
		       "it seems you are using somethings that I do not support yet...\n"
		       "from now on I will display data that I will get from /dev/random\n"
		       "blame yourself if your new smartphone starts smoking the cigar\n");
		break;
	}

	case CXCursor_DeclRefExpr: {
		origin = clang_getCursorReferenced(cursor);
		if (islvalue == 1) {
			lvalue = origin;
			islvalue = 0;
		}
		else {
		        var = scope_getVariable(restrack, lvalue);
			switch (clang_getCursorKind(origin)) {
			case CXCursor_FunctionDecl: {
				name = clang_getCursorSpelling(origin);
				for (idx = 0; allocators[idx] != NULL; ++idx) {
					if (strcmp(allocators[idx], clang_getCString(name)) == 0 && var != NULL) {
						printf("==> \033[01massigning %s to %s\033[00m\n",
						       clang_getCString(name),
						       clang_getCString(clang_getCursorSpelling(lvalue)));
						var->allocated = 1;
						break;
					}
				}
				STAILQ_FOREACH(function, &(restrack->functions), others) {
					if (clang_equalCursors(function->cursor, origin) && function->allocator == 1) {
						printf("==> \033[01massigning %s to %s\033[00m\n",
						       clang_getCString(name),
						       clang_getCString(clang_getCursorSpelling(lvalue)));
						var->allocated = 1;
						break;
					}
				}
			}
			case CXCursor_VarDecl: {
				var1 = scope_getVariable(restrack, lvalue);
				if (var1 != NULL && var1->allocated == 1)
					var->allocated = 1;
			}
			default:
				break;
			}
			islvalue = 1;
		}
		break;
	}

	case CXCursor_IntegerLiteral: {
		break;
	}
	default:
		break;
	}

	return (CXChildVisit_Recurse);
}

static inline enum CXChildVisitResult
visitor_call(CXCursor cursor, CXCursor parent __attribute__((unused)), CXClientData data)
{
	CXCursor origin;
	CXString name;
	restrack_t *restrack;
	static CXCursor func;
	variable_t *var, *var1;
	int idx;
	function_t *function;

	var = NULL;
	restrack = (restrack_t *)data;
	switch (clang_getCursorKind(cursor)) {
	case CXCursor_DeclRefExpr: {
		if (isFunc == 1) {
			func = clang_getCursorReferenced(cursor);
			isFunc = 0;
		}
		else {
		        origin = clang_getCursorReferenced(cursor);
			var = scope_getVariable(restrack, origin);
			name = clang_getCursorSpelling(func);
			for (idx = 0; deallocators[idx] != NULL; ++idx) {
				if (strcmp(deallocators[idx], clang_getCString(name)) == 0 &&
				    var != NULL /* && var->allocated == 1 */) {
					printf("==> \033[01mdeassigning %s using %s\033[00m\n",
					       clang_getCString(clang_getCursorSpelling(var->cursor)),
					       clang_getCString(clang_getCursorSpelling(func)));
					var->deallocated = 1;
					break;
				}
			}
			STAILQ_FOREACH(function, &(restrack->functions), others) {
				if (clang_equalCursors(func, function->cursor) && function->deallocator == 1 &&
					var != NULL /* && var->allocated == 1 */) {
					printf("==> \033[01mdeassigning %s using %s\033[00m\n",
					       clang_getCString(clang_getCursorSpelling(var->cursor)),
					       clang_getCString(clang_getCursorSpelling(func)));
					var->deallocated = 1;
					break;
				}
			}
		}
	}

	default:
		break;
	}

	if (var != NULL && var->deallocated == 1) {
		SLIST_FOREACH(var1, &(restrack->current_function->scope->variables), others) {
			if (clang_equalCursors(var1->cursor, var->cursor)) {
				restrack->current_function->deallocator = 1;
				break;
			}
		}
	}

	return (CXChildVisit_Recurse);
}

static inline enum CXChildVisitResult
visitor_return(CXCursor cursor, CXCursor parent __attribute__((unused)), CXClientData data)
{
	CXCursor origin;
	CXString name;
	function_t *func;
	int idx;
	restrack_t *restrack;
	variable_t *var;

	restrack = (restrack_t *)data;
	switch (clang_getCursorKind(cursor)) {
	case CXCursor_DeclRefExpr:
		break;
	default: {
		return (CXChildVisit_Recurse);
		break;
	}
	}

	origin = clang_getCursorReferenced(cursor);
	switch (clang_getCursorKind(origin)) {
	case CXCursor_FunctionDecl: {
		for (idx = 0; allocators[idx] != NULL; ++idx) {
			name = clang_getCursorSpelling(origin);
			if (strcmp(allocators[idx], clang_getCString(clang_getCursorSpelling(origin))) == 0) {
				restrack->current_function->allocator = 1;
				return (CXChildVisit_Recurse);
			}
		}
		STAILQ_FOREACH(func, &(restrack->functions), others) {
			if (clang_equalCursors(func->cursor, origin)) {
				func->allocator = 1;
				return (CXChildVisit_Recurse);
			}
		}

		break;
	}
	case CXCursor_VarDecl: {
		var = scope_getVariable(restrack, origin);
		if (var != NULL && var->allocated == 1) {
			var->returned = 1;
			restrack->current_function->allocator = 1;
		}
	}
	default:
		break;
	}

	return (CXChildVisit_Recurse);
}

static function_t *
function_create(CXCursor cursor)
{
	function_t *function;

	function = calloc(1, sizeof(function_t));
	if (function == NULL)
		errx(1, "fatal error: Not enough memory");
	function->cursor = cursor;
	return (function);
}

static void
function_destroy(function_t *function)
{
	free(function);
}

static void
function_enter(restrack_t *restrack, CXCursor cursor)
{
	function_t *function;
	CXString fname;

	fname = clang_getCursorSpelling(cursor);
	printf("entering function `%s'...\n", clang_getCString(fname));
	clang_disposeString(fname);
	function = function_create(cursor);
	STAILQ_INSERT_TAIL(&(restrack->functions), function, others);
	restrack->current_function = function;
	scope_enter(restrack);
	function->scope = restrack->current;
}

static void
function_exit(restrack_t *restrack)
{
	CXString name;

	scope_exit(restrack);
	name = clang_getCursorSpelling(restrack->current_function->cursor);
	printf("exiting function `%s'...\n", clang_getCString(name));
	restrack->current_function = NULL;
	clang_disposeString(name);
}

static scope_t *
scope_create(scope_t *parent)
{
	scope_t *scope;

	scope = calloc(1, sizeof(scope_t));
	if (scope == NULL)
		errx(1, "fatal error: Out of memory");
        scope->parent = parent;
	SLIST_INIT(&(scope->variables));
	return (scope);
}

static void
scope_destroy(scope_t *scope)
{
	variable_t *variable;

	while (!SLIST_EMPTY(&(scope->variables))) {
		variable = SLIST_FIRST(&(scope->variables));
		SLIST_REMOVE_HEAD(&(scope->variables), others);
		variable_destroy(variable);
	}
	free(scope);
}

static void
scope_enter(restrack_t *restrack)
{
	scope_t *scope;

	printf("entering in a new scope...\n");
	scope = scope_create(restrack->current);
	STAILQ_INSERT_TAIL(&(restrack->scopes), scope, childs);
	restrack->current = scope;
}

static void
scope_exit(restrack_t *restrack)
{
	printf("exiting scope...\n");
	restrack->current = restrack->current->parent;
}

static variable_t *
scope_getVariable(restrack_t *restrack, CXCursor cursor)
{
	scope_t *scope;
	variable_t *variable;
	CXFile file;
	unsigned int column, line, offset;

	for (scope = restrack->current; scope != NULL; /* void */) {
		SLIST_FOREACH(variable, &(scope->variables), others) {
			if (clang_equalCursors(variable->cursor, cursor)) {
				if (variable->allocated == 1 && variable->deallocated == 1) {
					clang_getSpellingLocation(clang_getCursorLocation(cursor),
								  &file, &line, &column, &offset);
					printf("==> %s:%u:%u: \033[01;36m%s\033[00m: "
					       "\033[01;31muse while it has been freed\033[00m\n",
					       clang_getCString(clang_getFileName(file)), line, column,
					       clang_getCString(clang_getCursorSpelling(variable->cursor)));
				}
				return (variable);
			}
		}
		scope = scope->parent;
	}
	return (NULL);
}

static variable_t *
variable_create(CXCursor cursor)
{
	variable_t *variable;

	variable = calloc(1, sizeof(variable_t));
	if (variable == NULL)
		errx(1, "fatal error: Not enough memory");
	variable->cursor = cursor;
	STAILQ_INIT(&(variable->useafterfree));
	return (variable);
}

static void
variable_destroy(variable_t *variable)
{
	free(variable);
}

static void
variable_register(restrack_t *restrack, CXCursor cursor)
{
	variable_t *variable;
	CXString vname;

	vname = clang_getCursorSpelling(cursor);
	printf("registering variable `%s'...\n", clang_getCString(vname));
	clang_disposeString(vname);
	variable = variable_create(cursor);
	SLIST_INSERT_HEAD(&(restrack->current->variables), variable, others);
}
