#pragma once
#include <iostream>

#define TOLERANCE 0.001

#define ASSERT(condition) {if(!(condition)) { std::cout << __FILE__ << ":" << __LINE__ <<  " - ERROR: " << #condition << " not true\n"; } }
#define ASSERT_EQ(lhs, rhs) { auto lv = (lhs); auto rv = (rhs); if((lv) != (rv)) { std::cout << __FILE__ << ":" << __LINE__ <<  " - ERROR: " << #lhs << " and " << #rhs << " are not equal\nlhs = " << lv << "\nrhs = " << rv << '\n'; } }
#define ASSERT_ALMOST_EQ(lhs, rhs) { auto lv = (lhs); auto rv = (rhs); if(std::fabs((lv) - (rv)) > TOLERANCE) { std::cout << __FILE__ << ":" << __LINE__ <<  " - ERROR: " << #lhs << " and " << #rhs << " are not even close\nlhs = " << lv << "\nrhs = " << rv << '\n'; } }

#define UT_CASE() { std::cout << "Running UTs in " << __FILE__ << '\n'; }

#define ASSERT_EQ_NOPRINT(lhs, rhs) { if((lhs) != (rhs)) { std::cout << __FILE__ << ":" << __LINE__ <<  " - ERROR: " << #lhs << " and " << #rhs << " are not equal\n"; } }
