#ifndef PATTERNDETECTOR_HPP
#define PATTERNDETECTOR_HPP

#include <vector>
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>
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
					cv::Mat& 							_pattern_img,
					// cv::Ptr<cv::FeatureDetector> 		_feature_detector 		= new cv::FastFeatureDetector(100),
					cv::Ptr<cv::FeatureDetector> 		_feature_detector       = new cv::ORB(600, 1.5, 4, 31, 0, 2, cv::ORB::FAST_SCORE ),
					cv::Ptr<cv::DescriptorExtractor> 	_descriptor_extractor	= new cv::ORB(600, 1.5, 4, 31, 0, 2, cv::ORB::FAST_SCORE ),
					// cv::Ptr<cv::DescriptorExtractor> 	_descriptor_extractor	= new cv::FREAK( true, false, 50, 16 ),
					cv::Ptr<cv::DescriptorMatcher> 		_descriptor_matcher		= new cv::BFMatcher( cv::NORM_HAMMING, false )
				);
				
			bool train();
			bool processFrame( cv::Mat& );
			bool flowTracking( const cv::Mat&,	cv::Mat& );
			void enableRatioTest();
			void disableRatioTest();
			void enableHomographyMatching();
			void disableHomographyMatching();
			void setHomographyReprojThreshold( float );

			//FOR DEBUG PURPOSES
//			cv::Mat camImg;

		protected:
			Pattern pattern;

			cv::Ptr<cv::FeatureDetector>		featureDetector;
			cv::Ptr<cv::DescriptorExtractor> 	descriptorExtractor;
			cv::Ptr<cv::DescriptorMatcher>		descriptorMatcher;

			cv::Mat 					frameDescriptors;
			cv::Mat						frameGray;
			std::vector<cv::KeyPoint> 	frameKeypoints;

			std::vector<cv::DMatch> 				matches;
			std::vector< std::vector<cv::DMatch> > 	multiMatches;
			
		private:
			cv::Mat warpedPattern;
			
			std::vector<cv::Point2f>  rawFrameKeyPoints;
			std::vector<cv::Point2f>  rawPatternKeyPoints;
			
			cv::Mat homography;
			
			bool ratioTestFlag;
			bool homographyMatchingFlag;
			
			float homographyReprojThreshold;
			
			bool patternFound;
			
			float homographyAcceptanceThreshold;
			float maxFlowError;
			
			bool computeFeaturesOnFrame(
					const cv::Mat&,
					std::vector<cv::KeyPoint>&,
					cv::Mat&
			);
			
			bool generateMatches(
					const cv::Mat&,
					std::vector<cv::KeyPoint>&,
					cv::Mat&,
					std::vector<cv::DMatch>&
			);
			
			bool homographyMatching(
					std::vector<cv::DMatch>&,
					cv::Mat&, std::vector<cv::KeyPoint>&,
					std::vector<cv::KeyPoint>&,
					float
			);
			
			void extractFeatures( const cv::Mat&, std::vector<cv::KeyPoint>&, cv::Mat& );
			
			void performMatching( const cv::Mat&, std::vector<cv::DMatch>& );
			
			void removeNonMatchedKeyPoints(
			 		std::vector<cv::DMatch>&,
			 		std::vector<cv::KeyPoint>&,
			 		std::vector<cv::KeyPoint>&
			 );
			 
			void removeNonMatchedPoints(
				 	std::vector<cv::DMatch>& _matches,
				 	std::vector<cv::Point2f>& _train_points,
				 	std::vector<cv::Point2f>& _query_points
			);
			
			void homographyRefinement(
					std::vector<cv::KeyPoint>,
					std::vector<cv::KeyPoint>,
					cv::Mat&
			);
			
			float calcHomographyError(
					cv::Mat 					_homography,
					std::vector<cv::KeyPoint>	_train_key_points,
					std::vector<cv::KeyPoint> 	_query_key_points,
					std::vector<cv::DMatch> 	_matches
			);

			void removePoorMatches( float _t, std::vector<cv::DMatch>& _matches );

			bool isGoodHomography( cv::Mat& h );
	
	};

}

#endif
