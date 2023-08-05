#ifndef UTILS
#define UTILS

#include <sstream>

namespace vm
{
    template <typename T>
    std::string to_string(const T &value)
    {
        std::stringstream sstr;
        sstr << value;
        return sstr.str();
    }

    template <>
    std::string to_string(const std::nullptr_t &)
    {
        return "null";
    }

    template <>
    std::string to_string(const bool &value)
    {
        return value ? "true" : "false";
    }

    template <typename T>
    T remainder(const T &a, const T &b);

    template <>
    std::int64_t remainder(const std::int64_t &a, const std::int64_t &b)
    {
        return a % b;
    }

    template <>
    double remainder(const double &a, const double &b)
    {
        // 求商
        std::int64_t q = static_cast<std::int64_t>(a / b);
        // 求积
        double p = static_cast<double>(q) * b;
        // 求差
        return a - p;
    }

} // namespace vm

#endif
