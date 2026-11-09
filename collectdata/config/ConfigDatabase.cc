/*
 * Settings section "database", copied from lotr_analyzer/config/ConfigDatabase.cc
 * original source: https://github.com/majorkingleo/lotr_analyzer
 * author: Martin Oberzalek
 */
#include "ConfigGlobal.h"
#include "cppdir.h"
#include "ConfigDatabase.h"

using namespace Tools;

Configfile2::SectionKey<ConfigSectionDatabase> ConfigSectionDatabase::KEY( "database" );


ConfigSectionDatabase::ConfigSectionDatabase( const std::string & name_, Configfile2 *config_file_ )
: Section( name_, config_file_ ),
  Host( "host", "localhost" ),
  UserName( "username", "pi_sensor" ),
  Password( "password", "" ),
  Instance( "instance", "pi_sensor" ),
  retry_db_timeout( "retry_db_timeout", 60 )
{
	registerValue( &Host );
	registerValue( &UserName );
	registerValue( &Password );
	registerValue( &Instance );
	registerValue( &retry_db_timeout );
}

void ConfigSectionDatabase::registerSection( Configfile2 *config_file )
{
	config_file->registerSection( new ConfigSectionDatabase(ConfigSectionDatabase::KEY.name, config_file ) );
}








