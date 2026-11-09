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

bool CSVUtil::moveToStartOfLine(BufferedFile& fs)
{
	return fs.moveToStartOfLine();
}

bool CSVUtil::moveToEndOfLine(BufferedFile& fs)
{
	return fs.moveToEndOfLine();
}

std::string CSVUtil::getLastLineInFile(BufferedFile& fs)
{
    return fs.lastLine();
}

std::vector<std::string> CSVUtil::getLastLine()
{
	std::string line = getLastLineInFile( in );

	DEBUG( format( "line: %s", line ) );

	return split_simple( line, ";" );
}

std::string CSVUtil::getPrevLineInFile(BufferedFile& fs)
{
    if (!moveToStartOfLine(fs))
        return "";

    return fs.readLine();
}


std::vector<std::string> CSVUtil::getPrevLine()
{
	std::string line = getPrevLineInFile( in );
	return split_simple( line, ";" );
}

std::vector<std::string> CSVUtil::getLine()
{
	return split_simple( in.readLine(), ";" );
}

CSVUtil::iterator CSVUtil::begin()
{
	return iterator( this, 0 );
}

CSVUtil::iterator CSVUtil::end()
{
	return iterator( this, std::fstream::pos_type( in.size() ) );
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
	if( in.size() <= 0 ) {
		return reverse_iterator( this, -1 );
	}

	in.seek( in.size() - 1 );
	moveToStartOfLine( in );
	std::fstream::pos_type pos = std::fstream::pos_type( in.tell() );

	DEBUG( format( "start is at: %d end is at %d", pos, in.size() ) );

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

