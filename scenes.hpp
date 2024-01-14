#pragma once
#include "bvh.hpp"
#include "materials.hpp"
#include "sphere.hpp"
#include "texture.hpp"

hitable *random_scene() {

  int n = 500;
  hitable **list = new hitable *[n + 1];

  texture *checker =
      new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                          new constant_texture(vec3(0.9, 0.9, 0.9)));
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(checker));
  int i = 1;

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {

      float choose_material = random_double(0.0, 1.0);
      vec3 center(a + 0.9 * random_double(0.0, 1.0), 0.2,
                  b + 0.9 * random_double(0.0, 1.0));

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_material < 0.8) {
          list[i++] = new moving_sphere(
              center, center + vec3(0, 0.5 * random_double(0.0, 1.0), 0.0), 0.0,
              1.0, 0.2,
              new lambertian(new constant_texture(
                  vec3(random_double(0.0, 1.0), random_double(0.0, 1.0),
                       random_double(0.0, 1.0)))));
        } else if (choose_material < 0.95) {
          list[i++] =
              new sphere(center, 0.2,
                         new metal(vec3(0.5 * (1.0 + random_double(0.0, 1.0)),
                                        0.5 * (1.0 + random_double(0.0, 1.0)),
                                        0.5 * (1.0 + random_double(0.0, 1.0))),
                                   0.5 * random_double(0.8, 1.2)));
        } else {
          list[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }

  list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
  list[i++] =
      new sphere(vec3(-4, 1, 0), 1.0,
                 new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
  list[i++] =
      new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

  return new bvh_node(list, i, 0.0, 1.0);
}

hitable *four_spheres() {

  hitable *list[4];

  list[0] =
      new sphere(vec3(0, 0, -1), 0.5,
                 new lambertian(new constant_texture(vec3(0.8, 0.3, 0.3))));
  list[1] =
      new sphere(vec3(0, -100.5, -1), 100,
                 new lambertian(new constant_texture(vec3(0.8, 0.8, 0.0))));
  list[2] =
      new sphere(vec3(1, 0, -1), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.1));
  list[3] = new sphere(vec3(-1, 0, -1), -0.45, new dielectric(1.5));

  return new bvh_node(list, 4, 0.0, 1.0);
}

hitable *two_horizontal_spheres() {

  float R = cos(M_PI / 4);
  hitable *list[2];

  list[0] = new sphere(vec3(-R, 0, -1), R,
                       new lambertian(new constant_texture(vec3(0, 0, 1))));
  list[1] = new sphere(vec3(R, 0, -1), R,
                       new lambertian(new constant_texture(vec3(1, 0, 0))));

  return new bvh_node(list, 2, 0.0, 1.0);
}

hitable *two_checker_spheres() {
  texture *checker =
      new checker_texture(new constant_texture(vec3(0.2, 0.3, 0.1)),
                          new constant_texture(vec3(0.9, 0.9, 0.9)));

  int n = 50;
  hitable **list = new hitable *[n + 1];
  list[0] = new sphere(vec3(0, -10, 0), 10, new lambertian(checker));
  list[1] = new sphere(vec3(0, 10, 0), 10, new lambertian(checker));

  return new hitable_list(list, 2);
}

hitable *two_perlin_spheres() {
  texture *pertext = new noise_texture();

  int n = 50;
  hitable **list = new hitable *[n + 1];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext));
  list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext));

  return new hitable_list(list, 2);
}