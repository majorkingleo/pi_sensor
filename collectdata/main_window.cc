/*
 * main_window.cc
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */
#include "main_window.h"
#include "theme.h"
#include <time.h>
#include "csv_util.h"
#include <iostream>
#include "DataRecord.h"
#include <string_utils.h>
#include <debug.h>
#include <FilterDataRecords.h>
#include <FilterDay.h>
#include "FXPNGImage.h"
#include "FXJPGImage.h"
#include "Gen24HoursPlot.h"
#include "GenXDaysPlot.h"
#include "cppdir.h"

using namespace Tools;

FXDEFMAP(MainWindow) MainWindowMap[]={
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CLOCK_TIMER,MainWindow::onClockTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_DATA_TIMER,MainWindow::onDataTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_MINMAX_DATA_TIMER,MainWindow::onMinMaxDataTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_LAST24_HOUR_TIMER,MainWindow::onLast24HourTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_LAST14_DAYS_TIMER,MainWindow::onLast14DaysTimeout),
};

FXIMPLEMENT( MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER( MainWindowMap));

MainWindow::MainWindow(FXApp *a)
:	FXMainWindow(a,"Show Data",NULL,NULL,DECOR_ALL,0,0,1000,700),
	DATA_FILE_NAME( "temperatures.csv" ),
	degree_inside(0),
	humidity_inside(0),
	degree_outside(0),
	humidity_outside(0),
	lastDataRecord(0),
	IMAGE_FILE_LAST24_HOURS("dataLast24Hours.png"),
	IMAGE_FILE_LAST14_DAYS("dataLast14Days.png")
{
	// Tooltip
	new FXToolTip(getApp());

	// Status bar
	statusbar=new FXStatusBar(this,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|STATUSBAR_WITH_DRAGCORNER);

	status_line_error_color = FXRGB( 255, 0, 40 );
	status_line_normal_color = statusbar->getStatusLine()->getTextColor();

	// Menubar
	menubar=new FXMenuBar(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|FRAME_RAISED);
	FXMenuPane *filemenu=new FXMenuPane(this);
	new FXMenuCommand(filemenu,"&Quit the application",NULL,getApp(),FXApp::ID_QUIT);
	new FXMenuTitle(menubar,"&File",NULL,filemenu);

	// Separator
	new FXHorizontalSeparator(this,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

	// Contents
	contents=new FXHorizontalFrame(this,LAYOUT_SIDE_TOP|FRAME_NONE|LAYOUT_FILL_X|LAYOUT_FILL_Y|PACK_UNIFORM_WIDTH, 0,0,0,0, 0,0,0,0);

	// Switcher
	tabbook=new FXTabBook(contents,this,ID_PANEL,PACK_UNIFORM_WIDTH|PACK_UNIFORM_HEIGHT|LAYOUT_FILL_X|LAYOUT_FILL_Y|LAYOUT_RIGHT);

	// First item is a list
	tab1=new ThemeTabItem(tabbook,"&Current Data",NULL);

	contents_currentdata = new FXVerticalFrame(tabbook,FRAME_RAISED);
/*
	FXHorizontalFrame* boxframe=new FXHorizontalFrame(contents_currentdata,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXList* simplelist=new FXList(boxframe,NULL,0,LIST_EXTENDEDSELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	simplelist->appendItem("First Entry");
	simplelist->appendItem("Second Entry");
	simplelist->appendItem("Third Entry");
	simplelist->appendItem("Fourth Entry");
*/

	// create clock
	{
		FXHorizontalFrame *f = new FXHorizontalFrame(contents_currentdata);

		new ThemeLabel( f, "Current date and time:" );
		clock = new ThemeLabel( f, "00:00" );
	}

	new FXHorizontalSeparator(contents_currentdata,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

	{
		FXHorizontalFrame *f = new FXHorizontalFrame(contents_currentdata);

		new ThemeLabel( f, "Dataset date and time:" );
		dataset_date_and_time = new ThemeLabel( f, "00:00" );
	}

	new FXHorizontalSeparator(contents_currentdata,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

	degree_inside_target.connect(degree_inside);
	humidity_inside_target.connect(humidity_inside);

	degree_outside_target.connect(degree_outside);
	humidity_outside_target.connect(humidity_outside);

	FXComposite *fdata = new FXHorizontalFrame(contents_currentdata, LAYOUT_FILL_Y|LAYOUT_TOP);

	createDataFields( fdata, "Inside", &degree_inside_target, &humidity_inside_target );

	new FXHorizontalSeparator(fdata,LAYOUT_FILL_Y);

	createDataFields( fdata, "Outside", &degree_outside_target, &humidity_outside_target );



	// First item is a list
	tab2=new ThemeTabItem(tabbook,"&Min/Max last 24h",NULL);

	contents_min_max = new FXVerticalFrame(tabbook,FRAME_RAISED);

	FXComposite *fdata_min_max = new FXHorizontalFrame(contents_min_max, LAYOUT_FILL_Y|LAYOUT_TOP);

	createDataFields( fdata_min_max, "Inside Min",
					  &min_max[WHERE_INSIDE][MINMAX_MIN].target_degrees,
					  &min_max[WHERE_INSIDE][MINMAX_MIN].target_humidity );

	new FXHorizontalSeparator(fdata,LAYOUT_FILL_Y);

	createDataFields( fdata_min_max, "Inside Max",
						  &min_max[WHERE_INSIDE][MINMAX_MAX].target_degrees,
						  &min_max[WHERE_INSIDE][MINMAX_MAX].target_humidity );


	fdata_min_max = new FXHorizontalFrame(contents_min_max, LAYOUT_FILL_Y|LAYOUT_TOP);

	createDataFields( fdata_min_max, "Outside Min",
					  &min_max[WHERE_OUTSIDE][MINMAX_MIN].target_degrees,
					  &min_max[WHERE_OUTSIDE][MINMAX_MIN].target_humidity );

	new FXHorizontalSeparator(fdata,LAYOUT_FILL_Y);

	createDataFields( fdata_min_max, "Outside Max",
						  &min_max[WHERE_OUTSIDE][MINMAX_MAX].target_degrees,
						  &min_max[WHERE_OUTSIDE][MINMAX_MAX].target_humidity );




	// First item is a list
	tab3=new ThemeTabItem(tabbook,"&Temp last 24h",NULL);
	contents_temp_last24 = new FXVerticalFrame(tabbook,FRAME_RAISED);

	FXComposite *fdata_temp_last24 = new FXHorizontalFrame(contents_temp_last24, LAYOUT_FILL_Y|LAYOUT_FILL_X);
	imageview_last24hours=new FXImageView(fdata_temp_last24,NULL,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y);


	tab3=new ThemeTabItem(tabbook,"&Temp last 14d",NULL);
	contents_temp_last14days = new FXVerticalFrame(tabbook,FRAME_RAISED);

	FXComposite *fdata_temp_last14days = new FXHorizontalFrame(contents_temp_last14days, LAYOUT_FILL_Y|LAYOUT_FILL_X);
	imageview_last14days=new FXImageView(fdata_temp_last14days,NULL,NULL,0,LAYOUT_FILL_X|LAYOUT_FILL_Y);
}

void MainWindow::createDataFields( FXComposite *fdata, const char *title, FXDataTarget *degree_target,  FXDataTarget *humidity_target )
{

	FXComposite *ftitle = new FXGroupBox(fdata, title, FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXMatrix *f = new FXMatrix( ftitle, 2, MATRIX_BY_COLUMNS );

	FXProgressBar *bar = new FXProgressBar(f,degree_target,FXDataTarget::ID_VALUE,PROGRESSBAR_VERTICAL|PROGRESSBAR_NORMAL|LAYOUT_CENTER_X|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	bar->setTotal(50);
	bar->setBarSize( 50 );

	FXProgressBar *humidity_bar = new FXProgressBar(f,humidity_target,FXDataTarget::ID_VALUE,PROGRESSBAR_DIAL| PROGRESSBAR_PERCENTAGE |LAYOUT_CENTER_X|LAYOUT_FILL_X);
	humidity_bar->setTotal(100);
	humidity_bar->setBarSize(100);


	FXComposite *f1 = new FXHorizontalFrame(f);
	FXTextField *degree_value = new FXTextField( f1, 5, degree_target, FXDataTarget::ID_VALUE, FRAME_SUNKEN  | TEXTFIELD_READONLY );
	degree_value->setJustify( JUSTIFY_RIGHT );
	new FXLabel( f1, "°" );


	FXComposite *f2 = new FXHorizontalFrame(f);
	FXTextField *humidity_value = new FXTextField( f2, 5, humidity_target, FXDataTarget::ID_VALUE, FRAME_SUNKEN  | TEXTFIELD_READONLY );
	humidity_value->setJustify( JUSTIFY_RIGHT );
	new FXLabel( f2, "%" );
}

MainWindow::~MainWindow()
{

}

// Show up
void MainWindow::create()
{
   init_theme(getApp());

   // Reset timer for next time
   getApp()->addTimeout(this,ID_CLOCK_TIMER,0);
   getApp()->addTimeout(this,ID_DATA_TIMER,0);
   getApp()->addTimeout(this,ID_MINMAX_DATA_TIMER,1000);

   loadIfExists( IMAGE_FILE_LAST24_HOURS, imageview_last24hours);
   getApp()->addTimeout(this,ID_LAST24_HOUR_TIMER,2000);


   if( loadIfExists( IMAGE_FILE_LAST14_DAYS, imageview_last14days) ) {
	   getApp()->addTimeout(this,ID_LAST14_DAYS_TIMER,20000);
   } else {
	   getApp()->addTimeout(this,ID_LAST14_DAYS_TIMER,2500);
   }

   FXMainWindow::create();

   show(PLACEMENT_SCREEN);
}

long MainWindow::onClockTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	struct tm *tmp = localtime(&now);

	char acBuffer[100];
	strftime( acBuffer, sizeof(acBuffer), "%F %H:%M:%S", tmp );

	clock->setText( acBuffer );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_CLOCK_TIMER,1000);
	return 1;
}

long MainWindow::onDataTimeout(FXObject*,FXSelector,void*)
{
	CSVUtil csv_util( DATA_FILE_NAME );

	static double delta;
	// delta += 1;

	if( !csv_util ) {
		std::cerr << "cannot open file " << "temperatures.csv" << std::endl;
	}

	std::vector<std::string> record = csv_util.getLastLine();

	if( record.size() >= DataRecord::MAX_FIELDS ) {

		// date and time
		std::string date_and_time = record[DataRecord::FIELD_DATE] + " " + record[DataRecord::FIELD_TIME];
		dataset_date_and_time->setText( date_and_time.c_str() );

		lastDataRecord = getDateAndTime( date_and_time );

		// degree
		degree_inside = s2x<double>(record[DataRecord::FIELD_DEGREE_INSIDE], 0.0);
		degree_inside += delta;

		// humidity
		humidity_inside = s2x<double>(record[DataRecord::FIELD_HUMIDITY_INSIDE], 0.0);
		humidity_inside += delta;


		// degree
		degree_outside = s2x<double>(record[DataRecord::FIELD_DEGREE_OUTSIDE], 0.0);
		degree_outside += delta;

		// humidity
		humidity_outside = s2x<double>(record[DataRecord::FIELD_HUMIDITY_OUTSIDE], 0.0);
		humidity_outside += delta;

		time_t now = time(0);

		if( lastDataRecord == 0 ) {

			setStatusNotOk( "Cannot read last dataset entry" );

		} else if( lastDataRecord < (now - 60*10) ) {

			long age_in_seconds = (now - lastDataRecord);

			if( age_in_seconds > 60 ) {

				if( (age_in_seconds / 60 / 60) > 24 ) {
					setStatusNotOk( format( "Last temperature record is already %d days old.", (age_in_seconds / 60 / 60 / 24) ) );
				} else {
					setStatusNotOk( format( "Last temperature record is already %d hours old.", (age_in_seconds / 60 / 60) ) );
				}
			} else {
				setStatusNotOk( format( "Last temperature record is already %d minutes old.", (age_in_seconds / 60 ) ) );
			}

		} else {
			setStatusOk();
		}
	}


	// Reset timer for next time
	getApp()->addTimeout(this,ID_DATA_TIMER,5000);
	return 1;
}

void MainWindow::setStatusOk()
{
	statusbar->getStatusLine()->setNormalText("");
	statusbar->getStatusLine()->setTextColor( status_line_normal_color );
}

void MainWindow::setStatusNotOk( const std::string & message )
{
	statusbar->getStatusLine()->setNormalText(message.c_str());
	statusbar->getStatusLine()->setTextColor( status_line_error_color );
}

time_t MainWindow::getDateAndTime( const std::string & date_time_str )
{
	struct tm tm = {};
	if( strptime( date_time_str.c_str(), "%Y-%m-%d %H:%M:%S", &tm ) != 0 )
	{
		time_t timestamp = mktime( &tm );

		if( timestamp == (time_t)-1) {
			DEBUG( format( "Error converting time: %s to a time", date_time_str ) );
			return 0;
		}

		return timestamp;
	}

	DEBUG( format( "Error converting time: %s to a time", date_time_str ) );

	return 0;
}

long MainWindow::onMinMaxDataTimeout(FXObject*,FXSelector,void*)
{
	CSVUtil csv_util( DATA_FILE_NAME );


	if( !csv_util ) {
		std::cerr << "cannot open file " << "temperatures.csv" << std::endl;
	}

	time_t start_at = time(0)-24*60*60;

	FilterDataRecords data_filter;
	data_filter.setSeekdir(FilterDataRecords::SEEKDIR_BACKWARDS);
	data_filter.setStrategy(FilterDataRecords::STRATEGY_ANY_FILTERS_IS_MATCHING);
	data_filter.addFilter( new FilterDay( start_at ) );
	data_filter.addFilter( new FilterDay( time(0) ) );

	auto res = data_filter.filter(csv_util);

	std::list<DataRecord> entries;

	for( auto csv_line : res )
	{
		entries.push_back( DataRecord( csv_line ) );
	}

	entries.sort(DataRecord::sort_by_time_asc());

	// reset min/max
	for( auto & row : min_max )
	{
		for( auto & pos : row )
		{
			pos.value_degrees = 0;
			pos.value_humidity = 0;
			pos.initialized = false;
		}
	}

	for( auto rec : entries )
	{
		if( rec.timestamp < start_at ) {
			continue;
		}

		if( !min_max[WHERE_INSIDE][MINMAX_MIN].initialized ) {
			 min_max[WHERE_INSIDE][MINMAX_MIN].value_degrees = rec.degrees_inside;
			 min_max[WHERE_INSIDE][MINMAX_MIN].value_humidity = rec.humidity_inside;
			 min_max[WHERE_INSIDE][MINMAX_MIN].initialized = true;
		}

		if( !min_max[WHERE_INSIDE][MINMAX_MAX].initialized ) {
			 min_max[WHERE_INSIDE][MINMAX_MAX].value_degrees = rec.degrees_inside;
			 min_max[WHERE_INSIDE][MINMAX_MAX].value_humidity = rec.humidity_inside;
			 min_max[WHERE_INSIDE][MINMAX_MAX].initialized = true;
		}

		if( !min_max[WHERE_OUTSIDE][MINMAX_MIN].initialized ) {
			 min_max[WHERE_OUTSIDE][MINMAX_MIN].value_degrees = rec.degrees_outside;
			 min_max[WHERE_OUTSIDE][MINMAX_MIN].value_humidity = rec.humidity_outside;
			 min_max[WHERE_OUTSIDE][MINMAX_MIN].initialized = true;
		}

		if( !min_max[WHERE_OUTSIDE][MINMAX_MAX].initialized ) {
			 min_max[WHERE_OUTSIDE][MINMAX_MAX].value_degrees = rec.degrees_outside;
			 min_max[WHERE_OUTSIDE][MINMAX_MAX].value_humidity = rec.humidity_outside;
			 min_max[WHERE_OUTSIDE][MINMAX_MAX].initialized = true;
		}

		min( min_max[WHERE_INSIDE][MINMAX_MIN].value_degrees, rec.degrees_inside );
		min( min_max[WHERE_INSIDE][MINMAX_MIN].value_humidity, rec.humidity_inside );

		min( min_max[WHERE_OUTSIDE][MINMAX_MIN].value_degrees, rec.degrees_outside );
		min( min_max[WHERE_OUTSIDE][MINMAX_MIN].value_humidity, rec.humidity_outside );

		max( min_max[WHERE_INSIDE][MINMAX_MAX].value_degrees, rec.degrees_inside );
		max( min_max[WHERE_INSIDE][MINMAX_MAX].value_humidity, rec.humidity_inside );

		max( min_max[WHERE_OUTSIDE][MINMAX_MAX].value_degrees, rec.degrees_outside );
		max( min_max[WHERE_OUTSIDE][MINMAX_MAX].value_humidity, rec.humidity_outside );
	}

	// Reset timer for next time
	getApp()->addTimeout(this,ID_MINMAX_DATA_TIMER,1000*5*60);
	return 1;
}

void MainWindow::min( FXint & value, float & rec_value )
{
	value = std::min( value, static_cast<FXint>(rec_value) );
}

void MainWindow::max( FXint & value, float & rec_value )
{
	value = std::max( value, static_cast<FXint>(rec_value) );
}

bool MainWindow::loadImage(const FXString& file, FXImageView *imageview)
{
	FXString ext=FXPath::extension(file);
	FXImage *img=NULL;
	FXImage *old;
	if(comparecase(ext,"gif")==0){
		img=new FXGIFImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"bmp")==0){
		img=new FXBMPImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"xpm")==0){
		img=new FXXPMImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"pcx")==0){
		img=new FXPCXImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"ico")==0 || comparecase(ext,"cur")==0){
		img=new FXICOImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"tga")==0){
		img=new FXTGAImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"rgb")==0){
		img=new FXRGBImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"xbm")==0){
		img=new FXXBMImage(getApp(),NULL,NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"ppm")==0){
		img=new FXPPMImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"iff")==0 || comparecase(ext,"lbm")==0){
		img=new FXIFFImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"ras")==0){
		img=new FXRASImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"png")==0){
		img=new FXPNGImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}
	else if(comparecase(ext,"jpg")==0){
		img=new FXJPGImage(getApp(),NULL,IMAGE_KEEP|IMAGE_SHMI|IMAGE_SHMP);
	}

	// Perhaps failed
	if(img==NULL){
		FXMessageBox::error(this,MBOX_OK,"Error Loading Image","Unsupported type: %s",ext.text());
		return FALSE;
	}

	// Load it
	FXFileStream stream;
	if(stream.open(file,FXStreamLoad)){
		getApp()->beginWaitCursor();
		img->loadPixels(stream);
		stream.close();

		//img->gradient(FXRGB(255,0,0),FXRGB(0,255,0),FXRGB(0,0,255),FXRGB(255,255,255));
		//img->gradient(FXRGB(255,0,0),FXRGB(255,255,0),FXRGB(0,0,255),FXRGB(255,0,255));
		//img->hgradient(FXRGB(255,0,0),FXRGB(0,0,255));
		//img->vgradient(FXRGB(255,0,0),FXRGB(0,0,255));
		//img->fill(FXRGB(255,0,0));
		//img->fade(FXRGB(255,255,255),128);
		//img->blend(getApp()->getBaseColor());
		//img->blend(FXRGB(255,128,255));
		//img->xshear(-30*256,FXRGB(0,255,128));
		//img->yshear(-50*256,FXRGB(0,255,128));


		img->create();
		old=imageview->getImage();
		imageview->setImage(img);
		delete old;
		getApp()->endWaitCursor();
	}
	return TRUE;
}


