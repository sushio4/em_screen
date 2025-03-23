#include "../core.hpp"
#include "testutils.hpp"

void test_hole_len() {
    hex_params hp;
    hp.side = 1;
    float theta = M_PI / 6;
    
    ASSERT_ALMOST_EQ(hp.hole_len(theta), 2);

    theta = 0;
    ASSERT_ALMOST_EQ(hp.hole_len(theta), M_SQRT3);
}

void test_hole_area() {
    hex_params hp;
    hp.side = 1;
    
    auto area = hole_area(hp, {2, 2, 2});
    auto hex_ar = 1 * M_SQRT3 / 4;
    ASSERT_ALMOST_EQ(area, hex_ar * 4);

    area = hole_area(hp, {3, 2, 3});
    /*    []  []  []
     *      []  [] 
     *    []  []  []
     */
    ASSERT_ALMOST_EQ(area, hex_ar * 8);
}

void test_rows_and_columns() {
    hex_params hp;
    hp.side = 0.1;
    hp.h_dist = 0.2;
    hp.v_dist = 0.1;
    hp.theta0 = hp.theta1 = 0;

    int expected_columns0 = 2;
    int expected_columns1 = 1;
    int expected_rows = 3;

    auto col_rows = calculate_columns_and_rows(hp);
    int columns0 = std::get<0>(col_rows);
    int columns1 = std::get<1>(col_rows);
    int rows = std::get<2>(col_rows);

    ASSERT_EQ(expected_columns0, columns0);
    ASSERT_EQ(expected_columns1, columns1);
    ASSERT_EQ(expected_rows, rows);

    hp.side = board_side;
    std::tuple expected{0,0,0};
    ASSERT_EQ_NOPRINT(expected, calculate_columns_and_rows(hp));
}

void test_attenuation() {
    hex_params hp;
    hp.side = 0.05;
    hp.h_dist = 0.15;
    hp.v_dist = 1;
    hp.theta0 = 0;
    
    auto hole_len = hp.hole_len(0);
    auto cols_and_rows = calculate_columns_and_rows(hp);

    auto expected = calculate_row_attenuation(lambda, hole_len, std::get<0>(cols_and_rows));
    ASSERT_EQ(expected, attenuation(hp));
}

void test_core() {
    UT_CASE();
    test_hole_len();
    test_hole_area();
    test_rows_and_columns();
    test_attenuation();
}
