#include "core/core.hpp"

std::shared_ptr<Instance> create_module_instance(Module&& module, const std::string& name)
{
    auto klass = std::make_shared<Class>(name, std::unordered_map<std::string, std::shared_ptr<Function>>{});
    auto instance = std::make_shared<Instance>(klass);

    for(auto& [method_name, method_value] : module.methods)
    {
        instance->fields[method_name] = method_value;
    }

    return instance;
}

std::shared_ptr<Instance> init_os()
{
    Module os;
    os.set_method("system", make_native_method(nullptr, NativeMethod::SYSTEM));
    return create_module_instance(std::move(os), "os");
}

std::shared_ptr<Instance> init_text()
{
    Module text;
    text.set_method("parse_json", make_native_method(nullptr, NativeMethod::PARSE_JSON));
    text.set_method("stringify", make_native_method(nullptr, NativeMethod::STRINGIFY));
    text.set_method("to_json", make_native_method(nullptr, NativeMethod::TO_JSON));
    return create_module_instance(std::move(text), "text");
}

std::shared_ptr<Instance> init_random()
{
    Module random;
    random.set_method("double", make_native_method(nullptr, NativeMethod::RAND));
    random.set_method("integer", make_native_method(nullptr, NativeMethod::RANDINT));
    return create_module_instance(std::move(random), "random");
}

std::shared_ptr<Instance> init_array()
{
    Module array;
    array.set_method("fill", make_native_method(nullptr, NativeMethod::FILL));
    array.set_method("init", make_native_method(nullptr, NativeMethod::INIT));
    array.set_method("input", make_native_method(nullptr, NativeMethod::ARRAY_INPUT));
    return create_module_instance(std::move(array), "array");
}

std::shared_ptr<Instance> init_file()
{
    Module file;
    file.set_method("exists", make_native_method(nullptr, NativeMethod::EXISTS));
    file.set_method("is_file", make_native_method(nullptr, NativeMethod::IS_FILE));
    file.set_method("is_directory", make_native_method(nullptr, NativeMethod::IS_DIRECTORY));
    file.set_method("make_directory", make_native_method(nullptr, NativeMethod::MAKE_DIRECTORY));
    file.set_method("remove_directory", make_native_method(nullptr, NativeMethod::REMOVE_DIRECTORY));
    file.set_method("list_directories", make_native_method(nullptr, NativeMethod::LIST_DIRECTORIES));
    file.set_method("list_files", make_native_method(nullptr, NativeMethod::LIST_FILES));
    file.set_method("read", make_native_method(nullptr, NativeMethod::READ_FILE));
    file.set_method("write", make_native_method(nullptr, NativeMethod::WRITE_FILE));
    file.set_method("append", make_native_method(nullptr, NativeMethod::APPEND_FILE));
    file.set_method("erase", make_native_method(nullptr, NativeMethod::ERASE_FILE));
    file.set_method("copy", make_native_method(nullptr, NativeMethod::OS_COPY));
    file.set_method("rename", make_native_method(nullptr, NativeMethod::RENAME));
    return create_module_instance(std::move(file), "file");
}

std::shared_ptr<Instance> init_profiler()
{
    Module profiler;
    profiler.set_method("start", make_native_method(nullptr, NativeMethod::PROFILE_START));
    profiler.set_method("end", make_native_method(nullptr, NativeMethod::PROFILE_END));
    profiler.set_method("report", make_native_method(nullptr, NativeMethod::PROFILE_REPORT));
    profiler.set_method("reset", make_native_method(nullptr, NativeMethod::PROFILE_RESET));
    return create_module_instance(std::move(profiler), "profiler");
}