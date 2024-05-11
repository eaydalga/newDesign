SUBDIRS = lib

libedivaldocdir = ${prefix}/doc/edival-0.1.8
libedivaldoc_DATA = \
	README          \
	COPYING         \
	AUTHORS         \
	ChangeLog       \
	INSTALL         \
	NEWS            \
	TODO

libedivalexampledir = ${libedivaldocdir}/examples
libedivalexample_DATA = examples/scan997.c examples/scan837.c examples/scanedi.c examples/README

EXTRA_DIST = $(libedivaldoc_DATA)

# Copy all the spec files. Of cource, only one is actually used.
dist-hook:
	for specfile in *.spec; do          \
		if test -f $$specfile; then      \
			cp -p $$specfile $(distdir);  \
		fi                               \
	done
