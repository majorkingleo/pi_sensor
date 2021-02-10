#ifndef GENGNUPLOTDATA_H_
#define GENGNUPLOTDATA_H_

#include <string>
#include <vector>
#include <map>

class GenGnuPlotData
{
public:
	static const std::string XTIC;

protected:
	int width;
	int height;

	std::string image_file;
	std::string data_file;

	std::map< std::string, std::vector<std::string> > data;

public:
	GenGnuPlotData()
	: width(800),
	  height(500),
	  image_file("gnuplot.png"),
	  data_file("gnuplot")
	{}

	virtual ~GenGnuPlotData() {}

	void setWidth( int width_ ) {
		width = width_;
	}

	void setHeight( int height_ ) {
		height = height_;
	}

	void setImageFileName( const std::string & name ) {
		image_file = name;
	}

	void setDataFileName( const std::string & name ) {
		data_file = name;
	}

	void addData( const std::string & title, const std::vector<std::string> & data_ ) {
		data[title] = data_;
	}

	void setXDescriptionValues( const std::vector<std::string> & data_ ) {
		data[XTIC] = data_;
	}


	virtual void create();

protected:
	virtual unsigned long unifyData();
};



#endif /* GENGNUPLOTDATA_H_ */
