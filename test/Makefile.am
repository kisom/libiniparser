AM_CFLAGS =	-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align
AM_CFLAGS +=	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations
AM_CFLAGS +=	-Wnested-externs -Winline -Wno-long-long  -Wunused-variable
AM_CFLAGS +=	-Wstrict-prototypes -Werror -g -O0 -I../src/
AM_LDFLAGS =	-L../src/

check_PROGRAMS = iniparser_test
iniparser_test_SOURCES = test.c
iniparser_test_LDADD = -liniparser
dist_noinst_DATA = testdata/*


