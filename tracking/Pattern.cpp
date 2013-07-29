#include "Pattern.hpp"

namespace tracking
{		

	Pattern::Pattern( const cv::Mat& _pattern_img )
	{
		Helper::getGray( _pattern_img, this->image );
		this->image_color = _pattern_img;
		
		float w = this->image.cols;
		float h = this->image.rows;

		this->points2d.resize(4);
		this->points3d.resize(4);
		
		const float maxSize = std::max(w,h);
	    const float unitW = w / maxSize;
	    const float unitH = h / maxSize;

	    this->points2d[0] = cv::Point2f(0,0);
	    this->points2d[1] = cv::Point2f(w,0);
	    this->points2d[2] = cv::Point2f(0,h);
	    this->points2d[3] = cv::Point2f(w,h);

	    this->points3d[0] = cv::Point3f(-unitW, -unitH, 0);
	    this->points3d[1] = cv::Point3f( unitW, -unitH, 0);
	    this->points3d[2] = cv::Point3f(-unitW,  unitH, 0);
	    this->points3d[3] = cv::Point3f( unitW,  unitH, 0);
	}
	
	cv::Size Pattern::getSize()
	{
		return cv::Size( this->image.cols, this->image.rows );
	}
	
}
