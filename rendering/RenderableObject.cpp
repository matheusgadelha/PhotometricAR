#include "RenderableObject.hpp"

namespace rendering
{
	/**
	 * Creates object and sets its active state.
	 */
	RenderableObject::RenderableObject()
	{
		isActive = true;
	}

	RenderableObject::~RenderableObject()
	{
		isActive = false;
	}

	/**
	 * Virtual method to draw objects
	 * @param cam Camera containing object view config.
	 */
	void RenderableObject::draw( BaseCamera& cam )
	{}
        
        /**
	 * Virtual method to draw shadow maps for the given light
	 * @param lightDirection Direction of the light with respect to the 
         * shadow map will be drawn.
	 */
	void RenderableObject::drawShadowMap(glm::vec3 _lightDirection)
	{}

}