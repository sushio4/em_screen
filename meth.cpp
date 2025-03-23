#include "meth.hpp"
#include <functional>

vector5f operator*(float lhs, const vector5f& rhs) {
    return rhs * lhs;
}

vector5f grad5f(std::function<float(const vector5f&)> func, const vector5f& x, const float dx) {
    vector5f result;
    for(int i = 0; i < 5; i++) {
        vector5f xplusdx = x;
        xplusdx[i] += dx;
        result[i] = (func(xplusdx) - func(x)) / dx;
    }
    return result;
}
