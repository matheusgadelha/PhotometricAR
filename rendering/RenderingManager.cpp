#include "RenderingManager.hpp"

namespace rendering {

    RenderingManager::RenderingManager(BaseCamera& cam, glm::vec2 _viewportSize, glm::vec2 _shadowMapSize) {
        this->camera = &cam;
        this->initShadowMapBuffer();
        this->viewportSize = _viewportSize;
        this->shadowMapSize = _shadowMapSize;
        
        this->light.direction = vec3(0.3,-0.6,-1.0);
        this->light.direction = glm::normalize(this->light.direction);
        this->light.color = vec3(1.0,1.0,1.0);
    }
    
    RenderingManager::~RenderingManager() {

    }
    
    void RenderingManager::renderWithShadows(bool _shadows) {
        this->useShadows = _shadows;
    }



    /**
     * Renders all RenderableObjects
     */
    void RenderingManager::render() {
        
        if( this->useShadows )
        {
            this->renderShadowMap();
        }
        
        glBindFramebuffer( GL_FRAMEBUFFER, 0 );
        glViewport(0,0,viewportSize.x,viewportSize.y);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        for (unsigned i = 0; i < objects.size(); ++i) {
            if (objects[i]->isActive) {
                objects[i]->draw(*(this->camera));
            }
        }
    }

    void RenderingManager::renderShadowMap() {

        glBindFramebuffer(GL_FRAMEBUFFER, shadowBufferName);
        glViewport(0, 0, shadowMapSize.x, shadowMapSize.y);
//
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (unsigned i = 0; i < objects.size(); ++i) {
            if (objects[i]->castShadows) {
                objects[i]->drawShadowMap( this->light.direction );
            }
        }
    }

    void RenderingManager::setLightColor(glm::vec3 _color) {
        this->light.color = _color;
    }

    void RenderingManager::setLightDirection(glm::vec3 _direction) {
        this->light.direction = _direction;
    }
    void RenderingManager::initShadowMapBuffer() {
        // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
        shadowBufferName = 0;
        glGenFramebuffers(1, &shadowBufferName);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowBufferName);

        // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
        depthTexture = 0;
        glActiveTexture(GL_TEXTURE1);
        glGenTextures(1, &depthTexture);
        glBindTexture(GL_TEXTURE_2D, depthTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

        glDrawBuffer(GL_NONE); // No color buffer is drawn to.

        // Always check that our framebuffer is ok
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Shadow buffer error\n";
    }

    void RenderingManager::appendVerticesList( vector<vec3>& _a, vector<vec3>& _b ){
        _a.reserve( _a.size() + _b.size() );
        _a.insert( _a.end(), _b.begin(), _b.end()  );
    }

    void RenderingManager::calcAmbientOcclusion() {
        
        vector<vec3> allSceneVertices;
        
        for( unsigned i=0; i<objects.size(); ++i )
        {
            if(Mesh* mesh = dynamic_cast<Mesh*>(objects[i])){
                std::cout << "Mesh: " << mesh->vertices.size() << std::endl;
                appendVerticesList( allSceneVertices, mesh->vertices );                
            }
        }
        
        for( unsigned i=0; i<objects.size(); ++i )
        {
            if(Mesh* mesh = dynamic_cast<Mesh*>(objects[i])){
                mesh->calcVertexVisibility( allSceneVertices );
            }
        }
    }

    /**
     * Adds new object to the RenderingManger
     * @param obj New RenderableObject
     */
    void RenderingManager::add(RenderableObject& obj) {
        this->objects.push_back(&obj);
    }
}