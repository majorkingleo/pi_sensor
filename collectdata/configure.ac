
dnl @synopsis CXX_FLAGS_CHECK [compiler flags]                                        
dnl @summary check whether compiler supports given C++ flags or not                   
AC_DEFUN([CXX_FLAGS_CHECK],                                                            
[dnl                                                                                  
  AC_MSG_CHECKING([if $CXX supports $1])
  AC_LANG_PUSH([C++])
  ac_saved_cxxflags="$CXXFLAGS"                                                       
  CXXFLAGS="-Werror $1"                                                               
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],                                            
    [AC_MSG_RESULT([yes])],                                                           
    [AC_MSG_ERROR([no])]                                                              
  )                                                                                   
  CXXFLAGS="$ac_saved_cxxflags"                                                       
  AC_LANG_POP([C++])
])


AC_INIT(collectdata, 0.1)
AM_INIT_AUTOMAKE([subdir-objects -Wall])
AC_PROG_CC
AC_CONFIG_FILES([
Makefile
])
AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_INSTALL
CXX_FLAGS_CHECK([-std=c++20])
CXX_FLAGS_CHECK([-pthread])


AC_ARG_WITH([fox],
  [AS_HELP_STRING([--without-fox],[build only import_temperatures, no FOX GUI])],
  [],
  [with_fox=yes]
)

AC_ARG_WITH([fox16],
  [AS_HELP_STRING([--with-fox16],[use FOX-Toolkit 1.6])],
  [],
  [with_fox16=no]
)

AC_ARG_WITH([fox17],
  [AS_HELP_STRING([--with-fox17],[use FOX-Toolkit 1.7])],
  [],
  [with_fox17=no]
)

AC_ARG_WITH([fox2],
  [AS_HELP_STRING([--with-fox2],[use FOX-Toolkit 2.0])],
  [],
  [with_fox2=no]
)

