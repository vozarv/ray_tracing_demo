#include "bvh.hpp"
#include "camera.hpp"
#include "float.h"
#include "hitable_list.hpp"
#include "materials.hpp"
#include "scenes.hpp"
#include "sphere.hpp"
#include "utils.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <thread>

using namespace std;

void save_image(vec3 *image, int width, int height, ofstream &file) {

  file << "P3\n" << width << " " << height << "\n255\n";

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

    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

    if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
      return emitted + attenuation * color(scattered, world, depth + 1);
    }

    else {
      return emitted;
    }
  }

  else {
    return vec3(0.0, 0.0, 0.0);
    // vec3 unit_direction = unit_vector(r.direction());
    // float t = 0.5 * (unit_direction.y() + 1.0);
    // return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
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
  int ny = 200;
  int ns = 200;

  // vec3 lookfrom(13, 2, 3);
  // vec3 lookat(0, 2, 0);
  // float dist_to_focus = (lookfrom - lookat).length();
  // float aperture = 0.0;
  // float vfov = 30.0;

  // hitable *world = random_scene();
  // hitable *world = two_horizontal_spheres();
  // hitable *world = four_spheres();
  // hitable *world = two_checker_spheres();
  // hitable *world = two_perlin_spheres();
  // hitable *world = earth_sphere();
  // hitable *world = simple_light();


  vec3 lookfrom(278, 278, -800);
  vec3 lookat(278, 278, 0);
  float dist_to_focus = 10;
  float aperture = 0.0;
  float vfov = 40.0;

  //hitable *world = cornell_box();
  hitable *world = cornell_smoke();

  camera cam(lookfrom, lookat, vec3(0, 1, 0), vfov, float(nx) / float(ny),
             aperture, dist_to_focus, 0.0, 1.0);

  
  vec3 *image = new vec3[nx * ny];

  time_t start, end;
  time(&start);

  for (int j = ny - 1; j >= 0; j--) {
    std::clog << "\rScanlines remaining: " << setw(4) << setfill('0') << j
              << std::flush;

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