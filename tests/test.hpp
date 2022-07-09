#include <iostream>
#include <string_view>

namespace my
{

template <typename A, typename B> bool check_eq(const A& a, const B& b, std::string_view file, int line)
{
    if (a != b) {
        std::cerr << "Assertion failed: " << file << ":" << line << '\n';
        std::cerr << "| Left:  " << a << '\n';
        std::cerr << "| Right: " << b << '\n';
        std::cerr << std::endl;
        return false;
    }
    return true;
}

#define ASSERT_EQ(a, b) assert(my::check_eq((a), (b), __FILE__, __LINE__))
#define CHECK_EQ(a, b)  my::check_eq((a), (b), __FILE__, __LINE__)

}