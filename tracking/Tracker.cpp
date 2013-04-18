#include "Tracker.hpp"

namespace tracking
{


	Tracker::Tracker( const cv::Mat& _pattern ) 
		: pattern( _pattern )
		, patternDetector( _pattern )
	{
	}
	
	bool Tracker::processFrame( cv::Mat& _input_frame )
	{
		this->patternDetector.processFrame( _input_frame );
		return true;
	}



}


