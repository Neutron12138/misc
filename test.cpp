#include <iostream>
#include "VM.hpp"

int main(int argc, char *argv[])
{
    // 变量测试
    /*vm::Instruction inst;
    std::cout << inst << std::endl;
    vm::Value var = nullptr;
    std::cout << var << std::endl;
    var = vm::Bool(true);
    std::cout << var << std::endl;
    var = vm::Byte('a');
    std::cout << var << std::endl;
    var = vm::Int(123);
    std::cout << var << std::endl;
    var = 45.6;
    std::cout << var << std::endl;
    var = vm::String("string");
    std::cout << var << std::endl;

    vm::Value var2 = var;
    var2.m_str = "string2";
    std::cout << var << "," << var2 << std::endl;*/

    // 执行器测试
    /*try
    {
        vm::Executer exe;
        exe.m_vars = {
            vm::make_ref(vm::String(",str")),
            vm::make_ref(nullptr),
            vm::make_ref(vm::Int(10)),
            vm::make_ref(vm::Int(vm::Value::INT)),
        };
        exe.print_vars();
        exe.execute_syscall(vm::SysCall::INPUT);
        exe.print_vars();
        exe.add_val(vm::Int(4));
        exe.print_vars();
        exe.execute_syscall(vm::SysCall::PRINT);
        exe.execute_syscall(vm::SysCall::EXIT);
        // exe.run();
    }
    catch (const std::string &e)
    {
        std::cout << "[" << e << "]" << std::endl;
    }*/

    // 可执行文件测试
    /*try
    {
        vm::Executable file;
        file.m_vars = {
            vm::make_ref(vm::Value(vm::Int(2))),
            vm::make_ref(vm::Value(vm::String("\n"))),
            vm::make_ref(vm::Value(vm::Int(10))),
        };
        file.m_insts = {
            vm::Instruction(vm::Instruction::PUSH, 2),
            vm::Instruction(vm::Instruction::ADD, vm::Value::INT),
            vm::Instruction(vm::Instruction::PUSH, 0),
            vm::Instruction(vm::Instruction::SYSCALL, vm::SysCall::PRINT),
            vm::Instruction(vm::Instruction::PAUSE),
        };
        vm::binary_write_to_file("test.e", file);

        vm::Executable file2 = vm::binary_read_from_file<vm::Executable>("test.e");
        vm::Executer exe = file2;
        // exe.debug_run(0, true, true, false, true);
        exe.run();
    }
    catch (const std::string &e)
    {
        std::cout << "[" << e << "]" << std::endl;
    }*/

    return 0;
}
