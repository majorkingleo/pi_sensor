/*
 * FilterDateRange.h
 *
 * Filter for records whose date is inside a closed date range.
 */

#ifndef FILTERDATERANGE_H_
#define FILTERDATERANGE_H_

#include "FilterDataRecords.h"
#include "DataRecord.h"

#include <string>

/**
 * Matches records with from_date <= FIELD_DATE <= to_date.
 *
 * The dates are compared as "YYYY-MM-DD" strings, so the filter works on the
 * raw record without converting it, and it stops a backwards scan as soon as
 * a record older than the range shows up (see
 * FilterDataRecords::Filter::continueProcessing()).
 */
class FilterDateRange : public FilterDataRecords::Filter
{
protected:
	const std::string from_date;
	const std::string to_date;

public:
	FilterDateRange( const std::string & from_date_, const std::string & to_date_ )
	: from_date( from_date_ ),
	  to_date( to_date_ )
	{}

	FilterDateRange( time_t from, time_t to )
	: from_date( DataRecord::getDateStr( from ) ),
	  to_date( DataRecord::getDateStr( to ) )
	{}

	virtual bool operator()( const std::vector<std::string> & records );

	const std::string & getFromDate() const {
		return from_date;
	}

	const std::string & getToDate() const {
		return to_date;
	}
};



#endif /* FILTERDATERANGE_H_ */
