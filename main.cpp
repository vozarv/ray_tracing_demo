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
#include <thread>

using namespace std;

void save_image(vec3 *image, int width, int height, ofstream &file) {

  for (int j = height - 1; j >= 0; j--) {
    for (int i = 0; i < width; i++) {

      vec3 pixel = image[i + j * width];

      int ir = int(255.99 * pixel[0]);
      int ig = int(255.99 * pixel[1]);
      int ib = int(255.99 * pixel[2]);

      file << ir << " " << ig << " " << ib << "\n";
    }
  }
}

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

void calc_pixel(int x, int y, vec3 *image, int width, int height, int ns,
                camera cam, hitable *world) {

  vec3 col(0, 0, 0);

  for (int s = 0; s < ns; s++) {

    float u = float(x + random_double(0.0, 1.0)) / float(width);
    float v = float(y + random_double(0.0, 1.0)) / float(height);

    ray r = cam.get_ray(u, v);
    col += color(r, world, 0);
  }

  col /= float(ns);
  col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

  image[y * width + x] = col;
}

int main() {

  ofstream file("../output.ppm");

  const int numThreads = 32;
  cout << "Number of threads: " << numThreads << endl;

  vector<thread> threads;

  int nx = 200;
  int ny = 100;
  int ns = 50;

  camera cam(vec3(-2, 2, 1), vec3(0, 0, -1), vec3(0, 1, 0), 30, float(nx) / float(ny));

  hitable *list[4];
  list[0] =
      new sphere(vec3(0, 0, -1), 0.5, new lambertian(vec3(0.8, 0.3, 0.3)));
  list[1] =
      new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.8, 0.8, 0.0)));
  list[2] =
      new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.1));
  list[3] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));
  hitable *world = new hitable_list(list, 4);

  // float R = cos(M_PI / 4);
  // hitable *list[2];
  // list[0] = new sphere(vec3(-R, 0, -1), R, new lambertian(vec3(0, 0, 1)));
  // list[1] = new sphere(vec3( R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
  // hitable *world = new hitable_list(list, 2);

  vec3 *image = new vec3[nx * ny];

  file << "P3\n" << nx << " " << ny << "\n255\n";

  time_t start, end;
  time(&start);

  for (int j = ny - 1; j >= 0; j--) {
    std::clog << "\rScanlines remaining: " << j << std::flush;

    for (int i = 0; i < nx; i += numThreads) {

      for (int t = 0; t < numThreads; t++) {
        if (i + t < nx) {
          threads.emplace_back(calc_pixel, i + t, j, image, nx, ny, ns, cam,
                               world);
        }
      }

      for (auto &thread : threads) {
        thread.join();
      }

      threads.clear();
    }
  }

  time(&end);

  std::clog << "\rDone.                 \n";
  std::clog << "Elapsed time: " << difftime(end, start) << " seconds"
            << std::endl;

  save_image(image, nx, ny, file);
  file.close();
}