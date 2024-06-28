#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "qoir.h"

#define to_uint32(buf) \
((buf[0] << 24) | \
(buf[1] << 16) | \
(buf[2] << 8)  | \
(buf[3]))

#define set_seen_pixel() seen_pixels[( \
         (cur_col.r * 3) + (cur_col.g * 5) + \
         (cur_col.b * 7) + (cur_col.a * 11) \
      ) % 64 ] = cur_col;

#define add_pixel() \
  pixels[pixels_count] = cur_col.r; \
  pixels[pixels_count+1] = cur_col.g; \
  pixels[pixels_count+2] = cur_col.b; \
  pixels[pixels_count+3] = cur_col.a; \
  pixels_count += 4;

qoi_t *read_qoi(char *path, uint8_t *flag)
{
  qoi_t *info = malloc(sizeof(qoi_t));

  rgb_t cur_col = (rgb_t) { 0, 0, 0, 255 };
  rgb_t seen_pixels[64]; 
  for (int i = 0; i < 64; i ++)
  {
    seen_pixels[i] = (rgb_t) { 0, 0, 0, 255 };
  }

  FILE *file = fopen(path, "rb");

  if (file == NULL)
  {
    *flag = 1;
    return NULL;
  }
  
  fseek(file, 0, SEEK_END);
  int f_len = ftell(file);
  fseek(file, 0, SEEK_SET);

  uint8_t buf[4];
  fread(buf, 1, 4, file);

  if (strcmp(buf, "qoif") != 0)
  {
    *flag = 2;
    return NULL;
  }

  fread(buf, 4, 1, file);
  info->height = to_uint32(buf);
  fread(buf, 4, 1, file);
  info->width = to_uint32(buf);

  fread(buf, 1, 2, file);
  uint8_t ch = buf[0];

  uint8_t *pixels = malloc((info->height * info->width) * 4);
  int pixels_count = 0;
 
  uint32_t i = 21;

  uint8_t last_index = 0;

  while(1)
  { 
    if (i >= f_len) break;

    fread(buf, 1, 1, file);

    if (buf[0] >> 6 == 0)
    {
      if (last_index == 2)
        break;

      if (buf[0] == 0)
        last_index++;
      else
        last_index = 0;

      cur_col = seen_pixels[buf[0]];

      add_pixel();
      i++;
      continue;
    }

    last_index = 0;

    if (buf[0] == 254)
    {
      fread(buf, 1, 3, file);
      
      cur_col.r = buf[0];
      cur_col.g = buf[1];
      cur_col.b = buf[2];

      set_seen_pixel();
      add_pixel();

      i += 4;
      continue;
    }
    
    if (buf[0] == 255)
    {
      fread(buf, 1, 4, file);
      
      cur_col.r = buf[0];
      cur_col.g = buf[1];
      cur_col.b = buf[2];
      cur_col.a = buf[3];

      set_seen_pixel();
      add_pixel();

      i += 5;
      continue;
    }

    if (buf[0] >> 6 == 1)
    {
      int8_t dr = ( (buf[0] & ~64) >> 4) - 2;
      cur_col.r = (uint8_t) ((cur_col.r + dr) & 0xFF);

      int8_t dg = (( (buf[0] & ~64) & 12) >> 2) - 2;
      cur_col.g = (uint8_t) ((cur_col.g + dg) & 0xFF);
      
      int8_t db = ( (buf[0] & ~64) & 3) - 2;
      cur_col.b = (uint8_t) ((cur_col.b + db) & 0xFF);

      set_seen_pixel();
      add_pixel();

      i++;
      continue;
    }

    if (buf[0] >> 6 == 3)
    {
      uint8_t run_len = (buf[0] & ~192) + 1;
      if(run_len == 0) break;

      for (uint8_t i = 0; i < run_len; i ++)
      {
        add_pixel();
      }
      i++;
      continue;
    }

    if (buf[0] >> 6 == 2)
    {
      int8_t dg = (buf[0] & ~0b11000000) - 32;

      fread(buf, 1, 1, file);
      int8_t dr = ((buf[0] >> 4) - 8) + dg;
      int8_t db = ((buf[0] & 0b1111) - 8) + dg;

      cur_col.r = (uint8_t) ((cur_col.r + dr) & 0xFF);
      cur_col.g = (uint8_t) ((cur_col.g + dg) & 0xFF);
      cur_col.b = (uint8_t) ((cur_col.b + db) & 0xFF);

      set_seen_pixel();
      add_pixel();

      i+=2;
      continue;
    }

  }
  info->pixels = pixels;
  info->pixels_size = pixels_count;

  fclose(file);
  return info;
}
