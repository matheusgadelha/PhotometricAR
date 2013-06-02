#include "PatternDetector.hpp"

namespace tracking
{

	/**
	 * Class constructor.
	 * @param _pattern_img an image representing the pattern to be tracked. A simple .jpg or .png loaded into a cv::Mat.
	 * @param _feature_detector an object to detect keypoints. It will be used to detect scene and pattern keypoints.
	 * @param _descriptor_extractor a descriptor extractor to compute descriptors for detected keypoints. It will be used do compute scene and pattern descriptors.
	 * @param _descriptor_matcher a descriptor matched used to match scene and pattern descriptors in order to compute homography.
	 */
	PatternDetector::PatternDetector
		(
			const cv::Mat& 						_pattern_img,
			cv::Ptr<cv::FeatureDetector> 		_feature_detector,
			cv::Ptr<cv::DescriptorExtractor> 	_descriptor_extractor,
			cv::Ptr<cv::DescriptorMatcher> 		_descriptor_matcher
		)
		: pattern( _pattern_img )
		, featureDetector( _feature_detector )
		, descriptorExtractor( _descriptor_extractor )
		, descriptorMatcher( _descriptor_matcher )
		, homographyReprojThreshold( 4.0f )
		, patternFound( false )
		, homographyAcceptanceThreshold( 300.0f )
		, maxFlowError( 35.0f )
			
	{
		// Using ratio test flag
		this->ratioTestFlag = true;
		// Removes outliers according to homography model. Reduces number of tracked points. Increases precision.
		this->homographyMatchingFlag = true;
		// Tests if pattern was correctly trained
		assert( this->train() );
	}
	
	/**
	 * Trains the patter. In other words, detects keypoints and computes its descriptors.
	 *
	 * @returns true if pattern was correctly trained (there are enough points to track). false if is a bad pattern.
	 */
	bool PatternDetector::train()
	{
		// Detects keypoints
		featureDetector->detect( this->pattern.image, this->pattern.keyPoints );
		// Converts keypoints do regular points (useful for optical flow tracking)
		cv::KeyPoint::convert(this->pattern.keyPoints, this->rawPatternKeyPoints);
		
		// Checks if there are enough points
		if( this->pattern.keyPoints.size() <= 15 )
		{
			std::cerr << "FATAL ERROR: Insuficient KeyPoints at pattern image...\n"
								<< "Terminating...\n";
			return false;
		}
		
		// Computes descriptors
		descriptorExtractor->compute
			(
				this->pattern.image,
				this->pattern.keyPoints,
				this->pattern.descriptors
			);
		
		// Clears all matches (sanity purposes)
		this->descriptorMatcher->clear();
		
		// Creates descriptor and trains matches
		std::vector<cv::Mat> descriptors(1);
		descriptors[0] = this->pattern.descriptors.clone();
		
		this->descriptorMatcher->add( descriptors );
		this->descriptorMatcher->train();
		
		return true;
	}
	
