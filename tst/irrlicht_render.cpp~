#include <irrlicht.h>
#include <opencv2/opencv.hpp>
#include <thread>
#include <iostream>

using namespace irr;

using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

bool g_running = true;

// Process keyboard event
class MyEventReceiver : public IEventReceiver
{
public:
    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
            if(event.KeyInput.Key == KEY_ESCAPE) {
                g_running = false;
            }
        }

        return false;
    }

};

void videoCaptureFunc( cv::Mat& _frame )
{
	cv::VideoCapture capture(0);
	while( true )
	{
		capture >> _frame;
		
		if( not _frame.empty() )
		{
			std::cout << "Frame\n";
		}
		
	}
}

int main()
{
//	
//	if (!device)
//		return 1;
//		
//	device->setWindowCaption(L"Irrlicht Rendering Test ");
	
//	/*
//	Get a pointer to the VideoDriver, the SceneManager and the graphical
//	user interface environment, so that we do not always have to write
//	device->getVideoDriver(), device->getSceneManager(), or
//	device->getGUIEnvironment().
//	*/
//	IVideoDriver* driver = device->getVideoDriver();
//	ISceneManager* smgr = device->getSceneManager();
//	IGUIEnvironment* guienv = device->getGUIEnvironment();
//	
//	/*
//	We add a hello world label to the window, using the GUI environment.
//	The text is placed at the position (10,10) as top left corner and
//	(260,22) as lower right corner.
//	*/
//	guienv->addStaticText(L"This is a Macaca!",
//		rect<s32>(10,10,242,26), true);
//		
//	/*
//	Add texture to store camera frames
//	*/
//	ITexture* frame_tex =
//		driver->addTexture(vector2d<u32>(WINDOW_WIDTH, WINDOW_HEIGHT), "video_stream");
//		
//	/*
//	Add a dull scene node to put a point light with a circular animation.
//	It would be better to put a directional one, but I was not able to find one
//	in the documentation.
//	If I create a custom scene node, as the tutorial 4 explains, it is possible
//	to create a scene node, anyway.
//	*/
//	ISceneNode* light_node = 0;
//	light_node = smgr->addLightSceneNode(0, vector3df(0,10,0),
//		SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f);
//	
//	ISceneNodeAnimator* anim = 0;
//	anim = smgr->createFlyCircleAnimator (vector3df(0,150,0),250.0f);
//	light_node->addAnimator(anim);
//	anim->drop();
//	
//	/*
//	Create a camera controlled like a FPS camera.
//	*/
//	ISceneNode* camera_node = 0;
//	camera_node = smgr->addCameraSceneNodeFPS();
//	camera_node->setPosition( vector3df(-50.0f, 80.0f, 80.0f) );
//	static_cast<ICameraSceneNode*>(camera_node)->setTarget( vector3df(0.0f) );
//		
//	/*
//	Adding a simple .obj model.
//	*/
//	IAnimatedMesh* mesh = smgr->getMesh("models/suzanne.obj");
//	if (!mesh)
//	{
//		device->drop();
//		return 1;
//	}
//	IAnimatedMeshSceneNode* node = smgr->addAnimatedMeshSceneNode( mesh );
//	/*
//	Scale suzanne mesh to make it look bigger.
//	*/
//	smgr->getMeshManipulator()->scale( mesh, vector3df(40.0f));
//	/*
//	Playing with suzanne material...
//	*/
//	node->setMaterialFlag(EMF_ANTI_ALIASING, true);
//	
//	/*
//	The model needs to look better. Now I will add a simple light.
//	*/
//	if (node)
//	{
//		node->setMaterialFlag(EMF_LIGHTING, true);
//	}
	
		/*
	Create an OpenCV object to capture camera frames and an image to store it.
	We also create a simple std thread to execute this action.
	*/
//	cv::VideoCapture capture(0);
	cv::Mat camera_frame;
	std::thread video_capture_thread( videoCaptureFunc, std::ref(camera_frame) );
	
	/*
	Start rendering loop and video capture thread
	*/
	video_capture_thread.detach();
	
	MyEventReceiver receiver;
	
	
	IrrlichtDevice* device =
		createDevice( video::EDT_OPENGL,
			dimension2d<u32>(WINDOW_WIDTH,WINDOW_HEIGHT), 16,
			false, false, false, &receiver);
//	while( camera_frame.empty() )
//		capture >> camera_frame;
		
//	while(device->run())
	while( true && g_running )
	{
//		if( cv::waitKey(50) >= 0 ) break;
//		if( !capture.grab() )
//		{
//			std::cout << "Unable to grab camera grame\n";
//		}
//		
//		capture >> camera_frame;
//		
//		if( ! camera_frame.empty() )
//		{
//			std::cout << "Camera Frame\n";	
//			cv::imshow("Video Stream", camera_frame);
//			/*
//			Converting an OpenCV Mat image to an Irrlicht texture.
//			*/
//			unsigned char *tex_buf = (unsigned char*)frame_tex->lock();
//		    unsigned char *frame_buf = camera_frame.data;
//		    // Convert from RGB to RGBA
//		    for(int y=0; y < camera_frame.rows; y++) {
//		        for(int x=0; x < camera_frame.cols; x++) {
//		        	*(tex_buf++) = *(frame_buf++);
//		            *(tex_buf++) = *(frame_buf++);
//		            *(tex_buf++) = *(frame_buf++);
//		            *(tex_buf++) = 255;
//		        }
//		    }
//		    frame_tex->unlock();
//		    
//		}
		/*
		Anything can be drawn between a beginScene() and an endScene()
		call. The beginScene() call clears the screen with a color and
		the depth buffer, if desired. Then we let the Scene Manager and
		the GUI Environment draw their content. With the endScene()
		call everything is presented on the screen.
		*/
//		driver->beginScene(true, true, SColor(255,100,101,140));
//		
//		/*
//		Draws camera image on screen.
//		*/
//		driver->draw2DImage(frame_tex, core::rect<s32>(0,0,WINDOW_WIDTH,WINDOW_HEIGHT),
//			core::rect<s32>(0,0,WINDOW_WIDTH,WINDOW_HEIGHT));
//		
//		smgr->drawAll();
//		guienv->drawAll();
//		
//		driver->endScene();
	}
	
//	device->drop();
	
	return 0;
	
}
