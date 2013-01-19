#ifndef PATTERNDETECTOR_HPP
#define PATTERNDETECTOR_HPP

#include <vector>
#include <string>
#include <cassert>
#include "Helper.hpp" //getGray
#include "Pattern.hpp" //Pattern
#include "opencv2/opencv.hpp"

namespace tracking
{

	class PatternDetector
	{
		
		public:
		
			PatternDetector
				( 
					cv::Mat& 													_pattern_img,
					cv::Ptr<cv::FeatureDetector> 			_feature_detector 		= new cv::ORB( 1000 ),
					cv::Ptr<cv::DescriptorExtractor> 	_descriptor_extractor	= new cv::FREAK( true, true ),
					cv::Ptr<cv::DescriptorMatcher> 		_descriptor_matcher		= new cv::BFMatcher( cv::NORM_HAMMING, true )
				);
				
			bool train();
			bool processFrame( const cv::Mat& );
			void enableRatioTest();
			void disableRatioTest();
			void enableHomographyMatching();
			void disableHomographyMatching();
			void setHomographyReprojThreshold( float );
			
			
		private:
			
			Pattern pattern;
		
			cv::Ptr<cv::FeatureDetector> 			featureDetector;
			cv::Ptr<cv::DescriptorExtractor> 	descriptorExtractor;
			cv::Ptr<cv::DescriptorMatcher>		descriptorMatcher;
			
			std::vector<cv::DMatch> 								matches;
			std::vector< std::vector<cv::DMatch> > 	multiMatches;
			
			cv::Mat 									frameDescriptors;
			std::vector<cv::KeyPoint> frameKeypoints;
			
			cv::Mat homography;
			
			bool ratioTestFlag;
			bool homographyMatchingFlag;
			
			bool computeFeaturesOnFrame( const cv::Mat& );
			bool generateMatches(const cv::Mat& );
			bool homographyMatching();
			
			float homographyReprojThreshold;
	
	};

}

#endif
