//
// Created by Bertral on 09/04/2019.
//

#include <stdexcept>
#include <cmath>
#include "Color.h"

Color Color::BLACK(0, 0, 0);
Color Color::WHITE(1, 1, 1);
Color Color::RED(1, 0, 0);
Color Color::GREEN(0, 1, 0);
Color Color::BLUE(0, 0, 1);


Color::Color(const double r, const double g, const double b) : r(r), g(g), b(b) {
    if(r < 0 || r > 1 || g < 0 || g > 1 || b < 0 || b > 1) {
        std::invalid_argument("color components must be between 0 and 1 (inclusive)");
    }
}

double Color::get_r() const {
    return r;
}

double Color::get_g() const {
    return g;
}

double Color::get_b() const {
    return b;
}

const Color Color::mix_with(const Color &other, double p) const {
    if (p < 0 || p > 1) {
        throw std::invalid_argument("mix_with : p must be between 0 and 1 (inclusive)");
    }

    return Color(r - r * p + other.r * p, g - g * p + other.g * p, b - p * b + other.b * p);
}

unsigned int Color::packed_rgb() const {
    return srgb_encode(r, 255 * 255) + srgb_encode(g, 255) + srgb_encode(b, 1);
}

unsigned int Color::srgb_encode(double v, unsigned int max) {
    if (v <= 0.0031308) {
        return (max * 12.92 * v);
    } else {
        return (max * (1.055 * pow(v, 1 / 2.4) - 0.055));
    }
}

