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

}