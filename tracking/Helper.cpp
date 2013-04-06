#include "Helper.hpp"

namespace Helper
{

	void getGray( const cv::Mat& image, cv::Mat& gray )
	{
		 if (image.channels()  == 3)
			 cv::cvtColor(image, gray, CV_BGR2GRAY);
		 else if (image.channels() == 4)
			 cv::cvtColor(image, gray, CV_BGRA2GRAY);
		 else if (image.channels() == 1)
			 gray = image;
	}
	
	cv::Mat getMatchesImage
		(
			cv::Mat query,
			cv::Mat pattern,
			const std::vector<cv::KeyPoint> queryKp,
			const std::vector<cv::KeyPoint> trainKp,
			std::vector<cv::DMatch> matches,
			unsigned maxMatchesDrawn
		)
  {
  	cv::Mat outImg;

    if (matches.size() > maxMatchesDrawn)
    {
   	 	matches.resize(maxMatchesDrawn);
    }

    cv::drawMatches
    		(
         	query, 
          queryKp, 
          pattern, 
          trainKp,
          matches, 
          outImg, 
          cv::Scalar(0,200,0,255), 
          cv::Scalar::all(-1),
          std::vector<char>(), 
          cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS
        );

  	return outImg;
  }
  
  void drawPoints	(	cv::Mat& img, std::vector<cv::Point2f> points )
  {
  	for (unsigned int i = 0; i < points.size(); ++i)
  	{
  		cv::circle( img, points[i], 3, cv::Scalar( 255, 0, 0 ), 3 );
  	}
  }

  void drawPattern( cv::Mat& img, cv::Mat& h, std::vector<cv::Point2f> corners )
  {
    std::vector<cv::Point2f> img_corners;
    cv::perspectiveTransform( corners, img_corners, h );

    cv::line( img, img_corners[0], img_corners[1], cv::Scalar(0,255,0), 3 );
    cv::line( img, img_corners[0], img_corners[2], cv::Scalar(0,255,0), 3 );
    cv::line( img, img_corners[3], img_corners[1], cv::Scalar(0,255,0), 3 );
    cv::line( img, img_corners[3], img_corners[2], cv::Scalar(0,255,0), 3 );
  }

  void drawPattern( cv::Mat& img, std::vector<cv::Point2f> corners )
  {
    cv::line( img, corners[0], corners[1], cv::Scalar(255,255,255), 3 );
    cv::line( img, corners[0], corners[2], cv::Scalar(255,255,255), 3 );
    cv::line( img, corners[3], corners[1], cv::Scalar(255,255,255), 3 );
    cv::line( img, corners[3], corners[2], cv::Scalar(255,255,255), 3 );
  }

}
