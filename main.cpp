#include "bvh.hpp"
#include "camera.hpp"
#include "float.h"
#include "hitable_list.hpp"
#include "materials.hpp"
#include "pdf.hpp"
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

vec3 color(const ray &r, hitable *world, hitable* light_shape, int depth) {

  hit_record hrec;

  if (world->hit(r, 0.00001, MAXFLOAT, hrec)) {

    scatter_record srec;

    vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);

    if (depth < 50 && hrec.mat_ptr->scatter(r, hrec, srec)) {

      if (srec.is_specular) {
        return srec.attenuation * color(srec.specular_ray, world, light_shape, depth + 1);
      } 
      
      else {
        hitable_pdf p0(light_shape, hrec.p);
        cosine_pdf p1(hrec.normal);
        mixture_pdf p(&p0, &p1);
        ray scattered = ray(hrec.p, p.generate(), r.time());
        float pdf_val = p.value(scattered.direction());
        return emitted + srec.attenuation *
                             hrec.mat_ptr->scattering_pdf(r, hrec, scattered) *
                             color(scattered, world, light_shape, depth + 1) / pdf_val;
      }
    }

    else {
      return emitted;
    }
  }

  else {
    return vec3(0.0, 0.0, 0.0);
  }
}

void calc_pixel(int x, int y, vec3 *image, int width, int height, int ns,
                camera cam, hitable *world) {

  vec3 col(0, 0, 0);
  
  hitable *light_shape = new xz_rect(213, 343, 227, 332, 554, 0);
  hitable *glass_sphere = new sphere(vec3(190, 90, 190), 90, 0);
  hitable *a[2];
  a[0] = light_shape;
  a[1] = glass_sphere;
  hitable_list hlist(a, 2);


  for (int s = 0; s < ns; s++) {

    float u = float(x + random_double(0.0, 1.0)) / float(width);
    float v = float(y + random_double(0.0, 1.0)) / float(height);

    ray r = cam.get_ray(u, v);
    col += de_nan(color(r, world, &hlist, 0));
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

  int nx = 500;
  int ny = 500;
  int ns = 100;

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
  // hitable *world = cornell_smoke();
  hitable *world = cornell_glass_sphere();

  // vec3 lookfrom(478, 278, -600);
  // vec3 lookat(278, 278, 0);
  // float dist_to_focus = 10;
  // float aperture = 0.0;
  // float vfov = 40.0;

  // hitable *world = final_scene();

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