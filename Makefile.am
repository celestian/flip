dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS README

ACLOCAL_AMFLAGS = -I build-aux/m4 -Wall -Werror

if DEBUG
AM_CFLAGS = -g3 -O0 -DDEBUG
else
AM_CFLAGS = -O2
endif

AM_CPPFLAGS = \
	$(TALLOC_CFLAGS) \
	$(NANOMSG_CFLAGS) \
	${LIBSYSTEMD_CFLAGS}


bin_PROGRAMS = \
	flip_crawler \
	flip_collector


flip_crawler_SOURCES = \
	src/app/crawler.c \
	src/utils/utils.c \
	src/utils/daemon.c \
	src/utils/logs.c \
	src/config/inih/ini.c \
	src/config/config.c \
	src/nbus/nbus.c \
	src/url/url.c \
	src/json/jsmn/jsmn.c \
	src/json/json.c \
	src/json/btc-e_ticker.c
flip_crawler_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS} \
	${CURL_LIBS}
flip_crawler_LDFLAGS = \
	${CURL_CFLAGS}


flip_collector_SOURCES = \
	src/app/collector.c \
	src/utils/utils.c \
	src/utils/daemon.c \
	src/utils/logs.c \
	src/config/inih/ini.c \
	src/config/config.c \
	src/nbus/nbus.c \
	src/json/jsmn/jsmn.c \
	src/json/json.c \
	src/json/btc-e_ticker.c \
	src/sql/sql.c
flip_collector_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${NANOMSG_LIBS} \
	${SQLITE3_LDFLAGS}
flip_collector_LDFLAGS = \
	${SQLITE3_CFLAGS}
