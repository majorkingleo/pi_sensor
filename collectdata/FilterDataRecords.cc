/*
 * FilterDataRecords.cc
 *
 *  Created on: 24.01.2021
 *      Author: martin
 */
#include "FilterDataRecords.h"
#include <format.h>
#include <debug.h>

using namespace Tools;

bool FilterDataRecords::Filter::continueProcessing()
{
	if( found_any_record ) {
		if( !found_last_record ) {
			return false;
		}
	}

	return true;
}

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
	unsigned long records_read = 0;

	switch( seekdir )
	{
		case SEEKDIR_FORWARDS:
		{
			DEBUG( "SEEKDIR_FORWARDS" );
			bool continueProcessing = true;
			for( auto it = csv_util.begin(); it != csv_util.end() && continueProcessing; it++ )
			{
				records_read++;

				switch( strategy )
				{
					case STRATEGY_ALL_FILTERS_ARE_MATCHING:
					{
						bool all_filters_are_matching = true;
						for( auto filter : filters )
						{
							if( !(*filter)(*it) ) {
								all_filters_are_matching = false;
								continueProcessing = filter->continueProcessing();
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
								continueProcessing = filter->continueProcessing();
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
			DEBUG( "SEEKDIR_BACKWARDS" );
			bool continueProcessing = true;
			for( auto it = csv_util.rbegin(); it != csv_util.rend() && continueProcessing; it++ )
			{
				records_read++;
				switch( strategy )
				{
					case STRATEGY_ALL_FILTERS_ARE_MATCHING:
					{
						bool all_filters_are_matching = true;
						for( auto filter : filters )
						{
							if( !(*filter)(*it) ) {
								all_filters_are_matching = false;
								continueProcessing = filter->continueProcessing();
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
								continueProcessing = filter->continueProcessing();
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

	DEBUG( format( "%d records read", records_read ));

	return results;
}



