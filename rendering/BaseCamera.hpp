#ifndef BASECAMERA_H
#define BASECAMERA_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BaseShader.hpp"
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

using namespace glm;

namespace rendering
{

	class BaseCamera
	{
		
		public:
			BaseCamera();

			mat4 getViewMatrix();
			mat4 getProjectionMatrix();
			mat4 getViewProjectionMatrix();

			void setPerspectiveProjection(
				float _fovy = 45.0f,
				float _aspect = 4/3.0f,
				float _zNear = 0.01f,
				float _zFar = 300.0f
			);

			void setOrthogonalProjection(
				float _left = 0.0f,
				float _right = 400.0f,
				float _up = 0.0,
				float _down = 300.0f,
				float _zNear = 0.01f,
				float _zFar = 300.0f
			);

			void rotate( float angle, vec3 axis);
			void translate( vec3 _trans);
			void goFront( float _step );
			void goBack( float _step );

		protected:

			// mat4 viewMatrix;
			mat4 projectionMatrix;

			vec3 position;
			vec3 up;
			vec3 lookAtDir;
};

}

#endif
