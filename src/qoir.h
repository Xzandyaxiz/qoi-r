#ifndef qoi
#define qoi

typedef struct {
  uint8_t *pixels;
  int pixels_size;
  uint32_t width;
  uint32_t height;
} qoi_info_t;

typedef struct {
  uint8_t r, g, b, a;
} rgb_t;

qoi_info_t *read_qoi(
  FILE *file,
  uint8_t *flag
);

void *fill_qoi(
    qoi_info_t *info,
    uint8_t *pixel_data
);

#endif
