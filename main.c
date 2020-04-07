#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "shader.h"
#include "objloader.h"


int main(/*int argc, char** argv*/)
{

    GLFWwindow* window;

	printf("Hello Galaxy 3D!\n");
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);


	// Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Galaxy 3D", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    GLuint ProgramID = LoadShaders("shaders/SimpleVertexShader.vertexshader", "shaders/SimpleFragmentShader.fragmentshader");
    if(ProgramID == 0)
    {
       fprintf(stderr, "Failed to load shaders.\n" );
       //getchar();
       glfwTerminate();
       return -1;
    }

    model_data_t model_data = load_obj("fighter.obj", "models");
    if(model_data.vertices == NULL)
    {
        fprintf(stderr, "Failed to load models.\n" );
        glfwTerminate();
        return -1;
    }
/*
    for(int i=0;i<model_data.num_vertices*3;i++)
        printf("%f ", model_data.colors[i]);
*/
    GLuint vertexbuffer;
    GLuint EBO;
    GLuint VertexArrayID;
    GLuint colorbuffer;

    glGenBuffers(1, &EBO);
    glGenBuffers(1, &vertexbuffer);
    glGenVertexArrays(1, &VertexArrayID);
    glGenBuffers(1, &colorbuffer);


    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_data.vertices)*model_data.num_vertices*3, model_data.vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, // The attribute we want to configure
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(model_data.colors)*model_data.num_vertices*3, model_data.colors, GL_STATIC_DRAW);
    glVertexAttribPointer(
        1,                                // Атрибут. Здесь необязательно указывать 1, но главное, чтобы это значение совпадало с layout в шейдере..
        3,                                // Размер
        GL_FLOAT,                         // Тип
        GL_FALSE,                         // Нормализован?
        0,                                // Шаг
        (void*)0                          // Смещение
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(model_data.indices)*model_data.num_faces, model_data.indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    mat4 Projection;
    glm_perspective(45.0f, 4/3, 0.1f, 100.0f, Projection);

    mat4 View;
    //vec3 camera = {0.4,0.3,0.2};
    vec3 camera = {10,10,10};
    vec3 origin = {0,0,0};
    vec3 head = {0,1,0};
    glm_lookat(
        camera, // Camera is at (4,3,3), in World Space
        origin, // and looks at the origin
        head,  // Head is up (set to 0,-1,0 to look upside-down)
        View);

    mat4 Model;
    glm_mat4_identity(Model);

    mat4 mvp;
    glm_mat4_mulN((mat4 *[]){&Projection, &View, &Model}, 3, mvp);

    //glm_mat4_identity(mvp);

    GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	do{
        glEnable(GL_DEPTH_TEST); //| GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        //glCullFace(GL_FRONT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(ProgramID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        glBindVertexArray(VertexArrayID);

        // Draw the triangle !
        glDrawElements(GL_TRIANGLES, model_data.num_faces, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
		
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteProgram(ProgramID);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
