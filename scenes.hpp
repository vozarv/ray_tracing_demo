#pragma once
#include "box.hpp"
#include "bvh.hpp"
#include "constant_medium.hpp"
#include "materials.hpp"
#include "rect.hpp"
#include "sphere.hpp"
#include "texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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
  texture *pertext_small = new noise_texture(2);
  int n = 50;
  hitable **list = new hitable *[n + 1];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext_small));
  list[1] = new sphere(vec3(0, 2, 0), 2, new lambertian(pertext_small));

  return new hitable_list(list, 2);
}

hitable *earth_sphere() {
  texture *pertext_small = new noise_texture(2);

  int nx, ny, nz;
  unsigned char *tex_data = stbi_load("../images/earthmap.jpg", &nx, &ny, &nz, 0);
  if (tex_data == NULL) {
    std::cout << "Failed to load earthmap.jpg" << std::endl;
    return NULL;
  }
  std::cout << "Loaded image with dimensions: " << nx << " " << ny << " " << nz
            << std::endl;

  texture *earth_texture = new image_texture(tex_data, nx, ny);
  material *mat = new lambertian(earth_texture);

  int n = 50;
  hitable **list = new hitable *[n + 1];
  list[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(pertext_small));
  list[1] = new sphere(vec3(0, 2, 0), 2, mat);

  return new hitable_list(list, 2);
}

hitable *simple_light() {
  texture *pertext_small = new noise_texture(2);

  int nx, ny, nz;
  unsigned char *tex_data = stbi_load("../images/earthmap.jpg", &nx, &ny, &nz, 0);
  if (tex_data == NULL) {
    std::cout << "Failed to load earthmap.jpg" << std::endl;
    return NULL;
  }
  std::cout << "Loaded image with dimensions: " << nx << " " << ny << " " << nz
            << std::endl;

  texture *earth_texture = new image_texture(tex_data, nx, ny);
  material *mat = new lambertian(earth_texture);

  int n = 50;
  hitable **list = new hitable *[n + 1];
  int i = 0;

  list[i++] =
      new sphere(vec3(0, -1000, 0), 1000,
                 new lambertian(new constant_texture(vec3(0.7, 0.2, 0.2))));
  list[i++] = new sphere(vec3(0, 2, 0), 2, mat);
  list[i++] = new sphere(
      vec3(0, 2, 0), 1, new diffuse_light(new constant_texture(vec3(4, 4, 4))));
  list[i++] = new flip_normals(new xy_rect(
      3, 5, 1, 3, 2, new diffuse_light(new constant_texture(vec3(4, 4, 4)))));
  list[i++] = new xy_rect(
      3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4, 4, 4))));

  return new hitable_list(list, i);
}

hitable *cornell_box() {

  int n = 50;

  material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
  material *green =
      new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

  material *aluminium = new metal(vec3(0.8, 0.85, 0.88), 0.0);

  hitable **list = new hitable *[n + 1];

  int i = 0;
  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);

  list[i++] = new translate(
      new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
      vec3(130, 0, 65));
  list[i++] = new translate(
      new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), aluminium), 15),
      vec3(265, 0, 295));

  return new hitable_list(list, i);
}

hitable *cornell_glass_sphere() {

  int n = 50;

  material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
  material *green =
      new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

  material *aluminium = new metal(vec3(0.8, 0.85, 0.88), 0.0);

  hitable **list = new hitable *[n + 1];

  int i = 0;
  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

  list[i++] = new xz_rect(213, 343, 227, 332, 554, light);

  list[i++] = new sphere(vec3(190, 90, 190), 90, new dielectric(1.5));

  // list[i++] = new translate(
  //     new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
  //     vec3(130, 0, 65));
  list[i++] = new translate(
      new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
      vec3(265, 0, 295));

  return new hitable_list(list, i);
}

hitable *cornell_smoke() {

  int n = 50;

  material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
  material *green =
      new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
  material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

  hitable **list = new hitable *[n + 1];

  int i = 0;
  list[i++] = new flip_normals(new yz_rect(0, 555, 0, 555, 555, green));
  list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
  list[i++] = new flip_normals(new xz_rect(0, 555, 0, 555, 555, white));
  list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
  list[i++] = new flip_normals(new xy_rect(0, 555, 0, 555, 555, white));

  list[i++] = new xz_rect(113, 443, 127, 432, 554, light);

  list[i++] = new constant_medium(
      new translate(
          new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
          vec3(130, 0, 65)),
      0.01, new constant_texture(vec3(1.0, 1.0, 1.0)));
  list[i++] = new constant_medium(
      new translate(
          new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
          vec3(265, 0, 295)),
      0.01, new constant_texture(vec3(0.0, 0.0, 0.0)));

  return new hitable_list(list, i);
}

hitable *final_scene() {

  hitable **list = new hitable *[30];
  hitable **boxlist = new hitable *[10000];
  hitable **boxlist2 = new hitable *[10000];

  material *ground =
      new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));
  material *white =
      new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
  material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

  int nb = 20;
  int b = 0;
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < nb; j++) {
      float w = 100;
      float x0 = -1000 + i * w;
      float z0 = -1000 + j * w;
      float y0 = 0;
      float x1 = x0 + w;
      float y1 = 100 * random_double(0.0, 1.0) + 0.01;
      float z1 = z0 + w;
      boxlist[b++] = new box(vec3(x0, y0, z0), vec3(x1, y1, z1), ground);
    }
  }

  int l = 0;
  list[l++] = new bvh_node(boxlist, b, 0, 1);
  list[l++] = new xz_rect(123, 423, 147, 412, 554, light);

  vec3 center(400, 400, 200);
  list[l++] = new moving_sphere(
      center, center + vec3(30, 0, 0), 0, 1, 50,
      new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1))));

  list[l++] = new sphere(vec3(260, 150, 45), 70, new dielectric(1.5));
  list[l++] =
      new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0));

  hitable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
  list[l++] = boundary;

  list[l++] = new constant_medium(boundary, 0.2,
                                  new constant_texture(vec3(0.2, 0.4, 0.9)));

  boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
  list[l++] = new constant_medium(boundary, 0.0001,
                                  new constant_texture(vec3(1.0, 1.0, 1.0)));

  int nx, ny, nn;
  unsigned char *tex_data = stbi_load("../images/earthmap.jpg", &nx, &ny, &nn, 0);
  if (tex_data == NULL) {
    std::cout << "Failed to load earthmap.jpg" << std::endl;
    return NULL;
  }
  std::cout << "Loaded image with dimensions: " << nx << " " << ny << " " << nn
            << std::endl;

  texture *earth_texture = new image_texture(tex_data, nx, ny);
  material *mat = new lambertian(earth_texture);
  list[l++] = new sphere(vec3(400, 200, 400), 100, mat);

  texture *pertext = new noise_texture(0.1);
  list[l++] = new sphere(vec3(220, 280, 300), 80, new lambertian(pertext));

  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxlist2[j] = new sphere(vec3(165 * random_double(0.0, 1.0), 165 * random_double(0.0, 1.0), 165 * random_double(0.0, 1.0)), 10, white);
  }
  list[l++] =
      new translate(new rotate_y(new bvh_node(boxlist2, ns, 0.0, 1.0), 15),
                    vec3(-100, 270, 395));

  return new hitable_list(list, l);
}