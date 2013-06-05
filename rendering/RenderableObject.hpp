#ifndef RENDERABLEOBJ_H
#define RENDERABLEOBJ_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BaseCamera.hpp"

namespace rendering
{

	class RenderableObject
	{
		public:
			RenderableObject();
			~RenderableObject();

			virtual void draw( BaseCamera& );

			bool isActive;

	};
}

#endif