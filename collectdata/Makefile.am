bin_PROGRAMS=collectdata

collectdata_SOURCES=\
		file_option.cc\
		main.cc\
		OutDebug.cc\
		colored_output.cc \
		main_window.cc \
		csv_util.cc \
		FilterDataRecords.cc \
		DataRecord.cc \
		FilterDay.cc \
		FilterString.cc \
		GenGnuPlotData.cc \
		Gen24HoursPlot.cc


AM_CPPFLAGS = -I$(top_srcdir)/tools \
	-I$(top_srcdir)/cpputils/cpputilsshared  \
	-I$(top_srcdir)/cpputils/cpputilsshared/cpputilsformat \
	-DNOWAMAS \
	-std=c++11 \
	`pkgconf --cflags fox` \
	-pthread

noinst_LIBRARIES=tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a

tools_libtools_a_SOURCES= \
	tools/arg.cpp \
	tools/database.cpp \
	tools/db.cpp \
	tools/dbi.cpp \
	tools/debug.cpp \
	tools/mysql_db.cpp \
	tools/odbc_db.cpp \
	tools/oracle_db.cpp \
	tools/pairs.cpp \
	tools/theme.cpp \
	tools/thread.cpp


cpputils_cpputilsshared_libcpputilsshared_a_SOURCES= \
		cpputils/cpputilsshared/backtrace.cc \
		cpputils/cpputilsshared/cppdir.cc \
		cpputils/cpputilsshared/leoini.cc \
		cpputils/cpputilsshared/logtool2.cc \
		cpputils/cpputilsshared/string_utils.cc \
		cpputils/cpputilsshared/vec_utils.cc \
		cpputils/cpputilsshared/xml.cc


cpputils_cpputilsshared_cpputilsformat_libcpputilsformat_a_SOURCES= \
	cpputils/cpputilsshared/cpputilsformat/format1.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.cc

collectdata_LDADD = tools/libtools.a cpputils/cpputilsshared/libcpputilsshared.a cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a

collectdata_LDFLAGS=`pkgconf --libs fox` -pthread
