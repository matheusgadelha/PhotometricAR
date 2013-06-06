CC = g++

CFLAGS = -Wall -msse4.2 -O3 -ggdb

OPENGL_LS = -lGL -lGLEW -lglut

INC_DIR_GLM = -I../glm

# OPENCV_LS = -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_objdetect -lopencv_video

# OPENCV_INCLUDES = -I/usr/local/include/opencv2 -I/usr/local/include

# OPENCV_SO = /usr/local/lib/libopencv_calib3d.so /usr/local/lib/libopencv_contrib.so /usr/local/lib/libopencv_core.so /usr/local/lib/libopencv_features2d.so /usr/local/lib/libopencv_flann.so /usr/local/lib/libopencv_gpu.so /usr/local/lib/libopencv_highgui.so /usr/local/lib/libopencv_imgproc.so /usr/local/lib/libopencv_legacy.so /usr/local/lib/libopencv_ml.so /usr/local/lib/libopencv_nonfree.so /usr/local/lib/libopencv_objdetect.so /usr/local/lib/libopencv_photo.so /usr/local/lib/libopencv_stitching.so /usr/local/lib/libopencv_ts.so /usr/local/lib/libopencv_video.so /usr/local/lib/libopencv_videostab.so

# OPENCV = $(OPENCV_LS) $(OPENCV_INCLUDES)

all: RenderingWindow.o BaseShader.o Mesh.o BaseCamera.o  RenderableObject.o RenderingManager.o Sprite.o
	ar rvs librender.a RenderingWindow.o BaseShader.o Mesh.o BaseCamera.o RenderableObject.o RenderingManager.o Sprite.o
	ranlib librender.a
	mv librender.a ../lib/

Sprite.o: Sprite.cpp
	$(CC) Sprite.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

RenderableObject.o: RenderableObject.cpp
	$(CC) RenderableObject.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

RenderingManager.o: RenderingManager.cpp
	$(CC) RenderingManager.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

RenderingWindow.o: RenderingWindow.cpp
	$(CC) RenderingWindow.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

Mesh.o: Mesh.cpp
	$(CC) Mesh.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

BaseShader.o: BaseShader.cpp
	$(CC) BaseShader.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)

BaseCamera.o: BaseCamera.cpp
	$(CC) BaseCamera.cpp -c $(CFLAGS) $(OPENGL_LS) $(INC_DIR_GLM)
	
clean:
	rm -r *.o
