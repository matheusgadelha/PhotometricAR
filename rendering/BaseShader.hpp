#ifndef BASESHADER_H
#define BASESHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <cstdio>

#include "glm.hpp"

namespace rendering {

    class BaseShader {
    public:
        BaseShader();
        BaseShader(const char* _vert_filename, const char* _frag_filename);
        ~BaseShader();

        GLuint vertexShaderId;
        GLuint fragmentShaderId;
        GLuint programId;

        void createCompleteShader(const char* _vert_filename, const char* _frag_filename);

    private:

        char* readFile(const char* _filename);
        void printCompilationError(GLuint _obj);
        GLuint createGeneralShader(const char* _filename, GLenum _type);
    };

}

#endif