	// Performs flow tracking. This is not beign used anymore. I will keep this code here just for safety.
	bool PatternDetector::flowTracking
		(
			const cv::Mat& _previous_frame,
			cv::Mat& _current_frame
		)
	{
		std::cout << "FLOW PROCESSING\n";
		if( this->patternFound )
		{
			cv::Mat previousGray, currentGray;
			Helper::getGray( _previous_frame, previousGray );
			Helper::getGray( _current_frame, currentGray );
			
			std::vector<cv::Point2f> currentFrameMatchedPts;
			std::vector<cv::Point2f> trackedFrameMatchedPts;
			std::vector<cv::Point2f> trackedPatternMatchedPts;
			
			std::vector<unsigned char> opticalFlowStatus( this->matches.size() );
			std::vector<float> opticalFlowError( this->matches.size() );
			
			cv::calcOpticalFlowPyrLK(
				previousGray,
				currentGray,
				this->rawFrameKeyPoints,
				currentFrameMatchedPts,
				opticalFlowStatus,
				opticalFlowError
			);

			for( unsigned i=0; i < currentFrameMatchedPts.size(); ++i )
			{
				if( opticalFlowStatus[i] && opticalFlowError[i] < this->maxFlowError )
				{
					trackedFrameMatchedPts.push_back( currentFrameMatchedPts[i] );
					trackedPatternMatchedPts.push_back( this->rawPatternKeyPoints[i] );
				}
			}

			this->rawFrameKeyPoints = trackedFrameMatchedPts;
			this->rawPatternKeyPoints = trackedPatternMatchedPts;

			if( rawFrameKeyPoints.size() <= 6)
			{
				this->patternFound = false;
				cv::KeyPoint::convert(this->pattern.keyPoints, this->rawPatternKeyPoints);
				return false;
			}

			this->homography = cv::findHomography(
				this->rawPatternKeyPoints,
				this->rawFrameKeyPoints,
				CV_FM_RANSAC,
				this->homographyReprojThreshold
			);
													
			float error;
													
			if( !this->isGoodHomography( this->homography ) ||
				(error = calcHomographyError(
							this->homography,
							this->pattern.keyPoints,
							this->frameKeypoints,
							this->matches
			 			)
			 	) > homographyAcceptanceThreshold ||
			 	this->rawFrameKeyPoints.size() <= 4)
			{
				this->patternFound = false;
				cv::KeyPoint::convert(this->pattern.keyPoints, this->rawPatternKeyPoints);
				return false;
			}
			
			// std::cout << error << std::endl;

			cv::warpPerspective(
				currentGray,
				this->warpedPattern,
				this->homography,
				this->pattern.getSize(),
				cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
			);

			// std::vector<cv::KeyPoint> rectifiedPatternKps;
			// std::vector<cv::Point2f> rectifiedPatternPts;
			// cv::Mat rectifiedPatternDescs;

			// std::vector<cv::KeyPoint> rectifiedFrameKps;
			// std::vector<cv::Point2f> rectifiedFramePts;
			// cv::Mat rectifiedFrameDescs;

			// this->featureDetector->detect( this->warpedPattern, rectifiedPatternKps );
			// this->descriptorExtractor->compute( this->warpedPattern, rectifiedPatternKps, rectifiedPatternDescs );

			// cv::KeyPoint::convert( rectifiedPatternKps, rectifiedPatternPts );

			// cv::perspectiveTransform( rectifiedPatternPts, rectifiedFramePts, this->homography );

			// cv::KeyPoint::convert( rectifiedFramePts, rectifiedFrameKps );

			// this->descriptorExtractor->compute( currentGray, rectifiedFrameKps, rectifiedFrameDescs );

			// this->multiMatches.clear();
			// this->matches.clear();
			// this->descriptorMatcher->knnMatch( rectifiedFrameDescs, this->multiMatches, 1 );
			
			// for( size_t i = 0; i < this->multiMatches.size(); ++i  )
			// {
			// 	const cv::DMatch& bestMatch = this->multiMatches[i][0];

			// 	if( bestMatch.distance < 50 )
			// 	{
			// 		this->matches.push_back( bestMatch );
			// 	}
			// }

			// this->rawFrameKeyPoints.clear();
			// this->rawFrameKeyPoints.resize(this->matches.size());

			// this->rawPatternKeyPoints.clear();
			// this->rawPatternKeyPoints.resize(this->matches.size());

			// for( unsigned i = 0; i < this->matches.size(); ++i )
			// {
			// 	this->rawFrameKeyPoints[i] = rectifiedFrameKps[this->matches[i].queryIdx].pt;
			// 	this->rawPatternKeyPoints[i] = rectifiedFrameKps[this->matches[i].trainIdx].pt;
			// }

			// this->homography = cv::findHomography(
			// 	this->rawPatternKeyPoints,
			// 	this->rawFrameKeyPoints,
			// 	CV_FM_RANSAC,
			// 	this->homographyReprojThreshold
			// );

#ifdef DEBUG_MODE
			// Helper::drawPoints( _current_frame, rectifiedFramePts );
			Helper::drawPattern( _current_frame, this->homography, this->pattern.points2d );
#endif
			
			cv::imshow("Warped Image", this->warpedPattern);
			return true;
		}
		else
		{
			this->processFrame( _current_frame );
			return false;
		}
	}
	
