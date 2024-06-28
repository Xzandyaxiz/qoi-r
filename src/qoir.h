#ifndef qoi
#define qoi

typedef struct {
  uint8_t *pixels;
  int pixels_size;
  uint32_t width;
  uint32_t height;
} qoi_t;

typedef struct {
  uint8_t r, g, b, a;
} rgb_t;

qoi_t *read_qoi(
  char *path,
  uint8_t *flag
);

#endif
