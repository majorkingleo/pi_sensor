#include "GenGnuPlotData.h"
#include <fstream>
#include <cpp_utils.h>

using namespace Tools;

const std::string GenGnuPlotData::XTIC = "xtic";

void GenGnuPlotData::create()
{
	std::ofstream config( "gnuplot.input", std::ios_base::trunc );

	config << "set terminal png size " << width << "," << height << " lw 3\n";
	config << "set output '" << image_file << "'\n";
	config << "plot";

	int dataFileCounter = 1;

	for( auto & pair : data )
	{
		if( dataFileCounter > 1 ) {
			config << ", ";
		}

		std::string title = pair.first;
		auto & vData = pair.second;

		std::string dataFileName = format( "%s%d.dat", data_file, dataFileCounter++ );

		config << " '" << dataFileName << "' with lines ";

		if( !title.empty() ) {
			config << " title '" << title << "' ";
		}

		std::ofstream fdata( dataFileName.c_str(), std::ios_base::trunc );

		for( auto & s : vData )
		{
			fdata << s << '\n';
		}
	}
	config << '\n';

	system( "gnuplot gnuplot.input" );
}

unsigned long GenGnuPlotData::unifyData()
{
	unsigned long max = 0;

	for( auto & pair : data )
	{
		max = std::max( static_cast<unsigned long>(pair.second.size()), max );
	}

	for( auto & pair : data )
	{
		pair.second.resize( max );
	}

	return max;
}

