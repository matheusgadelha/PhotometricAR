#ifndef HELPER_HPP
#define HELPER_HPP

#include "opencv2/opencv.hpp"
#include <vector>

#define DEBUG_MODE

namespace Helper
{

	void getGray( const cv::Mat& image, cv::Mat& gray );
	
	cv::Mat getMatchesImage
		(
			cv::Mat query,
			cv::Mat pattern,
			const std::vector<cv::KeyPoint> queryKp,
			const std::vector<cv::KeyPoint> trainKp,
			std::vector<cv::DMatch> matches,
			unsigned maxMatchesDrawn
		);
		
	void drawPoints	( cv::Mat&, std::vector<cv::Point2f> );

	void drawPattern( cv::Mat&, cv::Mat&, std::vector<cv::Point2f> );
	void drawPattern( cv::Mat&, std::vector<cv::Point2f> );


}

#endif


