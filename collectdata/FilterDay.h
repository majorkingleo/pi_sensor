/*
 * FilterDay.h
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */

#ifndef FILTERDAY_H_
#define FILTERDAY_H_

#include "FilterString.h"

class FilterDay : public FilterString
{
public:
	FilterDay( time_t day );

	virtual bool operator()( const std::vector<std::string> & records );
};



#endif /* FILTERDAY_H_ */
