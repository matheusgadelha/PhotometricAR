#ifndef SPRITE_H
#define SPRITE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include "BaseCamera.hpp"
#include "BaseShader.hpp"
#include "RenderableObject.hpp"

// Add a different library for image support
#include "opencv2/opencv.hpp"

using namespace glm;
using namespace std;

namespace rendering {

    class Sprite : public RenderableObject {
    public:
        
        Sprite( BaseShader& _shader );
        Sprite(const char* _img_path, BaseShader& _shader);
        ~Sprite();

        virtual void draw(BaseCamera& _camera);
        virtual void drawShadowMap(glm::vec3);

        void updateTextureData(cv::Mat& _tex);
        void assignBufferTexToData( GLuint textureLoc );

    private:

        void initGeometry();
        void initDataFromImagePath( const char* );

        vector<vec2> vertices;
        vector<vec2> uvs;

        void* imageData;
        int imageWidth;
        int imageHeight;

        GLint vertexAttribute;
        GLint uvAttribute;

        mat4 modelMatrix;

        GLuint vertexBuffer;
        GLuint texCoordBuffer;
        GLuint vaoIdx;

        GLuint texId;

        GLuint projectionMatLoc;
        GLuint viewMatLoc;
        GLuint modelMatLoc;
        GLuint texLoc;

        GLuint programId;
    };

}

#endif
