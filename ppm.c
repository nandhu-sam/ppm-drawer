

#include <stddef.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include "ppm.h"

static void sizetToStr(char* buf, size_t n);
static size_t stringlen(const char* s);

struct PPM PPM_init(size_t width,
                    size_t height,
                    uint16_t maxval,
                    const char* filename)
{
  struct PPM img;
  img.width  = width;
  img.height = height;
  img.maxval = maxval;

  img.fd = open(filename,
                O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR
                );
  
  const char* magic = "P6";
  char wdbuf[25];
  char htbuf[25];  
  char mxbuf[25];

  sizetToStr(wdbuf, width);
  sizetToStr(htbuf, height);
  sizetToStr(mxbuf, maxval);
  
  write(img.fd, magic, stringlen(magic));
  write(img.fd, " ", 1);
  write(img.fd, wdbuf, stringlen(wdbuf));
  write(img.fd, " ", 1);
  write(img.fd, htbuf, stringlen(htbuf));
  write(img.fd, " ", 1);
  write(img.fd, mxbuf, stringlen(mxbuf));
  write(img.fd, " ", 1);

  return img;  
}


void PPM_setPixel(struct PPM* img,
                  size_t xcor,
                  size_t ycor,
                  struct Pixel p)
{
  if(xcor >= img->width || ycor >= img->height) {
    return;
  }
  
  size_t header_len = 2+4;
  char wdbuf[25];
  char htbuf[25];  
  char mxbuf[25];
  sizetToStr(wdbuf, img->width);
  sizetToStr(htbuf, img->height);
  sizetToStr(mxbuf, img->maxval);
  header_len += stringlen(wdbuf)
              + stringlen(htbuf)
              + stringlen(mxbuf);
  
  size_t pixel_offset = xcor + (ycor * img->width);
  if(img->maxval < 256) {
    pixel_offset *= sizeof(uint8_t);
  }
  else {
    pixel_offset *= sizeof(uint16_t);
  }
  pixel_offset *= 3;

  if(img->maxval < 256) {
    uint8_t pix[3];
    pix[0] = (uint8_t)(p.r * img->maxval);
    pix[1] = (uint8_t)(p.g * img->maxval);
    pix[2] = (uint8_t)(p.b * img->maxval);
    lseek(img->fd, header_len + pixel_offset, SEEK_SET);
    write(img->fd, (void*)pix, 3*sizeof(uint8_t));
  }
  else {
    // Has trouble with endianness. This part of code
    // should be fixed.
    uint16_t pix[3];
    pix[0] = (uint16_t)(p.r * img->maxval);
    pix[1] = (uint16_t)(p.g * img->maxval);
    pix[2] = (uint16_t)(p.b * img->maxval);
    lseek(img->fd, header_len + pixel_offset, SEEK_SET);
    write(img->fd, (void*)pix, 3*sizeof(uint16_t));
  }
}


void PPM_destroy(struct PPM* img) {
  fsync(img->fd);
  close(img->fd);
}

static void sizetToStr(char* buf, size_t n) {
  size_t t = n;
  size_t c = 0;

  do {
    c++;
    t/=10;
  } while(t > 0);

  buf[c] = '\0';

  do {
    c--;
    buf[c] = '0' + (n%10);
    n/=10;
  } while(n);  
}


static size_t stringlen(const char* s) {
  char* p = (char*)s;
  size_t c = 0;
  while(*p != '\0') {
    c++;
    p++;
  }
  return c;
}
