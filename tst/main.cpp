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
#include "ARDrawingContext.hpp"
#include "DebugHelpers.hpp"

////////////////////////////////////////////////////////////////////
// Standard includes:
#include <opencv2/opencv.hpp>

#include "RenderingWindow.hpp"
#include "MarkerMesh.hpp"
#include "Sprite.hpp"
#include "BaseCamera.hpp"
#include "RenderingManager.hpp"

using namespace rendering;
using namespace glm;
using std::vector;

const int FRAME_WIDTH = 1280;
const int FRAME_HEIGHT = 720;

class IlluminationWindow : public RenderingWindow {
public:    
    
    Sprite* sprite;
    ARPipeline* pipeline;
    Transformation patternPose;
    Mesh model;
    MarkerMesh markerMesh;
    BaseShader shader2;
    BaseShader spriteShader;
    BaseShader shadowMapShader;
    BaseShader markerShader;
    BaseCamera camera;
    cv::VideoCapture * capture;
    cv::Mat currentFrame;
	bool loadFromImage;
	cv::Mat procFrame;
    
    cv::Mat patternImg;
    cv::Mat frontPatternImg;
    cv::Mat rightPatternImg;
    cv::Mat leftPatternImg;
    
    CameraCalibration calibration;
    mat4 view;
    mat4 projection;
    RenderingManager* manager;

    IlluminationWindow() : RenderingWindow("Illumination Test", FRAME_WIDTH, FRAME_HEIGHT)
    , calibration(
    1.1909911382906164e+03, 1.1966779800715951e+03, 5.8939933247655733e+02, 3.6740283897984250e+02
    ) {
        float distortionCoeffs[] = {
            -3.5468741711036957e-02, -5.1582696807048167e-01,
            1.8045443244378054e-02, -1.2729640751119192e-03,
            9.8664068944900507e-01
        };

        CameraCalibration new_calibration(
            1.1909911382906164e+03,
            1.1966779800715951e+03,
            5.8939933247655733e+02,
            3.6740283897984250e+02,
            distortionCoeffs
        );

        calibration = new_calibration;
    }

    virtual void start(int argc, char* argv[]) {
        RenderingWindow::start(argc, argv);
        camera.usingCustomViewMatrix = true;
        manager = new RenderingManager(camera, vec2(FRAME_WIDTH,FRAME_HEIGHT), vec2(1024,1024));
        manager->renderWithShadows( true );

		if( argc == 6 )
		{
			this->loadFromImage = true;
			procFrame = cv::imread(argv[5]);
		}
		else
		{
			this->loadFromImage = false;
		}

        if(argc < 5)
        {
            std::cout << "Error. More images needed.\n"
					  << "Usage: ./program mainPattern frontPattern rightPattern leftPattern\n";
			std::exit( -1 );
        }

        shader.createCompleteShader("shaders/simple.vert", "shaders/simple.frag");
        spriteShader.createCompleteShader("shaders/Sprite.vert", "shaders/Sprite.frag");
        shadowMapShader.createCompleteShader("shaders/shadowMap.vert", "shaders/shadowMap.frag" );
        markerShader.createCompleteShader("shaders/markerShader.vert", "shaders/markerShader.frag" );

        capture = new cv::VideoCapture("data/testVidBig.avi");
        //capture = new cv::VideoCapture(0);
        buildProjectionMatrix();
        camera.setProjectionMatrix(projection);
        
        sprite = new Sprite("data/test-001.png", spriteShader);

        model.load("models/dragon.obj", shader, shadowMapShader);
        model.castShadows = true;
        model.shadowMapTexId = manager->depthTexture;

        manager->add(*sprite);
        manager->add(model);
        
        patternImg = cv::imread(argv[1]);
        
        cv::Point3f pixelSizeCube;
        pixelSizeCube.x = patternImg.cols;
        pixelSizeCube.z = patternImg.rows;
        
        frontPatternImg = cv::imread(argv[2]);
        
        pixelSizeCube.y = (patternImg.cols/(float)frontPatternImg.cols) * frontPatternImg.rows;

        rightPatternImg = cv::imread(argv[3]);
        leftPatternImg = cv::imread(argv[4]);
        
        cv::resize( frontPatternImg, frontPatternImg, cv::Size( pixelSizeCube.x, pixelSizeCube.y ) );
        cv::resize( rightPatternImg, rightPatternImg, cv::Size( pixelSizeCube.z, pixelSizeCube.y ) );
        cv::resize( leftPatternImg, leftPatternImg, cv::Size( pixelSizeCube.z, pixelSizeCube.y ) );
        
//        cv::imshow("FRONT", frontPatternImg);
//        cv::imshow("RIGHT", rightPatternImg);
//        cv::imshow("LEFT", leftPatternImg);
//        cv::waitKey();
        
        pipeline = new ARPipeline(patternImg, frontPatternImg, rightPatternImg, leftPatternImg, calibration);
//        pipeline = new ARPipeline(patternImg, calibration);
        
        markerMesh.load( vec2(patternImg.cols,patternImg.rows), markerShader, shadowMapShader );
        markerMesh.castShadows = true;
        markerMesh.shadowMapTexId = manager->depthTexture;
//        
        manager->add(markerMesh);
//        manager->calcAmbientOcclusion();

        // manager->add( macaca2 );

        glutMainLoop();
    }

