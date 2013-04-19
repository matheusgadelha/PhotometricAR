#ifndef PATTERN_HPP
#define PATTERN_HPP

#include <vector>
#include <string>
#include "Helper.hpp"
#include "opencv2/opencv.hpp"

namespace tracking
{

	class Pattern
	{
		
		public:
			
			Pattern( const cv::Mat& _pattern_img );
			cv::Size getSize();
		
			cv::Mat	image;
			cv::Mat image_color;
			cv::Mat	descriptors;
			
			std::vector<cv::KeyPoint> keyPoints;
			std::vector<cv::Point2f> 	points2d;
			std::vector<cv::Point3f> 	points3d;
	
	};

}

#endif
