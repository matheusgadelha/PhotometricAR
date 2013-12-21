#include "BaseCamera.hpp"

namespace rendering{

	/**
	 * Default constructor initializes projection matrix as perspective
	 */
	BaseCamera::BaseCamera()
	{
		this->setPerspectiveProjection();

		this->up = vec3(0.0f, 1.0f, 0.0f);
		this->lookAtDir = vec3(0.0f, 0.0f, 1.0f);
		this->position = vec3(0.0f, 0.0f, 0.0f);

		this->usingCustomViewMatrix = true;
		this->usingCustomProjectionMatrix = true;
		// this->viewMatrix = mat4();
	}

	/**
	 * Gets the current view matrix based on the camera up vector, positioning
	 * and look at direction.
	 * @return Current view matrix
	 */
	mat4 BaseCamera::getViewMatrix()
	{
		if( usingCustomViewMatrix ) return this->viewMatrix;
		return lookAt(this->position, this->position+this->lookAtDir, this->up);
	}

	/**
	 * Sets custom camera projection matrix
	 * @param _proj New projection matrix
	 */
	void BaseCamera::setProjectionMatrix( mat4 _proj )
	{
		this->projectionMatrix = _proj;
	}

	/**
	 * Translates camera according to vector
	 * @param trans Vector describing translation
	 */
	void BaseCamera::translate( vec3 _trans)
	{
		this->position += _trans;
	}

	/**
	 * Method to translate camera to the front, using the same direction of lookAt
	 * @param step Amount of space translated
	 */
	void BaseCamera::goFront( float _step )
	{
		this->position += _step * this->lookAtDir;
	}

	/**
	 * Method to translate camera to back, using the same direction of lookAt
	 * @param step Amount of space translated
	 */
	void BaseCamera::goBack( float _step )
	{
		this->position += -_step * this->lookAtDir;
	}

	/**
	 * Assigns a new view matrix to the camera
	 * @param _view new view matrix
	 */
	void BaseCamera::setCustomViewMatrix( mat4 _view )
	{
		viewMatrix = _view;
	}

	/**
	 * Method to rotate camera
	 * @param angle rotation angle
	 * @param axis  rotation axis
	 */
	void BaseCamera::rotate( float angle, vec3 axis )
	{
		mat4 rotationMatrix = glm::rotate( mat4(), angle, axis );

		this->lookAtDir = vec3(rotationMatrix * vec4(this->lookAtDir, 0.0f));
		this->lookAtDir = normalize( this->lookAtDir );

		this->up = vec3(inverse(rotationMatrix) * vec4(this->up, 0.0f));
		this->up = normalize( this->up );
	}

	/**
	 * Gets the current projection matrix based on the camera up vector, positioning
	 * and look at direction.
	 * @return Current projection matrix
	 */
	mat4 BaseCamera::getProjectionMatrix()
	{
		return this->projectionMatrix;
		// return mat4();
	}

	/**
	 * Getter for view matrix
	 * @return Current view matrix
	 */
	mat4 BaseCamera::getViewProjectionMatrix()
	{
		return this->getViewMatrix() * this->getProjectionMatrix();
	}

	/**
	 * Method used to define a perspective projetction as the camera projection
	 * matrix
	 * @param _fovy   Field of view in angles
	 * @param _aspect Projection aspect ratio
	 * @param _zNear  Near plane distance
	 * @param _zFar   Far plane distance
	 */
	void BaseCamera::setPerspectiveProjection(
		float _fovy,
		float _aspect,
		float _zNear,
		float _zFar
	)
	{
		this->projectionMatrix = perspective( _fovy, _aspect, _zNear, _zFar);
	}

	/**
	 * Method used to define a orthogonal projetction as the camera projection
	 * matrix
	 * @param _left  Left screen side coordinate
	 * @param _right Right screen side coordinate
	 * @param _up    Up screen side coordinate
	 * @param _down  Down screen side coordinate
	 * @param _zNear Near plane distance
	 * @param _zFar  Far plane distance
	 */
	void BaseCamera::setOrthogonalProjection(
		float _left,
		float _right,
		float _up,
		float _down,
		float _zNear,
		float _zFar
	)
	{
		this->projectionMatrix = ortho( _left, _right, _up, _down, _zNear, _zFar );
	}

}