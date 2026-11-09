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
	struct MinMax
	{
		FXDataTarget target_degrees;
		FXint		 value_degrees;
		FXDataTarget target_humidity;
		FXint		 value_humidity;
		bool 		 initialized;

		MinMax()
		: target_degrees(),
		  value_degrees(0),
		  target_humidity(),
		  value_humidity(0),
		  initialized(true)
		{
			target_degrees.connect( value_degrees );
			target_humidity.connect( value_humidity );
		}
	};

	enum WHERE {
		WHERE_INSIDE = 0,
		WHERE_OUTSIDE
	};

	enum MINMAX {
		MINMAX_MIN = 0,
		MINMAX_MAX
	};

protected:

	// Member data
	FXMenuBar*         menubar;
	FXTabBook*         tabbook;
	FXTabItem*         tab1;
	FXTabItem*         tab2;
	FXTabItem*         tab3;
	FXHorizontalFrame* contents;
	FXVerticalFrame*   contents_currentdata;
	FXVerticalFrame*   contents_min_max;
	FXVerticalFrame*   contents_temp_last24;
	FXVerticalFrame*   contents_temp_last14days;
	FXLabel*		   clock;
	FXLabel*		   dataset_date_and_time;
	FXImageView*	   imageview_last24hours;
	FXImageView*	   imageview_last14days;

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

	MinMax	   		   min_max[2][2];

	std::string 	   IMAGE_FILE_LAST24_HOURS;
	std::string 	   IMAGE_FILE_LAST14_DAYS;

	MainWindow() {}

public:
	enum {
		ID_PANEL = FXMainWindow::ID_LAST,
		ID_CLOCK_TIMER,
		ID_DATA_TIMER,
		ID_MINMAX_DATA_TIMER,
		ID_LAST24_HOUR_TIMER,
		ID_LAST14_DAYS_TIMER,
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
	long onMinMaxDataTimeout(FXObject*,FXSelector,void*);
	long onLast24HourTimeout(FXObject*,FXSelector,void*);
	long onLast14DaysTimeout(FXObject*,FXSelector,void*);

protected:
	void createDataFields( FXComposite *fdata, const char *title, FXDataTarget *degree_target,  FXDataTarget *humidity_target );
	void setStatusOk();
	void setStatusNotOk( const std::string & message );
	time_t getDateAndTime( const std::string & date_time_str );

	void min( FXint & value, float & rec_value );
	void max( FXint & value, float & rec_value );

	bool loadImage(const FXString& file, FXImageView *imageview );
	bool loadIfExists( const std::string & file, FXImageView *imageview );
};



#endif /* MAIN_WINDOW_H_ */
