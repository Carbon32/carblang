#ifndef CORE_HPP
#define CORE_HPP

#include "deps.hpp"
#include "opcode.hpp"
#include "token_type.hpp"

inline bool error_trigger = false;
inline bool runtime_error_trigger = false;

#include "forward.hpp"
#include "ang.hpp"
#include "html/cml.hpp"
#include "native.hpp"
#include "define.hpp"
#include "module.hpp"
#include "json.hpp"
#include "csv.hpp"
#include "profile.hpp"

std::shared_ptr<BoundMethod> make_native_method(Value value, NativeMethod method);

#include "vm.hpp"
#include "error.hpp"
#include "token.hpp"
#include "expr.hpp"
#include "stmt.hpp"
#include "scanner.hpp"
#include "parser.hpp"

class Carblang
{
public:
    Carblang();
    void start(int argc, char **argv);
    void run(std::string code);

    std::string read_file(const char *file);

    void handle_file(const char *file);
};

#include "compiler.hpp"

#endif