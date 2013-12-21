#include "Sprite.hpp"

namespace rendering
{
	/**
	 * Sprite constructor
	 */
	Sprite::Sprite( const char* _img_path, BaseShader& _shader ){

		initGeometry();

		glGenVertexArrays( 1, &this->vaoIdx );
		glBindVertexArray( this->vaoIdx );

		this->programId = _shader.programId;

		glGenBuffers( 1, &this->vertexBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, this->vertexBuffer );
		glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &this->vertices[0], GL_STATIC_DRAW );

		glGenBuffers( 1, &this->texCoordBuffer );
		glBindBuffer( GL_ARRAY_BUFFER, this->texCoordBuffer );
		glBufferData( GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &this->uvs[0], GL_STATIC_DRAW );

		glBindAttribLocation( this->programId, 0, "in_vertex" );
		glBindAttribLocation( this->programId, 1, "in_texcoord" );

		glUseProgram( this->programId );
		
		// projectionMatLoc = glGetUniformLocation( this->programId, "projection" );
		// viewMatLoc = glGetUniformLocation( this->programId, "view" );
		// modelMatLoc = glGetUniformLocation( this->programId, "model" );

/////////////////////////////////////////////////////////////////////////////
//****IMPORTANT****
// To add different libs of image support, add such code here using
// proper library diretives.
/////////////////////////////////////////////////////////////////////////////
// Test if OpenCV is available
#ifdef __OPENCV_ALL_HPP__

		cv::Mat img = cv::imread( _img_path );
		cv::cvtColor(img, img, CV_BGR2RGB);
		// Maybe this will not work... A deep copy is possibly needed...
		this->imageData = img.data;

		this->imageWidth = img.cols;
		this->imageHeight = img.rows;

#endif

		// Texture init
		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &texId );
		glBindTexture( GL_TEXTURE_2D, texId );
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth,
			this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
			this->imageData
		);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		texLoc = glGetUniformLocation( programId, "Tex1" );

		if( texLoc >= 0) glUniform1i( texLoc, 0 );
		else std::cerr << "Uniform variable Tex1 was not found!\n";

	}

	Sprite::~Sprite(){}

	/**
	 * Method to create sprite geometry
	 */
	void Sprite::initGeometry()
	{
		const float s = 1.0;

		vertices.push_back( vec2(-s,-s) );
		vertices.push_back( vec2( s,-s) );
		vertices.push_back( vec2(-s, s) );
		vertices.push_back( vec2( s,-s) );
		vertices.push_back( vec2( s, s) );
		vertices.push_back( vec2(-s, s) );

		uvs.push_back( vec2( 0.0f, 1.0f ));
		uvs.push_back( vec2( 1.0f, 1.0f ));
		uvs.push_back( vec2( 0.0f, 0.0f ));
		uvs.push_back( vec2( 1.0f, 1.0f ));
		uvs.push_back( vec2( 1.0f, 0.0f ));
		uvs.push_back( vec2( 0.0f, 0.0f ));

	}

	/**
	 * Updates texture info according to new image
	 * @param _tex [description]
	 */
	void Sprite::updateTextureData( cv::Mat& _tex )
	{
		glUseProgram( this->programId );

#ifdef __OPENCV_ALL_HPP__

		cv::cvtColor(_tex, _tex, CV_BGR2RGB);
		// Maybe this will not work... A deep copy is possibly needed...
		this->imageData = _tex.data;

		this->imageWidth = _tex.cols;
		this->imageHeight = _tex.rows;

#endif

		// Texture init
		glActiveTexture( GL_TEXTURE0 );
		glGenTextures( 1, &texId );
		glBindTexture( GL_TEXTURE_2D, texId );
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, this->imageWidth,
			this->imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,
			this->imageData
		);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

		texLoc = glGetUniformLocation( programId, "Tex1" );

		if( texLoc >= 0) glUniform1i( texLoc, 0 );
		else std::cerr << "Uniform variable Tex1 was not found!\n";

	}

	// void Sprite::initShader()gl
	// {

	// }

	void Sprite::draw( BaseCamera& _camera ){

		glUseProgram( this->programId );

		this->vertexAttribute = 0;
		this->uvAttribute = 1;

		glUniformMatrix4fv(
			this->modelMatLoc,
			1, GL_FALSE,
			value_ptr( this->modelMatrix )
		);

		glEnableVertexAttribArray( this->vertexAttribute );
		glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer);
		glVertexAttribPointer(
			this->vertexAttribute,
			2,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray( this->uvAttribute );
		glBindBuffer(GL_ARRAY_BUFFER, this->texCoordBuffer);
		glVertexAttribPointer(
			this->uvAttribute,            // attribute
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture(GL_TEXTURE_2D, texId);
		glUniform1i( texLoc, 0 );

		glDrawArrays(GL_TRIANGLES, 0, this->vertices.size() );

		glDisableVertexAttribArray( this->vertexAttribute );
		glDisableVertexAttribArray( this->uvAttribute );

		glClear( GL_DEPTH_BUFFER_BIT );
	}
}