#ifndef qoi
#define qoi

typedef struct {
  uint8_t *pixels;
  int pixels_size;
  uint32_t width;
  uint32_t height;
  uint8_t channels;
  uint8_t colorspace;
} qoi_info_t;

typedef struct {
  uint8_t r, g, b, a;
} rgb_t;

uint8_t read_qoi(
  qoi_info_t *info,
  FILE *file
);

uint8_t write_qoi(
    uint8_t pixels
);

void *fill_qoi(
    qoi_info_t *info,
    uint8_t *pixel_data
);

#endif
