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

    float abs() {
        return sqrt(x*x + y*y + z*z + w*w + v*v);
    }
};

vector5f grad5f(std::function<float(vector5f)> function, const vector5f& x, const float dx = 0.01);
