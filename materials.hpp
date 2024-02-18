#pragma once
#include "hitable.hpp"
#include "onb.hpp"
#include "ray.hpp"
#include "texture.hpp"
#include "utils.hpp"
#include "pdf.hpp"

struct scatter_record
{
  ray specular_ray;
  bool is_specular;
  vec3 attenuation;
  pdf *pdf_ptr;
};

class material {
public:
  virtual bool scatter(const ray &r_in, const hit_record &hrec, scatter_record & srec) const {
    return false;
  };
  virtual float scattering_pdf(const ray &r_in, const hit_record &rec,
                               const ray &scattered) const {
    return 1;
  }
  virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v,
                       const vec3 &p) const {
    return vec3(0, 0, 0);
  }
};

class lambertian : public material {
public:
  lambertian(texture *a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &hrec, scatter_record& srec) const {
    
    srec.is_specular = false;
    srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
    srec.pdf_ptr = new cosine_pdf(hrec.normal);
    
    return true;
  }

  float scattering_pdf(const ray &r_in, const hit_record &rec,
                       const ray &scattered) const {
    float cos_theta = dot(rec.normal, unit_vector(scattered.direction()));
    return cos_theta < 0 ? 0 : cos_theta / M_PI;
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
  virtual bool scatter(const ray &r_in, const hit_record &hrec,
                       scatter_record& srec) const {
    vec3 reflected = reflect(unit_vector(r_in.direction()), hrec.normal);
    srec.specular_ray = ray(hrec.p, reflected + fuzz*random_in_unit_sphere(), r_in.time());
    srec.is_specular = true;
    srec.attenuation = albedo;
    srec.pdf_ptr = 0;
    return (true);
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
                       scatter_record& srec) const {

    vec3 outward_normal;
    vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
    float ni_over_nt;

    srec.attenuation = vec3(1.0, 1.0, 1.0);
    srec.is_specular = true;
    srec.pdf_ptr = 0;

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
      srec.specular_ray = ray(rec.p, reflected, r_in.time());
    } else {
      srec.specular_ray = ray(rec.p, refracted, r_in.time());
    }

    return true;
  }

  float ref_idx;
};

class diffuse_light : public material {
public:
  diffuse_light(texture *a) : emit(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &hrec,
                       scatter_record& srec) const {

    return false;
  }
  virtual vec3 emitted(const ray &r_in, const hit_record &rec, float u, float v,
                       const vec3 &p) const {

    if (dot(rec.normal, r_in.direction()) < 0)
      return vec3(0, 0, 0);
    else
      return emit->value(u, v, p);
  }
  texture *emit;
};

class isotropic : public material {
public:
  isotropic(texture *a) : albedo(a) {}
  virtual bool scatter(const ray &r_in, const hit_record &hrec,
                       scatter_record& srec) const {

    srec.specular_ray = ray(hrec.p, random_in_unit_sphere());
    srec.attenuation = albedo->value(hrec.u, hrec.v, hrec.p);
    srec.is_specular = false;
    srec.pdf_ptr = new sphere_pdf();

    return true;
  }
  float scattering_pdf(const ray &r_in, const hit_record &rec,
                       const ray &scattered) const override {
    return 1 / (4 * M_PI);
  }

  texture *albedo;
};