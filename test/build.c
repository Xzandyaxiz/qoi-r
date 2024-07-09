#include <stdint.h>
#include <stdio.h>
#include "src/qoir.h"

#define width 200
#define height 200

int main(int argc, char **argv)
{
  uint8_t flag;
  uint8_t *pixels; 

  qoi_info_t *info = malloc(sizeof(qoi_info_t));
  if (info == NULL) return -1;

  // Write pixels to qoi format and store inside
  // (qoi_info_t *info)
  if ((flag = write_qoi(pixels, width, height)) != 0)
  {
    print_error(flag);
    return 1;
  }

  // Write to file
  FILE *fp = fopen(argv[1], "wb");
  if (fp == NULL)
  {
    printf("Fatal: Bad file path.");
    return -1;
  }

  fwrite()

  free(info->pixels);
  free(info);
}
