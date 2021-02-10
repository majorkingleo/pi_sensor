#include "Gen24HoursPlot.h"
#include <fstream>
#include <cpp_utils.h>

using namespace Tools;

void Gen24HoursPlot::create()
{
	std::ofstream config( "gnuplot.input", std::ios_base::trunc );

	config << "set terminal png size " << width << "," << height << " lw 3\n";
	config << "set output '" << image_file << "'\n";
	config << "set xdata time\n";
	config << "set timefmt '%Y-%m-%d %H:%M:%S'\n";
	config << "set format x '%H:%M'\n";
	// config << "set xrange ['00:00:00':'23:59:59']\n";
	config << "set datafile separator ','\n";
	config << "set style data lines\n";
	config << "plot";

	unsigned long max = unifyData();

	auto xtics_it = data.find( XTIC );

	std::string dataFileName = format( "%s.dat", data_file );

	config << " '" << dataFileName << "' ";

	int dataCounter = 1;

	for( auto & pair : data )
	{
		if( pair.first == XTIC ) {
			continue;
		}
		std::string title = pair.first;
		dataCounter++;

		if( dataCounter > 2 ) {
			config << ", \"\" ";
		}

		config << " using 1:" << dataCounter << " title '" << title << "' ";
	}

	std::ofstream fdata( dataFileName.c_str(), std::ios_base::trunc );

	for( unsigned long i = 0; i < max; i++ )
	{
		fdata << xtics_it->second[i];
		for( auto & pair : data )
		{
			if( pair.first == XTIC ) {
				continue;
			}

			fdata << "," << pair.second[i];
		}

		fdata << "\n";
	}

	config << '\n';

	system( "gnuplot gnuplot.input" );
}


