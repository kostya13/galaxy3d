#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

const char* get_file_data(size_t* len, const char* filename);
int unmap_file(size_t len, const char* mem);

#endif
