#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([protoc-eg], [0.1], [amao.chj@gmail.com])
AC_CONFIG_SRCDIR([src/generated-sources/amessage.pb-c.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE(1.8 foreign -Wall -Werror)

#if we will create lib
AM_PROG_AR
AC_PROG_LIBTOOL
LT_INIT

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT
