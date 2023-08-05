#ifndef EXECUTABLE
#define EXECUTABLE

#include "Utils.hpp"
#include "Executer.hpp"

namespace vm
{
    // ----------起始段----------

    struct BinaryHeader
    {
        static constexpr Byte text[2] = {'v', 'm'};
        std::size_t m_vars_count = 0;
        std::size_t m_insts_count = 0;
        Int m_curr_inst = 0;

        BinaryHeader() {}
        BinaryHeader(std::size_t vars_count, std::size_t insts_count, Int curr_inst)
            : m_vars_count(vars_count),
              m_insts_count(insts_count),
              m_curr_inst(curr_inst) {}
    };

    std::ostream &operator<<(std::ostream &os, const BinaryHeader &header)
    {
        os << "["
           << header.m_vars_count
           << ","
           << header.m_insts_count
           << ","
           << header.m_curr_inst
           << "]";
        return os;
    }

    template <>
    BinaryHeader binary_read(std::istream &is)
    {
        BinaryHeader result;

        Byte t[2];
        is.read(reinterpret_cast<char *>(t), sizeof(t));
        if (t[0] != BinaryHeader::text[0] || t[1] != BinaryHeader::text[1])
            throw VM_UTILS_ERROR(
                "header error",
                to_string("header text: \"", reinterpret_cast<char *>(t), "\""));

        result.m_vars_count = binary_read<std::size_t>(is);
        result.m_insts_count = binary_read<std::size_t>(is);
        result.m_curr_inst = binary_read<Int>(is);

        return result;
    }

    template <>
    void binary_write(std::ostream &os, const BinaryHeader &value)
    {
        os.write(reinterpret_cast<const char *>(BinaryHeader::text), sizeof(BinaryHeader::text));
        binary_write(os, value.m_vars_count);
        binary_write(os, value.m_insts_count);
        binary_write(os, value.m_curr_inst);
    }

    // ----------结尾段----------

    struct BinaryEnd
    {
        static constexpr Byte text[3] = {'e', 'n', 'd'};
    };

    template <>
    BinaryEnd binary_read(std::istream &is)
    {
        Byte t[3];
        is.read(reinterpret_cast<char *>(t), sizeof(t));
        if (t[0] != BinaryEnd::text[0] || t[1] != BinaryEnd::text[1] || t[2] != BinaryEnd::text[2])
            throw VM_UTILS_ERROR(
                "header error",
                to_string("end text: \"", reinterpret_cast<char *>(t), "\""));

        return {};
    }

    template <>
    void binary_write(std::ostream &os, const BinaryEnd &)
    {
        os.write(reinterpret_cast<const char *>(BinaryEnd::text), sizeof(BinaryEnd::text));
    }

    // ----------Value的二进制读写----------

    template <>
    Value binary_read(std::istream &is)
    {
        Int type = binary_read<Int>(is);

        switch (type)
        {
        case Value::NIL:
            return Value(nullptr);

        case Value::BOOL:
            return Value(binary_read<Bool>(is));

        case Value::BYTE:
            return Value(binary_read<Byte>(is));

        case Value::INT:
            return Value(binary_read<Int>(is));

        case Value::FLOAT:
            return Value(binary_read<Float>(is));

        case Value::STRING:
            return Value(binary_read<String>(is));

        default:
            throw VM_UTILS_TYPE_ERROR(type);
        }
    }

    template <>
    void binary_write(std::ostream &os, const Value &val)
    {
        binary_write(os, val.m_type);

        switch (val.m_type)
        {
        case Value::NIL:
            break;

        case Value::BOOL:
            binary_write(os, val.m_b);
            break;

        case Value::BYTE:
            binary_write(os, val.m_ub);
            break;

        case Value::INT:
            binary_write(os, val.m_i);
            break;

        case Value::FLOAT:
            binary_write(os, val.m_f);
            break;

        case Value::STRING:
            binary_write(os, val.m_str);
            break;

        default:
            throw VM_UTILS_TYPE_ERROR(val.m_type);
        }
    }

    // ----------可执行文件----------

    class Executable
    {
    public:
        VarList m_vars = {};
        InstList m_insts = {};
        Int m_curr_inst = 0;

    public:
        Executable() {}
        Executable(const Executer &exe)
            : m_vars(exe.m_vars),
              m_insts(exe.m_insts),
              m_curr_inst(exe.m_curr_inst) {}

    public:
        operator Executer() const
        {
            return {m_vars, m_insts, m_curr_inst};
        }

    public:
        std::size_t add_val(const Value &val)
        {
            return add_ref(make_ref(val));
        }

        std::size_t add_val(const Reference &ref)
        {
            return add_val(ref->copy());
        }

        std::size_t add_ref(const Reference &ref)
        {
            m_vars.push_back(ref);
            return m_vars.size() - 1;
        }

        std::size_t add_inst(const Instruction &inst)
        {
            m_insts.push_back(inst);
            return m_insts.size() - 1;
        }

        std::size_t add_func(const InstList &insts)
        {
            for (auto iter = insts.cbegin(); iter != insts.cend(); iter++)
                m_insts.push_back(*iter);
            return m_insts.size() - insts.size();
        }
    };

    template <>
    Executable binary_read(std::istream &is)
    {
        Executable result;

        BinaryHeader header = binary_read<BinaryHeader>(is);
        result.m_curr_inst = header.m_curr_inst;

        for (std::size_t i = 0; i < header.m_vars_count; i++)
            result.add_val(binary_read<Value>(is));

        for (std::size_t i = 0; i < header.m_insts_count; i++)
            result.add_inst(binary_read<Instruction>(is));

        BinaryEnd end = binary_read<BinaryEnd>(is);

        return result;
    }

    template <>
    void binary_write(std::ostream &os, const Executable &value)
    {
        BinaryHeader header = {value.m_vars.size(), value.m_insts.size(), value.m_curr_inst};
        binary_write(os, header);

        for (auto iter = value.m_vars.cbegin(); iter != value.m_vars.cend(); iter++)
            binary_write(os, **iter);

        for (auto iter = value.m_insts.cbegin(); iter != value.m_insts.cend(); iter++)
            binary_write(os, *iter);

        binary_write(os, BinaryEnd());
    }

} // namespace vm

#endif
