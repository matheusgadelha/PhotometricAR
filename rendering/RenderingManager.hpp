#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BaseCamera.hpp"
#include "RenderableObject.hpp"

namespace rendering
{

	class RenderingManager
	{
		public:

			RenderingManager( BaseCamera& );
			~RenderingManager();

			void render();
			void add( RenderableObject& obj );

		private:
			BaseCamera* camera;
			std::vector<RenderableObject*> objects;
	};

}

#endif