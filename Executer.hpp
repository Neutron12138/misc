#ifndef EXECUTER
#define EXECUTER

#include <cstdint>
#include <array>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <memory.h>
#include <sstream>
#include "Utils.hpp"

#define VM_VALUE_IF(type, TYPE)

#define VM_VALUE_TYPE(ltype, type, TYPE, var)                     \
    void setup_##ltype()                                          \
    {                                                             \
        static const type src = type();                           \
        memcpy(this, &src, sizeof(src));                          \
        m_type = TYPE;                                            \
    }                                                             \
                                                                  \
    Value(const type &val) { *this = val; }                       \
    Value &operator=(const type &val)                             \
    {                                                             \
        setup_##ltype();                                          \
        var = val;                                                \
        return *this;                                             \
    }                                                             \
                                                                  \
    template <typename FuncType, typename... ArgsType>            \
    void if_##ltype(FuncType func, const ArgsType &...args)       \
    {                                                             \
        if (m_type == TYPE)                                       \
            func(*this, std::forward<ArgsType>(args)...);         \
    }                                                             \
                                                                  \
    template <typename FuncType, typename... ArgsType>            \
    void if_##ltype(FuncType func, const ArgsType &...args) const \
    {                                                             \
        if (m_type == TYPE)                                       \
            func(*this, std::forward<ArgsType>(args)...);         \
    }

namespace vm
{
    // ----------基本类型----------

    using Nil = std::nullptr_t;
    using Bool = bool;
    using Byte = unsigned char;
    using Int = std::int64_t;
    using Float = double;
    using String = std::string;

    //----------指令----------

    struct Instruction
    {
        enum OpCode : Int
        {
            NOP = 0,

            MOV,
            PUSH,
            POP,
            SYSCALL,
            CALL,

            ADD,
            SUB,
            MUL,
            DIV,
            MOD,

            JMP,
            JE,
            JNE,
            JL,
            JLE,
            JG,
            JGE,
        };

        OpCode m_opcode = NOP;
        std::array<Int, 2> m_operands = {0, 0};

        Instruction(OpCode opc = NOP, const std::array<Int, 2> &ope = {0, 0})
            : m_opcode(opc), m_operands(ope) {}
    };

    std::ostream &operator<<(std::ostream &os, const Instruction &inst)
    {
        os << "["
           << inst.m_opcode
           << ","
           << inst.m_operands[0]
           << ","
           << inst.m_operands[1]
           << "]";
        return os;
    }

    using InstList = std::vector<Instruction>;

    // ----------变量----------

    struct Value
    {
        enum Type
        {
            NIL = 0,
            BOOL,
            BYTE,
            INT,
            FLOAT,
            STRING,
        };

        Type m_type = NIL;
        union
        {
            Nil m_n;
            Bool m_b;
            Byte m_ub;
            Int m_i;
            Float m_f;
            String m_str;
        };

        Value() : m_type(NIL), m_n(nullptr) {}
        Value(const Value &from) { *this = from; }
        ~Value() {}

        VM_VALUE_TYPE(nil, Nil, NIL, m_n)
        VM_VALUE_TYPE(bool, Bool, BOOL, m_b)
        VM_VALUE_TYPE(byte, Byte, BYTE, m_ub)
        VM_VALUE_TYPE(int, Int, INT, m_i)
        VM_VALUE_TYPE(float, Float, FLOAT, m_f)
        VM_VALUE_TYPE(string, String, STRING, m_str)

        Value &operator=(const Value &from)
        {
            switch (from.m_type)
            {
            case Value::NIL:
                *this = from.m_n;
                break;

            case Value::BOOL:
                *this = from.m_b;
                break;

            case Value::BYTE:
                *this = from.m_ub;
                break;

            case Value::INT:
                *this = from.m_i;
                break;

            case Value::FLOAT:
                *this = from.m_f;
                break;

            case Value::STRING:
                *this = from.m_str;
                break;

            default:
                throw "type error";
                break;
            }

            return *this;
        }

        Value copy() const
        {
            return static_cast<Value>(*this);
        }
    };

    template <typename T>
    T value_cast(const Value &val)
    {

        switch (val.m_type)
        {
        case Value::NIL:
            return static_cast<T>(val.m_n);

        case Value::BOOL:
            return static_cast<T>(val.m_b);

        case Value::BYTE:
            return static_cast<T>(val.m_ub);

        case Value::INT:
            return static_cast<T>(val.m_i);

        case Value::FLOAT:
            return static_cast<T>(val.m_f);

        case Value::STRING:
            return static_cast<T>(val.m_str);

        default:
            throw "type error";
        }
    }

