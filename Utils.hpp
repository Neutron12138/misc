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

} // namespace vm

#endif
