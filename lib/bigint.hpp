#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <limits>
#include <numeric>
#include <ostream>
#include <vector>

namespace my
{

template <typename T> class big_uint; // forward declaration

namespace internal
{
template <typename D> big_uint<D> most_significant_n(big_uint<D> input, std::size_t n)
{
    return big_uint<D>(std::next(input.digits_.begin(), n), input.digits_.end());
}

template <typename D> big_uint<D> least_significant_n(big_uint<D> input, std::size_t n)
{
    return big_uint<D>(input.digits_.begin(), std::next(input.digits_.begin(), n));
}

template <typename T> constexpr std::size_t hi_mask()
{
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    T t = ~T{0};
    return t << (std::numeric_limits<T>::digits / 2);
}

template <typename T> constexpr std::size_t lo_mask()
{
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return ~hi_mask<T>();
}

template <typename T> T lo(T x)
{
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return x & lo_mask<T>();
}

template <typename T> T hi(T x)
{
    static_assert(std::is_unsigned<T>::value, "T must be unsigned");
    return x >> (std::numeric_limits<T>::digits / 2);
}

template <typename D> std::tuple<D, D> sum_digits(D L, D R)
{
    constexpr auto max = std::numeric_limits<D>::max();
    if (L > max - R) { // overflow
        return std::make_tuple(L + R, 1);
    }
    return std::make_tuple(L + R, 0);
}

template <typename D> std::tuple<D, D> prod_digits(D lhs, D rhs)
{
    if (lhs == 0 || rhs == 0)
        return std::make_tuple(0, 0);

    constexpr auto base = std::numeric_limits<D>::max();

    auto max_multiple = (base - 1) / rhs;
    if (lhs < max_multiple) {
        return std::make_tuple(lhs * rhs, 0);
    }

    D p = hi(lhs);
    D q = lo(lhs);
    D x = hi(rhs);
    D y = lo(rhs);

    D overflow = p * x;
    D result = q * y;

    auto [r, o] = sum_digits<D>(p * y, q * x);
    D u = hi(r);
    D l = lo(r) << (std::numeric_limits<D>::digits / 2);

    overflow += u + (o << (std::numeric_limits<D>::digits / 2));

    auto [rho, omega] = sum_digits(l, result);

    overflow += omega;
    result = rho;

    return std::make_tuple(result, overflow);
}
} // namespace internal

template <typename Dtype> class big_uint
{
  public:
    using D = Dtype;

    static_assert(std::is_unsigned<D>::value, "D must be unsigned");

    constexpr big_uint() noexcept : digits_{}
    {
    }

    big_uint(D d) : digits_(1, d){};

    template <typename ForwardIterator = typename std::vector<D>::const_iterator>
    big_uint(ForwardIterator begin, ForwardIterator end) : digits_(begin, end)
    {
    }

    big_uint(std::string_view sv, std::uint8_t base = 10)
    {
        digits_.push_back(0);
        for (const char c : sv) {
            D d = c - '0';
            if (d < 0 || d > 9) {
                throw std::invalid_argument("invalid character: " + std::string(1, c));
            }
            append_digit(d, base);
            trim();
        }
    }

    void append_digit(D a, std::uint8_t base = 10)
    {
        for (auto& curr_digit : digits_) {
            auto [shifted, ovf1] = internal::prod_digits<D>(curr_digit, base);
            auto [c, ovf2] = internal::sum_digits<D>(shifted, a); // Overflow impossible
            curr_digit = c;
            a = ovf1 + ovf2;
        }
        if (a != 0) {
            digits_.push_back(a);
        }
    }

    bool operator<(big_uint const& other) const
    {
        return compare(other) == comparison::LT;
    }
    bool operator>(big_uint const& other) const
    {
        return compare(other) == comparison::GT;
    }
    bool operator==(big_uint const& other) const
    {
        return compare(other) == comparison::EQ;
    }
    bool operator!=(big_uint const& other) const
    {
        return !(*this == other);
    }
    bool operator<=(big_uint const& other) const
    {
        return !(*this > other);
    }
    bool operator>=(big_uint const& other) const
    {
        return !(*this < other);
    }

    std::vector<D> digits_ = {};
    static constexpr auto base = std::numeric_limits<D>::max();

    std::string to_string() const
    {
        if (digits_.empty()) {
            return "0x0";
        }

        std::stringstream ss;
        constexpr auto hex_digits = std::numeric_limits<D>::digits / 4;
        std::for_each(digits_.rbegin(), digits_.rend(), [&](auto d) {
            ss << std::hex << std::setw(hex_digits) << std::setfill('0') << static_cast<std::intmax_t>(d);
        });

        const auto str = ss.str();
        auto first_dig = std::find_if(str.cbegin(), str.cend(), [](auto c) { return c != '0'; });

        return "0x" + std::string(first_dig, str.cend());
    }

  private:
    enum class comparison
    {
        LT,
        EQ,
        GT
    };

    void trim()
    {
        auto last_nonzero = std::find_if(digits_.rbegin(), digits_.rend(), [](D d) { return d != 0; }).base();
        digits_.erase(last_nonzero, digits_.end());
    }

    comparison compare(big_uint const& other) const
    {
        if (digits_.size() != other.digits_.size()) {
            return digits_.size() < other.digits_.size() ? comparison::LT : comparison::GT;
        }

        auto mm = std::mismatch(digits_.rbegin(), digits_.rend(), other.digits_.rbegin());
        if (mm.first == digits_.rend()) {
            return comparison::EQ;
        }
        return *mm.first < *mm.second ? comparison::LT : comparison::GT;
    }

  public:
    friend big_uint operator+(const big_uint& lhs, const big_uint& rhs)
    {
        auto result = big_uint{};

        const auto longest = std::max(lhs.digits_.size(), rhs.digits_.size());
        const_cast<big_uint&>(lhs).digits_.resize(longest, 0);
        const_cast<big_uint&>(rhs).digits_.resize(longest, 0);

        const_cast<big_uint&>(result).digits_.reserve(longest + 1);
        const_cast<big_uint&>(result).digits_.push_back(0);

        const auto reduction = [&](auto acc, auto folded) {
            auto& [local, ovf1] = folded;
            auto [old_digit, ovf2] = internal::sum_digits(local, acc.back());
            acc.back() = old_digit;
            const auto new_digit = ovf1 + ovf2;

            acc.push_back(new_digit);
            return acc;
        };

        result.digits_ = std::inner_product(lhs.digits_.cbegin(), lhs.digits_.cend(), rhs.digits_.cbegin(),
                                            result.digits_, reduction, internal::sum_digits<D>);

        const_cast<big_uint&>(lhs).trim();
        const_cast<big_uint&>(rhs).trim();
        result.trim();

        return result;
    }

    friend /* Underflow undefined */
      big_uint
      operator-(const big_uint& lhs, big_uint rhs)
    {
        // Two's complement
        rhs.digits_.resize(lhs.digits_.size(), 0u);
        std::for_each(rhs.digits_.begin(), rhs.digits_.end(), [](auto& d) { d = ~d; });
        rhs.digits_.push_back(1);
        auto one = big_uint{1u};
        const auto rhs_2c = rhs + one;

        auto result = lhs + rhs_2c;
        result.digits_.pop_back();
        result.trim();
        return result;
    }

    friend big_uint operator*(const big_uint& lhs, const big_uint& rhs)
    {
        if (lhs.digits_.empty() || rhs.digits_.empty()) {
            return big_uint{};
        }

        big_uint result;
        if (lhs.digits_.size() == 1 && rhs.digits_.size() == 1) {
            auto [d0, d1] = internal::prod_digits(lhs.digits_.front(), rhs.digits_.front());
            result.digits_.push_back(d0);
            if (d1 != 0)
                result.digits_.push_back(d1);
            return result;
        }
        const std::size_t n = std::max(lhs.digits_.size(), rhs.digits_.size());
        const std::size_t h = n / 2;

        // Kratsuba multiplication
        const big_uint a = internal::most_significant_n(lhs, h);
        const big_uint b = internal::least_significant_n(lhs, h);

        const big_uint c = internal::most_significant_n(rhs, h);
        const big_uint d = internal::least_significant_n(rhs, h);

        const big_uint ac = a * c;
        big_uint bd = b * d;

        const big_uint ab_cd = (a + b) * (c + d);

        big_uint middle = ab_cd - (ac + bd);

        // result = ac * 2^(n*2) + middle * 2^n + bd
        result.digits_.swap(bd.digits_);
        result.digits_.reserve(2 * n + 1);
        result.digits_.resize(2 * h, 0);
        result.digits_.insert(result.digits_.end(), ac.digits_.begin(), ac.digits_.end());

        middle.digits_.reserve(result.digits_.size());
        const auto len = middle.digits_.size();
        middle.digits_.resize(middle.digits_.size() + h, 0);
        std::rotate(middle.digits_.begin(), std::next(middle.digits_.begin(), len), middle.digits_.end());

        return result + middle;
    }

    friend std::ostream& operator<<(std::ostream& os, const big_uint& longint)
    {
        return os << longint.to_string();
    }
};

} // namespace my