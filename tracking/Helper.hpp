#ifndef HELPER_HPP
#define HELPER_HPP

#include "opencv2/opencv.hpp"

namespace Helper
{

	void getGray( const cv::Mat& image, cv::Mat& gray );
	
	cv::Mat getMatchesImage
		(
			cv::Mat query,
			cv::Mat pattern,
			const std::vector<cv::KeyPoint>& queryKp,
			const std::vector<cv::KeyPoint>& trainKp,
			std::vector<cv::DMatch> matches,
			unsigned maxMatchesDrawn
		);

}

#endif


