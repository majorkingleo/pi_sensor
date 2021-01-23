/*
 * main_window.h
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <fx.h>
#include <string>

class MainWindow : public FXMainWindow
{
	FXDECLARE(MainWindow)
protected:

	// Member data
	FXMenuBar*         menubar;
	FXTabBook*         tabbook;
	FXTabItem*         tab1;
	FXTabItem*         tab2;
	FXHorizontalFrame* contents;
	FXVerticalFrame*   contents_currentdata;
	FXVerticalFrame*   contents_min_max;
	FXLabel*		   clock;
	FXLabel*		   dataset_date_and_time;

	FXDataTarget       degree_inside_target;
	FXint	           degree_inside;

	FXDataTarget       humidity_inside_target;
	FXint	           humidity_inside;

	FXDataTarget       degree_outside_target;
	FXint	           degree_outside;

	FXDataTarget       humidity_outside_target;
	FXint	           humidity_outside;

	FXStatusBar*       statusbar;
	FXColor			   status_line_error_color;
	FXColor			   status_line_normal_color;

	time_t			   lastDataRecord;

	std::string 	   DATA_FILE_NAME;

	MainWindow() {}

public:
	enum {
		ID_PANEL = FXMainWindow::ID_LAST,
		ID_CLOCK_TIMER,
		ID_DATA_TIMER,
	};

public:
	MainWindow( FXApp *a );
	virtual ~MainWindow();

	virtual void create();

	void setDataFileName( const std::string & file_name ) {
		DATA_FILE_NAME = file_name;
	}

	long onClockTimeout(FXObject*,FXSelector,void*);
	long onDataTimeout(FXObject*,FXSelector,void*);

protected:
	void createDataFields( FXComposite *fdata, const char *title, FXDataTarget *degree_target,  FXDataTarget *humidity_target );
	void setStatusOk();
	void setStatusNotOk( const std::string & message );
	time_t getDateAndTime( const std::string & date_time_str );
};



#endif /* MAIN_WINDOW_H_ */
