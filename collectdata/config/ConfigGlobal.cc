/*
 * Settings section "global", copied from lotr_analyzer/config/ConfigGlobal.cc
 * original source: https://github.com/majorkingleo/lotr_analyzer
 * author: Martin Oberzalek
 */
#include "ConfigGlobal.h"

using namespace Tools;

Configfile2::SectionKey<ConfigSectionGlobal> ConfigSectionGlobal::KEY( "global" );

ConfigSectionGlobal::ConfigSectionGlobal( const std::string & name_, Configfile2 *config_file_ )
: Section( name_, config_file_ ),
  LogFile( "LogFile", Configfile2::makePathsAbsolute( "~/.pi_sensor.log" ) )
{
	registerValue( &LogFile );
}


void ConfigSectionGlobal::registerSection( Configfile2 *config_file )
{
	config_file->registerSection( new ConfigSectionGlobal(ConfigSectionGlobal::KEY.name, config_file ) );
}
