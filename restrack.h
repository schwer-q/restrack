/*
 * Copyright (c) 2015, Quentin Schwerkolt
 * All rights reserved.
 *
 */

#ifndef __RESTRACK_H
#define __RESTRACK_H

typedef struct _allocator allocator_t;
typedef struct _deallocator deallocator_t;
typedef struct _function function_t;
typedef struct _restrack restrack_t;
typedef struct _scope scope_t;
typedef struct _variable variable_t;
typedef struct _useafterfree useafterfree_t;

struct _restrack {
	STAILQ_HEAD(functions, _function) functions;
	STAILQ_HEAD(scopes, _scope) scopes;

	function_t	*current_function;
	scope_t		*current;
};

struct _function {
	CXCursor	cursor;
	scope_t		*scope;

	int		allocator;
	int		deallocator;

	STAILQ_ENTRY(_function) others;
};

struct _scope {
	scope_t		*parent;

	int		returning;

	SLIST_HEAD(variables, _variable) variables;
	STAILQ_ENTRY(_scope) childs;
};

struct _variable {
	CXCursor	cursor;

	int		allocated;
	int		deallocated;
	int		returned;

	STAILQ_HEAD(useafterfree, _useafterfree) useafterfree;
	SLIST_ENTRY(_variable) others;
};

struct _useafterfree {
	CXCursor	cursor;

	STAILQ_ENTRY(_useafterfree) next;
};

#endif	/* __RESTRACK_H */
