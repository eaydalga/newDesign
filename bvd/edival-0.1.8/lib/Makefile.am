## Process this file with automake to produce Makefile.in

INCLUDES =

AM_CFLAGS = -O3 -fomit-frame-pointer -Wall -c -fmessage-length=0

lib_LTLIBRARIES = libedival.la

edival_public_h_SOURCES = edival.h               

edival_private_h_SOURCES = \
	common.h               \
	complextype.h          \
	edifactparser.h        \
	ediparse.h             \
	hashtable.h            \
	schema.h               \
	simpletype.h           \
	statemachine.h         \
	x12parser.h    

edival_C_SOURCES =	\
	common.c				\
	complextype.c		\
	ediparse.c			\
	edifactparser.c   \
	hashtable.c			\
	schema.c				\
	simpletype.c		\
	statemachine.c		\
	x12parser.c
	
libedival_la_SOURCES =			\
	$(edival_public_h_SOURCES) \
	$(edival_private_h_SOURCES)\
	$(edival_C_SOURCES)

libedival_la_LDFLAGS = \
	$(all_libraries)    \
	-version-info 1:0:0 \
	-no-undefined

libedival_la_LIBADD = 

edivalincludedir = $(includedir)
edivalinclude_HEADERS = $(edival_public_h_SOURCES)
