/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/

////////////////////////////////////////////////////////////////////
// File includes:
#include "ARPipeline.hpp"

void maxCromaticity( cv::Mat& src, cv::Mat& res)
{
    std::vector<cv::Mat> channels_src;
    cv::split(src, channels_src);
    res = cv::max( cv::max( channels_src[0], channels_src[1]  ), channels_src[2] );
}

void minCromaticity( cv::Mat& src, cv::Mat& res)
{
    std::vector<cv::Mat> channels_src;
    cv::split(src, channels_src);
    res = cv::min( cv::min( channels_src[0], channels_src[1]  ), channels_src[2] );
}

float meanScalar( cv::Scalar s )
{
	return (s[0] + s[1] + s[2])/3.0f;
}

ARPipeline::ARPipeline(
        const cv::Mat& mainPattern,
        const cv::Mat& frontPattern,
        const cv::Mat& rightPattern, 
        const cv::Mat& leftPattern, 
        const CameraCalibration& calibration)
: m_calibration(calibration)
{
    m_patternDetector.m_cubeSize = cv::Point3f( mainPattern.cols, frontPattern.rows, mainPattern.rows );
    m_patternDetector.buildMainPatternFromImage(mainPattern, m_mainPattern);
    m_patternDetector.buildFrontPatternFromImage(frontPattern, m_frontPattern);
    m_patternDetector.buildRightPatternFromImage(rightPattern, m_rightPattern);
    m_patternDetector.buildLeftPatternFromImage(leftPattern, m_leftPattern);
    m_patternDetector.train(m_mainPattern);
    
    small_rate = 0.1f;
    
    cv::resize( mainPattern, sMain, cv::Size(0,0), small_rate, small_rate );
    cv::resize( frontPattern, sFront, cv::Size(0,0), small_rate, small_rate );
    cv::resize( rightPattern, sRight, cv::Size(0,0), small_rate, small_rate );
    cv::resize( leftPattern, sLeft, cv::Size(0,0), small_rate, small_rate );

	cv::Scalar mainLightFactor = cv::Scalar(-1,-1,-1);
	cv::Scalar frontLightFactor = cv::Scalar(-1,-1,-1);
	cv::Scalar rightLightFactor = cv::Scalar(-1,-1,-1);
	cv::Scalar leftLightFactor = cv::Scalar(-1,-1,-1);

}


ARPipeline::ARPipeline(const cv::Mat& patternImage, const CameraCalibration& calibration)
  : m_calibration(calibration)
{
  m_patternDetector.buildMainPatternFromImage(patternImage, m_mainPattern);
  m_patternDetector.train(m_mainPattern);
}

