/*****************************************************************************
*   Markerless AR desktop application.
******************************************************************************
*   by Khvedchenia Ievgen, 5th Dec 2012
*   http://computer-vision-talks.com
******************************************************************************
*   Ch3 of the book "Mastering OpenCV with Practical Computer Vision Projects"
*   Copyright Packt Publishing 2012.
*   http://www.packtpub.com/cool-projects-with-opencv/book
*****************************************************************************/
#include <GL/glew.h>
#include <GL/freeglut.h>
////////////////////////////////////////////////////////////////////
// File includes:
#include "ARPipeline.hpp"
#include "DebugHelpers.hpp"

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>

#include "RenderingWindow.hpp"
#include "Mesh.hpp"
#include "Sprite.hpp"
#include "BaseCamera.hpp"
#include "RenderingManager.hpp"

using namespace rendering;
using namespace glm;
using std::vector;

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
        ARPipeline* pipeline;

        Transformation patternPose;

        Mesh macaca;

        BaseShader shader2;
        BaseShader spriteShader;

        BaseCamera camera;

        cv::VideoCapture * capture;
        cv::Mat currentFrame;

        cv::Mat patternImg;

        CameraCalibration calibration;

        mat4 view;
        mat4 projection;

        RenderingManager* manager;

        virtual void start( int argc, char* argv[]) 
        {
            RenderingWindow::start( argc, argv );

            camera.usingCustomViewMatrix = true;

            shader.createCompleteShader( "shaders/simple.vert", "shaders/simple.frag" );

            spriteShader.createCompleteShader("shaders/Sprite.vert", "shaders/Sprite.frag");

            capture = new cv::VideoCapture(0);
            buildProjectionMatrix();
            camera.setProjectionMatrix( projection );

            sprite = new Sprite("data/algebra.jpg", spriteShader);
                        
            macaca.load( "models/suzanne.obj", shader );

            manager->add( *sprite );
            manager->add( macaca );

            patternImg = cv::imread( "data/emtec.jpg");
            pipeline = new ARPipeline( patternImg, calibration );

            // manager->add( macaca2 );

            glutMainLoop();
        }

        // 

        virtual void idle()
        {
            (*capture) >> currentFrame;
            if( ! currentFrame.empty() )
            {
                bool isPatternFound = pipeline->processFrame( currentFrame );
                patternPose = pipeline->getPatternLocation();

                if( isPatternFound )
                {
                    camera.setCustomViewMatrix( patternPose.getGlmMat44() );
                }

                sprite->updateTextureData( currentFrame );
                this->render();
            }
        }

        virtual void render()
        {

            glClearColor(0.0, 0.0, 0.0, 1.0);
            // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
            glClear( GL_COLOR_BUFFER_BIT );

            manager->render();

            glutSwapBuffers();
        }

        void buildProjectionMatrix()
        {
            float nearPlane = 0.01f;  // Near clipping distance
            float farPlane  = 100.0f;  // Far clipping distance

            // Camera parameters
            float f_x = calibration.fx(); // Focal length in x axis
            float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
            float c_x = calibration.cx(); // Camera primary point x
            float c_y = calibration.cy(); // Camera primary point y

            projection[0][0] = -2.0f * f_x / 800;
            projection[0][1] = 0.0f;
            projection[0][2] = 0.0f;
            projection[0][3] = 0.0f;

            projection[1][0] = 0.0f;
            projection[1][1] = 2.0f * f_y / 600;
            projection[1][2] = 0.0f;
            projection[1][3] = 0.0f;

            projection[2][0] = 2.0f * c_x / 800 - 1.0f;
            projection[2][1] = 2.0f * c_y / 600 - 1.0f;    
            projection[2][2] = -( farPlane + nearPlane) / ( farPlane - nearPlane );
            projection[2][3] = -1.0f;

            projection[3][0] = 0.0f;
            projection[3][1] = 0.0f;
            projection[3][2] = -2.0f * farPlane * nearPlane / ( farPlane - nearPlane );        
            projection[3][3] = 0.0f;
        }
};

int main( int argc, char* argv[] )
{
    const char* windowName = "Augmented Scene";
    RenderingWindow* window = new IlluminationWindow();
    window->start( argc, argv );

    return 0;
}

// void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration )
// {
//     cv::VideoCapture capture(0);
//     // Grab first frame to get the frame dimensions
//     cv::Mat currentFrame;  
//     capture >> currentFrame;

//     // Check the capture succeeded:
//     if (currentFrame.empty())
//     {
//         std::cout << "Cannot open video capture device" << std::endl;
//         return;
//     }

//     cv::Size frameSize(currentFrame.cols, currentFrame.rows);

//     ARPipeline pipeline(patternImage, calibration);
//     ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

//     bool shouldQuit = false;
//     do
//     {
//         capture >> currentFrame;
//         if (currentFrame.empty())
//         {
//             shouldQuit = true;
//             continue;
//         }

//         shouldQuit = processFrame(currentFrame, pipeline, drawingCtx);
//     } while (!shouldQuit);
// }

// void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image)
// {
//     cv::Size frameSize(image.cols, image.rows);
//     ARPipeline pipeline(patternImage, calibration);
//     ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);

//     bool shouldQuit = false;
//     do
//     {
//         shouldQuit = processFrame(image, pipeline, drawingCtx);
//     } while (!shouldQuit);
// }

// bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx)
// {
//     // Clone image used for background (we will draw overlay on it)
//     cv::Mat img = cameraFrame.clone();

//     // Draw information:
//     if (pipeline.m_patternDetector.enableHomographyRefinement)
//         cv::putText(img, "Pose refinement: On   ('h' to switch off)", cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
//     else
//         cv::putText(img, "Pose refinement: Off  ('h' to switch on)",  cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

//     cv::putText(img, "RANSAC threshold: " + ToString(pipeline.m_patternDetector.homographyReprojectionThreshold) + "( Use'-'/'+' to adjust)", cv::Point(10, 30), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));

//     // Set a new camera frame:
//     drawingCtx.updateBackground(img);

//     // Find a pattern and update it's detection status:
//     drawingCtx.isPatternPresent = pipeline.processFrame(cameraFrame);

//     // Update a pattern pose:
//     drawingCtx.patternPose = pipeline.getPatternLocation();

//     // Request redraw of the window:
//     drawingCtx.updateWindow();

//     // Read the keyboard input:
//     int keyCode = cv::waitKey(5); 

//     bool shouldQuit = false;
//     if (keyCode == '+' || keyCode == '=')
//     {
//         pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2f;
//         pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
//     }
//     else if (keyCode == '-')
//     {
//         pipeline.m_patternDetector.homographyReprojectionThreshold -= 0.2f;
//         pipeline.m_patternDetector.homographyReprojectionThreshold = std::max(0.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
//     }
//     else if (keyCode == 'h')
//     {
//         pipeline.m_patternDetector.enableHomographyRefinement = !pipeline.m_patternDetector.enableHomographyRefinement;
//     }
//     else if (keyCode == 27 || keyCode == 'q')
//     {
//         shouldQuit = true;
//     }

//     return shouldQuit;
// }


