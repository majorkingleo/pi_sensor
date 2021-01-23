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

bool CSVUtil::moveToEndOfLine(std::ifstream& fs) const
{
	fs.clear();
	while( !fs.eof() )
	{
		if( fs.get() == '\n' )
		{
			return true;
		}
	}

	return false;
}

std::string CSVUtil::getLastLineInFile(std::ifstream& fs) const
{
    // Go to the last character before EOF
    fs.seekg(-1, std::ios_base::end);
    if (!moveToStartOfLine(fs))
        return "";

    std::string lastline;
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

std::string CSVUtil::getPrevLineInFile(std::ifstream& fs) const
{
    if (!moveToStartOfLine(fs))
        return "";

    std::string line;
    getline(fs, line);
    return line;
}


std::vector<std::string> CSVUtil::getPrevLine()
{
	std::string line = getPrevLineInFile( in );
	return split_simple( line, ";" );
}

std::vector<std::string> CSVUtil::getLine()
{
	std::string line;
	getline(in, line);
	return split_simple( line, ";" );
}

CSVUtil::iterator CSVUtil::begin()
{
	return iterator( this, 0 );
}

CSVUtil::iterator CSVUtil::end()
{
	in.seekg(0, std::ios_base::end);
	std::fstream::pos_type pos = in.tellg();
	return iterator( this, pos );
}

CSVUtil::iterator CSVUtil::iterator::operator++( int amount )
{
	if( amount == 0 ) {
		amount = 1;
	}

	iterator ret(*this);

	std::fstream::pos_type p = pos + 1l;

	parent->setCurrentPos(p);

	for( int n = 0; n < amount; n++ )
	{
		if( !parent->moveToEndOfLine(parent->in) ) {
			DEBUG( "moveToEndOfLine failed" );
			break;
		}
	}

	pos = parent->getCurrentPos();

	return ret;
}

CSVUtil::iterator & CSVUtil::iterator::operator++()
{
	std::fstream::pos_type p = pos + 1l;

	parent->setCurrentPos(p);

	if( !parent->moveToEndOfLine(parent->in) ) {
		DEBUG( "moveToEndOfLine failed" );
	}

	pos = parent->getCurrentPos();

	return *this;
}

CSVUtil::iterator CSVUtil::iterator::operator--( int amount )
{
	if( amount == 0 ) {
		amount = 1;
	}

	iterator ret(*this);

	std::fstream::pos_type p = pos - 1l;

	if( pos == 0 ) {
		p = 0;
	}

	parent->setCurrentPos(p);

	for( int n = 0; n < amount; n++ )
	{
		DEBUG( format("n: %d", n ) );
		if( !parent->moveToStartOfLine(parent->in) ) {
			DEBUG( "moveToEndOfLine failed" );
			break;
		}
	}

	pos = parent->getCurrentPos();

	return ret;
}

CSVUtil::iterator & CSVUtil::iterator::operator--()
{
	std::fstream::pos_type p = pos - 1l;

	if( pos == 0 ) {
		p = 0;
	}

	parent->setCurrentPos(p);

	if( !parent->moveToStartOfLine(parent->in) ) {
		DEBUG( "moveToStartOfLine failed" );
	}

	pos = parent->getCurrentPos();

	return *this;
}

std::vector<std::string> CSVUtil::iterator::operator*()
{
	parent->setCurrentPos(pos);
	return parent->getLine();
}
