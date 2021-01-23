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

using namespace Tools;

FXDEFMAP(MainWindow) MainWindowMap[]={
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CLOCK_TIMER,MainWindow::onClockTimeout),
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_DATA_TIMER,MainWindow::onDataTimeout),
};

FXIMPLEMENT( MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER( MainWindowMap));

MainWindow::MainWindow(FXApp *a)
:	FXMainWindow(a,"Show Data",NULL,NULL,DECOR_ALL,0,0,600,400),
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
}

void MainWindow::createDataFields( FXComposite *fdata, const char *title, FXDataTarget *degree_target,  FXDataTarget *humidity_target )
{
#if 0
	FXComposite *ftitle = new FXGroupBox(fdata, title, FRAME_RIDGE|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	FXComposite *f = new FXHorizontalFrame(ftitle, LAYOUT_FILL_Y);
	FX7Segment *seven=new FX7Segment(f,FXString::null,SEVENSEGMENT_NORMAL|JUSTIFY_RIGHT|LAYOUT_CENTER_Y|LAYOUT_FILL_ROW|LAYOUT_FILL_X);
	seven->setTarget(degree_target);
	seven->setSelector(FXDataTarget::ID_VALUE);

	FXProgressBar *bar = new FXProgressBar(f,degree_target,FXDataTarget::ID_VALUE,PROGRESSBAR_VERTICAL|PROGRESSBAR_NORMAL|LAYOUT_FILL_Y);
	bar->setTotal(50);
	bar->setBarSize( 50 );

	FXComposite *f_humidity = new FXVerticalFrame(f, LAYOUT_FILL_Y);

	FXProgressBar *humidity_bar = new FXProgressBar(f_humidity,humidity_target,FXDataTarget::ID_VALUE,PROGRESSBAR_DIAL| PROGRESSBAR_PERCENTAGE |LAYOUT_FILL_Y);
	humidity_bar->setTotal(100);

	FXComposite *f2 = new FXHorizontalFrame(f_humidity, LAYOUT_FILL_Y | LAYOUT_SIDE_BOTTOM );
	FXTextField *humidity_value = new FXTextField( f2, 5, humidity_target, FXDataTarget::ID_VALUE, FRAME_SUNKEN  | TEXTFIELD_READONLY );
	humidity_value->setJustify( JUSTIFY_RIGHT );
	new FXLabel( f2, "%" );
#else
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

#endif
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
	getApp()->addTimeout(this,ID_DATA_TIMER,1000);
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

