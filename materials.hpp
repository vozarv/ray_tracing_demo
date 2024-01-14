#pragma once
#include "hitable.hpp"
#include "ray.hpp"
#include "utils.hpp"
#include "texture.hpp"
class material {
public:
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered) const = 0;
};

class lambertian : public material {
public:
  lambertian(texture *a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered) const {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    scattered = ray(rec.p, target - rec.p, r_in.time());
    attenuation = albedo->value(0, 0, rec.p);
    return true;
  }

  texture *albedo;
};

class metal : public material {
public:
  metal(const vec3 &a, float f) : albedo(a) {
    if (f < 1.0)
      fuzz = f;
    else
      fuzz = 1.0;
  }
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

  vec3 albedo;
  float fuzz;
};


float schlick(float cosine, float ref_idx) {
  float r0 = (1 - ref_idx) / (1 + ref_idx);
  r0 = r0 * r0;
  return r0 + (1 - r0) * pow((1 - cosine), 5);
}

class dielectric : public material {
public:
  dielectric(float ri) : ref_idx(ri) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered) const {

    vec3 outward_normal;
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    float ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    vec3 refracted;
    float reflect_prob;
    float cosine;

    if (dot(r_in.direction(), rec.normal) > 0) {
      outward_normal = -rec.normal;
      ni_over_nt = ref_idx;
      cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
    } 
    else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();

    }

    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
      reflect_prob = schlick(cosine, ref_idx);
    } 
    else {
      reflect_prob = 1.0;
    }

    if(random_double(0.0, 1.0) < reflect_prob) {
      scattered = ray(rec.p, reflected, r_in.time());
    }
    else{
      scattered = ray(rec.p, refracted, r_in.time());
    }

    return true;
  }

  float ref_idx;
};