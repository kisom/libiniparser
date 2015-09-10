AM_CFLAGS =	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
AM_CFLAGS +=	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations
AM_CFLAGS +=	-Wnested-externs -Winline -Wno-long-long  -Wunused-variable
AM_CFLAGS +=	-Wstrict-prototypes -Werror -g -O0
AM_LDFLAGS =	-L.

lib_LTLIBRARIES =		libiniparser.la
nobase_include_HEADERS =	iniparser/iniparser.h
libiniparser_la_SOURCES =	iniparser.c

iniparser_test: iniparser.c test.c
	$(CC) $(AM_CFLAGS) -o $@ iniparser.c test.c
