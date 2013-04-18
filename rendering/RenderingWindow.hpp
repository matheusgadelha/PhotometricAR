#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>


namespace rendering
{

	class RenderingWindow
	{
		
		public:
			RenderingWindow( std::string, int, int );

			void render();
			void reshape( int v, int w );
			void start( int argc, char* argv[] );

		private:

			static void renderWrapper();
			static void reshapeWrapper( int v, int w );

			static RenderingWindow* instance;

			std::string windowName;
			int width;
			int height;
	};

}

#endif
