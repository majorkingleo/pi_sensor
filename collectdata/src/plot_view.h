/*
 * plot_view.h
 *
 * Interactive line chart based on the FOX 2.0 chart classes (FXChart),
 * replaces the gnuplot generated PNG images of the temperature tabs.
 */

#ifndef PLOT_VIEW_H_
#define PLOT_VIEW_H_

#include <fx.h>
#include <FXCharts.h>

#include <ctime>
#include <string>
#include <vector>

// One line drawn by a PlotView
struct PlotSeries
{
	std::string         label;
	FXColor             color;
	std::vector<double> x;
	std::vector<double> y;
};

/**
 * A FX2DPlot that plots time based series.
 *
 * The x values are given in time units ("hours" or "days") relative to
 * a base time, so the axis numbers stay small. The cursor overlay converts
 * them back into a real date and time.
 */
class PlotView : public FX2DPlot
{
	FXDECLARE(PlotView)

protected:
	struct CurveData
	{
		FXCurve *curve;
		FXData  *xdata;
		FXData  *ydata;

		CurveData()
		: curve(nullptr),
		  xdata(nullptr),
		  ydata(nullptr)
		{}
	};

	std::vector<CurveData> curves;

	time_t   time_base;     // the time at x == 0
	double   seconds_per_x; // seconds in one x unit
	std::string time_unit;  // "hours" or "days"

	bool     cursor_inside;
	FXint    cursor_x;
	FXint    cursor_y;

	void removeCurves();
	void drawLegend( FXDC & dc, const FXRectangle & area ) const;
	void drawCursor( FXDC & dc, const FXRectangle & area ) const;
	std::string timeAt( double x ) const;

	PlotView()
	: FX2DPlot(),
	  time_base(0),
	  seconds_per_x(3600.0),
	  time_unit("hours"),
	  cursor_inside(false),
	  cursor_x(0),
	  cursor_y(0)
	{}

public:
	enum {
		ID_LAST = FX2DPlot::ID_LAST
	};

	PlotView( FXComposite *p, FXuint opts = CAPTION_ABOVE|CAPTION_SHOWN );

	virtual void create() override;

	/// The x value 0 is drawn as this time, one x unit is seconds_per_unit seconds
	void setTimeBase( time_t base, double seconds_per_unit, const std::string & unit );

	/// Replace the plotted data
	void setSeries( const std::vector<PlotSeries> & series );

	long onMotion( FXObject*, FXSelector, void* );
	long onLeave( FXObject*, FXSelector, void* );

	virtual void drawOverlay( FXDC & dc ) const override;

	virtual ~PlotView();
};

#endif /* PLOT_VIEW_H_ */
