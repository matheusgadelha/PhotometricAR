#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <cstring>

#include "BaseShader.hpp"
#include "RenderableObject.hpp"

#include "glm.hpp"
#include "gtc/type_ptr.hpp"
#include "gtc/matrix_transform.hpp"

using namespace std;
using namespace glm;

namespace rendering
{
	class Mesh : public RenderableObject
	{
		public:

			Mesh();
			~Mesh();

			void load( const char* _path, BaseShader& _shader);

			virtual void draw( BaseCamera& _camera );

			mat4 modelMatrix;

		private:

			bool loadOBJ(
				const char * path,
				vector<vec3> & out_vertices, 
				vector<vec2> & out_uvs, 
				vector<vec3> & out_normals
			);

			vector<vec3> vertices;
			vector<vec3> normals;
			vector<vec2> uvs;

			GLint vertexAttribute;
			GLint normalAttribute;

			GLuint vertexBuffer;
			GLuint normalBuffer;
			GLuint vaoIdx;
			GLuint projectionMatLoc;
			GLuint viewMatLoc;
			GLuint modelMatLoc;
			GLuint normalMatLoc;
			GLuint programId;


	};
}

#endif