
#include <stddef.h>
#include <stdint.h>

struct PPM {
  size_t width, height;
  uint16_t maxval;
  int fd;  
};

struct Pixel { float r, g, b; };

struct PPM PPM_init(size_t width,
                    size_t height,
                    uint16_t maxval,
                    const char* filename);

void PPM_destroy(struct PPM* img);

void PPM_setPixel(struct PPM* img,
                  size_t xcor,
                  size_t ycor,
                  struct Pixel p);

