#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

#include "BaseCamera.hpp"
#include "RenderableObject.hpp"
#include "Mesh.hpp"

namespace rendering {
    
    struct DirectionalLightInfo
    {
        glm::vec3 direction;
        glm::vec3 color;
    };

    class RenderingManager {
    public:

        RenderingManager(BaseCamera&, glm::vec2, glm::vec2);
        ~RenderingManager();

        void render();
        void add( RenderableObject& obj );
        
        void calcAmbientOcclusion();
        void setLightDirection( glm::vec3 _direction );
        void setLightColor( glm::vec3 _color );
        void renderWithShadows( bool _shadows );
        
        GLuint shadowBufferName;
        GLuint depthTexture;

        GLuint SSAOBufferName;
        GLuint SSAOTexture;

        DirectionalLightInfo light;

    private:
        void renderShadowMap();
        void initShadowMapBuffer();
        
        void appendVerticesList( vector<vec3>&, vector<vec3>& );
        
        bool useShadows;
        BaseCamera* camera;
        std::vector<RenderableObject*> objects;
        glm::vec2 shadowMapSize;
        glm::vec2 viewportSize;
    };

}

#endif
