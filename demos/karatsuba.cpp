#include <assert.h>
#include <iostream>
#include <sstream>

#include "include/bigint.hpp"

int main()
{
    std::cout << "Solving:" << std::endl;
    const my::big_uint<std::uint8_t> A{"3141592653589793238462643383279502884197169399375105820974944592"};
    std::cout << std::setw(110) << A << '\n';

    const my::big_uint<std::uint8_t> B{"2718281828459045235360287471352662497757247093699959574966967627"};
    std::cout << 'x' << std::setw(109) << B << '\n';

    for (std::size_t i = 0; i < 110; ++i)
        std::cout << "-";

    std::cout << '\n' << std::setw(110) << A * B << std::endl;
}