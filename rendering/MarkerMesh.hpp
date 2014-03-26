/* 
 * File:   MarkerMesh.hpp
 * Author: matheus
 *
 * Created on March 11, 2014, 2:52 PM
 */

#ifndef MARKERMESH_HPP
#define	MARKERMESH_HPP

#include "Mesh.hpp"

namespace rendering {

    class MarkerMesh : public Mesh {
    public:
        MarkerMesh();
        MarkerMesh(const MarkerMesh& orig);
        virtual ~MarkerMesh();
        
        void load( glm::vec2 _size, BaseShader& _shader, BaseShader& _shadowMapShader );
        
    private:
        
        void initGeometry();
        
        glm::vec2 size;

    };

}

#endif	/* MARKERMESH_HPP */

