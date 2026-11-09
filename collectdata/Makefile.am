bin_PROGRAMS=collectdata

collectdata_SOURCES=\
		src/file_option.cc\
		src/main.cc\
		src/OutDebug.cc\
		src/colored_output.cc \
		src/main_window.cc \
		src/csv_util.cc \
		src/FilterDataRecords.cc \
		src/DataRecord.cc \
		src/FilterDay.cc \
		src/FilterString.cc \
		src/GenGnuPlotData.cc \
		src/Gen24HoursPlot.cc \
		src/GenXDaysPlot.cc \
		src/AsyncOutDebug.cc \
		src/AsyncFileLogger.cc \
		src/logging.cc \
		src/BufferedFile.cc


AM_CPPFLAGS = -I$(top_srcdir)/tools \
	-I$(top_srcdir)/cpputils/cpputilsshared  \
	-I$(top_srcdir)/cpputils/cpputilsshared/cpputilsformat \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/common \
	-DNOWAMAS \
	-std=c++20 \
	`pkgconf --cflags fox` \
	-pthread

noinst_LIBRARIES=tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a \
	common/libcommon.a

tools_libtools_a_SOURCES= \
	tools/arg.cpp \
	tools/debug.cpp \
	tools/pairs.cpp \
	tools/theme.cpp \
	tools/thread.cpp


cpputils_cpputilsshared_libcpputilsshared_a_SOURCES= \
		cpputils/cpputilsshared/backtrace.cc \
		cpputils/cpputilsshared/cppdir.cc \
		cpputils/cpputilsshared/leoini.cc \
		cpputils/cpputilsshared/string_utils.cc \
		cpputils/cpputilsshared/xml.cc \
		cpputils/cpputilsshared/stderr_exception.cc


cpputils_cpputilsshared_cpputilsformat_libcpputilsformat_a_SOURCES= \
	cpputils/cpputilsshared/cpputilsformat/format1.cc \
	cpputils/cpputilsshared/cpputilsformat/format2.cc \
	cpputils/cpputilsshared/cpputilsformat/cformat.cc

collectdata_LDADD = tools/libtools.a cpputils/cpputilsshared/libcpputilsshared.a cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a

collectdata_LDFLAGS=-pthread

common_libcommon_a_SOURCES= \
	common/TestUtils.cc \
	common/TestUtils.h

check_PROGRAMS = test_csv_util

TESTS = test_csv_util

test_csv_util_SOURCES= \
	src_test/test_csv_util_main.cc \
	src_test/test_csv_util.cc \
	src_test/test_csv_util.h \
	src_test/test_datarecord.cc \
	src_test/test_datarecord.h \
	src_test/test_filters.cc \
	src_test/test_filters.h \
	src_test/test_helpers.h \
	src/csv_util.cc \
	src/BufferedFile.cc \
	src/DataRecord.cc \
	src/FilterDataRecords.cc \
	src/FilterDay.cc \
	src/FilterString.cc \
	src/colored_output.cc \
	common/TestUtils.h

test_csv_util_LDADD = common/libcommon.a \
	tools/libtools.a \
	cpputils/cpputilsshared/libcpputilsshared.a \
	cpputils/cpputilsshared/cpputilsformat/libcpputilsformat.a

test_csv_util_LDFLAGS = -pthread

LIBS= \
    $(LIBFOX_LIBS)
