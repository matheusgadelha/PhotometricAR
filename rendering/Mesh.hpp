#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cstring>

#include "BaseShader.hpp"
#include "RenderableObject.hpp"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/intersect.hpp"

using namespace std;
using namespace glm;

namespace rendering {

    class Mesh : public RenderableObject {
        
    public:

        Mesh();
        ~Mesh();

        void load(const char* _path, BaseShader& _shader, BaseShader& _shadowMapShader);
        virtual void draw(BaseCamera& _camera);
        virtual void drawShadowMap( glm::vec3 _lightDirection );
        void calcVertexVisibility( vector<vec3>& _sceneVertices );

        mat4 modelMatrix;
        mat4 augmentedViewMatrix;
        GLuint shadowMapTexId;
        
        vector<vec3> vertices;
        vector<float> visibilities;
        vector<vec3> normals;
        
        vec3 lightColor;
        vec3 ambientLight;

    protected:

        bool loadOBJ(
                const char * path,
                vector<vec3> & out_vertices,
                vector<vec2> & out_uvs,
                vector<vec3> & out_normals
                );
        
        void initBuffers( BaseShader& _shader, BaseShader& _shadowMapShader );

        vector<vec2> uvs;
        
        vec3 lightDirection;
        
        mat4 depthMVP;

        GLint vertexAttribute;
        GLint normalAttribute;
        GLint visibilityAttribute;

        GLuint vertexBuffer;
        GLuint normalBuffer;
        GLuint visibilityBuffer;
        GLuint vaoIdx;
        GLuint projectionMatLoc;
        GLuint viewMatLoc;
        GLuint modelMatLoc;
        GLuint normalMatLoc;
        GLuint depthMatLoc;
        GLuint depthBiasMatLoc;
        GLuint shadowMapLoc;
        GLuint programId;
        GLuint shadowMapProgram;
        GLuint lightDirectionLoc;
        GLuint lightColorLoc;
        GLuint ambientLightLoc;
    };
}

#endif