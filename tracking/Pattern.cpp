#include "Pattern.hpp"

namespace tracking
{		

	Pattern::Pattern( const cv::Mat& _pattern_img )
	{
		Helper::getGray( _pattern_img, this->image );
		
		float w = this->image.cols;
		float h = this->image.rows;
		
		this->points2d.push_back( cv::Point2f( 0, 0 ));
		this->points2d.push_back( cv::Point2f( w, 0 ));
		this->points2d.push_back( cv::Point2f( 0, h ));
		this->points2d.push_back( cv::Point2f( w, h ));
		
		this->points3d.push_back( cv::Point3f( 0, 0, 0 ));
		this->points3d.push_back( cv::Point3f( w, 0, 0 ));
		this->points3d.push_back( cv::Point3f( 0, h, 0 ));
		this->points3d.push_back( cv::Point3f( w, h, 0 ));
	}
	
	cv::Size Pattern::getSize()
	{
		return cv::Size( this->image.cols, this->image.rows );
	}
	
}