	/**
	 * This method processes a scene frame and tracks the pattern on it.
	 * If no pattern is found, the variavle patternFound is set to false.
	 * If the pattern was found, is possible to check the homography transformation that leads the pattern position and orientation on the scene.
	 * 
	 * @param _frame a frame image contatining a scene where the pattern will be tracked.
	 * @return true if pattern was found. false otherwise
	 */
	bool PatternDetector::processFrame( cv::Mat& _frame )
	{
		// Clock variables to check time spent on different parts of the code
		clock_t start = clock();
		// clock_t generate_start, generate_end, homo_start, homo_end;

		// Converts frame to gray
		Helper::getGray( _frame, this->frameGray );

		// Generate matches between pattern and scene descriptors
		if( this->generateMatches(
						this->frameGray,
						this->frameKeypoints,
						this->frameDescriptors,
						this->matches
		))
		{	
			// If we want to match points that fit homography...
			if( this->homographyMatchingFlag )
			{
				// If homography matching failed, no pattern is on the scene. return false
				if( not this->homographyMatching(
					this->matches,
					this->homography,
					this->pattern.keyPoints,
					this->frameKeypoints,
					this->homographyReprojThreshold
				))
				{
					this->patternFound = false;
					return false;
				} 

				// Generates warped image
				cv::warpPerspective
					(
						_frame,
						this->warpedPattern,
						this->homography,
						this->pattern.getSize(),
						cv::WARP_INVERSE_MAP | cv::INTER_CUBIC
					);

				// Scales warped image and original pattern by 0.125
				cv::resize(this->warpedPattern, this->warpedPattern, cv::Size(0,0), 0.5, 0.5);
				cv::Mat color_pattern;
				cv::resize(this->pattern.image_color, color_pattern, cv::Size(0,0), 0.5, 0.5);	

				int g_size = 35;
				cv::GaussianBlur(this->warpedPattern, this->warpedPattern, cv::Size(g_size,g_size), 4 );
				cv::GaussianBlur(color_pattern, color_pattern, cv::Size(g_size,g_size), 4 );
				// Illumination image
				cv::Mat illu_diff = this->warpedPattern - color_pattern;
				// Erodes image to reduce noise
				cv::erode( illu_diff, illu_diff, cv::Mat() );
				cv::erode( illu_diff, illu_diff, cv::Mat() );
				cv::Mat illu_area, gray_illu_diff;
				Helper::getGray( illu_diff, gray_illu_diff );
				cv::inRange(gray_illu_diff, cv::Scalar(50), cv::Scalar(255) ,illu_area);
				cv::blur( illu_area, illu_area, cv::Size(7,7));
				// cv::bitwise_and( illu_area, illu_diff, illu_diff);
				// cv::inRange(illu_area, cv::Scalar(1), cv::Scalar(255) ,illu_area);

				// Light map image
				cv::Mat light_map;
				light_map.convertTo(light_map, CV_32FC3);
				// color_pattern.convertTo(color_pattern, CV_32FC3);
				// this->warpedPattern.convertTo(this->warpedPattern, CV_32FC3);
				// cv::multiply( color_pattern, cv::Scalar(0.5, 0.5, 0.5), color_pattern);
				cv::divide(this->warpedPattern, color_pattern, light_map);
				// light_map *= 255;
				// cv::normalize(light_map, light_map, 0, 1, cv::NORM_MINMAX);
				cv::min(light_map, 1.0, light_map);
				cv::multiply(light_map, cv::Scalar(255,255,255), light_map);
				// cv::multiply(light_map, color_pattern, light_map);
				// light_map.convertTo(light_map, CV_8UC3);
				cv::blur( light_map, light_map, cv::Size(15,15));

				cv::Scalar env_light = cv::mean(light_map);
				env_light[0] = env_light[0]/255.0f;
				env_light[1] = env_light[1]/255.0f;
				env_light[2] = env_light[2]/255.0f;

				std::cout << "Environment Light:\n"
						  << "R => " << env_light[2] << std::endl
						  << "G => " << env_light[1] << std::endl
						  << "B => " << env_light[0] << std::endl;

				// cv::Mat pattern_diff = illu_diff + dark_diff;

				// cv::blur( pattern_diff, pattern_diff, cv::Size(7,7));
				// cv::dilate( pattern_diff, pattern_diff, cv::Mat() );

				// pattern_diff *= 5;

				cv::imwrite("results/light_map.png", light_map);
				cv::imwrite("results/rectified.png", this->warpedPattern);
				cv::imwrite("results/original.png", color_pattern);
				cv::imwrite("results/scene.png", _frame);

#ifdef DEBUG_MODE
				cv::imshow("Warped Image", this->warpedPattern);
				cv::imshow("Pattern resized", color_pattern);
				cv::imshow("Illu", illu_diff);
				cv::imshow("Area illuminated", illu_area);
				cv::imshow("Light Map", light_map);
#endif

				// this->homographyRefinement(
				// 	this->pattern.keyPoints,
				// 	this->frameKeypoints,
				// 	this->homography
				// );
			}
			// If we are not using homography matching
			else
			{
				// Matches points and computes homography
				std::vector<cv::Point2f> frameMatchedPts, patternMatchedPts;
				frameMatchedPts = this->rawFrameKeyPoints;
				patternMatchedPts = this->rawPatternKeyPoints;

				this->removeNonMatchedPoints( this->matches, patternMatchedPts, frameMatchedPts );

				this->homography = cv::findHomography(
					patternMatchedPts,
					frameMatchedPts,
					CV_RANSAC, 10
				);
			}

			// Stores pattern matched points for homography error computation
			std::vector<cv::KeyPoint> patternMatchedPoints = this->pattern.keyPoints;

			// Tests if we have a good homography and if its error is low below the defined threshold.
			if( this->isGoodHomography( this->homography ) && 
				this->calcHomographyError
					(
						this->homography,
						patternMatchedPoints,
						this->frameKeypoints,
						this->matches
			 		) < this->homographyAcceptanceThreshold
			 	)
			{
				this->patternFound = true;
			}
			else
			{
				this->patternFound = false;
				return false;
			}
			Helper::drawPattern( _frame, this->homography, this->pattern.points2d );
		}

		clock_t ends = clock();
	    std::cout << "Total time elapsed :" << (double) (ends - start) / CLOCKS_PER_SEC << std::endl;
		
		return true;
	}
	
