#include "CameraCalibration.hpp"

namespace tracking
{
	CameraCalibration::CameraCalibration( float _fx, float _fy, float _cx, float _cy )
	{
		this->intrinsicMatrix[0][0] = _fx;
		this->intrinsicMatrix[1][1] = _fy;
		this->intrinsicMatrix[0][2] = _cx;
		this->intrinsicMatrix[1][2] = _cy;
	}

	cv::Mat CameraCalibration::getIntrinsicMatrix()
	{
		cv::Mat result = cv::Mat::zeros(3,3,CV_32FC1);
		result.at<float>(0,0) = this->intrinsicMatrix[0][0];
		result.at<float>(1,1) = this->intrinsicMatrix[1][1];
		result.at<float>(0,2) = this->intrinsicMatrix[0][2];
		result.at<float>(1,2) = this->intrinsicMatrix[1][2];
		return result;
	}

	/**
	 * Returns projection matrix according to screen size and
	 * intrinsic parameters
	 * @return Projection matrix
	 */
	glm::mat4 CameraCalibration::getProjectionMatrix( int _screen_width, int _screen_height )
	{
		float nearPlane = 0.01f;  // Near clipping distance
  		float farPlane  = 1000.0f;

  		float f_x = fx(); // Focal length in x axis
  		float f_y = fy(); // Focal length in y axis (usually the same?)
  		float c_x = cx(); // Camera primary point x
  		float c_y = cy();

		glm::mat4 projectionMatrix;

		projectionMatrix[0][0] = -2.0f * f_x / _screen_width;
		projectionMatrix[0][1] = 0.0f;
		projectionMatrix[0][2] = 0.0f;
		projectionMatrix[0][3] = 0.0f;

		projectionMatrix[1][0] = 0.0f;
		projectionMatrix[1][1] = 2.0f * f_y / _screen_height;
		projectionMatrix[1][2] = 0.0f;
		projectionMatrix[1][3] = 0.0f;

		projectionMatrix[2][0] = 2.0f * c_x / _screen_width - 1.0f;
		projectionMatrix[2][1] = 2.0f * c_y / _screen_height - 1.0f;    
		projectionMatrix[2][2] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
		projectionMatrix[2][3] = -1.0f;

		projectionMatrix[3][0] = 0.0f;
		projectionMatrix[3][1] = 0.0f;
		projectionMatrix[3][2] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );        
		projectionMatrix[3][3] = 0.0f;

		return projectionMatrix;
	}

	/**
	 * Returns a reference to the fx matrix element
	 */
	float& CameraCalibration::fx()
	{
		return intrinsicMatrix[0][0];
	}

	/**
	 * Returns a reference to the fy matrix element
	 */
	float& CameraCalibration::fy()
	{
		return intrinsicMatrix[1][1];
	}

	/**
	 * Returns a reference to the cx matrix element
	 */
	float& CameraCalibration::cx()
	{
		return intrinsicMatrix[0][2];
	}

	/**
	 * Returns a reference to the cy matrix element
	 */
	float& CameraCalibration::cy()
	{
		return intrinsicMatrix[1][2];
	}


}