bool ARPipeline::processFrame(cv::Mat& inputFrame)
{
  bool patternFound = m_patternDetector.findPattern(inputFrame, m_patternInfo);
  std::vector<cv::Point2f> frontProjPoints, leftProjPoints, rightProjPoints;
  cv::Mat frontHomography, rightHomography, leftHomography, mainHomography;
  cv::Mat rectifiedFront, rectifiedRight, rectifiedLeft, rectifiedMain;

  if (patternFound)
  {
    m_patternInfo.computePose(m_mainPattern, m_calibration);
    
    //=========MAIN FACE==========//
    mainHomography = m_patternInfo.homography;
    cv::warpPerspective( inputFrame, rectifiedMain, mainHomography.inv(), 
            m_mainPattern.size, cv::INTER_CUBIC );
    cv::resize( rectifiedMain, rectifiedMain, cv::Size(0,0), small_rate, small_rate );
    //cv::imshow("RECTIFIED MAIN", rectifiedMain);
    cv::waitKey(12);
    //m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
    
    //=========FRONT FACE==========//
    cv::projectPoints( m_frontPattern.points3d, m_patternInfo.Rvec, m_patternInfo.Tvec,
            m_calibration.getIntrinsic(), m_calibration.getDistorsion(), frontProjPoints);
    isFrontVisible = left( frontProjPoints[0], frontProjPoints[1], frontProjPoints[2] );
    
    if(isFrontVisible)
    {
        m_patternInfo.points2d = frontProjPoints;
        frontHomography = cv::findHomography( m_frontPattern.points2d, frontProjPoints, CV_RANSAC );
        cv::warpPerspective( inputFrame, rectifiedFront, frontHomography.inv(), 
                m_frontPattern.size, cv::INTER_CUBIC );
        cv::resize( rectifiedFront, rectifiedFront, cv::Size(0,0), small_rate, small_rate );
        //cv::imshow("RECTIFIED FRONT", rectifiedFront);
        cv::waitKey(12);
        // m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
    }
    
    //=========RIGHT FACE==========//
    cv::projectPoints( m_rightPattern.points3d, m_patternInfo.Rvec, m_patternInfo.Tvec,
            m_calibration.getIntrinsic(), m_calibration.getDistorsion(), rightProjPoints);
    isRightVisible = left( rightProjPoints[0], rightProjPoints[1], rightProjPoints[2] );
    
    if( isRightVisible )
    {
        m_patternInfo.points2d = rightProjPoints;
        rightHomography = cv::findHomography( m_rightPattern.points2d, rightProjPoints, CV_RANSAC );
        cv::warpPerspective( inputFrame, rectifiedRight, rightHomography.inv(), 
                m_rightPattern.size, cv::INTER_CUBIC );
        cv::resize( rectifiedRight, rectifiedRight, cv::Size(0,0), small_rate, small_rate );
        //cv::imshow("RECTIFIED RIGHT", rectifiedRight);
        cv::waitKey(12);
        // m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );    
    }
    
    
    //=========LEFT FACE==========//
    cv::projectPoints( m_leftPattern.points3d, m_patternInfo.Rvec, m_patternInfo.Tvec,
            m_calibration.getIntrinsic(), m_calibration.getDistorsion(), leftProjPoints);
    isLeftVisible = left( leftProjPoints[0], leftProjPoints[1], leftProjPoints[2] );
    
    if( isLeftVisible )
    {
        m_patternInfo.points2d = leftProjPoints;
        leftHomography = cv::findHomography( m_rightPattern.points2d, leftProjPoints, CV_RANSAC );
        cv::warpPerspective( inputFrame, rectifiedLeft, leftHomography.inv(), 
                m_leftPattern.size, cv::INTER_CUBIC );
        cv::resize( rectifiedLeft, rectifiedLeft, cv::Size(0,0), small_rate, small_rate );
        cv::imshow("RECTIFIED LEFT", rectifiedLeft);
        cv::imshow("LEFT", sRight);
        cv::waitKey(12);
        // m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
    }
    
    currentDiffuseColor = computeLightColor(sMain,rectifiedMain);
    currentAmbientLight = computeLightColor(sMain,rectifiedMain);

	mainLightFactor = computeLightColor(sMain, rectifiedMain);

	if( isFrontVisible ){
		frontLightFactor = computeLightColor(sFront, rectifiedFront);
	}

	if( isRightVisible ){
		rightLightFactor = computeLightColor(sRight, rectifiedRight);
	}

	if( isLeftVisible ){
		leftLightFactor = computeLightColor(sLeft, rectifiedLeft);
	}

	cv::Mat diverImg;

	divergenceMask( sLeft, rectifiedLeft, diverImg );
	cv::imshow("DiverImg", diverImg);

	//std::cout << "Main: " << mainLightFactor[2] << " " << mainLightFactor[1] << " " << mainLightFactor[0] << std::endl
	//		  << "Front: " << frontLightFactor[2] << " " << frontLightFactor[1] << " " << frontLightFactor[0] << std::endl
	//		  << "Right: " << rightLightFactor[2] << " " << rightLightFactor[1] << " " << rightLightFactor[0] << std::endl
	//		  << "Left: " << leftLightFactor[2] << " " << leftLightFactor[1] << " " << leftLightFactor[0] << std::endl;

	std::vector<LightFactor> lightFactors;
	lightFactors.push_back( LightFactor( mainLightFactor, meanScalar( mainLightFactor )));
	lightFactors.push_back( LightFactor( frontLightFactor, meanScalar( frontLightFactor )));
	lightFactors.push_back( LightFactor( rightLightFactor, meanScalar( rightLightFactor )));
	lightFactors.push_back( LightFactor( leftLightFactor, meanScalar( leftLightFactor )));

	std::sort( lightFactors.begin(), lightFactors.end() );
	//std::cout << lightFactors[0].mean << " " << lightFactors[3].mean << std::endl;
	int envIdx = 0;
	for( unsigned i=0; i < lightFactors.size(); ++i )
	{
		if( lightFactors[i].mean <= 0 ){
			envIdx++;
		}
	}

	currentAmbientLight = lightFactors[envIdx].color;
	currentDiffuseColor = lightFactors[3].color;

//    std::cout << "Visibility List\n";
//    if( isFrontVisible ) std::cout <<"FRONT\n";
//    if( isRightVisible ) std::cout <<"RIGHT\n";
//    if( isLeftVisible ) std::cout <<"LEFT\n";
    
//    m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
  }

  return patternFound;
}

