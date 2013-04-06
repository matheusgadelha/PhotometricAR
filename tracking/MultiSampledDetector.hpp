#ifndef MULTISAMPLEDDETECTOR_HPP
#define MULTISAMPLEDDETECTOR_HPP

#include <vector>
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>

#include "Helper.hpp" 
#include "Pattern.hpp"
#include "PatternDetector.hpp"

#include "opencv2/opencv.hpp"

namespace tracking
{

	class MultiSampledDetector : public PatternDetector
	{
		
		public:
			MultiSampledDetector( cv::Mat&, int, int, float _smallest_scale = 0.5f );
			bool train();
			bool processFrame( cv::Mat& );

		private:
			void rotateImage( cv::Mat&, cv::Mat&, float angle );
			void scaleImage( cv::Mat&, cv::Mat&, float scale );
			void matchDescriptors( cv::Mat&, cv::Mat& );

			std::vector< std::vector<cv::Mat> > sampledDescriptors;
			std::vector< std::vector< std::vector<cv::KeyPoint> > > sampledKeyPoints;
			std::vector< std::vector< std::vector<cv::Point2f> > > sampledCorners;

			std::vector<cv::Point2f> corners;

			int scaleSteps;
			int rotSteps;
			float smallestScale;
	};
}

#endif