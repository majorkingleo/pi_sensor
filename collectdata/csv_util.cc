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
	if( fs.tellg() == 0 ) {
		return true;
	}

    fs.seekg(-1, std::ios_base::cur);
    for(long i = fs.tellg(); i >= 0; i--)
    {
        if(fs.peek() == '\n')
        {
            fs.get();
            return true;
        }
        fs.seekg(i, std::ios_base::beg);

    	if( fs.tellg() == 0 ) {
    		return true;
    	}

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
	if( pos == 0 ) {
		return *this;
	}

	if( amount == 0 ) {
		amount = 1;
	}

	iterator ret(*this);

	std::fstream::pos_type p = pos - 1l;

	parent->setCurrentPos(p);

	for( int n = 0; n < amount; n++ )
	{
		DEBUG( format("n: %d pos: %d", n, pos ) );
		if( !parent->moveToStartOfLine(parent->in) ) {
			DEBUG( "moveToStartOfLine failed" );
			break;
		}
	}

	pos = parent->getCurrentPos();
	DEBUG( format("pos is: %d", pos ) );

	return ret;
}

CSVUtil::iterator & CSVUtil::iterator::operator--()
{
	if( pos == 0 ) {
		return *this;
	}

	std::fstream::pos_type p = pos - 1l;

	parent->setCurrentPos(p);

	if( !parent->moveToStartOfLine(parent->in) ) {
		DEBUG( "moveToStartOfLine failed" );
	} else {
		pos = parent->getCurrentPos();
	}

	return *this;
}

std::vector<std::string> CSVUtil::iterator::operator*()
{
	parent->setCurrentPos(pos);
	return parent->getLine();
}

CSVUtil::reverse_iterator CSVUtil::rbegin()
{
	in.seekg(-1, std::ios_base::end);
	moveToStartOfLine( in );
	std::fstream::pos_type pos = in.tellg();

	in.seekg(0,std::ios_base::end);

	DEBUG( format( "start is at: %d end is at %d", pos,  in.tellg() ) );

	return reverse_iterator( this, pos );
}

CSVUtil::reverse_iterator CSVUtil::rend()
{
	return reverse_iterator( this, -1 );
}

CSVUtil::reverse_iterator & CSVUtil::reverse_iterator::operator++() {
	if( pos == 0 ) {
		pos = -1;
		return *this;
	}

	iterator::operator--();
	return *this;
}

CSVUtil::reverse_iterator CSVUtil::reverse_iterator::operator++(int amount) {

	reverse_iterator ret( *this );

	if( pos == 0 ) {
		pos = -1;
		return ret;
	}

	iterator::operator--(amount);

	return ret;
}

