/*
 * main_window.h
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */

#ifndef MAIN_WINDOW_H_
#define MAIN_WINDOW_H_

#include <fx.h>

class MainWindow : public FXMainWindow
{
	FXDECLARE(MainWindow)
protected:

	// Member data
	FXMenuBar*         menubar;
	FXTabBook*         tabbook;
	FXTabItem*         tab1;
	FXHorizontalFrame* contents;
	FXHorizontalFrame* contents_currentdata;
	FXLabel*		   clock;

	MainWindow() {}

public:
	enum {
		ID_PANEL = FXMainWindow::ID_LAST,
		ID_CLOCK_TIMER,
	};

public:
	MainWindow( FXApp *a );
	virtual ~MainWindow();

	virtual void create();

	long onClockTimeout(FXObject*,FXSelector,void*);
};



#endif /* MAIN_WINDOW_H_ */
