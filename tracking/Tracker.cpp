#include "Tracker.hpp"

namespace tracking
{


	Tracker::Tracker( const cv::Mat& _pattern ) 
		: pattern( _pattern )
	{
	}
	
	bool Tracker::processFrame( const cv::Mat& _input_frame )
	{
		return true;
	}



}