void ARPipeline::divergenceMask( cv::Mat& org, cv::Mat& rec, cv::Mat& result )
{
	cv::Mat org_lab, rec_lab;
	cv::Mat diff, diff_gray,diff_mask;

	cv::GaussianBlur( org, org, cv::Size(5,5), 0, 0);
	cv::GaussianBlur( rec, rec, cv::Size(5,5), 0, 0);

	cv::absdiff( org, rec, diff );

	std::vector<cv::Mat> vec_diff_channels (3, cv::Mat());
	cv::split(diff, vec_diff_channels);

	cv::cvtColor( diff, diff_gray, CV_BGR2GRAY );

	diff_gray = cv::max(cv::max(vec_diff_channels[0],vec_diff_channels[1]), vec_diff_channels[2]);


	const int t = 33330;

	//cv::cvtColor( org, org_lab, CV_BGR2LAB );
	//cv::cvtColor( rec, rec_lab, CV_BGR2LAB );

	//for( unsigned i = 0; i<org.rows; ++i ){
	//	for( unsigned j = 0; j<org.cols; ++j ){
	//		cv::Scalar ov = org.at<cv::Scalar>(i,j);
	//		cv::Scalar rv = rec.at<cv::Scalar>(i,j);

	//		float dist = cv::norm( ov - rv );
	//		std::cout << "Dist: " << dist << std::endl;

	//		diff.at<unsigned char>(i,j) = (unsigned char) dist;
	//	}
	//}
	cv::threshold( diff_gray, diff_mask, t, 255, cv::THRESH_BINARY_INV );

	result = diff_mask;

}

cv::Scalar ARPipeline::computeLightColor(cv::Mat& org, cv::Mat& rec)
{
    cv::Scalar color;
    
    cv::Mat f_rec, f_org, diff_mask, gray_diff;
    cv::Mat div;

	//cv::Mat diff = rec - org;
	//cv::cvtColor(diff, gray_diff, CV_BGR2GRAY );

	//cv::imshow("Diff", diff);

	//cv::threshold( gray_diff, diff_mask, 30, 255, cv::THRESH_BINARY_INV );
	//cv::erode( diff_mask, diff_mask, cv::Mat() );
	//cv::erode( diff_mask, diff_mask, cv::Mat() );
	//cv::dilate( diff_mask, diff_mask, cv::Mat() );
	//cv::dilate( diff_mask, diff_mask, cv::Mat() );
	//cv::imshow("Diff Mask", diff_mask);

	divergenceMask( org, rec, diff_mask );
        
    rec.convertTo(f_rec,CV_32FC3);
    org.convertTo(f_org, CV_32FC3);
      
    div = f_rec / f_org;

	//color = cv::mean(div);
    color = cv::mean(div,diff_mask);
        
    return color;
}

cv::Scalar ARPipeline::computeLight(cv::Mat& org, cv::Mat& rec)
{
    cv::Scalar org_mean, rec_mean;
    
    org_mean = cv::mean( org );
    rec_mean = cv::mean( rec );
    
    cv::Scalar color = 
            cv::Scalar(rec_mean[2]/org_mean[2],rec_mean[1]/org_mean[1],rec_mean[0]/org_mean[0]);
    
    //std::cout << color[2] << " " << color[1] << " " << color[0] << std::endl;
    
    return color;
}


cv::Point3f ARPipeline::computeLightDirection()
{
	cv::Point3f mainVec( 0, 1, 0 );
	cv::Point3f leftVec( -1, 0, 0 );
	cv::Point3f rightVec( 1, 0, 0 );
	cv::Point3f frontVec( 0, 0, 1 );

	std::vector<float> w_vec;

	w_vec.push_back(meanScalar( mainLightFactor )); 
	w_vec.push_back(meanScalar( leftLightFactor )); 
	w_vec.push_back(meanScalar( rightLightFactor ));
	w_vec.push_back(meanScalar( frontLightFactor ));

	std::sort( w_vec.begin(), w_vec.end() );

	float lw = 0.0f;
	for(unsigned i = 0; i<w_vec.size() ; ++i ){
		if( w_vec[i] > 0.0f )
		{
			lw = w_vec[i];
			break;
		}
	}

    cv::Point3f dir(0,0,0);
	dir = mainVec * (meanScalar( mainLightFactor ) - lw)+
		  leftVec * (meanScalar( leftLightFactor ) - lw)+
		  rightVec * (meanScalar( rightLightFactor ) - lw) +
		  frontVec * (meanScalar( frontLightFactor ) - lw);
	
	std::cout << "main: " << meanScalar( mainLightFactor ) << std::endl
			  << "left: " << meanScalar( leftLightFactor ) << std::endl
			  << "right: " << meanScalar( rightLightFactor ) << std::endl
			  << "front: " << meanScalar( frontLightFactor ) << std::endl;
	
	return -dir;
	//return -cv::Point3f(0,1,0);
}

bool ARPipeline::left(cv::Point2f a, cv::Point2f b, cv::Point2f c)
{
    return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) >= 0.3;          
}



const Transformation& ARPipeline::getPatternLocation() const
{
  return m_patternInfo.pose3d;
}
