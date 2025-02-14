#include "tgaimage.hh"

#include <immintrin.h>
#include <cstdint>

static TGAColor const green = TGAColor (0, 255, 0, 255);
static TGAColor const white = TGAColor (255, 255, 255, 255);
static TGAColor const red   = TGAColor (255, 0,   0,   255);
static TGAImage image (1920, 1080, TGAImage::RGB);

static int constexpr avx2_chunk_size {8};

namespace hyper
{
  // Lesson 1: Bresenham's Line Drawing Algorithm
  static void
  draw_horizontal_line_bresenham (int x0, int y0, int x1, int y1, TGAColor color)
  {
    if (x1 < x0)
      {
        std::swap (x0, x1);
        std::swap (y0, y1);
      }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int dy_abs = std::abs (y1 - y0);
    int D = 2 * dy_abs - dx;
    int y = y0;
    int y_step = (dy < 0) ? -1 : 1;

    for (int x = x0; x <= x1; ++x)
      {
        image.set (x, y, color);

        if (D > 0)
          {
            y += y_step;
            D += 2 * (dy_abs - dx);
          }
        else
          D += 2 * dy_abs;
      }
  }

  static void
  draw_vertical_line_bresenham (int x0, int y0, int x1, int y1, TGAColor color)
  {
    if (y1 < y0)
      {
        std::swap (x0, x1);
        std::swap (y0, y1);
      }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int dx_abs = std::abs (dx);
    int dy_abs = std::abs (dy);
    int D = 2 * dx_abs - dy_abs;

    int x = x0;
    int x_step = (dx < 0) ? -1 : 1;

    for (int y = y0; y <= y1; ++y)
      {
        image.set(x, y, color);

        if (D > 0)
          {
            x += x_step;
            D += 2 * (dx_abs - dy_abs);
          }
        else
          D += 2 * dx_abs;
      }
  }

  static void
  draw_line_bresenham (int x0, int y0, int x1, int y1, TGAColor color)
  {
    int const dx = x1 - x0;
    int const dy = y1 - y0;
    int const dy_abs = std::abs (dy);
    int const dx_abs = std::abs (dx);
    bool const steep = dy_abs > dx_abs;

    if (steep)
      draw_vertical_line_bresenham (x0, y0, x1, y1, color);
    else
      draw_horizontal_line_bresenham (x0, y0, x1, y1, color);
  }

  void
  draw_line (int x0, int y0, int x1, int y1, TGAColor color)
  {
    return draw_line_bresenham (x0, y0, x1, y1, color);
  }

};

int
main ()
{
  for (int i = 0; i < image.get_width (); ++i)
    hyper::draw_line (i, 0, i, image.get_height () - 1, red);

  for (int i = 0; i < image.get_height() / 2; ++i)
    hyper::draw_line (0, i, image.get_width () - 1, i, green);

  // I want to have the origin at the left bottom corner of the image
  image.flip_vertically();
  image.write_tga_file("output.tga");

  return 0;
}
