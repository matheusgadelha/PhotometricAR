#ifndef TRACKER_HPP
#define TRACKER_HPP

#include "Pattern.hpp"
#include "Helper.hpp"
#include "PatternDetector.hpp"

namespace tracking
{
	
	class Tracker
	{
		
		public:
		
			Tracker( const cv::Mat& _pattern );
			bool processFrame( cv::Mat& _input_frame );
			const cv::Mat getHomography();
			// TODO: put this on private and refactor
			PatternDetector patternDetector;
			
		private:
			cv::Mat homography;
			cv::Mat pattern;
	
	};

}

#endif
