#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include "RenderingWindow.hpp"
#include "Mesh.hpp"
#include "BaseCamera.hpp"
#include "RenderingManager.hpp"

#include "Tracker.hpp"
#include "PatternDetector.hpp"
#include "MultiSampledDetector.hpp"

using namespace rendering;
using namespace glm;
using std::vector;

#define BUFFER_OFFSET(i) ((void*)(i))

class IlluminationWindow : public RenderingWindow
{
	public:

		IlluminationWindow() : RenderingWindow( "Illumination Test", 800, 600 )
		{
			manager = new RenderingManager( camera );
		}

		Mesh macaca;
		Mesh macaca2;

		BaseShader shader2;

		BaseCamera camera;

		cv::VideoCapture* capture;
		cv::Mat currentFrame;

		RenderingManager* manager;

		virtual void start( int argc, char* argv[]) 
		{
			RenderingWindow::start( argc, argv );

			shader.createCompleteShader( "shaders/simple.vert", "shaders/simple.frag" );
			shader2.createCompleteShader( "shaders/alternate.vert", "shaders/alternate.frag" );
						
			macaca.load( "models/suzanne.obj", shader );
			macaca2.load( "models/suzanne.obj", shader2 );

			manager->add( macaca );
			manager->add( macaca2 );

			glutMainLoop();
		}

		virtual void keyboard( unsigned char k, int x, int y )
		{
			if( k=='a')
			{
				camera.rotate( 1.0f, vec3(0.0f, 1.0f, 0.0f));
			}
			if( k=='d')
			{
				camera.rotate( -1.0f, vec3(0.0f, 1.0f, 0.0f));
			}
			if( k=='s')
			{
				camera.goBack( 0.1f );
			}
			if( k=='w')
			{
				camera.goFront( 0.1f );
			}
		}

		virtual void render()
		{

			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear( GL_COLOR_BUFFER_BIT );
			
			manager->render();

			glutSwapBuffers();
		}

};

// void renderGL(void* param)
// {
// 	glClearColor(0.0, 0.0, 0.0, 1.0);
// 	glClear( GL_COLOR_BUFFER_BIT );
			
// 	// macaca.draw( camera );
// 	glFlush();

// }

int main( int argc, char* argv[] )
{
	const char* windowName = "Augmented Scene";
	RenderingWindow* window = new IlluminationWindow();
	window->start( argc, argv );
	// cv::VideoCapture capture(0);
	// cv::Mat currentFrame;

	// cv::namedWindow( windowName, CV_WINDOW_OPENGL );
	// cv::resizeWindow( windowName, 640, 480 );
	// cv::setOpenGlContext( windowName );
	// cv::setOpenGlDrawCallback( windowName, renderGL );

	// shader.createCompleteShader( "shaders/simple.vert", "shaders/simple.frag" );
	// // macaca.load( "models/suzanne.obj", shader );

	// while(true)
	// {
	// 	cv::updateWindow(windowName);
	// }

	return 0;
}