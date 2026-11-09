
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

AS_IF([test "x$with_fox16" != "xno"],
    [PKG_CHECK_MODULES([LIBFOX], [fox16],[FOX16="yes"],[FOX16="no"])]
)

AS_IF([test "x$with_fox17" != "xno"],
    [PKG_CHECK_MODULES([LIBFOX], [fox17],[FOX17="yes"],[FOX17="no"])]
)

AS_IF([test "x${FOX16}x${FOX17}" == "xx" ],
    PKG_CHECK_MODULES([LIBFOX], [fox], [],
        [PKG_CHECK_MODULES([LIBFOX], [fox17], [],
            [PKG_CHECK_MODULES([LIBFOX], fox16 )]
        )]
    )
)

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
