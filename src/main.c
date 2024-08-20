#include "common.h"

#include "stb_image.h"
#include "stb_image_write.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <complex.h>

///////////////////////////////////////////////////////////////
///                        I M A G E                        ///
///////////////////////////////////////////////////////////////

#define RGBA 4

typedef struct {
  stbi_uc r, g, b, a;
} Pixel;

_Static_assert(
  sizeof(Pixel[16]) == sizeof(stbi_uc[RGBA * 16]), 
  "Should be able to cast stbi_uc[4 * N] into Pixel[N]"
);

typedef struct {
  int width, height;
  Pixel *data;
} Image;

Image img_malloc(int width, int height) {
  Image img = { width, height, malloc(width * height * sizeof(Pixel)) };
  if (!img.data) PANIC("malloc failed\n");
  return img;
}

Pixel *img_at(Image img, int y, int x) {
  return &img.data[y * img.width + x];
}

void img_save_png(Image img, const char *filename) {
  if (stbi_write_png(filename, img.width, img.height, RGBA, img.data, 0)) {
    printf("Image saved at: %s\n", filename);
  } else {
    printf("Failed to save image: %s\n", filename);
  }
}

///////////////////////////////////////////////////////////////
///                         M A T H                         ///
///////////////////////////////////////////////////////////////

double scale_to_range(int value, int N, double min, double max) {
  return value * 1.0 / N * (max - min) + min;
}

int divergent_factor(double complex c) {
  const int N = 1 << 14;
  double complex z = 0;

  for (int i = 1; i <= N; ++i) {
    z = z * z + c;

    if (cabs(z) > 2) {
      return i;
    }
  }

  return 0;
}

///////////////////////////////////////////////////////////////
///                         M A I N                         ///
///////////////////////////////////////////////////////////////

// mandelbrot image settings

#define MIN_X -2.0
#define MAX_X +0.7

#define MIN_Y -1.3
#define MAX_Y +1.3

#define RESOLUTION 1000


Image calc_mandelbrot_img() {
  const int WIDTH = (MAX_X - MIN_X) * RESOLUTION, HEIGHT = (MAX_Y - MIN_Y) * RESOLUTION;
  printf("Creating mandelbrot image of size %dx%d pixels...\n", WIDTH, HEIGHT);

  Image img = img_malloc(WIDTH, HEIGHT);
  
  #pragma omp parallel for collapse(2) schedule(static, 10)
  for (int y = 0; y < HEIGHT / 2; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      double re = scale_to_range(x, WIDTH, MIN_X, MAX_X);
      double im = scale_to_range(y, HEIGHT, MIN_Y, MAX_Y);
      int factor = divergent_factor(re + im * I);
      stbi_uc value = log(factor + 1) * 10;
      Pixel pixel = { .r = value, .g = value, .b = value, .a = 0xff };
      *img_at(img, y, x) = *img_at(img, HEIGHT - 1 - y, x) = pixel;
    }
  }

  return img;
}

int main() {
  double start = omp_get_wtime();
  Image img = calc_mandelbrot_img();
  printf("Competed in %f seconds\n", omp_get_wtime() - start);
  
  char path[256];
  snprintf(path, sizeof(path), "mandelbrot_%dx%d.png", img.width, img.height);
  img_save_png(img, path);

  free(img.data);
}