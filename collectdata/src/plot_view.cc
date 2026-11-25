/*
 * plot_view.cc
 *
 * Interactive line chart based on the FOX 2.0 chart classes (FXChart).
 */

#include "plot_view.h"

#include <cmath>
#include <cstring>

FXDEFMAP(PlotView) PlotViewMap[] = {
	FXMAPFUNC(SEL_MOTION, 0, PlotView::onMotion),
	FXMAPFUNC(SEL_LEAVE, 0, PlotView::onLeave),
};

FXIMPLEMENT( PlotView, FX2DPlot, PlotViewMap, ARRAYNUMBER(PlotViewMap) )

PlotView::PlotView( FXComposite *p, FXuint opts )
: FX2DPlot( p, nullptr, 0, opts, 0, 0, 0, 0, 6, 6, 6, 6 ),
  time_base(0),
  seconds_per_x(3600.0),
  time_unit("hours"),
  cursor_inside(false),
  cursor_x(0),
  cursor_y(0)
{
	// fill styles from the desktop theme
	FXColor back  = getApp()->getBackColor();
	FXColor base  = getApp()->getBaseColor();
	FXColor hilite = getApp()->getHiliteColor();
	FXColor shadow = getApp()->getShadowColor();

	FillStyle bs = { nullptr, back, FXRGBA(0,0,0,0), shadow, back, STIPPLE_NONE, FILLSTYLE_HORIZONTAL };
	FillStyle cs = { nullptr, base, FXRGBA(0,0,0,0), base, hilite, STIPPLE_NONE, FILLSTYLE_VERTICAL };

	setBackStyle( bs );
	setChartStyle( cs );

	setTimeBase( 0, 3600.0, "hours" );
}

PlotView::~PlotView()
{
	removeCurves();
}

void PlotView::create()
{
	FX2DPlot::create();
}

void PlotView::removeCurves()
{
	// FXCurve owns its FXData objects, clearCurves() deletes the curves
	clearCurves();
	curves.clear();
}

void PlotView::setTimeBase( time_t base, double seconds_per_unit, const std::string & unit )
{
	time_base = base;
	seconds_per_x = (seconds_per_unit > 0.0) ? seconds_per_unit : 3600.0;
	time_unit = unit;

	FXString label = FXString::value( "Time [%s]", unit.c_str() );

	getAxis( XLO )->setLabel( label );
	getAxis( XHI )->setLabel( label );
	getAxis( YLO )->setLabel( "°C" );
	getAxis( YHI )->setLabel( "°C" );
}

void PlotView::setSeries( const std::vector<PlotSeries> & series )
{
	// keep the curves (and their visibility) if the number of series is unchanged
	if( curves.size() != series.size() ) {
		removeCurves();

		for( size_t i = 0; i < series.size(); i++ ) {
			CurveData cd;
			cd.xdata = new FXData();
			cd.ydata = new FXData();
			cd.curve = new FXCurve( this, series[i].label.c_str() );
			appendCurve( cd.curve );
			curves.push_back( cd );
		}
	}

	FXColor background = getChartStyle().forecolor;

	for( size_t i = 0; i < series.size(); i++ ) {
		CurveData & cd = curves[i];
		const PlotSeries & s = series[i];

		cd.curve->setLabel( s.label.c_str() );

		LineStyle ls;
		ls.color = readableColor( s.color, background );
		// FX2DPlot draws the curves with FXDC::setAntialias(true); the FOX
		// rasterizer only antialiases solid lines up to one pixel wide, so a
		// thicker line would be drawn aliased (wideLine).
		ls.weight = 1;
		ls.style = LINESTYLE_SOLID;
		cd.curve->setLineStyle( ls );

		cd.xdata->clear();
		cd.ydata->clear();

		size_t count = std::min( s.x.size(), s.y.size() );

		if( count > 0 ) {
			cd.xdata->reserve( static_cast<FXint>(count) );
			cd.ydata->reserve( static_cast<FXint>(count) );

			for( size_t j = 0; j < count; j++ ) {
				cd.xdata->append( s.x[j] );
				cd.ydata->append( s.y[j] );
			}
		}

		cd.curve->setXData( cd.xdata );
		cd.curve->setYData( cd.ydata );
	}

	// do not throw away a zoom range the user picked
	if( !isZoomed() ) {
		fitData();
	}

	invalidateChart();
}

std::string PlotView::timeAt( double x ) const
{
	time_t stamp = static_cast<time_t>( Math::lrint( static_cast<FXdouble>(time_base) + x * seconds_per_x ) );

	struct tm tmv;
	if( localtime_r( &stamp, &tmv ) == nullptr ) {
		return std::string();
	}

	char buffer[100];
	strftime( buffer, sizeof(buffer), "%a %d.%m. %H:%M", &tmv );

	return buffer;
}

