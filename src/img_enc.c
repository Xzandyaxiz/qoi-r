#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "qoir.h"

#define set_last_pixel()  \
  last_pixel = 0;         \
  last_pixel = (          \
    (pixels[i])         | \
    (pixels[i+1] << 8)  | \
    (pixels[i+2] << 16) | \
    (pixels[i+3] << 24)   \
  );

int8_t is_seen(uint32_t last_pixel, uint32_t *seen_pixels)
{
  for (int8_t i = 0; i < 64; i++)
  {
    if (seen_pixels[i] == last_pixel)
    {
      return i;
    }
  }

  return -1;
}

uint8_t *qoi_to_bytes(uint8_t *pixels, uint32_t width, uint32_t height)
{
  uint32_t last_pixel;
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

  uint32_t seen_pixels[64] = { 0 };

  for (int i = 0; i < ((width * height) * 4); i += 4)
  {
    // QOI_OP_RUN
    // Checking run bytes
    if (pixels[i] == last_pixel)
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
      counter++;
      run_byte++;
    }

    run_loops = 0;

    // QOI_OP_LUMA
    if ((int) abs(last_pixel[0] - pixels[i]) > 3 &&
      (int) abs(last_pixel[1] - pixels[i+1]) > 3 &&
      (int) abs(last_pixel[2] - pixels[i+2]) > 3)
    {
      set_last_pixel();
    }
    // QOI_OP_DIFF
    else
    {
      set_last_pixel();
    }

    uint8_t index_byte = is_seen(last_pixel, &seen_pixels);
    if (index_byte >= 0)
    {
      bytes[counter] = index_byte;
      counter++;
    }
  } 
}
