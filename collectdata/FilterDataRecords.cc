/*
 * FilterDataRecords.cc
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */
#include "FilterDataRecords.h"

FilterDataRecords::~FilterDataRecords()
{
	for( auto filter : filters )
	{
		delete filter;
	}
	filters.clear();
}

const std::list< std::vector<std::string> > &  FilterDataRecords::filter( CSVUtil & csv_util )
{
	switch( seekdir )
	{
		case SEEKDIR_FORWARDS:
		{
			bool continueProcessing = true;
			for( auto it = csv_util.begin(); it != csv_util.end() && continueProcessing; it++ )
			{
				switch( strategy )
				{
					case STRATEGY_ALL_FILTERS_ARE_MATCHING:
					{
						bool all_filters_are_matching = true;
						for( auto filter : filters )
						{
							if( !(*filter)(*it) ) {
								all_filters_are_matching = false;
								break;
							}

							continueProcessing = filter->continueProcessing();
						}

						if( all_filters_are_matching ) {
							results.push_back( *it );
						}
					}
					break;

					case STRATEGY_ANY_FILTERS_IS_MATCHING:
					{
						bool any_filters_is_matching = false;
						for( auto filter : filters )
						{
							if( (*filter)(*it) ) {
								any_filters_is_matching = true;
								break;
							}

							continueProcessing = filter->continueProcessing();
						}

						if( any_filters_is_matching ) {
							results.push_back( *it );
						}
					}
					break;
				} // switch
			} // for
		} // case
		break;

		case SEEKDIR_BACKWARDS:
		{
			bool continueProcessing = true;
			for( auto it = csv_util.rbegin(); it != csv_util.rend(); it++ )
			{
				switch( strategy )
				{
					case STRATEGY_ALL_FILTERS_ARE_MATCHING:
					{
						bool all_filters_are_matching = true;
						for( auto filter : filters )
						{
							if( !(*filter)(*it) ) {
								all_filters_are_matching = false;
								break;
							}

							continueProcessing = filter->continueProcessing();
						}

						if( all_filters_are_matching ) {
							results.push_back( *it );
						}
					}
					break;

					case STRATEGY_ANY_FILTERS_IS_MATCHING:
					{
						bool any_filters_is_matching = false;
						for( auto filter : filters )
						{
							if( (*filter)(*it) ) {
								any_filters_is_matching = true;
								break;
							}

							continueProcessing = filter->continueProcessing();
						}

						if( any_filters_is_matching ) {
							results.push_back( *it );
						}
					}
					break;
				} // switch
			} // for
		} // case
		break;

	} // switch

	return results;
}