	/**
	 * Computes features on a frame
	 * 
	 * @param _frame a frame where will extract keypoints and their descriptors
	 * @param _key_points a set of keypoints that will receive the _frame detect points
	 * @param _descriptors a set of descriptor that will receive the _frame computed descriptors
	 * @return true if enough keypoints are found. false otherwise.
	 */
	bool PatternDetector::computeFeaturesOnFrame(
			const cv::Mat& 				_frame,
			std::vector<cv::KeyPoint>& 	_key_points,
			cv::Mat& 					_descriptors 
		)
	{	
		// Variables to measure detection an description time
		// clock_t detection_start, detection_end, desc_start, desc_end;

		// detection_start = clock();
		featureDetector->detect( _frame, _key_points );
		// Too few keypoints were detecte...
		if( this->pattern.keyPoints.size() <= 4 )
		{
			return false;
		}
		// detection_end = clock();
		
		// desc_start = clock();
		// Compute descriptors
		descriptorExtractor->compute
			(
				_frame,
				_key_points,
				_descriptors
			);
		// desc_end = clock();

		// std::cout << "Detection:" << (double) (detection_end - detection_start) / CLOCKS_PER_SEC << std::endl;
		// std::cout << "Description:" << (double) (desc_end - desc_start) / CLOCKS_PER_SEC << std::endl;
		
		return true;		
	}
	
