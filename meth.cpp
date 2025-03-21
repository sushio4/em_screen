#include "meth.hpp"

vector5f grad5f(std::function<float(const vector5f&)> function, const vector5f& x, const float dx) {
    vector5f result;
    for(int i = 0; i < 5; i++) {
        vector5f xplusdx = x;
        xplusdx[i] += dx;
        result[i] = (function(xplusdx) - function(x)) / dx;
    }
    return result;
}
