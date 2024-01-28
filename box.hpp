#include "rect.hpp"
#include "hitable_list.hpp"

class box : public hitable {
    public:
    box() {}
    box(const vec3& p0, const vec3& p1, material *mat_ptr);
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const override;
    virtual bool bounding_box(float t0, float t1, aabb& output_box) const override {
        output_box = aabb(pmin, pmax);
        return true;
    }

    hitable *list_ptr;
    vec3 pmin, pmax;
};

box::box(const vec3& p0, const vec3& p1, material *mat_ptr){
    pmin = p0;
    pmax = p1;

    hitable **list = new hitable*[6];

    list[0] =                   new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat_ptr);
    list[1] = new flip_normals( new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat_ptr));
    
    list[2] =                   new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat_ptr);
    list[3] = new flip_normals( new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat_ptr));

    list[4] =                   new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat_ptr);
    list[5] = new flip_normals( new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat_ptr));

    list_ptr = new hitable_list(list, 6);
}

bool box::hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
    return list_ptr->hit(r, t_min, t_max, rec);
}