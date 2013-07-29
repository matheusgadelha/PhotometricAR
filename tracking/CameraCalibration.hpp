#ifndef CAMERACALIBRATION_HPP
#define CAMERACALIBRATION_HPP

#include <vector>
#include <string>
#include <cassert>
#include <ctime>
#include <cmath>
#include "Helper.hpp" //getGray
#include "Pattern.hpp" //Pattern
#include "opencv2/opencv.hpp"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

namespace tracking
{
	class CameraCalibration
	{
		public:
			CameraCalibration( float _fx, float _fy, float _cx, float _cy );

			cv::Mat getIntrinsicMatrix();
			glm::mat4 getProjectionMatrix( int _screen_width, int _screen_height );

		private:
			glm::mat3 intrinsicMatrix;
			float& fx();
			float& fy();
			float& cx();
			float& cy();
	};
}

#endif