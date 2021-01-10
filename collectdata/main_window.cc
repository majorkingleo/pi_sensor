/*
 * main_window.cc
 *
 *  Created on: 10.01.2021
 *      Author: martin
 */
#include "main_window.h"
#include "theme.h"
#include <time.h>

using namespace Tools;

FXDEFMAP(MainWindow) MainWindowMap[]={
		FXMAPFUNC(SEL_TIMEOUT,MainWindow::ID_CLOCK_TIMER,MainWindow::onClockTimeout),
};

FXIMPLEMENT( MainWindow, FXMainWindow, MainWindowMap, ARRAYNUMBER( MainWindowMap));

MainWindow::MainWindow(FXApp *a)
:	FXMainWindow(a,"Show Data",NULL,NULL,DECOR_ALL,0,0,600,400)
{
	// Tooltip
	new FXToolTip(getApp());

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

	contents_currentdata = new FXHorizontalFrame(tabbook,FRAME_RAISED);
/*
	FXHorizontalFrame* boxframe=new FXHorizontalFrame(contents_currentdata,FRAME_THICK|FRAME_SUNKEN|LAYOUT_FILL_X|LAYOUT_FILL_Y, 0,0,0,0, 0,0,0,0);
	FXList* simplelist=new FXList(boxframe,NULL,0,LIST_EXTENDEDSELECT|LAYOUT_FILL_X|LAYOUT_FILL_Y);
	simplelist->appendItem("First Entry");
	simplelist->appendItem("Second Entry");
	simplelist->appendItem("Third Entry");
	simplelist->appendItem("Fourth Entry");
*/

	// create clock
	clock = new ThemeLabel( contents_currentdata, "00:00" );


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

