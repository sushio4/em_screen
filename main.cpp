#include <iostream>
#include <vector>
#include "core.hpp"

int main(int argc, char* argv[]) {
    auto grid = create_grid(10);
    
    int steps = 100;
    float dx = 0.001f;

    float max_area = 0;
    hex_params res;

    for(const auto& seed : grid) {
        auto x = maximize_area(seed.v, dx, steps);

        if(attenuation(x) < required_db) continue;
        
        float area = hole_area_v(x.v);
        if(area > max_area) {
            max_area = area;
            res = x;
        }
    }

    std::cout << "Results:\nhex hole side length [cm]: " << res.side * 100
    << "\nhex hole greatest diameter [cm]: " << res.side * 100 * M_SQRT3
    << "\nodd row hexes angle [rad]: " << res.theta0
    << "\neven row hexes angle [rad]: " << res.theta1
    << "\nhorizontal hex distance (center to center) [cm]: " << res.h_dist * 100
    << "\nvertical hex distance (center to center) [cm]: " << res.v_dist * 100
    << "\n\nattenuation [dB]: " << attenuation(res) 
    << "\ntotal hole area [cm^2]: " << hole_area_v(res.v) * 100000
    << "\nhole to board area [%]: " << hole_area_v(res.v) * 100 / (0.5 * 0.5)
    << "\n";
    
}