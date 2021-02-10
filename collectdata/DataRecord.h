/*
 * DataRecord.h
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */

#ifndef DATARECORD_H_
#define DATARECORD_H_

#include <string>
#include <vector>

class DataRecord
{
public:
	enum Record
	{
		FIELD_DATE = 0,
		FIELD_TIME,
		FIELD_DEGREE_INSIDE,
		FIELD_HUMIDITY_INSIDE,
		FIELD_DEGREE_OUTSIDE,
		FIELD_HUMIDITY_OUTSIDE,
		MAX_FIELDS
	};

	static const std::string DATE_FORMAT;
	static const std::string TIME_FORMAT;
	static const std::string DATE_TIME_FORMAT;


	class sort_by_time_asc
	{
	public:
		bool operator()( const DataRecord & a, const DataRecord & b )
		{
			return a.timestamp < b.timestamp;
		}
	};


	time_t timestamp;
	float  degrees_inside;
	float  humidity_inside;
	float  degrees_outside;
	float  humidity_outside;

public:
	DataRecord( const std::vector<std::string> & rec );

	static std::string getDateStr( time_t t );
	static std::string getTimeStr( time_t t );
	static std::string getDateTimeStr( time_t t );

	bool parse( const std::vector<std::string> & rec );

	std::string toString() const;
};

std::ostream & operator<<( std::ostream & out, const DataRecord & rec );

#endif /* DATARECORD_H_ */
