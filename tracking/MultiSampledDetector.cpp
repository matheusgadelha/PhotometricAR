#include "MultiSampledDetector.hpp"


namespace tracking{

	MultiSampledDetector::MultiSampledDetector(
		cv::Mat& _pattern,
		int _rot_steps,
		int _scale_steps,
		float _smallest_scale
	)
	: PatternDetector( _pattern, new cv::FastFeatureDetector( 100 ), new cv::BriefDescriptorExtractor( 32 ))
	, sampledDescriptors( _scale_steps, std::vector<cv::Mat>( _rot_steps, cv::Mat() ))
	, sampledKeyPoints( _scale_steps, std::vector< std::vector<cv::KeyPoint> >( _rot_steps, std::vector<cv::KeyPoint>() ))
	, sampledCorners( _scale_steps, std::vector< std::vector<cv::Point2f> >( _rot_steps, std::vector<cv::Point2f>(4) ))
	, scaleSteps( _scale_steps )
	, rotSteps( _rot_steps )
	, smallestScale( _smallest_scale)
	{
		assert( this->train() );
	}

	bool MultiSampledDetector::train()
	{
		const float scaleInc = exp( log(this->smallestScale) / (this->scaleSteps-1));
		const float rotInc = 360.0f / this->rotSteps;

		for( int i=0; i<this->scaleSteps; ++i)
		{
			const float currentScale = pow( scaleInc, i );
			cv::Mat scaledImage;
			this->scaleImage( this->pattern.image, scaledImage, currentScale );

			std::vector<cv::Point2f> baseCorners = this->pattern.points2d;
			for( unsigned b=0; b<baseCorners.size(); ++b) baseCorners[b] *= currentScale;

			for( int j=0; j<this->rotSteps; ++j)
			{
				cv::Mat rotatedImage;
				this->rotateImage( scaledImage, rotatedImage, rotInc*j );

				this->featureDetector->detect( rotatedImage, sampledKeyPoints[i][j] );
				this->descriptorExtractor->compute( rotatedImage, sampledKeyPoints[i][j], sampledDescriptors[i][j] );

				cv::Point2f center(scaledImage.cols/2, scaledImage.rows/2);
				cv::Mat rotMatrix = cv::getRotationMatrix2D( center, rotInc*j, 1.0f );
				cv::transform( baseCorners, sampledCorners[i][j], rotMatrix );

				// Helper::drawPattern( rotatedImage, sampledCorners[i][j] );
				// cv::imshow( "Transform w/ Corners", rotatedImage );
				// cv::waitKey();
			}
		}

		return true;
	}

	bool MultiSampledDetector::processFrame( cv::Mat& _frame )
	{
		Helper::getGray( _frame, this->frameGray );

		int maxNumMatches = 0, maxScaleIdx = 0, maxRotIdx = 0;
		float maxRatio = 0;

		this->featureDetector->detect( this->frameGray, this->frameKeypoints );
		this->descriptorExtractor->compute( this->frameGray, this->frameKeypoints, this->frameDescriptors );

		for( int scaleIdx=0; scaleIdx < this->scaleSteps; ++scaleIdx )
		{
			for( int rotIdx=0; rotIdx < this->rotSteps; ++rotIdx )
			{
				this->matchDescriptors( this->frameDescriptors, this->sampledDescriptors[scaleIdx][rotIdx] );
				if( matches.size() < 4 ) continue;

				const float ratio = float(matches.size()) / sampledKeyPoints[scaleIdx][rotIdx].size();
				if( ratio > maxRatio )
				{
					maxRatio = ratio;
					maxNumMatches = matches.size();
					maxScaleIdx = scaleIdx;
					maxRotIdx = rotIdx;
				}
			}
		}

		std::cout << "Match Ratio: " << maxRatio << std::endl;

		return true;
	}

	void MultiSampledDetector::rotateImage( cv::Mat& _src, cv::Mat& _dst, float angle )
	{
		cv::Point2f center = cv::Point(_src.cols/2.f, _src.rows/2.f);
		cv::Mat rotMatrix = cv::getRotationMatrix2D( center, angle, 1.0f);

		cv::warpAffine( _src, _dst, rotMatrix, cv::Size(_src.cols, _src.rows));
	}

	void MultiSampledDetector::matchDescriptors( cv::Mat& _query, cv::Mat& _train )
	{
		std::cout << "MATCHING...\n";
		this->multiMatches.clear();
		this->matches.clear();
		this->descriptorMatcher->knnMatch( _query, _train, this->multiMatches, 1 );
		
		for( size_t i = 0; i < this->multiMatches.size(); ++i  )
		{
			const cv::DMatch& bestMatch = this->multiMatches[i][0];
			if( bestMatch.distance < 50 )
			{
				this->matches.push_back( bestMatch );
			}
		}
	}

	void MultiSampledDetector::scaleImage( cv::Mat& _src, cv::Mat& _dst, float scale )
	{
		cv::Size imgSize( (int)_src.cols*scale, (int)_src.rows*scale );
		cv::resize( _src, _dst, imgSize);
	}

}