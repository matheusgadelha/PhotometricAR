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
			homographyReprojThreshold( 3.0f ),
			patternFound( false ),
			homographyAcceptanceThreshold( 4.0f )
			
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
	
	bool PatternDetector::flowTracking
		(
			const cv::Mat& _previous_frame,
			cv::Mat& _current_frame
		)
	{
		if( this->patternFound )
		{
			cv::Mat previousGray, currentGray;
			Helper::getGray( _previous_frame, previousGray );
			Helper::getGray( _current_frame, currentGray );
			
			std::vector<cv::Point2f> currentFrameMatchedPts;
			
//			this->removeNonMatchedPoints
//				(
//					this->matches,
//					this->rawPatternKeyPoints,
//					this->rawFrameKeyPoints
//				);
			
			std::vector<unsigned char> opticalFlowStatus( this->matches.size() );
			std::vector<float> opticalFlowError( this->matches.size() );
			
			cv::calcOpticalFlowPyrLK
				(
					previousGray,
					currentGray,
					this->rawFrameKeyPoints,
					currentFrameMatchedPts,
					opticalFlowStatus,
					opticalFlowError
				);
			
//			std::vector<cv::Point2f> oldPatternPoints = this->rawPatternKeyPoints;
//		
//			this->rawFrameKeyPoints.clear();
//			this->rawPatternKeyPoints.clear();
//			for (unsigned int i = 0; i < opticalFlowStatus.size(); ++i)
//			{
//				if( opticalFlowError[i] < 12.0f )
//				{
//					this->rawFrameKeyPoints.push_back( currentFrameMatchedPts[i] );
//					this->rawPatternKeyPoints.push_back( oldPatternPoints[i] );
//				}
//			}

			this->rawFrameKeyPoints = currentFrameMatchedPts;
			
			this->homography = cv::findHomography
													(
														this->rawPatternKeyPoints,
														this->rawFrameKeyPoints,
														CV_FM_RANSAC,
														this->homographyReprojThreshold
													);
													
			float error;
													
			if( (error = calcHomographyError
						(
							this->homography,
							this->pattern.keyPoints,
							this->frameKeypoints,
							this->matches
			 			) < 10 ))
			{
				this->patternFound = false;
				return false;
			}
			
			std::cout << error << std::endl;
			
//			this->homographyMatching
//				(
//					this->matches,
//					this->homography,
//					patternMatchedKp,
//					currentFrameMatchedKp,
//					this->homographyReprojThreshold			 
//			 	);

			cv::warpPerspective
					(
						currentGray,
						this->warpedPattern,
						this->homography,
						this->pattern.getSize(),
						cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
					);
			
#ifdef DEBUG_MODE
			Helper::drawPoints( _current_frame, this->rawFrameKeyPoints );
#endif
					
			cv::imshow("Warped Image", this->warpedPattern);
//			
//			std::vector<cv::KeyPoint> warpedKeyPoints;
//			std::vector<cv::Point2f> newFrameKeyPoints;
//			
//			this->featureDetector->detect( this->warpedPattern, warpedKeyPoints );
//			cv::KeyPoint::convert( warpedKeyPoints, newFrameKeyPoints );			
//			cv::perspectiveTransform( newFrameKeyPoints, this->rawFrameKeyPoints, this->homography );
//			
//			cv::KeyPoint::convert( this->rawFrameKeyPoints, this->frameKeypoints );
//			
//			descriptorExtractor->compute
//			(
//				frameGray,
//				this->frameKeypoints,
//				this->frameDescriptors
//			);
//			
//			this->descriptorMatcher->match( this->frameDescriptors, this->matches );
//			
//			this->rawPatternKeyPoints.resize( this->matches.size() );
//			this->rawFrameKeyPoints.resize( this->matches.size() );
//			
//			for( size_t i = 0; i < this->matches.size(); ++i )
//			{
//				this->rawFrameKeyPoints[i] = this->frameKeypoints[this->matches[i].queryIdx].pt;
//				this->rawPatternKeyPoints[i] = this->pattern.keyPoints[this->matches[i].trainIdx].pt;
//			}
//			
//			//this->homographyMatching();

			return true;
		}
		else
		{
			this->processFrame( _current_frame );
			return false;
		}
	}
	
	bool PatternDetector::processFrame( cv::Mat& _frame )
	{
		Helper::getGray( _frame, this->frameGray );
	
		if( this->generateMatches
					(
						this->frameGray,
						this->frameKeypoints,
						this->frameDescriptors,
						this->matches
					))
		{
#ifdef DEBUG_MODE
			cv::imshow( "Raw Matching", Helper::getMatchesImage( _frame, this->pattern.image,	this->frameKeypoints,	this->pattern.keyPoints, this->matches, 500 ));
#endif		
								
			if( this->homographyMatchingFlag )
			{
				if( not this->homographyMatching
									(
										this->matches,
										this->homography,
										this->pattern.keyPoints,
										this->frameKeypoints,
										this->homographyReprojThreshold
									)) return false;
#ifdef DEBUG_MODE
				cv::imshow( "RANSAC Refined",	Helper::getMatchesImage( _frame, this->pattern.image,	this->frameKeypoints, this->pattern.keyPoints, this->matches, 500	 ));
				Helper::drawPoints( _frame, this->rawFrameKeyPoints );
#endif
				cv::warpPerspective
					(
						this->frameGray,
						this->warpedPattern,
						this->homography,
						this->pattern.getSize(),
						cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
					);
#ifdef DEBUG_MODE
				cv::imshow("Warped Image", this->warpedPattern);
#endif
//				this->homographyRefinement
//					(
//						this->pattern.keyPoints,
//						this->frameKeypoints,
//						this->homography
//					);
			}

			std::vector<cv::KeyPoint> patternMatchedPoints = this->pattern.keyPoints;
#ifdef DEBUG_MODE	
			std::cout << "HOMOGRAPHY ERROR: " << this->calcHomographyError
				(
					this->homography,
					patternMatchedPoints,
					this->frameKeypoints,
					this->matches
			 	)
			<< std::endl;
#endif

			if(this->calcHomographyError
					(
						this->homography,
						patternMatchedPoints,
						this->frameKeypoints,
						this->matches
			 		) < this->homographyAcceptanceThreshold)
			{
				this->patternFound = true;
				std::cout << "PATTERN FOUND\n";
			}
		}
		
		return true;
	}
	
	bool PatternDetector::computeFeaturesOnFrame
		(
			const cv::Mat& 							_frame,
			std::vector<cv::KeyPoint>& 	_key_points,
			cv::Mat& 										_descriptors 
		)
	{		
		featureDetector->detect( _frame, _key_points );
		if( this->pattern.keyPoints.size() <= 4 )
		{
			return false;
		}
		
		descriptorExtractor->compute
			(
				_frame,
				_key_points,
				_descriptors
			);
		
		return true;		
	}
	
	bool PatternDetector::generateMatches
		(
			const cv::Mat&							_frame,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			cv::Mat& 										_query_descriptors,
			std::vector<cv::DMatch>& 		_matches 
		)
	{
		if( not this->computeFeaturesOnFrame
						(
							_frame,
							_query_key_points,
							_query_descriptors
						))
		{
			return false;
		}
		
		_matches.clear();
		
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
			this->descriptorMatcher->match( _query_descriptors, _matches );
		}
		
		if( _matches.size() < 4 ) return false;
		
		return true;
	}
	
	bool PatternDetector::homographyMatching
		(
			std::vector<cv::DMatch>& 		_matches,
			cv::Mat& 										_homography,
			std::vector<cv::KeyPoint>& 	_train_key_points,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			float												_reproj_threshold
		)
	{
		const unsigned minMatchesAllowed = 6;
		
		if( _matches.size() < minMatchesAllowed ) return false;
	
		this->rawPatternKeyPoints.resize( _matches.size() );
		this->rawFrameKeyPoints.resize( _matches.size() );
			
		for( size_t i = 0; i < _matches.size(); ++i )
		{
			this->rawFrameKeyPoints[i] = _query_key_points[_matches[i].queryIdx].pt;
			this->rawPatternKeyPoints[i] = _train_key_points[_matches[i].trainIdx].pt;
		}
				
		std::vector<unsigned char> inliersMask( this->rawPatternKeyPoints.size() );
		_homography = cv::findHomography
													(
														this->rawPatternKeyPoints,
														this->rawFrameKeyPoints,
														CV_FM_RANSAC,
														_reproj_threshold,
														inliersMask
													);
													
//		std::vector<cv::Point2f> oldFramePts, oldPatternPts;
//		oldFramePts = this->rawFrameKeyPoints;
//		oldPatternPts = this->rawPatternKeyPoints;
		
		this->rawFrameKeyPoints.clear();
		this->rawPatternKeyPoints.clear();
		
		std::vector<cv::DMatch> inliers;
		for( size_t i = 0; i<inliersMask.size(); ++i )
		{
			if( inliersMask[i] )
			{
				inliers.push_back( _matches[i] );
				this->rawFrameKeyPoints.push_back( _query_key_points[_matches[i].queryIdx].pt );
				this->rawPatternKeyPoints.push_back( _train_key_points[_matches[i].trainIdx].pt );
			}
		}
		_matches.swap( inliers );
		
//		cv::KeyPoint::convert(_query_key_points, this->rawFrameKeyPoints);
//		cv::KeyPoint::convert(_query_key_points, this->rawPatternKeyPoints);
		
		return _matches.size() > minMatchesAllowed;
	}
	
	void PatternDetector::extractFeatures
		(
			const cv::Mat& 							_frame,
			std::vector<cv::KeyPoint>& 	_key_points,
			cv::Mat& 										_descriptors
		)
	{
		cv::Mat grayFrame;
		Helper::getGray( _frame, grayFrame );
		
		this->featureDetector->detect( grayFrame, _key_points );
		this->descriptorExtractor->compute( grayFrame, _key_points, _descriptors );
	}
	
	void PatternDetector::performMatching
		(
			const cv::Mat& 						_descriptors,
			std::vector<cv::DMatch>& 	_matches
		)
	{
		this->descriptorMatcher->match( _descriptors, _matches );
	}
	
	void PatternDetector::homographyRefinement
		(
			std::vector<cv::KeyPoint>& 	_train_key_points,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			cv::Mat& 										_homography 
		)
	{
		cv::Mat warpedFrame;
		
		cv::warpPerspective
		(
			this->frameGray,
			warpedFrame,
			_homography,
			this->pattern.getSize(),
			cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
		);
					
		std::vector<cv::KeyPoint> warpedKeyPoints;
		cv::Mat warpedDescriptors;
		std::vector<cv::DMatch> refinedMatches;
		cv::Mat refinedHomography;
		cv::Mat oldHomography = _homography.clone();
		
		this->extractFeatures( warpedFrame, warpedKeyPoints, warpedDescriptors );
		
		this->performMatching( warpedDescriptors, refinedMatches );
		this->homographyMatching
			(
				refinedMatches,
				refinedHomography,
				_train_key_points,
				warpedKeyPoints,
				1.0f
			);
			
		_homography = oldHomography * refinedHomography;
			
		cv::warpPerspective
		(
			this->frameGray,
			this->warpedPattern,
			_homography,
			this->pattern.getSize(),
			cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
		);
#ifdef DEBUG_MODE
		cv::imshow("Warped Image Refined", this->warpedPattern);
#endif
	}
	
	float PatternDetector::calcHomographyError
		(
			cv::Mat& 										_homography,
			std::vector<cv::KeyPoint>& 	_train_key_points,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			std::vector<cv::DMatch>&		_matches
		)
	{
		std::vector<cv::KeyPoint> trainKp( _train_key_points );
		std::vector<cv::KeyPoint> queryKp( _query_key_points );
		
		this->removeNonMatchedKeyPoints
			(
				_matches,
				_train_key_points,
				_query_key_points
			);

		std::vector<cv::Point2f> queryPoints, resultPoints, trainPoints;
		cv::KeyPoint::convert( queryKp, queryPoints );
		cv::KeyPoint::convert( trainKp, trainPoints );
		
		cv::perspectiveTransform( queryPoints, resultPoints, _homography.inv() );
		
		float totalDistance = 0.0f;
		
		for( size_t i = 0; i < trainKp.size(); ++i )
		{
			totalDistance += cv::norm( trainPoints[i] - resultPoints[i] );
		}
		
		return totalDistance / float( resultPoints.size() );
	}
	
	void PatternDetector::removeNonMatchedKeyPoints
		(
		 	std::vector<cv::DMatch>& _matches,
		 	std::vector<cv::KeyPoint>& _train_key_points,
		 	std::vector<cv::KeyPoint>& _query_key_points
		)
	{
		std::vector<cv::KeyPoint> newTrainKp, newQueryKp;
		
		for( size_t i=0; i < _matches.size(); ++i )
		{
			newTrainKp.push_back( _train_key_points[_matches[i].trainIdx] );
			newQueryKp.push_back( _query_key_points[_matches[i].queryIdx] );
		}
		
		_train_key_points = newTrainKp;
		_query_key_points = newQueryKp;
	}
	
	void PatternDetector::removeNonMatchedPoints
		(
		 	std::vector<cv::DMatch>& _matches,
		 	std::vector<cv::Point2f>& _train_points,
		 	std::vector<cv::Point2f>& _query_points
		)
	{
		std::vector<cv::Point2f> newTrainPts( _matches.size() );
		std::vector<cv::Point2f> newQueryPts( _matches.size() );
		
		for( size_t i=0; i < _matches.size(); ++i )
		{
			newTrainPts[i] = this->pattern.keyPoints[_matches[i].trainIdx].pt;
			newQueryPts[i] = this->frameKeypoints[_matches[i].queryIdx].pt;
		}
		
		_train_points.swap( newTrainPts );
		_query_points.swap( newQueryPts );
	}

}

