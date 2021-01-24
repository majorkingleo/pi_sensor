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
public:
	class iterator
	{
	protected:
		CSVUtil *parent;
		std::fstream::pos_type pos;

	public:
		iterator( CSVUtil * parent_ )
		: parent( parent_ ),
		  pos(0)
		{}

		iterator( CSVUtil * parent_, std::fstream::pos_type pos_ )
		: parent( parent_ ),
		  pos(pos_)
		{}

		iterator( const iterator & other )
		: parent( other.parent ),
		  pos( other.pos )
		{}

		iterator & operator=( const iterator & other )
		{
			parent = other.parent;
			pos = other.pos;
			return *this;
		}

		bool operator!=( const iterator & other )
		{
			if( parent != other.parent ) {
				return true;
			}

			if( pos != other.pos ) {
				return true;
			}

			return false;
		}

		iterator operator++( int amount );
		iterator operator--( int amount );

		iterator & operator++();
		iterator & operator--();

		std::vector<std::string> operator*();
	};

	class reverse_iterator : public iterator
	{
	public:
		reverse_iterator( CSVUtil * parent_ )
		: iterator( parent_ )
		{}

		reverse_iterator( CSVUtil * parent_, std::fstream::pos_type pos_ )
		: iterator( parent_ , pos_)
		{}

		reverse_iterator( const iterator & other )
		: iterator( other )
		{}
/*
		reverse_iterator & operator=( const reverse_iterator & other )
		{
			parent = other.parent;
			pos = other.pos;
			return *this;
		}

		bool operator!=( const reverse_iterator & other )
		{
			if( parent != other.parent ) {
				return true;
			}

			if( pos != other.pos ) {
				return true;
			}

			return false;
		}
*/
		reverse_iterator & operator++();
		reverse_iterator operator++( int amount );

		reverse_iterator operator--( int amount ) {
			return iterator::operator++( amount );
		}

		reverse_iterator & operator--() {
			iterator::operator++();
			return *this;
		}
	};

	friend class iterator;

protected:

	std::ifstream in;

public:

	CSVUtil( const std::string & file );
	bool operator!() const { return in.operator!(); }

	std::vector<std::string> getLastLine();
	std::vector<std::string> getPrevLine();
	std::vector<std::string> getLine();

	iterator begin();
	iterator end();

	reverse_iterator rbegin();
	reverse_iterator rend();

	std::fstream::pos_type getCurrentPos() {
		return in.tellg();
	}

	void setCurrentPos( std::fstream::pos_type pos ) {
		in.seekg(pos, std::ios_base::beg);
	}

protected:
	bool moveToStartOfLine(std::ifstream& fs) const;
	bool moveToEndOfLine(std::ifstream& fs) const;
	std::string getLastLineInFile(std::ifstream& fs) const;
	std::string getPrevLineInFile(std::ifstream& fs) const;
};



#endif /* CSV_UTIL_H_ */
