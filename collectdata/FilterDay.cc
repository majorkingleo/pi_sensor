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


