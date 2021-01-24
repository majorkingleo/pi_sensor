#include <iostream>
#include <vector>
#include <fstream>
#include <errno.h>

#include "range.h"
#include "string_utils.h"
#include "xml.h"
#include "format.h"
#include "cpp_util.h"
#include <arg.h>
#include "file_option.h"
#include "OutDebug.h"
#include "colored_output.h"
#include "main_window.h"
#include "csv_util.h"
#include "FilterString.h"
#include "FilterDay.h"

using namespace Tools;

void usage( const std::string & prog )
{
  std::cerr << "usage: "
			<< prog << " temperaturen.csv\n";
}

class Co : public ColoredOutput
{
public:
	std::string good( const std::string & message )
	{
		return color_output( ColoredOutput::GREEN, message );
	}

	std::string bad( const std::string & message )
	{
		return color_output( ColoredOutput::BRIGHT_RED, message );
	}
};

void printLast( const std::string & days, const std::string & file )
{
	int nDays = s2x<int>(days,0);
}

std::ostream & operator<<( std::ostream & out, const std::vector<std::string> & data )
{
	for( const std::string & s : data )
	{
		out << s << ";";
	}

	return out;
}

void printAllRecords( const std::string & file )
{
	CSVUtil csv_util( file );

	if( !csv_util ) {
		throw REPORT_EXCEPTION( format( "Cannot open file %s ", file) );
	}

	std::vector<std::string> record = csv_util.getLastLine();

	int count = 0;

	for( auto it = csv_util.begin(); it != csv_util.end(); it++ )
	{
		std::cout << format( "%04d:", ++count );

		for( std::string & rec : *it )
		{
			std::cout << rec << ";";
		}

		std::cout << std::endl;
	}
}

void printAllRecords( const std::string & file, const std::string & filter )
{
	CSVUtil csv_util( file );

	if( !csv_util ) {
		throw REPORT_EXCEPTION( format( "Cannot open file %s ", file) );
	}

	FilterDataRecords data_filter;
	data_filter.addFilter( new FilterString( filter,  DataRecord::Record::FIELD_DATE ) );

	auto res = data_filter.filter(csv_util);

	int count = 0;
	for( auto csv_line : res )
	{
		std::cout << format( "%04d:", ++count ) << csv_line << std::endl;
	}
}

void printAllDateRecords( const std::string & file, time_t when )
{
	CSVUtil csv_util( file );

	if( !csv_util ) {
		throw REPORT_EXCEPTION( format( "Cannot open file %s ", file) );
	}

	FilterDataRecords data_filter;
	data_filter.addFilter( new FilterDay( when ) );

	auto res = data_filter.filter(csv_util);

	int count = 0;
	for( auto csv_line : res )
	{
		std::cout << format( "%04d:", ++count ) << csv_line << std::endl;
	}
}


void printAllRecordsReverse( const std::string & file )
{
	CSVUtil csv_util( file );

	if( !csv_util ) {
		throw REPORT_EXCEPTION( format( "Cannot open file %s ", file) );
	}

	int count = 0;

	for( auto it = csv_util.rbegin(); it != csv_util.rend(); it++ )
	{
		std::cout << format( "%04d:", ++count );

		for( std::string & rec : *it )
		{
			std::cout << rec << ";";
		}

		std::cout << std::endl;
	}
}



