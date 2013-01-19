#include "PatternDetector.hpp"

namespace tracking
{

	PatternDetector::PatternDetector
		(
			cv::Mat& 													_pattern_img,
			cv::Ptr<cv::FeatureDetector> 			_feature_detector,
			cv::Ptr<cv::DescriptorExtractor> 	_descriptor_extractor,
			cv::Ptr<cv::DescriptorMatcher> 		_descriptor_matcher
		)
		: pattern( _pattern_img ),
			featureDetector( _feature_detector ),
			descriptorExtractor( _descriptor_extractor ),
			descriptorMatcher( _descriptor_matcher ),
			homographyReprojThreshold(2)
	{
		this->ratioTestFlag = false;
		this->homographyMatchingFlag = true;
		assert( this->train() );
	}
	
	bool PatternDetector::train()
	{
		featureDetector->detect( this->pattern.image, this->pattern.keyPoints );
		
		if( this->pattern.keyPoints.size() <= 4 )
		{
			std::cerr << "FATAL ERROR: Insuficient KeyPoints at pattern image...\n"
								<< "Terminating...\n";
			return false;
		}
		
		descriptorExtractor->compute
			(
				this->pattern.image,
				this->pattern.keyPoints,
				this->pattern.descriptors
			);
			
		this->descriptorMatcher->clear();
		
		std::vector<cv::Mat> descriptors(1);
		descriptors[0] = this->pattern.descriptors.clone();
		
		this->descriptorMatcher->add( descriptors );
		this->descriptorMatcher->train();
		
		std::cout << "Training successful!\n";
		return true;
	}
	
	bool PatternDetector::processFrame( const cv::Mat& _frame )
	{
	
		if( this->generateMatches( _frame ) )
		{
		
			cv::imshow(
									"Raw Matching",
									Helper::getMatchesImage
										(
											_frame,
											this->pattern.image,
											this->frameKeypoints,
											this->pattern.keyPoints,
											this->matches,
											100							
										)
								);		
								
			if( this->homographyMatchingFlag )
			{
				if( not this->homographyMatching() ) return false;
				
				cv::imshow(
									"RANSAC Refined",
									Helper::getMatchesImage
										(
											_frame,
											this->pattern.image,
											this->frameKeypoints,
											this->pattern.keyPoints,
											this->matches,
											100							
										)
								);
								
				cv::Mat warpedImage;
				cv::warpPerspective
					(
						_frame,
						warpedImage,
						this->homography,
						this->pattern.getSize(),
						cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
					);
				
				cv::imshow("Warped Image", warpedImage);
			}
			
		}
		
		return true;
	}
	
	bool PatternDetector::computeFeaturesOnFrame( const cv::Mat& _frame )
	{
		cv::Mat gray_frame;
		Helper::getGray( _frame, gray_frame );
		
		featureDetector->detect( gray_frame, this->frameKeypoints );
		if( this->pattern.keyPoints.size() <= 4 )
		{
			return false;
		}
		
		descriptorExtractor->compute
			(
				gray_frame,
				this->frameKeypoints,
				this->frameDescriptors
			);
		
		return true;		
	}
	
	bool PatternDetector::generateMatches( const cv::Mat& _frame)
	{
		if( not this->computeFeaturesOnFrame( _frame ))
		{
			return false;
		}
		
		this->matches.clear();
		
		if( this->ratioTestFlag )
		{
			const float minRatio = 1.f / 1.5f;
			this->multiMatches.clear();
			this->descriptorMatcher->knnMatch( this->frameDescriptors, this->multiMatches, 2 );
			
			for( size_t i = 0; i < this->multiMatches.size(); ++i  )
			{
				const cv::DMatch& bestMatch = this->multiMatches[i][0];
				const cv::DMatch& betterMatch = this->multiMatches[i][1];
				
				float distanceRatio = bestMatch.distance / betterMatch.distance;
				
				if( distanceRatio < minRatio )
				{
					this->matches.push_back( bestMatch );
				}
			}
		}
		else
		{
			this->descriptorMatcher->match( this->frameDescriptors, this->matches );
		}
		
		if( this->matches.size() < 4 ) return false;
		
		return true;
	}
	
	bool PatternDetector::homographyMatching()
	{
		const unsigned minMatchesAllowed = 8;
		
		if( this->matches.size() < minMatchesAllowed ) return false;
		
		std::vector<cv::Point2f> srcPoints( this->matches.size() );
		std::vector<cv::Point2f> dstPoints( this->matches.size() );
		
		for( size_t i = 0; i < this->matches.size(); ++i )
		{
			srcPoints[i] = this->pattern.keyPoints[this->matches[i].trainIdx].pt;
			dstPoints[i] = this->frameKeypoints[this->matches[i].queryIdx].pt;
		}
		
		std::vector<unsigned char> inliersMask( srcPoints.size() );
		this->homography = cv::findHomography
													(
														srcPoints,
														dstPoints,
														CV_FM_RANSAC,
														this->homographyReprojThreshold,
														inliersMask
													);
		
		std::vector<cv::DMatch> inliers;
		for( size_t i = 0; i<inliersMask.size(); ++i )
		{
			if( inliersMask[i] ) inliers.push_back( matches[i] );
		}
		matches.swap( inliers );
		
		return matches.size() > minMatchesAllowed;
	}

}

