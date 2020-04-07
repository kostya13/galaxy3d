#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>

static const char* mmap_file(size_t* len, const char* filename) {
  FILE* f;
  long file_size;
  struct stat sb;
  char* p;
  int fd;

  (*len) = 0;

  f = fopen(filename, "r");
  if (f == NULL) {
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

const char* get_file_data(size_t* len, const char* filename) {
  size_t data_len = 0;
  const char* data = NULL;

   data = mmap_file(&data_len, filename);


  (*len) = data_len;
  return data;
}

int unmap_file(size_t len, const char* mem)
{
  if (munmap((void*) mem, len) == -1)
  {
      perror("munmap");
      return -1;
  }
  return 0;
}
