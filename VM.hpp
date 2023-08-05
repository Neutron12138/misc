#ifndef VM
#define VM

#include "Executer.hpp"
#include "Executable.hpp"
#include "Compiler.hpp"
#include "Parser.hpp"

namespace vm
{
    class VirtualMachine
    {
    public:
        Executer m_executer;
        Compiler m_compiler;
        Parser m_parser;

    public:
        void execute();
        void load();
        void compile();
        void parse();
    };

} // namespace vm

#endif
