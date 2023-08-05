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
    vm::Executer exe;
    exe.m_insts = {
        vm::Instruction(vm::Instruction::NOP),
        vm::Instruction(vm::Instruction::PAUSE),
    };
    exe.run();

    return 0;
}