long MainWindow::onLast24HourTimeout(FXObject*,FXSelector,void*)
{
	CSVUtil csv_util( DATA_FILE_NAME );

	if( !csv_util ) {
		std::cerr << "cannot open file " << "temperatures.csv" << std::endl;
	}

	time_t start_at = time(0)-24*60*60;

	FilterDataRecords data_filter;
	data_filter.setSeekdir(FilterDataRecords::SEEKDIR_BACKWARDS);
	data_filter.setStrategy(FilterDataRecords::STRATEGY_ANY_FILTERS_IS_MATCHING);
	data_filter.addFilter( new FilterDay( start_at) );
	data_filter.addFilter( new FilterDay( time(0) ) );

	auto res = data_filter.filter(csv_util);

	int count = 0;

	std::list<DataRecord> entries;

	for( auto csv_line : res )
	{
		entries.push_back( DataRecord( csv_line ) );
	}

	entries.sort(DataRecord::sort_by_time_asc());

	count = 0;

	auto it_begin_with_time = entries.end();

	for( auto it = entries.begin(); it != entries.end(); it++ )
	{
		auto & rec = *it;

		if( rec.timestamp < start_at ) {
			continue;
		}

		if( it_begin_with_time == entries.end() ) {
			it_begin_with_time = it;
		}
	}

	Gen24HoursPlot gen;

	const char * dataFile = "dataLast24Hours";

	gen.setImageFileName( IMAGE_FILE_LAST24_HOURS );
	gen.setDataFileName( dataFile );
	gen.setTitle( "Temperatures Last 24 Hours");

	std::vector<std::string> data_outside;
	std::vector<std::string> data_inside;
	std::vector<std::string> time_stamps;

	for( auto it = it_begin_with_time; it != entries.end(); it++ )
	{
		data_outside.push_back( x2s(it->degrees_outside) );
		data_inside.push_back( x2s(it->degrees_inside) );
		time_stamps.push_back( it->getDateTimeStr( it->timestamp ) );
	}

	gen.addData( "Outside", data_outside );
	gen.addData( "Inside", data_inside );
	gen.setXDescriptionValues( time_stamps );
	gen.create();

	loadImage( IMAGE_FILE_LAST24_HOURS.c_str(), imageview_last24hours);

	// Reset timer for next time
	getApp()->addTimeout(this,ID_LAST24_HOUR_TIMER,1000*5*60);
	return 1;
}