int main( int argc, char **argv )
{
  Co co;

  std::string data_file_name = "temperatures.csv";

  try {

    
  Arg::Arg arg( argc, argv );

  arg.addPrefix( "-" );
  arg.addPrefix( "--" );

  arg.setMinMatch(1);

  Arg::OptionChain oc_info;
  arg.addChainR( &oc_info );
  oc_info.setMinMatch( 1 );
  oc_info.setContinueOnMatch( false );
  oc_info.setContinueOnFail( true );

  Arg::FlagOption o_help( "h" );
  o_help.addName( "help" );
  o_help.setDescription( "Show this page" );
  oc_info.addOptionR( &o_help );

  Arg::FlagOption o_version( "v" );
  o_version.addName( "version" );
  o_version.setDescription( format( "Show %s version number", PACKAGE_NAME ) );
  oc_info.addOptionR( &o_version );


  Arg::OptionChain oc_path;
  arg.addChainR(&oc_path);
  oc_path.setMinMatch(1);
  oc_path.setContinueOnFail(true);
  oc_path.setContinueOnMatch(true);

  Arg::EmptyFileOption o_path(Arg::FILE::REGULAR);
  oc_path.addOptionR(&o_path);
  o_path.setMaxValues(1);
  o_path.setMinValues(1);
  o_path.setRequired(true);

  Arg::FlagOption o_debug("d");
  o_debug.addName("debug");
  o_debug.setDescription("print debugging messages");
  o_debug.setRequired(false);
  arg.addOptionR( &o_debug );

  Arg::OptionChain oc_print_stuff;
  arg.addChainR(&oc_print_stuff);

  Arg::IntOption o_print_last("l");
  oc_print_stuff.addOptionR(&o_print_last);
  o_print_last.addName("last");
  o_print_last.setDescription("print last %d days records");
  o_print_last.setMaxValues(1);
  o_print_last.setMinValues(1);
  o_print_last.setRequired(false);

  Arg::FlagOption o_print_all("a");
  oc_print_stuff.addOptionR(&o_print_all);
  o_print_all.addName("all");
  o_print_all.setDescription("print all records");
  o_print_all.setRequired(false);

  Arg::FlagOption o_print_rall("ra");
  oc_print_stuff.addOptionR(&o_print_rall);
  o_print_rall.addName("reverse-all");
  o_print_rall.setDescription("print all records reverse");
  o_print_rall.setRequired(false);

  Arg::StringOption o_print_filter("f");
  oc_print_stuff.addOptionR(&o_print_filter);
  o_print_filter.addName("filter");
  o_print_filter.setDescription("print filtered records");
  o_print_filter.setRequired(false);

  Arg::FlagOption o_print_today("today");
  oc_print_stuff.addOptionR(&o_print_today);
  o_print_today.setDescription("print today records");
  o_print_today.setRequired(false);

  const unsigned int console_width = 80;

  if( !arg.parse() )
    {
      if( o_version.getState() )
      {
    	  std::cout << format("%s version %s\n", argv[0], VERSION);
    	  return 0;
      } else {
    	  usage(argv[0]);
    	  std::cout << arg.getHelp(5,20,30, console_width ) << std::endl;
    	  return 1;
      }
    }

  if( o_help.getState() )
  {
	  usage(argv[0]);
	  std::cout << arg.getHelp(5,20,30, console_width ) << std::endl;
	  return 0;
  }

  if( o_debug.getState() )
  {
	  Tools::x_debug = new OutDebug();
  }

#if 0

  if( !o_replace.isSet() &&
	  !o_versid_remove.isSet() &&
	  !o_primanlist.isSet() &&
	  !o_mlm.isSet() &&
	  !o_restoreshell.isSet() &&
	  !o_correct_va_multiple_malloc.isSet() &&
	  !o_remove_generic_cast.isSet() &&
	  !o_compile_log.isSet() &&
	  !o_assign.isSet() &&
	  !o_sprintf.isSet() &&
	  !o_strform.isSet() &&
	  !o_prmget.isSet() &&
	  !o_fix_c_header_file.isSet() &&
	  !o_owcallback.isSet())
  {
	  usage(argv[0]);
	  std::cout << arg.getHelp(5,20,30, console_width ) << std::endl;
	  return 1;
  }
#endif

  if( o_path.isSet() ) {
	  data_file_name = *o_path.getValues()->begin();
  }

  if( o_print_last.isSet() )
  {
	  printLast( *o_print_last.getValues()->begin(), data_file_name );
	  return 0;
  }

  if( o_print_all.isSet() )
  {
	  printAllRecords( data_file_name );
	  return 0;
  }

  if( o_print_rall.isSet() )
  {
	  printAllRecordsReverse( data_file_name );
	  return 0;
  }

  if( o_print_filter.isSet() )
  {
	  printAllRecords( data_file_name, *o_print_filter.getValues()->begin() );
	  return 0;
  }

  if( o_print_today.isSet() )
  {
	  printAllDateRecords( data_file_name, time(0) );
	  return 0;
  }

  {
	  // Make application
	  FXApp application("CollectData","pi_sensor");
	  application.setNormalFont( new FXFont(&application,"helvetica,140") );

	  // Open display; this reads registry
	  application.init(argc,argv);

	  // Build a window
	  MainWindow *main = new MainWindow(&application);

	  if( o_path.isSet() ) {
		  main->setDataFileName( *o_path.getValues()->begin() );
	  }

	  // Create app
	  application.create();

	  // Run
	  return application.run();
  }

  } catch( std::exception & err ) {
	  std::cerr << "error: " << err.what() << std::endl;
	  return 10;
  }

  return 0;
}
