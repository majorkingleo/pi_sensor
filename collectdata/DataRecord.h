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

	time_t timestamp;

public:
	DataRecord( const std::vector<std::string> & rec );

	static std::string getDateStr( time_t t );

	bool parse( const std::vector<std::string> & rec );
};

#endif /* DATARECORD_H_ */
