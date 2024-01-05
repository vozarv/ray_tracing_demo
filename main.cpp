#include "camera.hpp"
#include "float.h"
#include "hitable_list.hpp"
#include "sphere.hpp"
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>
#include <stdlib.h>

using namespace std;


double random_double(double min, double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(gen);
}

vec3 random_in_unit_sphere() {
  vec3 p;
  do {
    p = 2.0 * vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), random_double(-1.0, 1.0)) - vec3(1.0, 1.0, 1.0);
  } while (p.length2() >= 1.0);
  return p;
}

vec3 color(const ray &r, hitable *world) {

  hit_record rec;
  if (world->hit(r, 0.00001, MAXFLOAT, rec)) {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    return 0.5 * color(ray(rec.p, target - rec.p), world);

  } else {
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

  hitable *list[2];
  list[0] = new sphere(vec3(0, 0, -1), 0.5);
  list[1] = new sphere(vec3(0, -100.5, -1), 100);
  hitable *world = new hitable_list(list, 2);

  int nx = 200;
  int ny = 100;
  int ns = 10;

  file << "P3\n" << nx << " " << ny << "\n255\n";

  for (int j = ny - 1; j >= 0; j--) {

    std::clog << "\rScanlines remaining: " << j << ' ' << std::flush;
    
    for (int i = 0; i < nx; i++) {

      vec3 col(0, 0, 0);

      for (int s = 0; s < ns; s++) {

        float u = float(i + random_double(0.0, 1.0)) / float(nx);
        float v = float(j + random_double(0.0, 1.0)) / float(ny);

        ray r = cam.get_ray(u, v);
        col += color(r, world);
      }

      col /= float(ns);
      col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
      int ir = int(255.99 * col[0]);
      int ig = int(255.99 * col[1]);
      int ib = int(255.99 * col[2]);
      file << ir << " " << ig << " " << ib << "\n";
    }
  }

  std::clog << "\rDone.                 \n";

  file.close();
}