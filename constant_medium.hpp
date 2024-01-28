#include "hitable.hpp"
#include "materials.hpp"
#include "texture.hpp"

class constant_medium : public hitable {
public:
  constant_medium(hitable *b, float d, texture *a) : boundary(b), density(d) {
    phase_function = new isotropic(a);
  }

  virtual bool hit(const ray &r, float t_min, float t_max,
                   hit_record &rec) const override {

    bool enable_debug = false;
    enable_debug = enable_debug && (random_double(0.0, 1.0) < 0.00001);

    hit_record rec1, rec2;

    if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1)) {
      if (boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2)) {

        if (enable_debug)
          std::cerr << "\nt0 t1 " << rec1.t << " " << rec2.t << "\n";

        if (rec1.t < t_min)
          rec1.t = t_min;
        if (rec2.t > t_max)
          rec2.t = t_max;
        if (rec1.t >= rec2.t)
          return false;
        if (rec1.t < 0.0)
          rec1.t = 0.0;

        float distance_inside_boundary =
            (rec2.t - rec1.t) * r.direction().length();
        float hit_distance = -(1 / density) * log(random_double(0.0, 1.0));

        if (hit_distance < distance_inside_boundary) {
          rec.t = rec1.t + hit_distance / r.direction().length();
          rec.p = r.point_at_parameter(rec.t);
          rec.normal = vec3(1, 0, 0); // arbitrary
          rec.mat_ptr = phase_function;

          if (enable_debug) {
            std::cerr << "hit_distance = " << hit_distance << "\n";
            std::cerr << "rec.t = " << rec.t << "\n";
            std::cerr << "rec.p = " << rec.p << "\n";
          }
          return true;
        }
      }
    }
    return false;
  }

  virtual bool bounding_box(float t0, float t1,
                            aabb &output_box) const override {

    return boundary->bounding_box(t0, t1, output_box);
  }

  hitable *boundary;
  float density;
  material *phase_function;
};