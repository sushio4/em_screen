#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <tuple>
#include <string>
#include <iostream>
#include "meth.hpp"

// Requirements
constexpr float f = 2.71 * 1000 * 1000 * 1000; // 2.71 GHz (1/s)
constexpr float lightspeed = 299792458; // [m/s]
constexpr float lambda = lightspeed / f;
constexpr float required_db = 13;
constexpr float board_side = 0.5; // 50 cm
constexpr float board_margin = 0.015; // 1.5 cm

// Sanity constrains
constexpr float max_hex_diagonal = lambda / 2;
constexpr float max_hex_side = max_hex_diagonal * 2;
constexpr float min_hex_side = 0.005; // 5mm
constexpr float min_hex_dist = lambda / 10;
constexpr float min_theta = 0;
constexpr float max_theta = M_PI / 3;


struct hex_params {
    vector5f v;
    float& side = v.x;

    // center to center
    float& v_dist = v.y;
    // center to center
    float& h_dist = v.z;
    
    // this is theta = 0
    //      _
    //  _.-" "-._
    // |         |
    // |         | 
    // |         |
    //  "-._ _.-" 
    //      "
    float& theta0 = v.w;
    float& theta1 = v.v;

    hex_params() {
        side = min_hex_side;
        v_dist = min_hex_dist + min_hex_side * 2;
        h_dist = min_hex_dist + min_hex_side * 2;
        theta0 = theta1 = 0;
    }

    hex_params(const vector5f& vec) {
        v = vec;
    } 

    hex_params& operator=(const hex_params& rhs) {
        v = rhs.v;
        return *this;
    }

    float hole_len(float theta) const {
        while(theta > M_PI / 3) {
            theta -= M_PI / 3;
        }
        while(theta < 0) {
            theta += M_PI / 3;
        }

        float h = side * M_SQRT3;

        if(theta > M_PI / 6) {
            return h / cos(M_PI / 3 - theta);
        }
        return h / cos(theta);
    }
};

std::string to_string(const hex_params& hp) {
    std::string res("");
    res += "[side: " + std::to_string(hp.side) + ", h_dist: " + std::to_string(hp.h_dist) + ", v_dist: " + std::to_string(hp.v_dist)
        + ", theta0: " + std::to_string(hp.theta0) + ", theta1: " + std::to_string(hp.theta1) + "]";
    return res;
}

// creates 5D grid with n elements in each "row"
std::vector<hex_params> create_grid(int n) {
    std::vector<hex_params> res;

    for(int i = 0; i < n; i++) {
        float side = min_hex_side + i * (max_hex_side - min_hex_side) / n;
        for(int j = 0; j < n; j++) {
            float vdist = min_hex_dist + min_hex_side * 2 + j * (0.2 - min_hex_dist - min_hex_side * 2) / n;
            for(int k = 0; k < n; k++) {
                float hdist = min_hex_dist + min_hex_side * 2 + k * (0.2 - min_hex_dist - min_hex_side * 2) / n;
                hex_params hp0, hp1, hp2, hp3;
                hp0.side = hp1.side = hp2.side = hp3.side = side;
                hp0.v_dist = hp1.v_dist = hp2.v_dist = hp3.v_dist = vdist;
                hp0.h_dist = hp1.h_dist = hp2.h_dist = hp3.h_dist = hdist;
                
                hp0.theta0 = hp0.theta1 = min_theta;

                hp1.theta0 = max_theta;
                hp1.theta1 = min_theta;

                hp2.theta0 = min_theta;
                hp2.theta1 = max_theta;

                hp3.theta0 = hp3.theta1 = max_theta;

                res.insert(res.end(), {hp0, hp1, hp2, hp3});
            }
        }
    }
    return res;
}

std::tuple<int,int,int> calculate_columns_and_rows(const hex_params& params) {
    float board_cap = board_side - 2 * board_margin;
    
    float horiz_hex_len = params.hole_len(params.theta0);
    float board_cap_horiz = board_cap - horiz_hex_len;
    int columns0 = 1 + floor(board_cap_horiz / params.h_dist);

    if(horiz_hex_len > board_cap_horiz) return { 0, 0, 0 };

    float horiz_hex_len1 = params.hole_len(params.theta1);
    int columns1 = columns0;
    if(params.h_dist / 2 + (columns0 - 1) * params.h_dist + horiz_hex_len1 > board_cap) {
        columns1--;
    }

    float vert_hex_len0 = params.hole_len(params.theta0 + M_PI_4);
    float vert_hex_len1 = params.hole_len(params.theta1 + M_PI_4);

    float vert_hex_len = std::max(vert_hex_len0, vert_hex_len1);

    float board_cap_vert = board_cap - vert_hex_len;
    int rows = 1 + floor(board_cap_vert / params.v_dist);

    if(vert_hex_len > board_cap) {
        rows = 0;
    }

    return {columns0, columns1, rows};
}

