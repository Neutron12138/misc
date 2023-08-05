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
    using Byte = signed char;
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

            CAST,
            ADD,
            SUB,
            MUL,
            DIV,
            MOD,
            CMP,

            JMP,
            JE,
            JNE,
            JL,
            JLE,
            JG,
            JGE,

            RESUME,
            PAUSE,
            ABORT,

            COUNT,
        };

        OpCode m_opcode = NOP;
        Int m_operand = 0;

        Instruction(OpCode opc = NOP, Int ope = 0)
            : m_opcode(opc), m_operand(ope) {}
    };

    std::ostream &operator<<(std::ostream &os, const Instruction &inst)
    {
        os << "["
           << inst.m_opcode
           << ","
           << inst.m_operand
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
        enum State : Int
        {
            RUNNING = 0,
            PAUSED,
            ABORTED,
        };

    public:
        VarList m_vars;
        InstList m_insts;
        Int m_curr_inst = 0;
        State m_state = PAUSED;

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
        void run(Int index = 0)
        {
            add_val(m_curr_inst);
            m_curr_inst = index;
            m_state = RUNNING;

            try
            {
                while (m_state == RUNNING)
                {

                    const Instruction &inst = m_insts.at(m_curr_inst);
                    run_inst(inst);
                    m_curr_inst++;
                }
            }
            catch (const char *e)
            {
                std::cout << "[" << e << "]" << std::endl;
                m_state = ABORTED;
            }
            catch (const std::exception &e)
            {
                std::cout << "[" << e.what() << "]" << std::endl;
                m_state = ABORTED;
            }
        }

        void resume()
        {
            run(m_curr_inst);
        }

        void pause()
        {
            m_state = PAUSED;
        }

        void abort()
        {
            m_state = ABORTED;
        }

        void run_inst(const Instruction &inst)
        {
            switch (inst.m_opcode)
            {
            case Instruction::NOP:
                execute_nop();
                break;

            case Instruction::MOV:
                execute_mov(inst.m_operand);
                break;

            case Instruction::PUSH:
                execute_push(inst.m_operand);
                break;

            case Instruction::POP:
                execute_pop();
                break;

            case Instruction::SYSCALL:
                execute_syscall(inst.m_operand);
                break;

            case Instruction::CALL:
                execute_call(inst.m_operand);
                break;

            case Instruction::CAST:
                execute_cast(inst.m_operand);
                break;

            case Instruction::ADD:
                execute_add(inst.m_operand);
                break;

            case Instruction::SUB:
                execute_sub(inst.m_operand);
                break;

            case Instruction::MUL:
                execute_mul(inst.m_operand);
                break;

            case Instruction::DIV:
                execute_div(inst.m_operand);
                break;

            case Instruction::MOD:
                execute_mod(inst.m_operand);
                break;

            case Instruction::CMP:
                execute_cmp(inst.m_operand);
                break;

            case Instruction::JMP:
                execute_jmp(inst.m_operand);
                break;

            case Instruction::JE:
                execute_je(inst.m_operand);
                break;

            case Instruction::JNE:
                execute_jne(inst.m_operand);
                break;

            case Instruction::JL:
                execute_jl(inst.m_operand);
                break;

            case Instruction::JLE:
                execute_jle(inst.m_operand);
                break;

            case Instruction::JG:
                execute_jg(inst.m_operand);
                break;

            case Instruction::JGE:
                execute_jge(inst.m_operand);
                break;

            case Instruction::RESUME:
                execute_resume();
                break;

            case Instruction::PAUSE:
                execute_pause();
                break;

            case Instruction::ABORT:
                execute_abort();
                break;

            default:
                throw "error instruction";
                break;
            }
        }

        template <typename T>
        T binary_calculate(Int op, const T &num0, const T &num1)
        {
            switch (op)
            {
            case Instruction::ADD:
                return num0 + num1;

            case Instruction::SUB:
                return num0 - num1;

            case Instruction::MUL:
                return num0 * num1;

            case Instruction::DIV:
                return num0 / num1;

            case Instruction::MOD:
                return remainder(num0, num1);

            default:
                throw "op error";
            }
        }

        Value binary_calculate(Int op, Int arg)
        {
            const Value &val1 = *remove_var();
            const Value &val0 = *remove_var();
            Value result;

            switch (arg)
            {
            case Value::INT:
            {
                Int num0 = value_cast<Int>(val0);
                Int num1 = value_cast<Int>(val1);
                result = binary_calculate(op, num0, num1);
            }
            break;

            case Value::FLOAT:
            {
                Float num0 = value_cast<Float>(val0);
                Float num1 = value_cast<Float>(val1);
                result = binary_calculate(op, num0, num1);
            }
            break;

            default:
                throw "type error";
                break;
            }

            return result;
        }

        template <typename T>
        bool condition_jump(Int op, const T &val)
        {
            switch (op)
            {
            case Instruction::JE:
                return val == static_cast<T>(0);

            case Instruction::JNE:
                return val != static_cast<T>(0);

            case Instruction::JL:
                return val < static_cast<T>(0);

            case Instruction::JLE:
                return val <= static_cast<T>(0);

            case Instruction::JG:
                return val > static_cast<T>(0);

            case Instruction::JGE:
                return val >= static_cast<T>(0);

            default:
                throw "op error";
            }
        }

        bool condition_jump(Int op)
        {
            const Value &val = *remove_var();

            switch (val.m_type)
            {
            case Value::INT:
                return condition_jump(op, val.m_i);

            case Value::FLOAT:
                return condition_jump(op, val.m_f);

            default:
                throw "type error";
            }
        }

    public:
        void execute_nop()
        {
        }

        void execute_mov(Int arg)
        {
            Reference ref = remove_var();
            m_vars.at(arg) = ref;
        }

        void execute_push(Int arg)
        {
            m_vars.push_back(m_vars.at(arg));
        }

        void execute_pop()
        {
            m_vars.pop_back();
        }

        void execute_syscall(Int arg)
        {
        }

        void execute_call(Int arg)
        {
        }

        void execute_cast(Int arg)
        {
            const Value &val = *remove_var();
            Value result;

            switch (arg)
            {
            case Value::NIL:
                result = value_cast<Nil>(val);
                break;

            case Value::BOOL:
                result = value_cast<Bool>(val);
                break;

            case Value::BYTE:
                result = value_cast<Byte>(val);
                break;

            case Value::INT:
                result = value_cast<Int>(val);
                break;

            case Value::FLOAT:
                result = value_cast<Float>(val);
                break;

            case Value::STRING:
                result = value_cast<String>(val);
                break;

            default:
                throw "type error";
                break;
            }

            add_val(result);
        }

        void execute_add(Int arg)
        {
            add_val(binary_calculate(Instruction::ADD, arg));
        }

        void execute_sub(Int arg)
        {
            add_val(binary_calculate(Instruction::SUB, arg));
        }

        void execute_mul(Int arg)
        {
            add_val(binary_calculate(Instruction::MUL, arg));
        }

        void execute_div(Int arg)
        {
            add_val(binary_calculate(Instruction::DIV, arg));
        }

        void execute_mod(Int arg)
        {
            add_val(binary_calculate(Instruction::MOD, arg));
        }

        void execute_cmp(Int arg)
        {
            execute_sub(arg);
        }

        void execute_jmp(Int arg)
        {
            m_curr_inst += arg;
        }

        void execute_je(Int arg)
        {
            if (condition_jump(Instruction::JE))
                m_curr_inst += arg;
        }

        void execute_jne(Int arg)
        {
            if (condition_jump(Instruction::JNE))
                m_curr_inst += arg;
        }

        void execute_jl(Int arg)
        {
            if (condition_jump(Instruction::JL))
                m_curr_inst += arg;
        }

        void execute_jle(Int arg)
        {
            if (condition_jump(Instruction::JLE))
                m_curr_inst += arg;
        }

        void execute_jg(Int arg)
        {
            if (condition_jump(Instruction::JG))
                m_curr_inst += arg;
        }

        void execute_jge(Int arg)
        {
            if (condition_jump(Instruction::JGE))
                m_curr_inst += arg;
        }

        void execute_resume()
        {
            resume();
        }

        void execute_pause()
        {
            pause();
        }

        void execute_abort()
        {
            abort();
        }
    };

} // namespace vm

#endif
