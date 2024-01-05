#pragma once

#include <iostream>
#include <math.h>
#include <stdlib.h>


class vec3 {
    public:
    vec3() {}
    vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }

    inline float x() const { return e[0]; }
    inline float y() const { return e[1]; }
    inline float z() const { return e[2]; }
    inline float r() const { return e[0]; }
    inline float g() const { return e[1]; }
    inline float b() const { return e[2]; }

    inline const vec3& operator+() const { return *this; }
    inline vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    inline float operator[](int i) const { return e[i]; }
    inline float& operator[](int i) { return e[i]; }

    inline vec3 operator+(const vec3& v) const { return vec3(e[0] + v.e[0], e[1] + v.e[1], e[2] + v.e[2]); }
    inline vec3 operator-(const vec3& v) const { return vec3(e[0] - v.e[0], e[1] - v.e[1], e[2] - v.e[2]); }
    inline vec3 operator*(const vec3& v) const { return vec3(e[0] * v.e[0], e[1] * v.e[1], e[2] * v.e[2]); }
    inline vec3 operator/(const vec3& v) const { return vec3(e[0] / v.e[0], e[1] / v.e[1], e[2] / v.e[2]); }

    inline vec3 operator+(float t) const { return vec3(e[0] + t, e[1] + t, e[2] + t); }
    inline vec3 operator-(float t) const { return vec3(e[0] - t, e[1] - t, e[2] - t); }
    inline vec3 operator*(float t) const { return vec3(e[0] * t, e[1] * t, e[2] * t); }
    inline vec3 operator/(float t) const { return vec3(e[0] / t, e[1] / t, e[2] / t); }

    inline vec3& operator+=(const vec3& v) { e[0] += v.e[0]; e[1] += v.e[1]; e[2] += v.e[2]; return *this; }
    inline vec3& operator-=(const vec3& v) { e[0] -= v.e[0]; e[1] -= v.e[1]; e[2] -= v.e[2]; return *this; }
    inline vec3& operator*=(const vec3& v) { e[0] *= v.e[0]; e[1] *= v.e[1]; e[2] *= v.e[2]; return *this; }
    inline vec3& operator/=(const vec3& v) { e[0] /= v.e[0]; e[1] /= v.e[1]; e[2] /= v.e[2]; return *this; }

    inline vec3& operator+=(float t) { e[0] += t; e[1] += t; e[2] += t; return *this; }
    inline vec3& operator-=(float t) { e[0] -= t; e[1] -= t; e[2] -= t; return *this; }
    inline vec3& operator*=(const float t) { e[0] *= t; e[1] *= t; e[2] *= t; return *this; }
    inline vec3& operator/=(const float t) { e[0] /= t; e[1] /= t; e[2] /= t; return *this; }

    inline vec3& operator=(const vec3& v) { e[0] = v.e[0]; e[1] = v.e[1]; e[2] = v.e[2]; return *this; }
    inline bool operator==(const vec3& v) const { return e[0] == v.e[0] && e[1] == v.e[1] && e[2] == v.e[2]; }
    inline bool operator!=(const vec3& v) const { return e[0]!= v.e[0] || e[1]!= v.e[1] || e[2]!= v.e[2]; }

    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float length2() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    inline float dot(const vec3& v) const { return e[0] * v.e[0] + e[1] * v.e[1] + e[2] * v.e[2]; }
    inline vec3 cross(const vec3& v) const { return vec3(e[1] * v.e[2] - e[2] * v.e[1], e[2] * v.e[0] - e[0] * v.e[2], e[0] * v.e[1] - e[1] * v.e[0]); }
    inline vec3 normalize() const { return *this / length(); }
    inline void make_unit_vector() { *this = normalize(); }
    inline float angle(const vec3& v) const { return acos(dot(v) / (length() * v.length())); }

    float e[3];
};

inline std::istream& operator>>(std::istream& is, vec3& v) {
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}
inline std::ostream& operator<<(std::ostream& os, vec3& v) {
    os << v.e[0] << " " << v.e[1] << " " << v.e[2];
    return os;
}

inline float dot(const vec3& v1, const vec3& v2) { return v1.dot(v2); }
inline vec3 cross(const vec3& v1, const vec3& v2) { return v1.cross(v2); }
inline vec3 operator*(float t, const vec3& v) { return v * t; }

vec3 unit_vector(const vec3& v){ return v.normalize(); }

vec3 reflect(const vec3& v, const vec3& n){ return v - 2 * dot(v, n) * n; }

bool refract(const vec3 &v, const vec3 &n, float ni_over_nt, vec3& refracted) {
  vec3 uv = unit_vector(v);
  float dt = uv.dot(n);
  float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
  
  if (discriminant > 0.0) {
    refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
    return true;
  }
  else {
    return false;
  }
}