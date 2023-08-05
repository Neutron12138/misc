#ifndef UTILS
#define UTILS

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

#define VM_UTILS_ERROR(type, info) \
    vm::error(__FILE__, __LINE__, __FUNCTION__, type, info)
#define VM_UTILS_TYPE_ERROR(type) \
    vm::type_error(__FILE__, __LINE__, __FUNCTION__, type)
#define VM_UTILS_LIST_ERROR(index, size) \
    vm::list_error(__FILE__, __LINE__, __FUNCTION__, index, size)
#define VM_UTILS_INSTRUCTION_ERROR(opc, ope) \
    vm::instruction_error(__FILE__, __LINE__, __FUNCTION__, opc, ope)
#define VM_UTILS_ARGUMENT_ERROR(arg) \
    vm::argument_error(__FILE__, __LINE__, __FUNCTION__, arg)
#define VM_UTILS_FILE_OPEN_ERROR(filename) \
    vm::file_open_error(__FILE__, __LINE__, __FUNCTION__, filename)

namespace vm
{
    template <typename T>
    std::string to_string(const T &value)
    {
        std::stringstream sstr;
        sstr << value;
        return sstr.str();
    }

    template <typename T, typename... Ts>
    std::string to_string(const T &arg, const Ts &...args)
    {
        return to_string(arg) + to_string(args...);
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

    std::string error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        const std::string &type = "",
        const std::string &info = "")
    {
        return to_string(
            "error type: \"", type,
            "\", at file: \"", file,
            "\", line: ", line,
            ", function: \"", func,
            "\", info: { ", info,
            " }");
    }

    std::string type_error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        std::int64_t type)
    {
        return error(
            file, line, func, "type error",
            to_string("value type: ", type));
    }

    std::string list_error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        std::size_t index,
        std::size_t size)
    {
        return error(
            file, line, func, "list error",
            to_string("index: ", index, ", size: ", size));
    }

    std::string instruction_error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        std::int64_t opc,
        std::int64_t ope)
    {
        return error(
            file, line, func, "instruction error",
            to_string("opcode: ", opc, ", operand: ", ope));
    }

    std::string argument_error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        std::int64_t arg)
    {
        return error(
            file, line, func, "argument error",
            to_string("argument: ", arg));
    }

    std::string file_open_error(
        const std::string &file,
        std::size_t line,
        const std::string &func,
        const std::string &filename)
    {
        return error(
            file, line, func, "file open error",
            to_string("filename: \"", filename, "\""));
    }

    template <typename T>
    T binary_read(std::istream &is)
    {
        T result;
        is.read(reinterpret_cast<char *>(&result), sizeof(T));
        return result;
    }

    template <typename T>
    void binary_write(std::ostream &os, const T &value)
    {
        os.write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    template <typename T, typename... Ts>
    void binary_write(std::ostream &os, const T &arg, const Ts &...args)
    {
        binary_write(os, arg);
        binary_write(os, args...);
    }

    template <>
    std::string binary_read(std::istream &is)
    {
        std::stringstream sstr;
        char ch;

        while ((ch = binary_read<char>(is)) != '\0')
            sstr << ch;

        return sstr.str();
    }

    template <>
    void binary_write(std::ostream &os, const std::string &value)
    {
        for (auto iter = value.cbegin(); iter != value.cend(); iter++)
            binary_write(os, *iter);
        binary_write(os, '\0');
    }

    template <typename T>
    T binary_read_from_file(const std::string &filename)
    {
        std::ifstream fin;
        fin.open(filename);
        if (fin.fail())
        {
            fin.close();
            throw VM_UTILS_FILE_OPEN_ERROR(filename);
        }

        T result = binary_read<T>(fin);
        fin.close();
        return result;
    }

    template <typename... Ts>
    void binary_write_to_file(const std::string &filename, const Ts &...args)
    {
        std::ofstream fout;
        fout.open(filename);
        if (fout.fail())
        {
            fout.close();
            throw VM_UTILS_FILE_OPEN_ERROR(filename);
        }

        binary_write(fout, args...);
        fout.close();
    }

} // namespace vm

#endif
