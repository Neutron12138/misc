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
    exe.add_val(vm::Int(10));
    exe.add_val(vm::Int(20));
    exe.execute_push(0);
    exe.execute_push(1);
    exe.execute_push(0);
    exe.execute_push(1);
    exe.execute_add(vm::Value::INT);
    exe.execute_add(vm::Value::INT);
    exe.execute_add(vm::Value::INT);
    std::cout << vm::value_cast<vm::Int>(*exe.remove_var()) << std::endl;

    return 0;
}
