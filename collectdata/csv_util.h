/*
 * csv_util.h
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */

#ifndef CSV_UTIL_H_
#define CSV_UTIL_H_

#include <vector>
#include <string>
#include <fstream>

class CSVUtil
{
	std::ifstream in;

public:
	CSVUtil( const std::string & file );
	bool operator!() const { return in.operator!(); }

	std::vector<std::string> getLastLine();

protected:
	bool moveToStartOfLine(std::ifstream& fs) const;
	std::string getLastLineInFile(std::ifstream& fs) const;
};



#endif /* CSV_UTIL_H_ */
