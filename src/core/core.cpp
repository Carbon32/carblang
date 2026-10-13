#include "core/core.hpp"

Carblang::Carblang()
{
}

void Carblang::start(int argc, char **argv)
{
    try
    {
        if (argc > 2 || argc < 2)
        {
            std::cout << "Usage: carblang <script>" << std::endl;
            std::exit(64);
        }
        else if (argc == 2)
        {
            this->handle_file(argv[1]);
        }
    }
    catch (const std::exception &exception)
    {
        std::cout << exception.what() << std::endl;
        exit(6);
    }
}

void Carblang::run(std::string code, std::filesystem::path source_path)
{
    gui.set_resource_root(source_path);
    Scanner scanner = Scanner(code);
    std::vector<Token> tokens = scanner.scan_tokens();
    Parser parser = Parser(tokens, source_path);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();
    ;

    if (error_trigger)
        return;

    Compiler compiler;
    Chunk chunk = compiler.compile(statements, std::move(source_path), parser.class_types());

    VM vm;
    vm.init_globals();
    vm.interpret(chunk);
}

void Carblang::handle_file(const char *file)
{
    this->run(this->read_file(file), std::filesystem::path(file));
    if (error_trigger)
    {
        std::exit(65);
    }

    if (runtime_error_trigger)
    {
        std::exit(70);
    }
}

std::string Carblang::read_file(char const *file)
{
    std::ifstream c_file(file, std::ios::in | std::ios::binary | std::ios::ate);
    if (!c_file)
    {
        std::cerr << "Failed to open file " << file << ": " << std::strerror(errno) << std::endl;
        std::exit(74);
    }

    const std::streampos end = c_file.tellg();
    if (end < 0)
        throw std::runtime_error("Failed to determine file size for " + std::string(file));

    std::string content(static_cast<size_t>(end), '\0');

    c_file.seekg(0, std::ios::beg);
    if (!c_file.read(content.data(), static_cast<std::streamsize>(content.size())))
        throw std::runtime_error("Failed to read file " + std::string(file));

    return content;
}

std::shared_ptr<BoundMethod> make_native_method(Value value, NativeMethod method)
{
    return std::make_shared<BoundMethod>(std::move(value), method);
}
