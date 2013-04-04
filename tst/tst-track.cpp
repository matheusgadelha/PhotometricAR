#include "Tracker.hpp" //Tracker
#include "PatternDetector.hpp" //PatternDetector

#include <cstdlib> //exit
#include <iostream> //std::cerr, std::cout

int main( int argc, char** argv )
{
	if( argc < 2 )
	{
		std::cerr << "FATAL ERROR: Wrong program usage.\n"
							<< "\tEXAMPLE: ./track <pattern_image_path>\n"
							<< "Terminating...\n";
		exit( -1 );
	}
	else
	{
		cv::VideoCapture capture(0);
	
		cv::Mat pattern_image = cv::imread(argv[1]);
		cv::Mat currentFrame, previousFrame;
		
		tracking::Tracker aTracker( pattern_image );
		tracking::PatternDetector patternDetector( pattern_image );		
		
		cv::namedWindow( "Pattern Image" );
		cv::imshow( "Pattern Image", pattern_image );
		
		while( true )
		{
			capture >> currentFrame;
			if( currentFrame.empty() )
			{
				std::cout << "ERROR: Cannot open camera device...\n";
				return -1;
			}
			
			if ( (cvWaitKey(10) & 255) == 27 ) break;
			if( previousFrame.empty() )
			{
				patternDetector.processFrame( currentFrame );
			}
			else
			{
				// patternDetector.flowTracking( previousFrame, currentFrame );
				patternDetector.processFrame( currentFrame );
			}
			cv::imshow( "Video", currentFrame );
			previousFrame = currentFrame.clone();
		}
		
	}
	
	return 0;	  
}
