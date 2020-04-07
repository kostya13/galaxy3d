#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <GL/glew.h>

typedef struct {
    GLfloat* vertices;
    GLfloat* colors;
    GLuint* indices;
    unsigned int num_faces;
    unsigned int num_vertices;
} model_data_t;

model_data_t load_obj(const char* filename, const char* model_dir);

#endif
