#include <sdl-file.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int64_t file_size(struct SDL_RWops *f) {
  return fseek(f->fp, 0, SEEK_END);
}

int64_t file_seek(struct SDL_RWops *f, int64_t offset, int whence) {
  return fseek(f->fp, offset, whence);
}

size_t  file_read(struct SDL_RWops *f, void *buf, size_t size, size_t nmemb) {
  return fread(buf, size, nmemb, f->fp);
}

size_t  file_write(struct SDL_RWops *f, const void *buf, size_t size, size_t nmemb) {
  return fwrite(buf, size, nmemb, f->fp);
}

int     file_close(struct SDL_RWops *f) {
  fclose(f->fp);
  free(f);
  return 0;
}

SDL_RWops* SDL_RWFromFile(const char *filename, const char *mode) {
  SDL_RWops *ret = (SDL_RWops *)malloc(sizeof(SDL_RWops));
  ret->size = file_size;
  ret->seek = file_seek;
  ret->read = file_read;
  ret->write = file_write;
  ret->close = file_close;
  ret->type = RW_TYPE_FILE;
  ret->fp = fopen(filename, mode);
  return ret;
}

int64_t mem_size(struct SDL_RWops *f) {
  return f->mem.size;
}

int64_t mem_seek(struct SDL_RWops *f, int64_t offset, int whence) {
  int64_t ret = 0;
  switch (whence)
  {
  case SEEK_CUR:
    f->mem.cur_pos += offset;
    ret = f->mem.cur_pos + offset;
    break;
  case SEEK_SET:
    f->mem.cur_pos = offset;
    ret = offset;
  case SEEK_END:
    printf("Warning: try to extend mem");
    f->mem.cur_pos = f->mem.size + offset;
    ret = f->mem.size + offset;
  
  default:
    break;
  }
  return ret;
}

size_t  mem_read(struct SDL_RWops *f, void *buf, size_t size, size_t nmemb) {
  int len = size / nmemb;
  memcpy(buf, f->mem.base + f->mem.cur_pos, len);
  return len;
}

size_t  mem_write(struct SDL_RWops *f, const void *buf, size_t size, size_t nmemb) {
  int len = size / nmemb;
  memcpy(f->mem.base + f->mem.cur_pos, buf, len);
  return len;
}

int  mem_close(struct SDL_RWops *f) {
  free(f);
  return 0;
}

SDL_RWops* SDL_RWFromMem(void *mem, int size) {
  SDL_RWops *ret = (SDL_RWops *)malloc(sizeof(SDL_RWops));
  ret->size = mem_size;
  ret->seek = mem_seek;
  ret->read = mem_read;
  ret->write = mem_write;
  ret->close = mem_close;
  ret->type = RW_TYPE_MEM;
  ret->mem.base = mem;
  ret->mem.size = size;
  ret->mem.cur_pos = 0;
  return ret;
}
