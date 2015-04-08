#
# Copyright (c) 2015, Quentin Schwerkolt
# All rights reserved.
#
#

bin_PROGRAMS = restrack


restrack_SOURCES =			\
	main.c				\
	rtrack.c			\
	scope.c				\
	xalloc.c			\
	getprogname.c			\
	strlcpy.c			\
	variable.c

restrack_CFLAGS = -W -Wall -Wextra -I ..
restrack_CFLAGS += ${clanginc}
restrack_LDFLAGS = ${clanglibs}