dnl FOX installs into /usr/local by default, e.g. /usr/local/lib64/pkgconfig/
dnl fox-2.0.pc and /usr/local/bin/fox-config. Neither /usr/local/lib64/pkgconfig
dnl nor /usr/local/bin is necessarily part of the pkg-config/PATH search path
dnl (Arch: pkg-config only looks at /usr/lib/pkgconfig:/usr/share/pkgconfig), so
dnl add the usual /usr/local locations before looking for FOX. Otherwise
dnl PKG_CHECK_MODULES reports "Package 'fox-2.0' not found" although FOX is
dnl installed.
PKG_PROG_PKG_CONFIG
AS_IF([test -n "$PKG_CONFIG"],
    [for pc_dir in /usr/local/lib64/pkgconfig /usr/local/lib/pkgconfig \
                  /usr/local/share/pkgconfig \
                  /usr/local/lib/x86_64-linux-gnu/pkgconfig
     do
         AS_IF([test -f "$pc_dir/fox-2.0.pc"],
             [AS_CASE([":$PKG_CONFIG_PATH:"],
                  [*":$pc_dir:"*], [],
                  [AS_IF([test -n "$PKG_CONFIG_PATH"],
                       [PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$pc_dir"],
                       [PKG_CONFIG_PATH="$pc_dir"])])])
     done
     export PKG_CONFIG_PATH
     AC_MSG_NOTICE([pkg-config search path: $PKG_CONFIG_PATH])])

dnl fox-config is a second, pkg-config independent source of FOX flags.
AC_PATH_PROG([FOX_CONFIG], [fox-config], [no])
AS_IF([test "x$FOX_CONFIG" = "xno" && test -x /usr/local/bin/fox-config],
    [FOX_CONFIG=/usr/local/bin/fox-config])
AC_SUBST([FOX_CONFIG])

AS_IF([test "x$with_fox16" != "xno"],
    [PKG_CHECK_MODULES([LIBFOX], [fox16],[FOX16="yes"],[FOX16="no"])]
)

AS_IF([test "x$with_fox17" != "xno"],
    [PKG_CHECK_MODULES([LIBFOX], [fox17],[FOX17="yes"],[FOX17="no"])]
)

AS_IF([test "x$with_fox2" != "xno"],
    [PKG_CHECK_MODULES([LIBFOX], [fox-2.0],[FOX2="yes"],[FOX2="no"])]
)

dnl FOX is optional: the MySQL importer and the tests do not need it, so a
dnl server build can run without any X11 libraries.
have_fox=no

AS_IF([test "x$with_fox" != "xno"],
    [AS_IF([test "x${FOX16}" = "xyes" || test "x${FOX17}" = "xyes" || test "x${FOX2}" = "xyes"],
        [have_fox=yes],
        [PKG_CHECK_MODULES([LIBFOX], [fox-2.0],
            [have_fox=yes; FOX2="yes"],
            [PKG_CHECK_MODULES([LIBFOX], [fox],
                [have_fox=yes],
                [PKG_CHECK_MODULES([LIBFOX], [fox17],
                    [have_fox=yes],
                    [PKG_CHECK_MODULES([LIBFOX], [fox16],
                        [have_fox=yes],
                        [have_fox=no])]
                )]
            )]
        )]
    )]
)

dnl FOX 2 installs into /usr/local by default, which is neither in the
dnl pkg-config search path nor in the linker search path of the runtime
dnl loader. Fall back to fox-config and keep the library directory in the
dnl binary.
AS_IF([test "x$have_fox" != "xyes" && test "x$with_fox" != "xno"],
    [AS_IF([test "x$FOX_CONFIG" != "xno"],
         [LIBFOX_CFLAGS=`$FOX_CONFIG --cflags`
          LIBFOX_LIBS=`$FOX_CONFIG --libs`
          have_fox=yes])]
)

fox_libdir=
AS_IF([test "x$have_fox" = "xyes"],
    [AS_IF([test "x$FOX2" = "xyes"],
        [fox_libdir=`$PKG_CONFIG --variable=libdir fox-2.0`])
     AS_IF([test -z "$fox_libdir" && test "x$FOX_CONFIG" != "xno"],
        [fox_libdir=`$FOX_CONFIG --libs | tr ' ' '\n' | sed -n 's/^-L//p' | head -1`])
     AS_IF([test -n "$fox_libdir"],
        [AC_MSG_NOTICE([FOX library directory: $fox_libdir])
         FOX_RPATH="-Wl,-rpath,$fox_libdir"])]
)
AC_SUBST([FOX_RPATH])

AS_IF([test "x$have_fox" = "xyes"],
    [AC_MSG_NOTICE([FOX found - building the collectdata GUI])],
    [AC_MSG_WARN([FOX toolkit not found - only import_temperatures will be built
If FOX is installed in /usr/local, run
  PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig ./configure
or pass --with-fox2 / make /usr/local/bin/fox-config visible in PATH.])]
)

AM_CONDITIONAL([HAVE_FOX], [test "x$have_fox" = "xyes"])

dnl the MySQL importer (import_temperatures) is optional
AC_ARG_WITH([mysql],
  [AS_HELP_STRING([--without-mysql],[do not build the MySQL import tool])],
  [],
  [with_mysql=yes]
)

have_mysql=no
AS_IF([test "x$with_mysql" != "xno"], [
    AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no])
    AS_IF([test "x$MYSQL_CONFIG" != "xno"], [
        MYSQL_CFLAGS=`$MYSQL_CONFIG --include`
        MYSQL_LIBS=`$MYSQL_CONFIG --libs`
        have_mysql=yes
    ], [
        AC_MSG_WARN([mysql_config not found - import_temperatures will not be built])
    ])
])

AC_SUBST([MYSQL_CFLAGS])
AC_SUBST([MYSQL_LIBS])
AM_CONDITIONAL([HAVE_MYSQL], [test "x$have_mysql" = "xyes"])

AC_OUTPUT
