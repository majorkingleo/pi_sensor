/*
 * DataRecord.cc
 *
 *  Created on: 23.01.2021
 *      Author: martin
 */
#include "DataRecord.h"
#include <errno.h>
#include <string.h>
#include <cpp_util.h>
#include <format.h>
#include <time.h>
#include <debug.h>
#include <string_utils.h>

using namespace Tools;

const std::string DataRecord::DATE_FORMAT = "%Y-%m-%d";
const std::string DataRecord::TIME_FORMAT = "%H:%M:%S";
const std::string DataRecord::DATE_TIME_FORMAT = DataRecord::DATE_FORMAT + " " + DataRecord::TIME_FORMAT;

DataRecord::DataRecord( const std::vector<std::string> & rec )
: timestamp(0),
  degrees_inside(0.0),
  humidity_inside(0.0),
  degrees_outside(0.0),
  humidity_outside(0.0)
{
	if( !parse( rec ) ) {
		throw REPORT_EXCEPTION( "cannot parse records" );
	}
}

std::string DataRecord::getDateStr( time_t t )
{
	struct tm *ptm = localtime( &t );

	if( ptm == NULL ) {
		throw REPORT_EXCEPTION( format( "localtime failed! %s", strerror(errno) ) );
	}

	struct tm tm = *ptm;

	char acBuffer[100];
	strftime( acBuffer, sizeof(acBuffer), DATE_FORMAT.c_str(), &tm );

	return acBuffer;
}

std::string DataRecord::getTimeStr( time_t t )
{
	struct tm *ptm = localtime( &t );

	if( ptm == NULL ) {
		throw REPORT_EXCEPTION( format( "localtime failed! %s", strerror(errno) ) );
	}

	struct tm tm = *ptm;

	char acBuffer[100];
	strftime( acBuffer, sizeof(acBuffer), TIME_FORMAT.c_str(), &tm );

	return acBuffer;
}

std::string DataRecord::getDateTimeStr( time_t t )
{
	struct tm *ptm = localtime( &t );

	if( ptm == NULL ) {
		throw REPORT_EXCEPTION( format( "localtime failed! %s", strerror(errno) ) );
	}

	struct tm tm = *ptm;

	char acBuffer[100];
	strftime( acBuffer, sizeof(acBuffer), DATE_TIME_FORMAT.c_str(), &tm );

	return acBuffer;
}

bool DataRecord::parse( const std::vector<std::string> & rec )
{
	struct tm tm = {};

	if( strptime(rec[FIELD_DATE].c_str(),
				 DATE_FORMAT.c_str(),
				 &tm ) != 0 )
	{
		if( strptime(rec[FIELD_TIME].c_str(),
					 TIME_FORMAT.c_str(),
					 &tm ) != 0 )
		{

			timestamp = mktime( &tm );

			if( timestamp == (time_t)-1) {
				DEBUG( format( "Error converting time: %s %s to a time", rec[FIELD_DATE], rec[FIELD_TIME] ) );
				return false;
			}
		} else {
			DEBUG( format( "Timevalue: %s cannot be parsed", rec[FIELD_TIME]));
		}
	} else {
		DEBUG( format( "Datevalue: %s cannot be parsed", rec[FIELD_DATE]));
	}

	degrees_inside   = s2x<float>( rec[FIELD_DEGREE_INSIDE], 0.0 );
	humidity_inside  = s2x<float>( rec[FIELD_HUMIDITY_INSIDE], 0.0 );
	degrees_outside  = s2x<float>( rec[FIELD_DEGREE_OUTSIDE], 0.0 );
	humidity_outside = s2x<float>( rec[FIELD_HUMIDITY_OUTSIDE], 0.0 );

	return true;
}

std::string DataRecord::toString() const
{
	std::stringstream str;

	char acBuffer[100] = "";
	struct tm *ptm = localtime( &timestamp );

	if( ptm ) {
		strftime( acBuffer, sizeof(acBuffer), DATE_TIME_FORMAT.c_str(), ptm );
		str << acBuffer;
	}

	str << ";" << format( "%02.1f;%02.1f;%02.1f;%02.1f",
						  degrees_inside, humidity_inside,
						  degrees_outside, humidity_outside );

	return str.str();
}

std::ostream & operator<<( std::ostream & out, const DataRecord & rec )
{
	return out << rec.toString();
}



