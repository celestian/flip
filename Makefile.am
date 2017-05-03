dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS README

ACLOCAL_AMFLAGS = -I build-aux/m4 -Wall -Werror

AM_CPPFLAGS = \
	$(TALLOC_CFLAGS) \
	$(NANOMSG_CFLAGS) \
	${CURL_CFLAGS} \
	${LIBSYSTEMD_CFLAGS}

library_LIBTOOLFLAGS = \
	$(NANOMSG_CFLAGS) \
	${CURL_CFLAGS} \
	${LIBSYSTEMD_CFLAGS}

lib_LTLIBRARIES = libflip_utils.la libflip_nbus.la libflip_url.la

include_HEADERS = \
	src/nbus/nbus.h \
	src/url/url.h

libflip_utils_la_SOURCES = \
	src/utils/logs.c
libflip_utils_la_LIBADD = \
	${LIBSYSTEMD_LIBS}

libflip_nbus_la_SOURCES = \
	src/nbus/nbus.c \
	src/nbus/nbus_private.h
libflip_nbus_la_LIBADD = \
	libflip_utils.la \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS}

libflip_url_la_SOURCES = \
	src/url/url.c \
	src/url/url_private.h
libflip_url_la_LIBADD = \
	libflip_utils.la \
	${TALLOC_LIBS} \
	${CURL_LIBS}


bin_PROGRAMS = flip_crawler

flip_crawler_SOURCES = \
	src/app_crawler/crawler.c
flip_crawler_LDADD = \
	libflip_utils.la \
	libflip_nbus.la \
	libflip_url.la \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS} \
	${CURL_LIBS}