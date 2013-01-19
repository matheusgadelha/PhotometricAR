#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "Pattern.hpp"
#include "Helper.hpp"

namespace tracking
{
	
	class Tracker
	{
		
		public:
		
			Tracker( const cv::Mat& _pattern );
			bool processFrame( const cv::Mat& _input_frame );
			const cv::Mat getHomography() const;
			
		private:
			
			cv::Mat homography;
			cv::Mat pattern;
	
	};

}

#endif
