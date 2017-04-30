dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS README

ACLOCAL_AMFLAGS = -I build-aux/m4 -Wall -Werror

AM_CPPFLAGS = \
	$(TALLOC_CFLAGS) \
	$(NANOMSG_CFLAGS) \
	${CURL_CFLAGS}
library_LIBTOOLFLAGS = $(NANOMSG_CFLAGS) ${CURL_CFLAGS}


lib_LTLIBRARIES = libflip_nbus.la libflip_url.la

include_HEADERS = \
	src/nbus/nbus.h \
	src/url/url.h

libflip_nbus_la_SOURCES = \
	src/nbus/nbus.c \
	src/nbus/nbus_private.h
libflip_nbus_la_LIBADD = \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS}

libflip_url_la_SOURCES = \
	src/url/url.c \
	src/url/url_private.h
libflip_url_la_LIBADD = \
	${TALLOC_LIBS} \
	${CURL_LIBS}


bin_PROGRAMS = listener

listener_SOURCES = \
	src/listener/listener.c
listener_LDADD = \
	libflip_nbus.la \
	libflip_url.la \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS} \
	${CURL_LIBS}