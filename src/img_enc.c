#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "qoir.h"

#define set_last_pixel()      \
  pixels[i] = last_pixel[0];  \
  pixels[i+1] = last_pixel[1];\
  pixels[i+2] = last_pixel[2];\
  pixels[i+3] = last_pixel[3];

int main()
{
  return 0;
}

uint8_t *qoi_to_bytes(uint8_t *pixels, uint32_t width, uint32_t height)
{
  uint8_t last_pixel[4] = { 0, 0, 0, 0 };
  uint8_t bytes[(width * height)*4];
  uint8_t counter = 0;
  
  uint8_t channels = 3;
  uint8_t colorspace = 0;

  uint8_t header[14] = { 'q', 'o', 'i', 'f' };
  memcpy(header+4, &width, 4);
  memcpy(header+8, &height, 4);
  memcpy(header+12, &channels, 1);
  memcpy(header+13, &colorspace, 1);
  memcpy(bytes, header, 14);
  
  uint8_t run_loops = 0;

  for (int i = 0; i < ((width * height) * 4); i += 4)
  {
    // Checking run bytes
    if (pixels[i] == last_pixel[0]   &&
        pixels[i+1] == last_pixel[1] &&
        pixels[i+2] == last_pixel[2] &&
        pixels[i+3] == last_pixel[3])
    {
      run_loops ++;
      if (run_loops >= 62)
        continue;
    }

    // Writing run bytes
    if (run_loops > 0)
    {
      uint8_t run_byte = 0xc0;
      run_byte |= run_loops;
      bytes[counter] = run_byte;
      run_byte++;
    }

    run_loops = 0;
  } 
}
