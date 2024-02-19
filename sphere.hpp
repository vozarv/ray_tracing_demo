#pragma once

#include "hitable.hpp"

class sphere : public hitable {
public:
  sphere() {}
  sphere(vec3 cen, float r, material *m) : center(cen), radius(r), mat(m){};
  virtual bool hit(const ray &r, float t_min, float t_max,
                   hit_record &rec) const;

  bool bounding_box(float t0, float t1, aabb &box) const override
  {
    box = aabb(center - vec3(radius, radius, radius),
                center + vec3(radius, radius, radius));
    return true;
  }

  float pdf_value(const vec3& o, const vec3& v) const override {
    hit_record rec;
    if(this->hit(ray(o, v), 0.001, FLT_MAX, rec)){
      float cos_theta_max = sqrt(1-radius*radius/(center-o).length2());
      float solid_angle = 2*M_PI*(1-cos_theta_max);
      return 1/solid_angle;
    }
    else return 0;
  }

  vec3 random(const vec3& o) const override {
    vec3 direction = center - o;
    float distance_squared = direction.length2();
    onb uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(radius, distance_squared));
    
  }

  vec3 center;
  float radius;
  material *mat;
};

bool sphere::hit(const ray &r, float t_min, float t_max,
                 hit_record &rec) const {

  vec3 oc = r.origin() - center;
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat;
      get_sphere_uv(rec.normal, rec.u, rec.v);
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center) / radius;
      rec.mat_ptr = mat;
      get_sphere_uv(rec.normal, rec.u, rec.v);
      return true;
    }
  }

  return false;
}


class moving_sphere : public hitable {
  public:
  moving_sphere() {}
  moving_sphere(vec3 cen0, vec3 cen1, float t0, float t1, float r, material *m) : center0(cen0), center1(cen1), time0(t0), time1(t1), radius(r), mat(m){};

  virtual bool hit(const ray &r, float t_min, float t_max, hit_record& rec) const;

  vec3 center(float time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
  }

  bool bounding_box(float t0, float t1, aabb &box) const override {
    
    aabb box0 = aabb(center0 - vec3(radius, radius, radius),
                center0 + vec3(radius, radius, radius));
    aabb box1 = aabb(center1 - vec3(radius, radius, radius),
                center1 + vec3(radius, radius, radius));

    box = surrounding_box(box0, box1);
    
    return true;
  }

  vec3 center0, center1;
  float time0, time1;
  float radius;
  material *mat;
};


bool moving_sphere::hit(const ray &r, float t_min, float t_max,
                 hit_record &rec) const {

  vec3 oc = r.origin() - center(r.time());
  float a = dot(r.direction(), r.direction());
  float b = dot(oc, r.direction());
  float c = dot(oc, oc) - radius * radius;
  float discriminant = b * b - a * c;

  if (discriminant > 0) {
    float temp = (-b - sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat;
      return true;
    }
    temp = (-b + sqrt(discriminant)) / a;
    if (temp < t_max && temp > t_min) {
      rec.t = temp;
      rec.p = r.point_at_parameter(rec.t);
      rec.normal = (rec.p - center(r.time())) / radius;
      rec.mat_ptr = mat;
      return true;
    }
  }

  return false;
}