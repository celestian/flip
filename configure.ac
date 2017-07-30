AC_INIT([flip], [0.0.0], [https://github.com/celestian/flip], [flip])

AC_CONFIG_HEADERS(config.h)
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux/m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects no-dist-gzip dist-bzip2])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
LT_INIT

AC_SUBST(PACKAGE_AUTHOR, ["celestian"])
AC_SUBST(PACKAGE_DESCRIPTION, ["Forex Simulation Platform"])
AC_SUBST(PACKAGE_HOMEPAGE, ["https://github.com/celestian/flip"])
AC_SUBST(PACKAGE_MONTH, [$(LANG=C date '+%B')])
AC_SUBST(PACKAGE_YEAR, [$(LANG=C date '+%Y')])

# Debug
AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

# Checks for programs.
AC_PROG_CC

AX_CHECK_COMPILE_FLAG([-std=c11], [
  CFLAGS+=" -std=c11"
], [
  echo "C compiler cannot compile C11 code"
  exit -1
])

# Checks for libraries.
PKG_CHECK_MODULES([TALLOC], [talloc])
PKG_CHECK_MODULES([TEVENT], [tevent])
PKG_CHECK_MODULES([NANOMSG], [nanomsg])
PKG_CHECK_MODULES([LIBSYSTEMD], [libsystemd])
PKG_CHECK_MODULES([UUID], [uuid])
PKG_CHECK_MODULES([LIBWEBSOCKETS], [libwebsockets])

AX_LIB_SQLITE3()

AX_LIB_CURL([7.50.0],,[
  AC_MSG_ERROR([Your system lacks libcurl >= 7.50.0])
])

# Checks for header files.
AC_STDC_HEADERS

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
