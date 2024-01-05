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
  vec3 p;
  do {
    p = 2.0 * vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), random_double(-1.0, 1.0)) - vec3(1.0, 1.0, 1.0);
  } while (p.length2() >= 1.0);
  return p;
}