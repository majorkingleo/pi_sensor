/*
 * FilterString.cc
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */
#include "FilterString.h"
#include <iostream>

bool FilterString::operator()( const std::vector<std::string> & records )
{
	if( records[field] == str ) {
		return true;
	}

	return false;
}
