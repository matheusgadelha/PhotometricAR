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
//    cv::imshow("RECTIFIED MAIN", rectifiedMain);
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
//        cv::imshow("RECTIFIED FRONT", rectifiedFront);
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
//        cv::imshow("RECTIFIED RIGHT", rectifiedRight);
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
//        cv::imshow("RECTIFIED LEFT", rectifiedLeft);
        cv::waitKey(12);
        // m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
    }
    
    currentDiffuseColor = computeLightColor(sMain,rectifiedMain);
    currentAmbientLight = computeAmbientLight(sMain,rectifiedMain);
    
//    std::cout << "Visibility List\n";
//    if( isFrontVisible ) std::cout <<"FRONT\n";
//    if( isRightVisible ) std::cout <<"RIGHT\n";
//    if( isLeftVisible ) std::cout <<"LEFT\n";
    
//    m_patternInfo.draw2dContour( inputFrame, cv::Scalar(255,0,0) );
  }

  return patternFound;
}

cv::Scalar ARPipeline::computeLightColor(cv::Mat& org, cv::Mat& rec)
{
    cv::Scalar color;
    
    cv::Mat f_rec, f_org;
    cv::Mat div;

	cv::Mat diff = rec - org;

	cv::imshow("Diff", diff);
        
    rec.convertTo(f_rec,CV_32FC3);
    org.convertTo(f_org, CV_32FC3);
      
    div = f_rec / f_org;
       
    color = cv::mean(div);
        
    return color;
}

cv::Scalar ARPipeline::computeAmbientLight(cv::Mat& org, cv::Mat& rec)
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
    
}

bool ARPipeline::left(cv::Point2f a, cv::Point2f b, cv::Point2f c)
{
    return (b.x-a.x)*(c.y-a.y) - (c.x-a.x)*(b.y-a.y) >= 0;          
}



const Transformation& ARPipeline::getPatternLocation() const
{
  return m_patternInfo.pose3d;
}
