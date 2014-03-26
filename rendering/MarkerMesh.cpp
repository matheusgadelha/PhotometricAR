/* 
 * File:   MarkerMesh.cpp
 * Author: matheus
 * 
 * Created on March 11, 2014, 2:52 PM
 */

#include "MarkerMesh.hpp"

namespace rendering {

    MarkerMesh::MarkerMesh() :
        Mesh(),
        size(0,0)
    {
    }

    MarkerMesh::~MarkerMesh() {
    }
    
    void MarkerMesh::initGeometry() {
        const float w = this->size.x/2.0f;
        const float h = this->size.y/2.0f;

        vertices.push_back(vec3(-w, -h, 0.0f));
        vertices.push_back(vec3(w, -h , 0.0f));
        vertices.push_back(vec3(-w, h , 0.0f));
        vertices.push_back(vec3(w, -h , 0.0f));
        vertices.push_back(vec3( w, h , 0.0f));
        vertices.push_back(vec3(-w, h , 0.0f));
        
        for(unsigned i=0; i < 6; ++i)
            this->normals.push_back(vec3(0,1,0));

        uvs.push_back(vec2(0.0f, 1.0f));
        uvs.push_back(vec2(1.0f, 1.0f));
        uvs.push_back(vec2(0.0f, 0.0f));
        uvs.push_back(vec2(1.0f, 1.0f));
        uvs.push_back(vec2(1.0f, 0.0f));
        uvs.push_back(vec2(0.0f, 0.0f));
        
        visibilities.assign(vertices.size(), 1.0f );

    }
    
    void MarkerMesh::load(glm::vec2 _size, BaseShader& _shader, BaseShader& _shadowMapShader) {
        
        glm::vec2 normalizedSize( _size.x/_size.x, _size.y/_size.x  );
        this->size = normalizedSize;
        
        this->initGeometry();
        this->initBuffers( _shader, _shadowMapShader );
        
        this->modelMatrix = glm::scale(mat4(), vec3(1.3f));
        this->modelMatrix = glm::translate( this->modelMatrix, vec3(0.2f,-0.1f,0.0f) );
        
    }


}