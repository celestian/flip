dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL LICENSE NEWS README

ACLOCAL_AMFLAGS = -I build-aux/m4 -Wall -Werror

if DEBUG
AM_CFLAGS = -g3 -O0 -DDEBUG
else
AM_CFLAGS = -O2
endif

AM_CPPFLAGS = \
	$(TALLOC_CFLAGS) \
	$(TEVENT_CFLAGS) \
	$(NANOMSG_CFLAGS) \
	${LIBSYSTEMD_CFLAGS} \
	${UUID_CFLAGS}


bin_PROGRAMS = \
	flip \
	flipd \
	flip_crawler \
	flip_collector



flip_SOURCES = \
	src/app/flip.c \
	src/common/utils/utils.c \
	src/common/utils/daemon.c \
	src/common/utils/logs.c \
	src/common/nbus/nbus.c
flip_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${TEVENT_LIBS} \
	${NANOMSG_LIBS}


flipd_SOURCES = \
	src/app/flipd.c \
	src/common/utils/inih/ini.c \
	src/common/utils/conf.c \
	src/common/utils/utils.c \
	src/common/utils/args.c \
	src/common/utils/daemon.c \
	src/common/utils/logs.c \
	src/common/nbus/nbus.c \
	src/common/json/jsmn/jsmn.c \
	src/common/json/json.c \
	src/common/msg/msg.c \
	src/module/flipd/listen_handler.c \
	src/module/flipd/daemon_handler.c
flipd_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${TEVENT_LIBS} \
	${NANOMSG_LIBS} \
	${UUID_LIBS}


flip_crawler_SOURCES = \
	src/app/crawler.c \
	src/common/utils/utils.c \
	src/common/utils/args.c \
	src/common/utils/daemon.c \
	src/common/utils/logs.c \
	src/common/nbus/nbus.c \
	src/common/url/url.c \
	src/common/json/jsmn/jsmn.c \
	src/common/json/json.c \
	src/common/json/btc-e_ticker.c \
	src/common/msg/msg.c \
	src/module/common_daemon.c
flip_crawler_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${TEVENT_LIBS} \
	${NANOMSG_LIBS} \
	${CURL_LIBS} \
	${LIBWEBSOCKETS_CFLAGS} \
	${UUID_LIBS}
flip_crawler_LDFLAGS = \
	${CURL_CFLAGS} \
	${LIBWEBSOCKETS_CFLAGS}

flip_collector_SOURCES = \
	src/app/collector.c \
	src/common/utils/utils.c \
	src/common/utils/args.c \
	src/common/utils/daemon.c \
	src/common/utils/logs.c \
	src/common/nbus/nbus.c \
	src/common/json/jsmn/jsmn.c \
	src/common/json/json.c \
	src/common/json/btc-e_ticker.c \
	src/common/msg/msg.c \
	src/common/sql/sql.c \
	src/module/common_daemon.c
flip_collector_LDADD = \
	${LIBSYSTEMD_LIBS} \
	${TALLOC_LIBS} \
	${TEVENT_LIBS} \
	${NANOMSG_LIBS} \
	${SQLITE3_LDFLAGS} \
	${UUID_LIBS}
flip_collector_LDFLAGS = \
	${SQLITE3_CFLAGS}
