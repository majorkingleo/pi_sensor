/*
 * FilterDay.cc
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */
#include "FilterDay.h"


FilterDay::FilterDay( time_t date )
: FilterString( DataRecord::getDateStr(date),  DataRecord::Record::FIELD_DATE )
{

}

bool FilterDay::operator()( const std::vector<std::string> & records )
{
	found_last_record = FilterString::operator()( records );

	if( found_last_record ) {
		found_any_record = true;
	}

	return found_last_record;
}


