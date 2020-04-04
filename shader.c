#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.h"

char* load_file(const char* filename)
{
    FILE *f = fopen(filename, "rb");
    if (f==NULL)
    {
        printf("Cannot open file.\n");
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    size_t fsize = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

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
    const char* VertexShaderCode = load_file(vertex_file_path);
    if(VertexShaderCode == NULL)
    {
        printf("Impossible to open %s. Are you in the right directory ? \n", vertex_file_path);
        return 0;
	}

	// Read the Fragment Shader code from the file
    const char* FragmentShaderCode = load_file(fragment_file_path);
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

    free((void*)VertexShaderCode);
    free((void*)FragmentShaderCode);

	return ProgramID;
}


