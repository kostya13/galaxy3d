#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/cglm.h>

#include "shader.h"
#include "tinyobj_loader_c.h"

static const char* mmap_file(size_t* len, const char* filename) {
  FILE* f;
  long file_size;
  struct stat sb;
  char* p;
  int fd;

  (*len) = 0;

  f = fopen(filename, "r");
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  fseek(f, 0, SEEK_END);
  file_size = ftell(f);
  fclose(f);

  fd = open(filename, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return NULL;
  }

  if (fstat(fd, &sb) == -1) {
    perror("fstat");
    return NULL;
  }

  if (!S_ISREG(sb.st_mode)) {
    fprintf(stderr, "%s is not a file\n", "lineitem.tbl");
    return NULL;
  }
  p = (char*)mmap(0, (size_t)file_size, PROT_READ, MAP_SHARED, fd, 0);

  if (p == MAP_FAILED) {
    perror("mmap");
    return NULL;
  }

  if (close(fd) == -1) {
    perror("close");
    return NULL;
  }

  (*len) = (size_t)file_size;

  return p;
}

static const char* get_file_data(size_t* len, const char* filename) {
  size_t data_len = 0;
  const char* data = NULL;

   data = mmap_file(&data_len, filename);


  (*len) = data_len;
  return data;
}


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

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);


    GLuint ProgramID = LoadShaders("../shaders/SimpleVertexShader.vertexshader", "../shaders/SimpleFragmentShader.fragmentshader");
    if(ProgramID == 0)
    {
       fprintf(stderr, "Failed to load shaders.\n" );
       //getchar();
       glfwTerminate();
       return -1;
    }

    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;

    size_t data_len = 0;
    const char* data = get_file_data(&data_len, "../models/box.obj");
    if (data == NULL) {
      exit(-1);
    }
    printf("filesize: %d\n", (int)data_len);


      unsigned int flags = 0;
      int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                                  &num_materials, data, data_len, flags);
      if (ret != TINYOBJ_SUCCESS) {
        return 0;
      }
/*
      printf("# of shapes    = %d\n", (int)num_shapes);
      printf("# of materials = %d\n", (int)num_materials);
      printf("#num_face_num_verts= %d\n", attrib.num_face_num_verts);
      for (int i = 0; i < attrib.num_face_num_verts; i++) {
      printf("%d ", attrib.material_ids[i]);
      }
*/


    GLfloat vertices1[attrib.num_vertices * 3];
    for(int i=0; i<attrib.num_vertices;i++)
    {
        vertices1[i * 3 + 0]= attrib.vertices[i * 3 + 0];
        vertices1[i * 3 + 1]= attrib.vertices[i * 3 + 1];
        vertices1[i * 3 + 2]= attrib.vertices[i * 3 + 2];

    }

    GLfloat colors1[attrib.num_vertices * 3];
    for(int i=0; i<attrib.num_face_num_verts;i++)
    {
        tinyobj_material_t material;
        int c_id = attrib.material_ids[i];
        material = materials[c_id];
        for(int j=0;j<3;j++)
        {
            int face_id = attrib.faces[i * 3 + j].v_idx;
            colors1[face_id * 3 + 0] =  material.diffuse[0];
            colors1[face_id * 3 + 1] =  material.diffuse[1];
            colors1[face_id * 3 + 2] =  material.diffuse[2];
        }
    }

    GLuint indices1[attrib.num_faces];
    for(int i=0; i<attrib.num_faces;i++)
    {
        indices1[i]= attrib.faces[i].v_idx;
    }


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
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors1), colors1, GL_STATIC_DRAW);
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    glBindVertexArray(0);

    mat4 Projection;
    glm_perspective(45.0f, 4/3, 0.1f, 100.0f, Projection);

    mat4 View;
    vec3 camera = {0.4,0.3,0.2};
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

	do{
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw nothing, see you in tutorial 2 !
        glUseProgram(ProgramID);
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);
        //glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindVertexArray(VertexArrayID);

        // Draw the triangle !
        glDrawElements(GL_TRIANGLES, attrib.num_faces, GL_UNSIGNED_INT, 0);
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
