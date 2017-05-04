dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS README

ACLOCAL_AMFLAGS = -I build-aux/m4 -Wall -Werror

AM_CPPFLAGS = \
	$(TALLOC_CFLAGS) \
	$(NANOMSG_CFLAGS) \
	${LIBSYSTEMD_CFLAGS}


bin_PROGRAMS = \
	flip_crawler \
	flip_collector


flip_crawler_SOURCES = \
	src/app/crawler.c \
	src/nbus/nbus.c \
	src/url/url.c \
	src/utils/daemon.c \
	src/utils/logs.c
flip_crawler_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS} \
	${CURL_LIBS}
flip_crawler_LDFLAGS = \
	${CURL_CFLAGS}


flip_collector_SOURCES = \
	src/app/collector.c \
	src/utils/daemon.c \
	src/utils/logs.c
flip_collector_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS}