/*
 * main_window.cc
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */
#include "main_window.h"
#include <time.h>
#include "csv_util.h"
#include <iostream>
#include "DataRecord.h"
#include <string_utils.h>
#include <debug.h>
#include <FilterDataRecords.h>
#include <FilterDay.h>
#include "FilterDateRange.h"

using namespace Tools;

// FOX timers are given in nanoseconds
static const FXTime MILLISECOND = 1000000;

namespace
{
	// the colors of the two temperature curves
	const FXColor COLOR_OUTSIDE = FXRGB( 220, 50, 47 );
	const FXColor COLOR_INSIDE  = FXRGB( 38, 139, 210 );

	std::string formatDateTime( time_t timestamp )
	{
		struct tm tmp;

		if( localtime_r( &timestamp, &tmp ) == nullptr ) {
			return std::string();
		}

		char buffer[100];
		strftime( buffer, sizeof(buffer), "%a %d.%m. %H:%M", &tmp );

		return buffer;
	}

	// start (00:00:00) of the month that contains when, shifted by add_months
	time_t firstOfMonth( time_t when, int add_months )
	{
		struct tm tmp;

		if( localtime_r( &when, &tmp ) == nullptr ) {
			return when;
		}

		tmp.tm_mday = 1;
		tmp.tm_hour = 0;
		tmp.tm_min = 0;
		tmp.tm_sec = 0;
		tmp.tm_mon += add_months;
		tmp.tm_isdst = -1;

		time_t start = mktime( &tmp );

		return start == (time_t)-1 ? when : start;
	}

	// start (01.01. 00:00:00) of the year that contains when
	time_t firstOfYear( time_t when )
	{
		struct tm tmp;

		if( localtime_r( &when, &tmp ) == nullptr ) {
			return when;
		}

		tmp.tm_mday = 1;
		tmp.tm_mon = 0;
		tmp.tm_hour = 0;
		tmp.tm_min = 0;
		tmp.tm_sec = 0;
		tmp.tm_isdst = -1;

		time_t start = mktime( &tmp );

		return start == (time_t)-1 ? when : start;
	}

	/**
	 * Feed the charts. The x values are the age of the records in seconds
	 * relative to start_at, expressed in seconds_per_unit units, so the axis
	 * numbers stay small. The real date and time is shown by the cursor
	 * overlay and in the caption.
	 */
	void fillPlot( PlotView * plot, const std::list<DataRecord> & entries, time_t start_at,
				   double seconds_per_unit, const std::string & unit, const std::string & title )
	{
		PlotSeries outside;
		outside.label = "Outside";
		outside.color = COLOR_OUTSIDE;

		PlotSeries inside;
		inside.label = "Inside";
		inside.color = COLOR_INSIDE;

		time_t last = start_at;

		for( const DataRecord & rec : entries )
		{
			if( rec.timestamp < start_at ) {
				continue;
			}

			double x = static_cast<double>( rec.timestamp - start_at ) / seconds_per_unit;

			outside.x.push_back( x );
			outside.y.push_back( rec.degrees_outside );

			inside.x.push_back( x );
			inside.y.push_back( rec.degrees_inside );

			last = std::max( last, rec.timestamp );
		}

		std::string caption = title;

		if( !outside.x.empty() ) {
			caption += "\n" + formatDateTime( start_at ) + " - " + formatDateTime( last );
		} else {
			caption += "\nno data";
		}

		plot->setTimeBase( start_at, seconds_per_unit, unit );
		plot->setCaption( caption.c_str() );
		plot->setSeries( { outside, inside } );
	}
}

FXDEFMAP(MainWindow) MainWindowMap[]={
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CLOCK_TIMER,MainWindow::onClockTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_DATA_TIMER,MainWindow::onDataTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_MINMAX_DATA_TIMER,MainWindow::onMinMaxDataTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_LAST24_HOUR_TIMER,MainWindow::onLast24HourTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_LAST14_DAYS_TIMER,MainWindow::onLast14DaysTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_LAST_MONTH_TIMER,MainWindow::onLastMonthTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CURRENT_MONTH_TIMER,MainWindow::onCurrentMonthTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CURRENT_YEAR_TIMER,MainWindow::onCurrentYearTimeout),
};

FXIMPLEMENT( MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER( MainWindowMap));

MainWindow::MainWindow(FXApp *a)
:	FXMainWindow(a,"Show Data",NULL,NULL,DECOR_ALL,0,0,1000,700),
	DATA_FILE_NAME( "temperatures.csv" ),
	degree_inside(0),
	humidity_inside(0),
	degree_outside(0),
	humidity_outside(0),
	lastDataRecord(0)
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
	tab1=new FXTabItem(tabbook,"&Current Data",NULL);

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

		new FXLabel( f, "Current date and time:" );
		clock = new FXLabel( f, "00:00" );
	}

	new FXHorizontalSeparator(contents_currentdata,LAYOUT_SIDE_TOP|LAYOUT_FILL_X|SEPARATOR_GROOVE);

	{
		FXHorizontalFrame *f = new FXHorizontalFrame(contents_currentdata);

		new FXLabel( f, "Dataset date and time:" );
		dataset_date_and_time = new FXLabel( f, "00:00" );
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
	tab2=new FXTabItem(tabbook,"&Min/Max last 24h",NULL);

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
	tab3=new FXTabItem(tabbook,"&Temp last 24h",NULL);
	contents_temp_last24 = new FXVerticalFrame(tabbook,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	plot_last24hours = new PlotView(contents_temp_last24, CAPTION_ABOVE|CAPTION_SHOWN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(contents_temp_last24,"Drag a rectangle to zoom, double click to reset the zoom",NULL,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|JUSTIFY_CENTER_X);


	tab3=new FXTabItem(tabbook,"&Temp last 14d",NULL);
	contents_temp_last14days = new FXVerticalFrame(tabbook,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	plot_last14days = new PlotView(contents_temp_last14days, CAPTION_ABOVE|CAPTION_SHOWN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(contents_temp_last14days,"Drag a rectangle to zoom, double click to reset the zoom",NULL,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|JUSTIFY_CENTER_X);

	tab3=new FXTabItem(tabbook,"Temp last &month",NULL);
	contents_temp_lastmonth = new FXVerticalFrame(tabbook,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	plot_lastmonth = new PlotView(contents_temp_lastmonth, CAPTION_ABOVE|CAPTION_SHOWN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(contents_temp_lastmonth,"Drag a rectangle to zoom, double click to reset the zoom",NULL,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|JUSTIFY_CENTER_X);

	tab3=new FXTabItem(tabbook,"Temp this m&onth",NULL);
	contents_temp_currentmonth = new FXVerticalFrame(tabbook,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	plot_currentmonth = new PlotView(contents_temp_currentmonth, CAPTION_ABOVE|CAPTION_SHOWN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(contents_temp_currentmonth,"Drag a rectangle to zoom, double click to reset the zoom",NULL,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|JUSTIFY_CENTER_X);

	tab3=new FXTabItem(tabbook,"Temp this &year",NULL);
	contents_temp_currentyear = new FXVerticalFrame(tabbook,FRAME_RAISED|LAYOUT_FILL_X|LAYOUT_FILL_Y);

	plot_currentyear = new PlotView(contents_temp_currentyear, CAPTION_ABOVE|CAPTION_SHOWN|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	new FXLabel(contents_temp_currentyear,"Drag a rectangle to zoom, double click to reset the zoom",NULL,LAYOUT_SIDE_BOTTOM|LAYOUT_FILL_X|JUSTIFY_CENTER_X);
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
   // The colors and the font come from the desktop theme; a palette set here
   // would reach the widgets only after they were constructed, so the widgets
   // keep the desktop colors while the themed painting would use the palette.

   // Reset timer for next time
   getApp()->addTimeout(this,ID_CLOCK_TIMER,0);
   getApp()->addTimeout(this,ID_DATA_TIMER,0);
   getApp()->addTimeout(this,ID_MINMAX_DATA_TIMER,1000*MILLISECOND);

   // the charts are filled from the data file, no pre-rendered images any more
   getApp()->addTimeout(this,ID_LAST24_HOUR_TIMER,0);
   getApp()->addTimeout(this,ID_LAST14_DAYS_TIMER,1000*MILLISECOND);
   getApp()->addTimeout(this,ID_LAST_MONTH_TIMER,1500*MILLISECOND);
   getApp()->addTimeout(this,ID_CURRENT_MONTH_TIMER,2000*MILLISECOND);
   getApp()->addTimeout(this,ID_CURRENT_YEAR_TIMER,2500*MILLISECOND);

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
	getApp()->addTimeout(this,ID_CLOCK_TIMER,1000*MILLISECOND);
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
	getApp()->addTimeout(this,ID_DATA_TIMER,5000*MILLISECOND);
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
	getApp()->addTimeout(this,ID_MINMAX_DATA_TIMER,1000*5*60*MILLISECOND);
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

void MainWindow::loadPlotRange( PlotView * plot, time_t start_at, time_t end_at,
								double seconds_per_unit, const std::string & unit,
								const std::string & title )
{
	CSVUtil csv_util( DATA_FILE_NAME );

	if( !csv_util ) {
		std::cerr << "cannot open file " << DATA_FILE_NAME << std::endl;
	}

	FilterDataRecords data_filter;
	data_filter.setSeekdir(FilterDataRecords::SEEKDIR_BACKWARDS);
	data_filter.setStrategy(FilterDataRecords::STRATEGY_ALL_FILTERS_ARE_MATCHING);
	data_filter.addFilter( new FilterDateRange( start_at, end_at ) );

	auto res = data_filter.filter(csv_util);

	std::list<DataRecord> entries;

	for( auto csv_line : res )
	{
		entries.push_back( DataRecord( csv_line ) );
	}

	entries.sort(DataRecord::sort_by_time_asc());

	fillPlot( plot, entries, start_at, seconds_per_unit, unit, title );
}

long MainWindow::onLast24HourTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	loadPlotRange( plot_last24hours, now-24*60*60, now, 60*60.0, "hours", "Temperatures Last 24 Hours" );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_LAST24_HOUR_TIMER,1000*5*60*MILLISECOND);
	return 1;
}

long MainWindow::onLast14DaysTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	loadPlotRange( plot_last14days, now-14*24*60*60, now, 24*60*60.0, "days", "Temperatures Last 14 Days" );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_LAST14_DAYS_TIMER,1000*60*60*MILLISECOND);
	return 1;
}

long MainWindow::onLastMonthTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	// the last complete calendar month
	time_t start_at = firstOfMonth( now, -1 );
	time_t end_at = firstOfMonth( now, 0 ) - 1;

	loadPlotRange( plot_lastmonth, start_at, end_at, 24*60*60.0, "days", "Temperatures Last Month" );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_LAST_MONTH_TIMER,1000*60*60*MILLISECOND);
	return 1;
}

long MainWindow::onCurrentMonthTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	loadPlotRange( plot_currentmonth, firstOfMonth( now, 0 ), now, 24*60*60.0, "days", "Temperatures This Month" );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_CURRENT_MONTH_TIMER,1000*60*15*MILLISECOND);
	return 1;
}

long MainWindow::onCurrentYearTimeout(FXObject*,FXSelector,void*)
{
	time_t now = time(0);

	loadPlotRange( plot_currentyear, firstOfYear( now ), now, 24*60*60.0, "days", "Temperatures This Year" );

	// Reset timer for next time
	getApp()->addTimeout(this,ID_CURRENT_YEAR_TIMER,1000*60*60*MILLISECOND);
	return 1;
}
