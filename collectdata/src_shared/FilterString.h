/*
 * FilterString.h
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */

#ifndef FILTERSTRING_H_
#define FILTERSTRING_H_

#include "FilterDataRecords.h"
#include "DataRecord.h"

class FilterString : public FilterDataRecords::Filter
{
protected:
	const std::string str;
	DataRecord::Record field;

public:
	FilterString( const std::string & str_, DataRecord::Record field_ )
	: str( str_ ),
	  field( field_ )
	{}

	virtual bool operator()( const std::vector<std::string> & records );
};



#endif /* FILTERSTRING_H_ */
