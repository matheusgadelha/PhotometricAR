#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BaseShader.hpp"

namespace rendering
{

	class RenderingWindow
	{
		
		public:
			RenderingWindow( std::string, int, int );

			virtual void render();
			virtual void reshape( int v, int w );
			virtual void start( int argc, char* argv[] );
			virtual void keyboard( unsigned char k, int x, int y );

		// TODO: Change necessary attributes to private fields and create accessors.
		protected:

			BaseShader shader;

			static void renderWrapper();
			static void reshapeWrapper( int v, int w );
			static void keyboardWrapper( unsigned char k, int x, int y );

			static RenderingWindow* instance;

			std::string windowName;
			int width;
			int height;
	};

}

#endif