    // 

    virtual void idle() {
		
		if( this->loadFromImage )
		{
			currentFrame = this->procFrame;
		}
		else
		{
	        (*capture) >> currentFrame;
		}

        //std::cout << currentFrame.cols << " " << currentFrame.rows << std::endl;

        if (!currentFrame.empty()) {
            bool isPatternFound = pipeline->processFrame(currentFrame);
            patternPose = pipeline->getPatternLocation();

            if (isPatternFound) {
                camera.setCustomViewMatrix(patternPose.getGlmMat44());
            }

            sprite->updateTextureData(currentFrame);

//            sprite->assignBufferTexToData(manager->depthTexture);
            this->render();
        }
    }

    virtual void render() {

        glClearColor(0.0, 0.0, 0.0, 1.0);
        // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
       
	   	cv::Point3f dirp = pipeline->computeLightDirection();
	   	vec3 lightDir( dirp.x, dirp.y, dirp.z );
		lightDir = glm::normalize(lightDir);
        
		mat4 normalMat = glm::transpose(glm::inverse(camera.getViewMatrix() * model.modelMatrix));

		vec4 lightDir4 = normalMat * vec4( lightDir, 1 );
		lightDir.x = lightDir4.x;
		lightDir.y = lightDir4.y;
		lightDir.z = lightDir4.z;
		lightDir = glm::normalize(lightDir);

		std::cout << lightDir.x << " " << lightDir.y << " " << lightDir.z << std::endl;

		manager->light.direction = glm::normalize(lightDir);
		//manager->light.direction = vec3(0,0,-1);
        manager->render();
        
        vec3 calcColor( pipeline->currentDiffuseColor[2], 
                pipeline->currentDiffuseColor[1], 
                pipeline->currentDiffuseColor[0] );
        
        vec3 ambLight( pipeline->currentAmbientLight[2],
                pipeline->currentAmbientLight[1],
                pipeline->currentAmbientLight[0]);
        
        model.lightColor = calcColor;
        model.ambientLight = ambLight;

        glutSwapBuffers();
    }

