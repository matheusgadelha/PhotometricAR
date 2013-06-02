#include "BaseShader.hpp"

namespace rendering
{
	/**
	 * Default constructor
	 */
	BaseShader::BaseShader()
	{ /* EMPTY */ }

	/**
	 * Alternative constructor that creates a complete shader program on instantiation.
	 */
	BaseShader::BaseShader( const char* _vert_filename, const char* _frag_filename )
	{
		this->createCompleteShader( _vert_filename, _frag_filename );
	}

	BaseShader::~BaseShader()
	{
		glDeleteProgram( this->programId );
	}
	
	/**
	 * Creates a complete shader program, with vertex and fragment shader
	 * @param _vert_filename Vertex shader path
	 * @param _frag_filename Fragment shader path
	 */
	void BaseShader::createCompleteShader( const char* _vert_filename, const char* _frag_filename)
	{
		this->vertexShaderId = createGeneralShader( _vert_filename, GL_VERTEX_SHADER );
		this->fragmentShaderId = createGeneralShader( _frag_filename, GL_FRAGMENT_SHADER );

		this->programId = glCreateProgram();
		glAttachShader( programId, this->vertexShaderId );
  		glAttachShader( programId, this->fragmentShaderId );

  		GLint link_ok = GL_FALSE;

		glLinkProgram( programId );
  		glGetProgramiv( programId, GL_LINK_STATUS, &link_ok );

  		if (!link_ok) {
    		fprintf(stderr, "glLinkProgram Error:");
    		return;
  		}
	}

	/**
	 * Mehthod to read shader file and return it
	 * @param  _filename Path to Shader file
	 * @return Shader file content on a char* type
	 */
	char* BaseShader::readFile( const char* _filename )
	{
		FILE* input = fopen( _filename, "rb" );
		if( input == NULL ) return NULL;
	
		if( fseek( input, 0, SEEK_END ) == -1 ) return NULL;
		long size = ftell( input );
		if( size == -1 ) return NULL;
		if( fseek(input, 0, SEEK_SET) == -1 ) return NULL;
	
		/*if using c-compiler: dont cast malloc's return value*/
		char *content = (char*) malloc( (size_t) size +1  ); 
		if(content == NULL) return NULL;
	
		fread( content, 1, (size_t)size, input );
		if( ferror(input) ) {
		  free(content);
		  return NULL;
		}
	
		fclose(input);
		content[size] = '\0';
		return content;
	}

	/**
	 * Method to show shader compilation errors
	 * @param _obj int identifying shader
	 */
	void BaseShader::printCompilationError( GLuint _obj )
	{
		GLint log_length = 0;
  		if ( glIsShader(_obj) )
  			glGetShaderiv(_obj, GL_INFO_LOG_LENGTH, &log_length);
  		else if ( glIsProgram(_obj) )
  			glGetProgramiv( _obj, GL_INFO_LOG_LENGTH, &log_length );
  		else {
  			fprintf( stderr, "printlog: Not a shader or a program\n" );
  			return;
  		}
 		
  		char* log = (char*)malloc(log_length);
 		
  		if ( glIsShader(_obj) )
  			glGetShaderInfoLog( _obj, log_length, NULL, log );
  		else if ( glIsProgram(_obj) )
  			glGetProgramInfoLog( _obj, log_length, NULL, log );
 		
  		fprintf( stderr, "%s", log );
  		free( log );
	}

	/**
	 * Creates a shader of the _type type
	 * @param  _filename Path to shader file
	 * @param  _type     Shader type
	 * @return           Shader program id
	 */
	GLuint BaseShader::createGeneralShader( const char* _filename, GLenum _type)
	{
		const GLchar* source = this->readFile(_filename);

  		if (source == NULL) {
	    	fprintf(stderr, "Error opening %s: ", _filename); perror("");
    		return 0;
  		}

  		GLuint res = glCreateShader( _type );
  		const GLchar* sources[2] = {"", source };

  		glShaderSource(res, 2, sources, NULL);
  		free((void*)source);
 
  		glCompileShader(res);
  		GLint compile_ok = GL_FALSE;
  		glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);

  		if (compile_ok == GL_FALSE) {
	    	fprintf(stderr, "%s:", _filename);
    		this->printCompilationError( res );
    		glDeleteShader( res );
    		return 0;
  		}
 
  		return res;	
	}

}