float calculate_row_attenuation(float lambda, float hole_len, int holes) {
    float S1 = 20 * log10f(lambda / (2 * hole_len));
    float S2 = - 20 * log10f(sqrt(holes));
    return S1 + S2;
}

bool wrong_params(const hex_params& params) {
    auto diag_dist = std::sqrtf(params.h_dist * params.h_dist / 4 + params.v_dist * params.v_dist);
    auto diag_angle = atan(2 * params.v_dist / params.h_dist);
    diag_dist -= params.hole_len(params.theta0 + diag_angle) / 2;
    diag_dist -= params.hole_len(params.theta1 + diag_angle) / 2;

    return (
        params.side < min_hex_side ||
        params.side > max_hex_side ||
        params.h_dist - params.hole_len(params.theta0) < min_hex_dist ||
        params.h_dist - params.hole_len(params.theta1) < min_hex_dist ||
        diag_dist < min_hex_dist
    );
}

float hole_area(const hex_params& params, const std::tuple<int, int, int>& columns_and_rows) {
    if(wrong_params(params)) {
        return 0;
    }

    float hex_area = params.side * params.side * M_SQRT3 / 4;
    
    int columns_odd = std::get<0>(columns_and_rows);
    int columns_even = std::get<1>(columns_and_rows);
    int rows = std::get<2>(columns_and_rows);
    
    int hexes_row_even = std::floor((float)rows / 2) * columns_even;
    int hexes_row_odd = std::ceil((float)rows / 2) * columns_odd;

    float holes_area = hex_area * (hexes_row_even + hexes_row_odd);

    return holes_area;
}

float hole_area_v(const vector5f& vec) {
    hex_params hp;
    hp.v = vec;
    auto cols_and_rows = calculate_columns_and_rows(hp);
    return hole_area(hp, cols_and_rows);
}

float attenuation(const hex_params& params) {
    if(wrong_params(params)) {
        return 0;
    }

    auto columns_and_rows = calculate_columns_and_rows(params);
    auto columns_odd = std::get<0>(columns_and_rows);
    auto columns_even = std::get<1>(columns_and_rows);
    auto rows = std::get<2>(columns_and_rows);
    
    // horizontal
    float hex_len_horiz = params.hole_len(params.theta0);
    float att_horiz = calculate_row_attenuation(lambda, hex_len_horiz, columns_odd);

    //diagonal (oh boy...)
    float angle = atan(2 * params.h_dist / params.v_dist);
    float hex_len_diag = std::max(params.hole_len(params.theta0 + angle), params.hole_len(params.theta1 + angle));
    float att_diag = calculate_row_attenuation(lambda, hex_len_diag, std::min(columns_odd, rows));

    // we take worst case scenario
    return std::min(att_horiz, att_diag);
}

float attenuation_v(const vector5f& params) {
    hex_params hp;
    hp.v = params;
    return attenuation(hp);
}

std::string hex_info(const hex_params& hp) {
    std::string res = to_string(hp);
    res += "\nattenuation: " + std::to_string(attenuation(hp));
    res += "\narea: " + std::to_string(hole_area_v(hp.v)) + '\n';
    return res;
}

hex_params maximize_area(const vector5f& seed, float dx, int steps) {
    vector5f x = seed;
    int CUTOFF = 100000;
    while(steps && CUTOFF--) {
        // std::cout << hex_info(hex_params(x));
        auto att = attenuation_v(x);
        
        vector5f grad;
        if(att < required_db) 
            grad = grad5f(attenuation_v, x, dx);
        else {
            grad = grad5f(hole_area_v, x, dx);
            steps--;
        }

        auto grad_normal = grad * (1 / grad.abs());
        x += grad_normal * dx;

        hex_params hp;
        hp.v = x;
        if(wrong_params(hp)) {
            x += grad_normal * (-dx);
            break;
        } 
    }

    hex_params hp;
    hp.v = x;
    return hp;
}