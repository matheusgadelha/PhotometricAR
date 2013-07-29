#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include "RenderingWindow.hpp"
#include "Mesh.hpp"
#include "Sprite.hpp"
#include "BaseCamera.hpp"
#include "RenderingManager.hpp"

#include "Tracker.hpp"
#include "PatternDetector.hpp"
#include "MultiSampledDetector.hpp"

using namespace rendering;
using namespace tracking;
using namespace glm;
using std::vector;

#define BUFFER_OFFSET(i) ((void*)(i))

class IlluminationWindow : public RenderingWindow
{
	public:

		IlluminationWindow() : RenderingWindow( "Illumination Test", 800, 600 )
		, calibration(
			7.7629785490292102e+02,
			7.7089386791151753e+02,
			2.9624412505877865e+02,
			2.0158242775595554e+02
		)
		{
			manager = new RenderingManager( camera );
		}

		Sprite* sprite;
		Mesh macaca;
		Mesh macaca2;

		BaseShader shader2;
		BaseShader spriteShader;

		BaseCamera camera;

		cv::VideoCapture * capture;
		cv::Mat currentFrame;

		Tracker * tracker;
		cv::Mat patternImg;

		CameraCalibration calibration;

		mat4 view;

		RenderingManager* manager;

		virtual void start( int argc, char* argv[]) 
		{
			RenderingWindow::start( argc, argv );

			camera.usingCustomViewMatrix = false;
			camera.setProjectionMatrix(
				calibration.getProjectionMatrix(800,600) );

			shader.createCompleteShader( "shaders/simple.vert", "shaders/simple.frag" );
			shader2.createCompleteShader( "shaders/alternate.vert", "shaders/alternate.frag" );

			spriteShader.createCompleteShader("shaders/Sprite.vert", "shaders/Sprite.frag");

			capture = new cv::VideoCapture(0);

			sprite = new Sprite("data/algebra.jpg", spriteShader);
						
			macaca.load( "models/suzanne.obj", shader );
			macaca2.load( "models/suzanne.obj", shader2 );

			manager->add( *sprite );
			manager->add( macaca );

			patternImg = cv::imread( "data/emtec.jpg");
			tracker = new Tracker( patternImg );

			// manager->add( macaca2 );

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

		virtual void idle()
		{
			(*capture) >> currentFrame;
			tracker->processFrame( currentFrame );

			if( tracker->patternDetector.patternFound )
			{
				camera.setCustomViewMatrix( computeViewMat() );
			}

			sprite->updateTextureData( currentFrame );
			this->render();
		}

		mat4 computeViewMat()
		{
			mat4 result;

			cv::Mat pose;
			tracker->patternDetector.cameraPoseFromHomography(
				tracker->getHomography(),
				pose,
				calibration
			);

			for( unsigned i=0; i<3; ++i )
			{
				for( unsigned j=0; j<4; ++j )
				{
					result[i][j] = pose.at<float>(i,j);
				}
			}

			for( unsigned i=0; i<4; ++i )
			{
				for( unsigned j=0; j<4; ++j )
				{
					std::cout << result[i][j] << " ";
				}
			}
			
			return result;
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

	return 0;
}