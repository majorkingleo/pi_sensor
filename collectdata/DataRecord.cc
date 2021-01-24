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

using namespace Tools;

const std::string DataRecord::DATE_FORMAT = "%Y-%m-%d";

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



