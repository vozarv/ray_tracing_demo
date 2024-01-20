#include <math.h>
#include <stdio.h>
#include <stdlib.h> // card > aek.ppm

struct vector {
  float x, y, z;
  vector operator+(vector r) { return vector(x + r.x, y + r.y, z + r.z); }
  vector operator*(float r) { return vector(x * r, y * r, z * r); }
  float operator%(vector r) { return x * r.x + y * r.y + z * r.z; }
  vector() {}
  vector operator^(vector r) {
    return vector(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
  }
  vector(float a, float b, float c) {
    x = a;
    y = b;
    z = c;
  }
  vector operator!() { return *this * (1 / sqrt(*this % *this)); }
};

int G[] = {247570, 280596, 280600, 249748, 18578, 18577, 231184, 16, 16};

float random_number() { return (float)rand() / RAND_MAX; }

int tracer(vector origin, vector direction, float &t, vector &normal) {

  t = 1e9;
  int m = 0;
  float p = -origin.z / direction.z;
  if (.01 < p) {
    t = p;
    normal = vector(0, 0, 1);
    m = 1;
  }

  for (int k = 19; k--;)
    for (int j = 9; j--;)
      if (G[j] & 1 << k) {
        vector p = origin + vector(-k, 0, -j - 4);
        float b = p % direction; 
        float c = p % p - 1; 
        float q = b * b - c;

        if (q > 0) {
          float s = -b - sqrt(q);
          if (s < t && s > .01) {
            t = s;
            normal = !(p + direction * t);
            m = 2;
          }
        }
      }
  return m;
}

vector sampler(vector origin, vector direction) {
  float t;
  vector normal;
  int m = tracer(origin, direction, t, normal);
  if (!m)
    return vector(.7, .6, 1) * pow(1 - direction.z, 4);

  vector hitpoint = origin + direction * t;
  vector light_direction = !(vector(9 + random_number(), 9 + random_number(), 16) + hitpoint * -1);
  vector reflected = direction + normal * (normal % direction * -2);

  float b = light_direction % normal;
  if (b < 0 || tracer(hitpoint, light_direction, t, normal))
    b = 0;
  float p = pow(light_direction % reflected * (b > 0), 99);
  if (m & 1) {
    hitpoint = hitpoint * .2;
    return ((int)(ceil(hitpoint.x) + ceil(hitpoint.y)) & 1 ? vector(3, 1, 1)
                                             : vector(3, 3, 3)) *
           (b * .2 + .1);
  }
  return vector(p, p, p) + sampler(hitpoint, reflected) * .5;
}

int main() {

  printf("P6 512 512 255 ");
  vector dir = !vector(-6, -16, 0);
  vector up = !(vector(0, 0, 1) ^ dir) * .002;
  vector right = !(dir ^ up) * .002;
  vector c = (up + right) * -256 + dir;

  for (int y = 512; y--;)
    for (int x = 512; x--;) {

      vector pixel(13, 13, 13);
      for (int r = 64; r--;) {
        vector t = up * (random_number() - 0.5) * 99 +
                   right * (random_number() - 0.5) * 99;
        pixel = sampler(vector(17, 16, 8) + t,
                        !(t * -1 + (up * (random_number() + x) +
                                    right * (y + random_number()) + c) *
                                       16)) *
                    3.5 +
                pixel;
      }
      printf("%c%c%c", (int)pixel.x, (int)pixel.y, (int)pixel.z);
    }
}