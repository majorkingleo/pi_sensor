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


int main( int argc, char **argv )
{
  Co co;

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

  Arg::FlagOption o_debug("debug");
  o_debug.setDescription("print debugging messages");
  o_debug.setRequired(false);
  arg.addOptionR( &o_debug );

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
  if( !o_path.isSet() )
  {
	  usage(argv[0]);
	  std::cout << arg.getHelp(5,20,30, console_width ) << std::endl;

	  std::cout << "\n\n"
			  	<< co.color_output( ColoredOutput::BRIGHT_RED, "PATH is missing")
			  	<< "\n";

	  return 13;
  }

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


  {
	  // Make application
	  FXApp application("CollectData","pi_sensor");
	  application.setNormalFont( new FXFont(&application,"helvetica,140") );

	  // Open display; this reads registry
	  application.init(argc,argv);

	  // Build a window
	  new MainWindow(&application);

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
