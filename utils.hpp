#pragma once
#include "vec3.hpp"
#include <random>

double random_double(double min, double max) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(gen);
}

vec3 random_in_unit_sphere() {

  double r = random_double(0.0, 1.0);
  double azimuth = random_double(0.0, 2.0 * M_PI);
  double elevation = random_double(0.0, M_PI);

  vec3 p(r * cos(elevation) * cos(azimuth), r * cos(elevation) * sin(azimuth),
         r * sin(elevation));

  return p;
}

vec3 random_in_unit_disk() {

  double r = random_double(0.0, 1.0);
  double azimuth = random_double(0.0, 2.0 * M_PI);

  vec3 p(r * cos(azimuth), r * sin(azimuth), 0.0);

  return p;
}

void get_sphere_uv(const vec3 &p, float &u, float &v) {
  
  float phi = atan2(p.z(), p.x());
  float theta = asin(p.y());

  u = 1 - (phi + M_PI) / (2.0 * M_PI);
  v = (theta + M_PI / 2) / M_PI;
}