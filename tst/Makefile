CC = g++

CFLAGS = -Wall -msse4.2 -O3

OPENCV_LS = -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_video

OPENCV_INCLUDES = -I/usr/local/include/opencv2 -I/usr/local/include

OPENCV_SO = /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so

OPENCV = $(OPENCV_LS) $(OPENCV_INCLUDES)

LIB_DIR = -L../lib/

OPENGL_LS = -lGL -lGLEW -lglut

LS = -ltrack -lrender

INCLUDES = -I../tracking -I../rendering	

all: tst-track.cpp tst-render.cpp
	$(CC) tst-track.cpp -o tst-track $(CFLAGS) $(LIB_DIR) $(LS) $(OPENCV) $(INCLUDES) 
	mv tst-track ../bin/
	$(CC) tst-render.cpp -o tst-render $(CFLAGS) $(LIB_DIR) $(LS) $(OPENGL_LS) $(INCLUDES) 
	mv tst-render ../bin/
	
clean:
	cd .
