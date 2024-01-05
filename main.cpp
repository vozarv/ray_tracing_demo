#include "camera.hpp"
#include "float.h"
#include "hitable_list.hpp"
#include "materials.hpp"
#include "sphere.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

vec3 color(const ray &r, hitable *world, int depth) {

  hit_record rec;

  if (world->hit(r, 0.00001, MAXFLOAT, rec)) {
    ray scattered;
    vec3 attenuation(0.01, 0.01, 0.01);

    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return attenuation * color(scattered, world, depth + 1);
    }

    else {
      return vec3(0.0, 0.0, 0.0);
    }
  }

  else {
    vec3 unit_direction = unit_vector(r.direction());
    float t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
  }
}


int main() {

  ofstream file("../output.ppm");

  camera cam;

  vec3 lower_left_corner(-2.0f, -1.0f, -1.0f);
  vec3 horizontal(4.0f, 0.0f, 0.0f);
  vec3 vertical(0.0f, 2.0f, 0.0f);
  vec3 origin(0.0f, 0.0f, 0.0f);

  hitable *list[4];
  list[0] = new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
  list[1] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
  list[2] = new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.1));
  list[3] = new sphere(vec3(-1, 0, -1), 0.5, new metal(vec3(0.8, 0.8, 0.8), 0.4));
  hitable *world = new hitable_list(list, 4);

  int nx = 200;
  int ny = 100;
  int ns = 1;

  vec3 *image = new vec3[nx * ny];

  file << "P3\n" << nx << " " << ny << "\n255\n";

  for (int j = ny - 1; j >= 0; j--) {
    std::clog << "\rScanlines remaining: " << j << ' ' << std::flush;

    for (int i = 0; i < nx; i++) {

      vec3 col(0, 0, 0);

      for (int s = 0; s < ns; s++) {

        float u = float(i + random_double(0.0, 1.0)) / float(nx);
        float v = float(j + random_double(0.0, 1.0)) / float(ny);

        ray r = cam.get_ray(u, v);
        col += color(r, world, 0);
      }

      col /= float(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

      image[j * nx + i] = col;
    }
  }

  std::clog << "\rDone.                 \n";

  save_image(image, nx, ny, file);
  file.close();
}