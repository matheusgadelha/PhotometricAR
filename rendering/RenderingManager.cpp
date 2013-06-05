#include "RenderingManager.hpp"

namespace rendering
{
	RenderingManager::RenderingManager( BaseCamera& cam )
	{
		this->camera = &cam;
	}

	/**
	 * Renders all RenderableObjects
	 */
	void RenderingManager::render()
	{
		for( unsigned i=0; i < objects.size(); ++i)
		{
			if( objects[i]->isActive )
			{
				objects[i]->draw( *(this->camera) );
			}
		}
	}

	/**
	 * Adds new object to the RenderingManger
	 * @param obj New RenderableObject
	 */
	void RenderingManager::add( RenderableObject& obj )
	{
		this->objects.push_back( &obj );
	}
}