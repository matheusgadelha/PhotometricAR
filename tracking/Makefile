CC = g++

CFLAGS = -Wall -msse4.2 -O3

OPENGL_LS = -lGL -lGLEW -lglut

INC_DIR_GLM = -I../glm

OPENCV_LS = -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_video

OPENCV_INCLUDES = -I/usr/local/include/opencv2 -I/usr/local/include

OPENCV_SO = /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so

OPENCV = $(OPENCV_LS) $(OPENCV_INCLUDES)

all: ARDrawingContext.o Pattern.o ARPipeline.o PatternDetector.o GeometryTypes.o CameraCalibration.o
	ar rvs libtrack.a ARDrawingContext.o Pattern.o ARPipeline.o PatternDetector.o GeometryTypes.o CameraCalibration.o
	ranlib libtrack.a
	mv libtrack.a ../lib/

CameraCalibration.o: CameraCalibration.cpp
	$(CC) CameraCalibration.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)

ARDrawingContext.o: ARDrawingContext.cpp
	$(CC) ARDrawingContext.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)
	
Pattern.o: Pattern.cpp
	$(CC) Pattern.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)
	
ARPipeline.o: ARPipeline.cpp
	$(CC) ARPipeline.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)
	
PatternDetector.o: PatternDetector.cpp
	$(CC) PatternDetector.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)

GeometryTypes.o: GeometryTypes.cpp
	$(CC) GeometryTypes.cpp -c $(CFLAGS) $(OPENGL_LS) $(OPENCV) $(INC_DIR_GLM)
	
clean:
	rm -r *.o