	/**
	 * Matches pattern descriptors with the descriptor passed as arguments.
	 *
	 * @param _frame frame where we will look for the pattern.
	 * @param _query_key_points a set where we will store the frame keypoints.
	 * @param _query_descriptorsa set where we will store the frame descriptors.
	 * @param _matches a set where we will store the performed matches.
	 * @return true if operation was completed succesfully. false otherwise.
	 */
	bool PatternDetector::generateMatches(
			const cv::Mat&				_frame,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			cv::Mat& 					_query_descriptors,
			std::vector<cv::DMatch>& 	_matches 
		)
	{
		// Tests if enough features were computed
		if( not this->computeFeaturesOnFrame(
							_frame,
							_query_key_points,
							_query_descriptors
			))
		{
			return false;
		}
		
		// Clears previous matches
		_matches.clear();
		
		// Tests if we want to perform a ratio test
		if( this->ratioTestFlag )
		{
			// const float minRatio = 1.f / 1.5f;
			// Cleans matches
			this->multiMatches.clear();
			// Selects only the best match
			this->descriptorMatcher->knnMatch( this->frameDescriptors, this->multiMatches, 1 );
			
			// Tests if each match is close enough to be considered a good one.
			for( size_t i = 0; i < this->multiMatches.size(); ++i  )
			{
				const cv::DMatch& bestMatch = this->multiMatches[i][0];
				// const cv::DMatch& betterMatch = this->multiMatches[i][1];
				// float distanceRatio = bestMatch.distance / betterMatch.distance;
				
				if( bestMatch.distance < 50 )
				{
					this->matches.push_back( bestMatch );
				}
			}
		}
		else
		{
			this->descriptorMatcher->match( _query_descriptors, _matches );
		}
		
		// If not enough matches have been done, return failure
		if( _matches.size() < 4 ) return false;
		
		// Everything worked!
		return true;
	}
	
	/**
	 * Matches descriptors according to then ransac generated homography.
	 *
	 * @param _reproj_threshold reprojection error tolerated by the homography to include points at inliers.
	 */
	bool PatternDetector::homographyMatching(
			std::vector<cv::DMatch>& 	_matches,
			cv::Mat& 					_homography,
			std::vector<cv::KeyPoint>& 	_train_key_points,
			std::vector<cv::KeyPoint>& 	_query_key_points,
			float						_reproj_threshold
		)
	{
		const unsigned minMatchesAllowed = 8;
		
		if( _matches.size() < minMatchesAllowed ) return false;
	
		this->rawPatternKeyPoints.resize( _matches.size() );
		this->rawFrameKeyPoints.resize( _matches.size() );
			
		// Deals only with matched points, excluding non previously matched.
		for( size_t i = 0; i < _matches.size(); ++i )
		{
			this->rawFrameKeyPoints[i] = _query_key_points[_matches[i].queryIdx].pt;
			this->rawPatternKeyPoints[i] = _train_key_points[_matches[i].trainIdx].pt;
		}
		
		// Compute an inliers mask
		std::vector<unsigned char> inliersMask( this->rawPatternKeyPoints.size() );
		// Compute homography passing the inliers mask
		_homography = cv::findHomography(
				this->rawPatternKeyPoints,
				this->rawFrameKeyPoints,
				CV_FM_RANSAC,
				_reproj_threshold,
				inliersMask
		);
		
		// Clears raw points
		this->rawFrameKeyPoints.clear();
		this->rawPatternKeyPoints.clear();
		
		// Fills points with inliers
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
		// Puts inliers on matches vector
		_matches.swap( inliers );
			
		// True if enough matches were found
		return _matches.size() > minMatchesAllowed;
	}

	// Extract feature from a frame
	void PatternDetector::extractFeatures(
			const cv::Mat& 				_frame,
			std::vector<cv::KeyPoint>& 	_key_points,
			cv::Mat& 					_descriptors
		)
	{
		cv::Mat grayFrame;
		Helper::getGray( _frame, grayFrame );
		
		this->featureDetector->detect( grayFrame, _key_points );
		this->descriptorExtractor->compute( grayFrame, _key_points, _descriptors );
	}
	
