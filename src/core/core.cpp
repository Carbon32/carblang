#include "core/core.hpp"

Carblang::Carblang()
{

}

void Carblang::start(int argc, char **argv)
{
    try
    {
        if(argc > 2)
        {
            std::cout << "Usage: carblang <script>" << std::endl;
            std::exit(64);
        }
        else if(argc == 2)
        {
            this->handle_file(argv[1]);
        }
        else
        {
            this->run_instructions();
        }
    }
    catch(const std::runtime_error& exception)
    {
        std::cout << exception.what() << std::endl;
        exit(6);
    }
}

void Carblang::run(std::string code)
{
    Scanner scanner = Scanner(code);
    std::vector<Token> tokens = scanner.scan_tokens();
    Parser parser = Parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();;

    if(error_trigger) return;

    Compiler compiler;
    Chunk chunk = compiler.compile(statements);

    VM vm;
    vm.init_globals();
    vm.interpret(chunk);
}

void Carblang::handle_file(const char *file)
{
    this->run(this->read_file(file));
    if(error_trigger)
    {
        std::exit(65);
    }

    if(runtime_error_trigger)
    {
        std::exit(70);
    }
}

void Carblang::run_instructions()
{
    while(true)
    {
        std::cout << "> ";
        std::string line;
        if(!std::getline(std::cin, line))
        {
            break;
        }
        this->run(line);
        error_trigger = false;
    }
}

std::string Carblang::read_file(char const *file)
{
    std::ifstream c_file(file, std::ios::in | std::ios::binary | std::ios::ate);
    if(!c_file)
    {
        std::cerr << "Failed to open file " << file << ": " << std::strerror(errno) << std::endl;
        std::exit(74);
    }

    std::streamsize size = c_file.tellg();
    std::string content(size, '\0');


    c_file.seekg(0, std::ios::beg);
    c_file.read(content.data(), content.size());

    return content;
}

std::shared_ptr<BoundMethod> make_native_method(Value value, NativeMethod method)
{
    return std::make_shared<BoundMethod>(std::move(value), method);
}

