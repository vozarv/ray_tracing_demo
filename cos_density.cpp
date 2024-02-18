#include "vec3.hpp"
#include "utils.hpp"

#include <iostream>
#include <iomanip>
#include <math.h>

double f(const vec3& d) {
    auto cos_theta = d.z();
    return cos_theta*cos_theta*cos_theta;
}

double pdf(const vec3& d) {
    return d.z() / M_PI;
}

int main() {
    int N = 100;

    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        vec3 d = random_cosine_direction();
        //vec3 d = random_in_unit_sphere();
        sum += f(d) / pdf(d);
        std::cout << d.x() << " " << d.y() << " " << d.z() << std::endl;
    }

    // std::cout << std::fixed << std::setprecision(12);
    // std::cout << "PI/2 = " << M_PI / 2.0 << '\n';
    // std::cout << "Estimate = " << sum / N << '\n';
}