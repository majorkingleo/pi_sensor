/*
 * FilterDataRecords.h
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */

#ifndef FILTERDATARECORDS_H_
#define FILTERDATARECORDS_H_

#include <ref.h>
#include "csv_util.h"
#include <list>

class FilterDataRecords
{
public:
	class Filter
	{
	protected:
		bool found_any_record;
		bool found_last_record;

	public:
		Filter()
		: found_any_record( false ),
		  found_last_record( false )
		{}

		virtual ~Filter() {}

		virtual bool operator()( const std::vector<std::string> & records ) = 0;

		virtual bool continueProcessing();
	};

	enum SEEKDIR
	{
		SEEKDIR_FORWARDS,
		SEEKDIR_BACKWARDS,
	};

	enum STRATEGY
	{
		STRATEGY_ALL_FILTERS_ARE_MATCHING,
		STRATEGY_ANY_FILTERS_IS_MATCHING,
	};

protected:
	std::list< Filter* > filters;
	std::list< std::vector<std::string> > results;

	SEEKDIR seekdir;
	STRATEGY strategy;

public:
	FilterDataRecords()
	: filters(),
	  results(),
	  seekdir( SEEKDIR_FORWARDS ),
	  strategy( STRATEGY_ALL_FILTERS_ARE_MATCHING )
	{}

	~FilterDataRecords();

	void addFilter( Filter* filter ) {
		filters.push_back( filter );
	}

	void setSeekdir( SEEKDIR seekdir_ ) {
		seekdir = seekdir_;
	}

	void setStrategy( STRATEGY strat ) {
		strategy = strat;
	}

	const std::list< std::vector<std::string> > & filter( CSVUtil & csv_util );

	const std::list< std::vector<std::string> > & getResults() const {
		return results;
	}
};



#endif /* FILTERDATARECORDS_H_ */