	// Matches descriptors
	void PatternDetector::performMatching(
			const cv::Mat& 				_descriptors,
			std::vector<cv::DMatch>& 	_matches
		)
	{
		// const float minRatio = 1.f / 1.5f;
		// Cleans matches
		this->multiMatches.clear();
		// Selects only the best match
		this->descriptorMatcher->knnMatch( _descriptors, this->multiMatches, 1 );
		
		// Tests if each match is close enough to be considered a good one.
		for( size_t i = 0; i < this->multiMatches.size(); ++i  )
		{
			const cv::DMatch& bestMatch = this->multiMatches[i][0];
			// const cv::DMatch& betterMatch = this->multiMatches[i][1];
			// float distanceRatio = bestMatch.distance / betterMatch.distance;
			
			if( bestMatch.distance < 50 )
			{
				_matches.push_back( bestMatch );
			}
		}
	}
	
	// Refines Homography
	void PatternDetector::homographyRefinement(
			std::vector<cv::KeyPoint> 	_train_key_points,
			std::vector<cv::KeyPoint> 	_query_key_points,
			cv::Mat& _homography 
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
	
	float PatternDetector::calcHomographyError(
			cv::Mat 					_homography,
			std::vector<cv::KeyPoint> 	_train_key_points,
			std::vector<cv::KeyPoint> 	_query_key_points,
			std::vector<cv::DMatch>		_matches
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
		
		for( unsigned i = 0; i < trainPoints.size(); ++i )
		{
			totalDistance += cv::norm( trainPoints[i] - resultPoints[i] );
		}

		if( resultPoints.size() < 4 ) return std::numeric_limits<float>::max();
		else
		{
			return totalDistance / float( resultPoints.size() );
		}

	}
	
	void PatternDetector::removeNonMatchedKeyPoints(
		 	std::vector<cv::DMatch>& _matches,
		 	std::vector<cv::KeyPoint>& _train_key_points,
		 	std::vector<cv::KeyPoint>& _query_key_points
		)
	{
		std::vector<cv::KeyPoint> newTrainKp ( _matches.size() );
		std::vector<cv::KeyPoint> newQueryKp ( _matches.size() );
		
		for( size_t i=0; i < _matches.size(); ++i )
		{
			newTrainKp[i] = _train_key_points[_matches[i].trainIdx];
			newQueryKp[i] = _query_key_points[_matches[i].queryIdx];
		}
		
		_train_key_points = newTrainKp;
		_query_key_points = newQueryKp;
	}
	
	void PatternDetector::removeNonMatchedPoints(
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

	void PatternDetector::removePoorMatches( float _t, std::vector<cv::DMatch>& _matches )
	{
		std::vector<cv::DMatch>::iterator it = _matches.begin();
		while( it != _matches.end() )
		{
			if((*it).distance > _t)
			{
				it = _matches.erase(it);
			}
			it++;
		}
	}


	bool PatternDetector::isGoodHomography( cv::Mat& h )
	{

		if( std::abs( cv::determinant(h) ) < 0.1 ) return false;
		if( std::abs( cv::determinant(h) ) > 3 ) return false;

		const double N1 = std::sqrt(h.at<double>(0,0)*h.at<double>(0,0) + h.at<double>(1,0)*h.at<double>(1,0));
		if( N1 > 4 || N1 < 0.1) return false;

		const double N2 = std::sqrt(h.at<double>(0,1)*h.at<double>(0,1) + h.at<double>(1,1)*h.at<double>(1,1));
		if( N2 > 4 || N2 < 0.1) return false;

		const double N3 = std::sqrt(h.at<double>(2,0)*h.at<double>(2,0) + h.at<double>(2,1)*h.at<double>(2,1));
		if( N3 > 0.002 ) return false;

		return true;
		
	}

}

