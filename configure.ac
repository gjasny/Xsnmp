# -*- Autoconf -*-

AC_PREREQ(2.59)
AC_INIT([xsnmp], [1.2.1])

AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_FILES([Makefile])

AM_INIT_AUTOMAKE([1.10 subdir-objects])

AM_MAINTAINER_MODE
AM_SILENT_RULES([yes])

# Checks for programs.
AC_PROG_CC([cc gcc clang])
AC_PROG_INSTALL
AC_PROG_LN_S

AC_CHECK_HEADER([net-snmp/net-snmp-config.h], [], [AC_MSG_ERROR([net-snmp includes not found])])
AC_CHECK_HEADER([pcre.h], [], [AC_MSG_ERROR([pcre includes not found])])

AC_CHECK_LIB([pcre], [pcre_compile], [], [AC_MSG_ERROR([libpcre not found])])
AC_CHECK_LIB([netsnmp], [init_snmp], [], [AC_MSG_ERROR([libnetsnmp not found])])
AC_CHECK_LIB([netsnmpagent], [init_agent], [], [AC_MSG_ERROR([libnetsnmpagent not found])])

AC_OUTPUT
