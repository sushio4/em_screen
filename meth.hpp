#pragma once
#include <cmath>
#include <functional>
#include <stdexcept>

#ifndef M_SQRT3
#define M_SQRT3	1.73205080756887719000
#endif 


struct vector5f {
    float x, y, z, w, v;

    float& operator[](int index) {
        switch (index) {
            case 0:
            return x;
            case 1:
            return y; 
            case 2:
            return z;
            case 3:
            return w;
            case 4:
            return v;
        }

        throw std::out_of_range("Invalid index for vector5f");
    }

    vector5f operator*(float rhs) const {
        vector5f v;
        v.x = this->x * rhs;
        v.y = this->y * rhs;
        v.z = this->z * rhs;
        v.w = this->w * rhs;
        v.v = this->v * rhs;
        return v;
    }

    vector5f operator+(const vector5f& rhs) const {
        vector5f v;
        v.x = this->x + rhs.x;
        v.y = this->y + rhs.y;
        v.z = this->z + rhs.z;
        v.w = this->w + rhs.w;
        v.v = this->v + rhs.v;
        return v;
    }

    vector5f operator+=(const vector5f& rhs) {
        *this = *this + rhs;
        return *this;
    }

    template<typename T>
    vector5f operator*=(T rhs) {
        *this = *this * rhs;
        return *this;
    }

    float abs() {
        return sqrt(x*x + y*y + z*z + w*w + v*v);
    }
};

vector5f operator*(float lhs, const vector5f& rhs);

vector5f grad5f(std::function<float(const vector5f&)> func, const vector5f& x, const float dx = 0.01);