    void buildProjectionMatrix() {
        float nearPlane = 0.01f; // Near clipping distance
        float farPlane = 1000.0f; // Far clipping distance

        // Camera parameters
        float f_x = calibration.fx(); // Focal length in x axis
        float f_y = calibration.fy(); // Focal length in y axis (usually the same?)
        float c_x = calibration.cx(); // Camera primary point x
        float c_y = calibration.cy(); // Camera primary point y

        projection[0][0] = -2.0f * f_x / FRAME_WIDTH;
        projection[0][1] = 0.0f;
        projection[0][2] = 0.0f;
        projection[0][3] = 0.0f;

        projection[1][0] = 0.0f;
        projection[1][1] = 2.0f * f_y / FRAME_HEIGHT;
        projection[1][2] = 0.0f;
        projection[1][3] = 0.0f;

        projection[2][0] = 2.0f * c_x / FRAME_WIDTH - 1.0f;
        projection[2][1] = 2.0f * c_y / FRAME_HEIGHT - 1.0f;
        projection[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        projection[2][3] = -1.0f;

        projection[3][0] = 0.0f;
        projection[3][1] = 0.0f;
        projection[3][2] = -2.0f * farPlane * nearPlane / (farPlane - nearPlane);
        projection[3][3] = 0.0f;
    }
};

int main(int argc, char* argv[]) {
    //    const char* windowName = "Augmented Scene";
    RenderingWindow* window = new IlluminationWindow();
    window->start(argc, argv);

    return 0;
}
//
//bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx);
//void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image);
//void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration );
//
//void processVideo(const cv::Mat& patternImage, CameraCalibration& calibration )
//{
//    cv::VideoCapture capture(0);
//    // Grab first frame to get the frame dimensions
//    cv::Mat currentFrame;  
//    capture >> currentFrame;
//
//    // Check the capture succeeded:
//    if (currentFrame.empty())
//    {
//        std::cout << "Cannot open video capture device" << std::endl;
//        return;
//    }
//
//    cv::Size frameSize(currentFrame.cols, currentFrame.rows);
//
//    ARPipeline pipeline(patternImage, calibration);
//    ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);
//
//    bool shouldQuit = false;
//    do
//    {
//        capture >> currentFrame;
//        if (currentFrame.empty())
//        {
//            shouldQuit = true;
//            continue;
//        }
//
//        shouldQuit = processFrame(currentFrame, pipeline, drawingCtx);
//    } while (!shouldQuit);
//}
//
//void processSingleImage(const cv::Mat& patternImage, CameraCalibration& calibration, const cv::Mat& image)
//{
//    cv::Size frameSize(image.cols, image.rows);
//    ARPipeline pipeline(patternImage, calibration);
//    ARDrawingContext drawingCtx("Markerless AR", frameSize, calibration);
//
//    bool shouldQuit = false;
//    do
//    {
//        shouldQuit = processFrame(image, pipeline, drawingCtx);
//    } while (!shouldQuit);
//}
//
//bool processFrame(const cv::Mat& cameraFrame, ARPipeline& pipeline, ARDrawingContext& drawingCtx)
//{
//    // Clone image used for background (we will draw overlay on it)
//    cv::Mat img = cameraFrame.clone();
//
//    // Draw information:
//    if (pipeline.m_patternDetector.enableHomographyRefinement)
//        cv::putText(img, "Pose refinement: On   ('h' to switch off)", cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
//    else
//        cv::putText(img, "Pose refinement: Off  ('h' to switch on)",  cv::Point(10,15), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
//
//    cv::putText(img, "RANSAC threshold: " + ToString(pipeline.m_patternDetector.homographyReprojectionThreshold) + "( Use'-'/'+' to adjust)", cv::Point(10, 30), CV_FONT_HERSHEY_PLAIN, 1, CV_RGB(0,200,0));
//
//    // Set a new camera frame:
//    drawingCtx.updateBackground(img);
//
//    // Find a pattern and update it's detection status:
//    drawingCtx.isPatternPresent = pipeline.processFrame(cameraFrame);
//
//    // Update a pattern pose:
//    drawingCtx.patternPose = pipeline.getPatternLocation();
//
//    // Request redraw of the window:
//    drawingCtx.updateWindow();
//
//    // Read the keyboard input:
//    int keyCode = cv::waitKey(5); 
//
//    bool shouldQuit = false;
//    if (keyCode == '+' || keyCode == '=')
//    {
//        pipeline.m_patternDetector.homographyReprojectionThreshold += 0.2f;
//        pipeline.m_patternDetector.homographyReprojectionThreshold = std::min(10.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
//    }
//    else if (keyCode == '-')
//    {
//        pipeline.m_patternDetector.homographyReprojectionThreshold -= 0.2f;
//        pipeline.m_patternDetector.homographyReprojectionThreshold = std::max(0.0f, pipeline.m_patternDetector.homographyReprojectionThreshold);
//    }
//    else if (keyCode == 'h')
//    {
//        pipeline.m_patternDetector.enableHomographyRefinement = !pipeline.m_patternDetector.enableHomographyRefinement;
//    }
//    else if (keyCode == 27 || keyCode == 'q')
//    {
//        shouldQuit = true;
//    }
//
//    return shouldQuit;
//}
//
//
//int main(int argc, const char * argv[])
//{
//    // Change this calibration to yours:
//    CameraCalibration calibration(1.1909911382906164e+03, 1.1966779800715951e+03, 5.8939933247655733e+02, 3.6740283897984250e+02);
//    
//    if (argc < 2)
//    {
//        std::cout << "Input image not specified" << std::endl;
//        std::cout << "Usage: markerless_ar_demo <pattern image> [filepath to recorded video or image]" << std::endl;
//        return 1;
//    }
//
//    // Try to read the pattern:
//    cv::Mat patternImage = cv::imread(argv[1]);
//    if (patternImage.empty())
//    {
//        std::cout << "Input image cannot be read" << std::endl;
//        return 2;
//    }
//
//    if (argc == 2)
//    {
//        processVideo(patternImage, calibration);
//    }
//    else if (argc == 3)
//    {
//        std::string input = argv[2];
//        cv::Mat testImage = cv::imread(input);
//        if (!testImage.empty())
//        {
//            processSingleImage(patternImage, calibration, testImage);
//        }
//        else 
//        {
//            cv::VideoCapture cap;
//            if (cap.open(input))
//            {
//                processVideo(patternImage, calibration);
//            }
//        }
//    }
//    else
//    {
//        std::cerr << "Invalid number of arguments passed" << std::endl;
//        return 1;
//    }
//
//    return 0;
//}


