/*
 * FilterDateRange.cc
 *
 * Filter for records whose date is inside a closed date range.
 */

#include "FilterDateRange.h"

bool FilterDateRange::operator()( const std::vector<std::string> & records )
{
	const std::string & date = records[DataRecord::Record::FIELD_DATE];

	found_last_record = (from_date <= date) && (date <= to_date);

	if( found_last_record ) {
		found_any_record = true;
	}

	return found_last_record;
}
