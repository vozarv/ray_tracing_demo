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

    double r1 = random_double(0.0, 1.0);
    double r2 = random_double(0.0, 1.0);
    double x = cos(2*M_PI*r1)*2*sqrt(r2*(1-r2));
    double y = sin(2*M_PI*r1)*2*sqrt(r2*(1-r2));
    double z = 1 - 2*r2;

    return vec3(x,y,z);
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

inline vec3 random_cosine_direction() {
    auto r1 = random_double(0.0, 1.0);
    auto r2 = random_double(0.0, 1.0);

    auto phi = 2*M_PI*r1;
    auto x = cos(phi)*sqrt(r2);
    auto y = sin(phi)*sqrt(r2);
    auto z = sqrt(1-r2);

    return vec3(x, y, z);
}

inline vec3 random_to_sphere(float radius, float distance_squared){
  float r1 = random_double(0.0, 1.0);
  float r2 = random_double(0.0, 1.0);
  float z = 1 + r2 * (sqrt(1 - radius*radius / distance_squared) - 1);
  float phi = 2 * M_PI * r1;
  float x = cos(phi) * sqrt(1 - z*z);
  float y = sin(phi) * sqrt(1 - z*z);
  return vec3(x, y, z);
}

inline vec3 de_nan(const vec3& c){
  vec3 temp = c;
  if(!(temp[0] == temp[0])) temp[0] = 0;
  if(!(temp[1] == temp[1])) temp[1] = 0;
  if(!(temp[2] == temp[2])) temp[2] = 0;

  return temp;
}