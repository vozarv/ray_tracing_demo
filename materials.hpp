#pragma once
#include "hitable.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "onb.hpp"
class material {
public:
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered, float& pdf) const  { return false; };
  virtual float scattering_pdf(const ray &r_in, const hit_record &rec, const ray& scattered) const { return 1; }
  virtual vec3 emitted(float u, float v, const vec3 &p) const {
    return vec3(0, 0, 0);
  }
};

class lambertian : public material {
public:
  lambertian(texture *a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &alb, ray &scattered, float& pdf) const {
    onb uvw;
    uvw.build_from_w(rec.normal);
    vec3 scatter_direction = uvw.local(random_cosine_direction());
    //vec3 target = rec.p + rec.normal + random_in_unit_sphere();
    //scattered = ray(rec.p, target - rec.p, r_in.time());
    scattered = ray(rec.p, unit_vector(scatter_direction), r_in.time());
    alb = albedo->value(rec.u, rec.v, rec.p);
    //pdf = dot(rec.normal, scattered.direction()) / M_PI;
    // pdf = 0.5 / M_PI;
    pdf = dot(uvw.w(), scattered.direction()) / M_PI;
    return true;
  }
  float scattering_pdf(const ray &r_in, const hit_record &rec, const ray& scattered) const {
    float cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0? 0 : cos_theta / M_PI;
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
                       vec3 &attenuation, ray &scattered, float& pdf) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    scattered =
        ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
    attenuation = albedo;
    pdf = 1;
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
                       vec3 &attenuation, ray &scattered, float& pdf) const {

    vec3 outward_normal;
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    float ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    pdf = 1;
    vec3 refracted;
    float reflect_prob;
    float cosine;

    if (dot(r_in.direction(), rec.normal) > 0) {
      outward_normal = -rec.normal;
      ni_over_nt = ref_idx;
      cosine = ref_idx * dot(r_in.direction(), rec.normal) /
               r_in.direction().length();
    } else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0 / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
    }

    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
      reflect_prob = schlick(cosine, ref_idx);
    } else {
      reflect_prob = 1.0;
    }

    if (random_double(0.0, 1.0) < reflect_prob) {
      scattered = ray(rec.p, reflected, r_in.time());
    } else {
      scattered = ray(rec.p, refracted, r_in.time());
    }

    return true;
  }

  float ref_idx;
};

class diffuse_light : public material {
public:
  diffuse_light(texture *a) : emit(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered, float& pdf) const {

    return false;
  }
  virtual vec3 emitted(float u, float v, const vec3 &p) const {
    return emit->value(u, v, p);
  }
  texture *emit;
};

class isotropic : public material {
public:
  isotropic(texture *a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       vec3 &attenuation, ray &scattered, float& pdf) const {

    scattered = ray(rec.p, random_in_unit_sphere());
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    pdf = 1 / (4 * M_PI);

    return true;
  }
  float scattering_pdf(const ray &r_in, const hit_record &rec, const ray& scattered) const override {
    return 1 / (4 * M_PI);
  }

  texture *albedo;
};