    template <>
    Nil value_cast(const Value &val)
    {
        return nullptr;
    }

    template <>
    Bool value_cast(const Value &val)
    {
        switch (val.m_type)
        {
        case Value::NIL:
            return false;

        case Value::BOOL:
            return val.m_b;

        case Value::BYTE:
            return static_cast<Bool>(val.m_ub);

        case Value::INT:
            return static_cast<Bool>(val.m_i);

        case Value::FLOAT:
            return static_cast<Bool>(val.m_f);

        case Value::STRING:
            return !val.m_str.empty();

        default:
            throw "type error";
        }
    }

    template <>
    Byte value_cast(const Value &val)
    {
        switch (val.m_type)
        {
        case Value::NIL:
            return 0;

        case Value::BOOL:
            return static_cast<Byte>(val.m_ub);

        case Value::BYTE:
            return static_cast<Byte>(val.m_ub);

        case Value::INT:
            return static_cast<Byte>(val.m_i);

        case Value::FLOAT:
            return static_cast<Byte>(val.m_f);

        case Value::STRING:
            return !val.m_str.empty();

        default:
            throw "type error";
        }
    }

    template <>
    Int value_cast(const Value &val)
    {
        switch (val.m_type)
        {
        case Value::NIL:
            return 0;

        case Value::BOOL:
            return static_cast<Int>(val.m_b);

        case Value::BYTE:
            return static_cast<Int>(val.m_ub);

        case Value::INT:
            return val.m_i;

        case Value::FLOAT:
            return static_cast<Int>(val.m_f);

        case Value::STRING:
            return std::stoll(val.m_str);

        default:
            throw "type error";
        }
    }

    template <>
    Float value_cast(const Value &val)
    {
        switch (val.m_type)
        {
        case Value::NIL:
            return 0;

        case Value::BOOL:
            return static_cast<Float>(val.m_b);

        case Value::BYTE:
            return static_cast<Float>(val.m_ub);

        case Value::INT:
            return static_cast<Float>(val.m_i);

        case Value::FLOAT:
            return val.m_f;

        case Value::STRING:
            return std::stod(val.m_str);

        default:
            throw "type error";
        }
    }

    template <>
    String value_cast(const Value &val)
    {
        switch (val.m_type)
        {
        case Value::NIL:
            return "null";

        case Value::BOOL:
            return val.m_b ? "true" : "false";

        case Value::BYTE:
            return to_string(val.m_ub);

        case Value::INT:
            return to_string(val.m_i);

        case Value::FLOAT:
            return to_string(val.m_f);

        case Value::STRING:
            return val.m_str;

        default:
            throw "type error";
        }
    }

    std::ostream &operator<<(std::ostream &os, const Value &val)
    {
        os << "["
           << val.m_type
           << ","
           << value_cast<String>(val)
           << "]";

        return os;
    }

    using Reference = std::shared_ptr<Value>;
    using VarList = std::vector<Reference>;

    Reference make_ref(const Value &val)
    {
        return std::make_shared<Value>(val);
    }

    // ----------执行器----------

    class Executer
    {
    public:
        VarList m_vars;
        InstList m_insts;
        std::size_t m_curr_inst = 0;

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

        Reference remove_var()
        {
            const Reference &var = m_vars.at(m_vars.size() - 1);
            m_vars.pop_back();
            return var;
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

    public:
        void execute_push(Int arg0)
        {
            m_vars.push_back(m_vars.at(arg0));
        }

        void execute_add(Int arg0)
        {
            const Value &val1 = *remove_var();
            const Value &val0 = *remove_var();
            Value result;

            switch (arg0)
            {
            case Value::INT:
            {
                Int num0 = value_cast<Int>(val0);
                Int num1 = value_cast<Int>(val1);
                result = num0 + num1;
            }
            break;

            case Value::FLOAT:
            {
                Int num0 = value_cast<Float>(val0);
                Int num1 = value_cast<Float>(val1);
                result = num0 + num1;
            }
            break;

            default:
                throw "type error";
                break;
            }

            add_val(result);
        }
    };

} // namespace vm

#endif
