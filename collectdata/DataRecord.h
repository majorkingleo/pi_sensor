/*
 * DataRecord.h
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */

#ifndef DATARECORD_H_
#define DATARECORD_H_

#include <string>

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

public:

	static std::string getDateStr( time_t t );
};

#endif /* DATARECORD_H_ */
