#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include "objloader.h"
#include "memory.h"

#define TINYOBJ_LOADER_C_IMPLEMENTATION
#include "tinyobj_loader_c.h"


model_data_t load_obj(const char* filename, const char* model_dir)
{
    size_t data_len = 0;
    char fullpath[strlen(filename) + strlen(model_dir) + 2]; // 2 - слеш и финальный 0
    strcpy(fullpath, model_dir);
    strcat(fullpath, "/");
    strcat(fullpath, filename);
    const char* data = get_file_data(&data_len, fullpath);
    if (data == NULL) {
      exit(-1);
    }
    printf("filesize: %d\n", (int)data_len);


    model_data_t model_data;
    model_data.vertices = NULL;

    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes = NULL;
    size_t num_shapes;
    tinyobj_material_t* materials = NULL;
    size_t num_materials;
    int ret = tinyobj_parse_obj(&attrib, &shapes, &num_shapes, &materials,
                              &num_materials, data, data_len, 0, model_dir);
    if (ret != TINYOBJ_SUCCESS) {
      return model_data;
    }

    int vertex_counter[num_materials][attrib.num_vertices];
    for(size_t i=0; i<num_materials; i++)
        for(size_t j=0; j<attrib.num_vertices; j++)
            vertex_counter[i][j] = -1;

    for(size_t i=0; i<attrib.num_faces; i++)
    {
        int face_num = (int)i/3;
        int material = attrib.material_ids[face_num];
        int idx = attrib.faces[i].v_idx;
        vertex_counter[material][idx] = 1;
    }
    GLuint vertex_for_material[num_materials];
    GLuint offsets[num_materials];
    GLuint total_vertex = 0;
    for(size_t i=0; i<num_materials; i++)
    {
        GLuint c = 0;
        offsets[i] = total_vertex;
        for(size_t j=0; j<attrib.num_vertices;j++)
        {
            if (vertex_counter[i][j] == 1)
                c++;
        }
        vertex_for_material[i] = c;
        total_vertex += c;
    }

    GLfloat vertices[total_vertex * 3];
    GLuint indices[attrib.num_faces];
    GLint idx_mapper[attrib.num_vertices];

    GLuint pos = 0;
    int prev_material = -1;
    for(size_t i=0; i<attrib.num_faces;i++)
    {
        int face_num = (int)i/3;
        int material = attrib.material_ids[face_num];
        if (prev_material != material)
        {
            prev_material = material;
            pos = 0;
            for(size_t j=0; j<attrib.num_vertices; j++)
                idx_mapper[j] = -1;
        }
        GLuint offset = offsets[material];
        int idx = attrib.faces[i].v_idx;
        if (idx_mapper[idx] < 0)
        {
            idx_mapper[idx] = (GLint)(offset + pos);
            indices[i] = offset + pos;
            GLuint base = (offset + pos) * 3;
            vertices[base + 0] = attrib.vertices[idx * 3 + 0];
            vertices[base + 1] = attrib.vertices[idx * 3 + 1];
            vertices[base + 2] = attrib.vertices[idx * 3 + 2];
            pos ++;
        }
        else
        {
            indices[i] = (GLuint)idx_mapper[idx];
        }
    }

    GLfloat colors[total_vertex * 3];
    int mpos =0;
    for(size_t i=0; i<num_materials;i++)
    {
        tinyobj_material_t material = materials[i];
        for(size_t j=0; j<vertex_for_material[i];j++)
        {
            colors[mpos * 3 + 0] =  material.diffuse[0];
            colors[mpos * 3 + 1] =  material.diffuse[1];
            colors[mpos * 3 + 2] =  material.diffuse[2];
            mpos++;
        }
    }



    model_data.vertices = (GLfloat *)malloc(sizeof(vertices));
    memcpy(model_data.vertices, vertices, sizeof(vertices));
    model_data.colors = (GLfloat *)malloc(sizeof(colors));
    memcpy(model_data.colors, colors, sizeof(colors));
    model_data.indices = (GLuint *)malloc(sizeof(indices));
    memcpy(model_data.indices, indices, sizeof(indices));
    model_data.num_faces = attrib.num_faces;
    model_data.num_vertices = total_vertex;

    return model_data;
}
