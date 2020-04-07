#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.h"
#include "memory.h"


void error_message(GLuint ShaderID, GLsizei InfoLogLength)
{
    char* error = malloc((size_t)InfoLogLength + 1);
    glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, error);
    error[InfoLogLength] = 0;
    printf("%s\n", error);
    free(error);
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
    size_t len_vertex_code;
    const char* VertexShaderCode = get_file_data(&len_vertex_code, vertex_file_path);
    if(VertexShaderCode == NULL)
    {
        printf("Impossible to open %s. Are you in the right directory ? \n", vertex_file_path);
        return 0;
	}

    // Read the Fragment Shader code from the
    size_t len_fragment_code;
    const char* FragmentShaderCode = get_file_data(&len_fragment_code, fragment_file_path);
    if(FragmentShaderCode == NULL)
    {
        printf("Impossible to open %s. Are you in the right directory ? \n", fragment_file_path);
        return 0;
    }

	GLint Result = GL_FALSE;
    GLsizei InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
    glShaderSource(VertexShaderID, 1, &VertexShaderCode , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
        error_message(VertexShaderID, InfoLogLength);
        return 0;
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
    glShaderSource(FragmentShaderID, 1, &FragmentShaderCode , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
        error_message(FragmentShaderID, InfoLogLength);
        return 0;
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
        error_message(ProgramID, InfoLogLength);
        return 0;
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

    unmap_file(len_vertex_code, FragmentShaderCode);
    unmap_file(len_fragment_code, FragmentShaderCode);

	return ProgramID;
}


