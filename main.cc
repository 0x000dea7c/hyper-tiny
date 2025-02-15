#include "tgaimage.hh"

#include <immintrin.h>
#include <cstdint>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <cassert>
#include <array>
#include <cmath>

static TGAColor const green = TGAColor (0, 255, 0, 255);
static TGAColor const white = TGAColor (255, 255, 255, 255);
static TGAColor const red   = TGAColor (255, 0,   0,   255);
static TGAImage image (1920, 1080, TGAImage::RGB);

namespace hyper
{
  struct Vec3f
  {
    Vec3f () = default;

    Vec3f (float xx, float yy, float zz)
      : x { xx },
        y { yy },
        z { zz }
    {}

    float x;
    float y;
    float z;
  };

  struct Model
  {
    std::vector<Vec3f> vertices;
    std::vector<std::vector<size_t>> faces;
  };

  bool
  load_obj_file (Model &model, char const *filepath)
  {
    assert (filepath);

    std::ifstream ifs (filepath);

    if (!ifs)
      {
        std::cerr << "couldn't open obj file at path " << filepath << '\n';
        return false;
      }

    float vertex_x;
    float vertex_y;
    float vertex_z;
    size_t vertex_id, ignore, k = 0;

    for (std::string line; std::getline (ifs, line); )
      {
        if ((line[0] == 'v' && line[1] == ' '))
          {
            std::istringstream line_iss (line.substr (2));
            line_iss >> vertex_x >> vertex_y >> vertex_z;
            model.vertices.emplace_back (vertex_x, vertex_y, vertex_z);
          }
        else if (line[0] == 'f' && line[1] == ' ')
          {
            model.faces.emplace_back ();

            // f a/b/c d/e/f g/h/i, I only care about a d and g.
            std::istringstream line_iss (line.substr (2));
            size_t vertices[3];

            for (size_t i = 0; i < 3; ++i)
              {
                line_iss >> vertex_id;
                line_iss.get ();
                line_iss >> ignore;
                line_iss.get ();
                line_iss >> ignore;
                vertices[i] = vertex_id;
              }

            // OBJ files are 1 indexed, so need to adjust this
            model.faces[k].emplace_back (vertices[0] - 1);
            model.faces[k].emplace_back (vertices[1] - 1);
            model.faces[k].emplace_back (vertices[2] - 1);

            ++k;
          }
      }

    return true;
  }

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
  hyper::Model model;

  if (!load_obj_file (model, "obj/african_head.obj"))
    return EXIT_FAILURE;

  for (size_t i = 0; i < model.faces.size (); ++i)
    {
      for (size_t j = 0; j < 3; ++j)
        {
          //
          // Draw 3 lines!
          //
          // screenX = ((x + 1) / 2) * image.get_width ()
          // screenY = (1.0 - ((y + 1) / 2)) * image.get_width ()
          //
          int x0 = (int) std::floor (((model.vertices[model.faces[i][j]].x + 1.0) / 2.0f) * image.get_width ());
          int y0 = (int) std::floor (((model.vertices[model.faces[i][j]].y + 1) / 2.0f) * (image.get_height () - 1));
          int x1 = (int) std::floor (((model.vertices[model.faces[i][(j + 1) % 3]].x + 1.0) / 2.0f) * image.get_width ());
          int y1 = (int) std::floor (((model.vertices[model.faces[i][(j + 1) % 3]].y + 1.0) / 2.0f) * (image.get_height () - 1));

          hyper::draw_line (x0, y0, x1, y1, white);
        }
    }

  // I want to have the origin at the left bottom corner of the image
  image.flip_vertically ();
  image.write_tga_file ("output.tga");

  return 0;
}