long MainWindow::onLast14DaysTimeout(FXObject*,FXSelector,void*)
{
	CSVUtil csv_util( DATA_FILE_NAME );

	if( !csv_util ) {
		std::cerr << "cannot open file " << "temperatures.csv" << std::endl;
	}

	time_t start_at = time(0)-24*60*60*14;
	time_t now = time(0);

	FilterDataRecords data_filter;
	data_filter.setSeekdir(FilterDataRecords::SEEKDIR_BACKWARDS);
	data_filter.setStrategy(FilterDataRecords::STRATEGY_ANY_FILTERS_IS_MATCHING);

	for( time_t x = start_at; x < now; x += 24*60*60 )
	{
		data_filter.addFilter( new FilterDay( x ) );
	}

	auto res = data_filter.filter(csv_util);

	int count = 0;

	std::list<DataRecord> entries;

	for( auto csv_line : res )
	{
		entries.push_back( DataRecord( csv_line ) );
	}

	entries.sort(DataRecord::sort_by_time_asc());

	count = 0;

	auto it_begin_with_time = entries.end();

	for( auto it = entries.begin(); it != entries.end(); it++ )
	{
		auto & rec = *it;

		if( rec.timestamp < start_at ) {
			continue;
		}

		if( it_begin_with_time == entries.end() ) {
			it_begin_with_time = it;
		}
	}

	GenXDaysPlot gen;

	const char * dataFile = "dataLast14Days";

	gen.setImageFileName( IMAGE_FILE_LAST14_DAYS );
	gen.setDataFileName( dataFile );
	gen.setTitle( "Temperatures Last 14 Days");

	std::vector<std::string> data_outside;
	std::vector<std::string> data_inside;
	std::vector<std::string> time_stamps;

	for( auto it = it_begin_with_time; it != entries.end(); it++ )
	{
		data_outside.push_back( x2s(it->degrees_outside) );
		data_inside.push_back( x2s(it->degrees_inside) );
		time_stamps.push_back( it->getDateTimeStr( it->timestamp ) );
	}

	gen.addData( "Outside", data_outside );
	gen.addData( "Inside", data_inside );
	gen.setXDescriptionValues( time_stamps );
	gen.create();

	loadImage(IMAGE_FILE_LAST14_DAYS.c_str(),imageview_last14days);

	// Reset timer for next time
	getApp()->addTimeout(this,ID_LAST14_DAYS_TIMER,1000*5*60);
	return 1;
}

bool MainWindow::loadIfExists( const std::string & file, FXImageView *imageview )
{
	CppDir::File f( file.c_str() );

	if( f.is_valid() ) {
		return loadImage(file.c_str(),imageview);
	}

	return false;
}




