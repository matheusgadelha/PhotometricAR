#include "RenderableObject.hpp"

namespace rendering
{

	RenderableObject::RenderableObject()
	{
		isActive = true;
	}

	RenderableObject::~RenderableObject()
	{
		isActive = false;
	}

	void RenderableObject::draw( BaseCamera& ){}

}