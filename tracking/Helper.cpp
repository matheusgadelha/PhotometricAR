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
			const std::vector<cv::KeyPoint>& queryKp,
			const std::vector<cv::KeyPoint>& trainKp,
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

}
