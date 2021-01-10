/*
 * csv_util.cc
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */


#include <csv_util.h>
#include <string_utils.h>
#include <iostream>
#include <debug.h>

using namespace Tools;

CSVUtil::CSVUtil( const std::string & file )
: in( file )
{

}

bool CSVUtil::moveToStartOfLine(std::ifstream& fs) const
{
    fs.seekg(-1, std::ios_base::cur);
    for(long i = fs.tellg(); i > 0; i--)
    {
        if(fs.peek() == '\n')
        {
            fs.get();
            return true;
        }
        fs.seekg(i, std::ios_base::beg);
    }
    return false;
}

std::string CSVUtil::getLastLineInFile(std::ifstream& fs) const
{
    // Go to the last character before EOF
    fs.seekg(-1, std::ios_base::end);
    if (!moveToStartOfLine(fs))
        return "";

    std::string lastline = "";
    getline(fs, lastline);
    return lastline;
}

std::vector<std::string> CSVUtil::getLastLine()
{
	in.clear();
	std::string line = getLastLineInFile( in );

	DEBUG( format( "line: %s", line ) );

	return split_simple( line, ";" );
}
