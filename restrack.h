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

	SLIST_ENTRY(_variable) others;
};

/* function_t	*function_new(CXCursor function); */
/* void		function_delete(function_t *function); */
/* void		function_set_scope(function_t *function, scope_t *scope); */

/* scope_t	*scope_new(scope_t *parent); */
/* void	scope_delete(scope_t *scope); */
/* void	scope_add_child(scope_t *scope, scope_t *child); */
/* void	scope_add_variable(scope_t *scope, variable_t *variable); */

/* variable_t	*variable_new(CXCursor variable); */
/* void		variable_delete(variable_t *variable); */

#endif	/* __RESTRACK_H */