void PlotView::drawLegend( FXDC & dc, const FXRectangle & area ) const
{
	if( curves.empty() ) {
		return;
	}

	FXFont * font = getAxis( XLO )->getNumberFont();
	FXint textheight = font->getFontHeight();
	FXint swatchwidth = 20;

	FXint textwidth = 0;
	for( const CurveData & cd : curves ) {
		textwidth = FXMAX( textwidth, textWidth( font, cd.curve->getLabel() ) );
	}

	FXint left = area.x + area.w - textwidth - swatchwidth - 4 - 8;
	FXint top = area.y + 6;

	dc.setFont( font );

	FXint y = top;
	for( const CurveData & cd : curves ) {
		FXColor color = cd.curve->isVisible() ? cd.curve->getLineStyle().color : getApp()->getShadowColor();

		dc.setForeground( color );
		dc.setLineWidth( 3 );
		dc.drawLine( left, y + textheight / 2, left + swatchwidth, y + textheight / 2 );

		dc.setForeground( getApp()->getForeColor() );
		drawText( dc, cd.curve->getLabel(), left + swatchwidth + 4, y, TEXT_ATTACH_LEFT|TEXT_ATTACH_TOP );

		y += textheight + 3;
	}

	dc.setLineWidth( 1 );
}

void PlotView::drawCursor( FXDC & dc, const FXRectangle & area ) const
{
	FXAxis * xaxis = getAxis( XLO );

	FXdouble xmin = xaxis->getAxisRangeMin();
	FXdouble xmax = xaxis->getAxisRangeMax();

	if( !(xmax > xmin) ) {
		return;
	}

	FXdouble xvalue = xmin + (xmax - xmin) * (cursor_x - area.x) / area.w;

	// vertical crosshair
	FXColor crosshair = readableColor( FXRGB(140,140,140), getChartStyle().forecolor );
	dc.setForeground( crosshair );
	dc.setLineWidth( 1 );
	dc.setLineStyle( LINE_ONOFF_DASH );
	dc.drawLine( cursor_x, area.y, cursor_x, area.y + area.h );
	dc.setLineStyle( LINE_SOLID );

	FXFont * font = xaxis->getNumberFont();
	FXint textheight = font->getFontHeight();

	dc.setFont( font );

	FXint textx = area.x + 6;
	FXint texty = area.y + 6;

	// the real date and time under the cursor
	dc.setForeground( getApp()->getForeColor() );
	drawText( dc, timeAt( xvalue ).c_str(), textx, texty, TEXT_ATTACH_LEFT|TEXT_ATTACH_TOP );

	texty += textheight + 3;

	for( const CurveData & cd : curves ) {
		if( !cd.curve->isVisible() ) {
			continue;
		}

		FXData * xs = cd.curve->getXData();
		FXData * ys = cd.curve->getYData();

		if( xs == nullptr || ys == nullptr || xs->no() <= 0 || ys->no() <= 0 ) {
			continue;
		}

		FXint index = xs->findIndexAt( xvalue );

		if( index < 0 ) {
			continue;
		}

		FXdouble samplex = xs->get( index );
		FXdouble sampley = ys->get( index );

		// interpolate between the neighbouring samples
		if( index + 1 < FXMIN( xs->no(), ys->no() ) ) {
			FXdouble nextx = xs->get( index + 1 );
			FXdouble nexty = ys->get( index + 1 );

			if( nextx > samplex && xvalue >= samplex && xvalue <= nextx ) {
				sampley += (nexty - sampley) * (xvalue - samplex) / (nextx - samplex);
			}
		}

		if( !Math::fpFinite( sampley ) ) {
			continue;
		}

		FXAxis * yaxis = getAxis( cd.curve->getYAxis() );
		FXdouble ymin = yaxis->getAxisRangeMin();
		FXdouble ymax = yaxis->getAxisRangeMax();

		FXColor color = cd.curve->getLineStyle().color;

		// marker on the curve
		if( ymax > ymin && sampley >= ymin && sampley <= ymax ) {
			FXint pointx = area.x + static_cast<FXint>( Math::lrint( (xvalue - xmin) * area.w / (xmax - xmin) ) );
			FXint pointy = area.y + area.h - static_cast<FXint>( Math::lrint( (sampley - ymin) * area.h / (ymax - ymin) ) );

			dc.setForeground( color );
			dc.fillEllipse( pointx - 3, pointy - 3, 7, 7 );
		}

		// value readout
		dc.setForeground( color );
		drawText( dc, FXString::value( "%s: %.1f °C", cd.curve->getLabel().text(), sampley ).text(),
				  textx, texty, TEXT_ATTACH_LEFT|TEXT_ATTACH_TOP );

		texty += textheight + 3;
	}
}

void PlotView::drawOverlay( FXDC & dc ) const
{
	FX2DPlot::drawOverlay( dc );

	const FXRectangle & area = getPlotArea();

	if( area.w <= 0 || area.h <= 0 ) {
		return;
	}

	drawLegend( dc, area );

	if( cursor_inside ) {
		drawCursor( dc, area );
	}
}

long PlotView::onMotion( FXObject*, FXSelector, void* ptr )
{
	FXEvent * event = static_cast<FXEvent*>(ptr);

	const FXRectangle & area = getPlotArea();

	bool inside = area.w > 0 && area.h > 0 &&
				  event->win_x >= area.x && event->win_x < area.x + area.w &&
				  event->win_y >= area.y && event->win_y < area.y + area.h;

	if( cursor_inside != inside ||
		(inside && (cursor_x != event->win_x || cursor_y != event->win_y)) ) {
		cursor_inside = inside;
		cursor_x = event->win_x;
		cursor_y = event->win_y;
		update();
	}

	// let the base class stretch the zoom rubber band
	return FX2DChart::onMotion( nullptr, 0, ptr );
}

long PlotView::onLeave( FXObject*, FXSelector, void* )
{
	if( cursor_inside ) {
		cursor_inside = false;
		update();
	}

	return 1;
}
