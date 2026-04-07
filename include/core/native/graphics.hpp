#define NATIVE_GUI_INIT                                                                        \
    case NativeMethod::GUI_INIT:                                                               \
    {                                                                                          \
        if (args.size() != 3)                                                                  \
            throw std::runtime_error("gui_init() expects 3 arguments (title, width, height)"); \
                                                                                               \
        if (!std::holds_alternative<std::string>(args[0]))                                     \
            throw std::runtime_error("gui_init() title must be a string");                     \
                                                                                               \
        if (!std::holds_alternative<double>(args[1]) ||                                        \
            !std::holds_alternative<double>(args[2]))                                          \
            throw std::runtime_error("gui_init() width/height must be numbers");               \
                                                                                               \
        const std::string &title = std::get<std::string>(args[0]);                             \
        int width = static_cast<int>(std::get<double>(args[1]));                               \
        int height = static_cast<int>(std::get<double>(args[2]));                              \
                                                                                               \
        gui.set_window_config(title.c_str(), width, height);                                   \
        push(nullptr);                                                                         \
        break;                                                                                 \
    }

#define NATIVE_GUI_START          \
    case NativeMethod::GUI_START: \
    {                             \
        gui.handle_event_loop();  \
        push(nullptr);            \
        break;                    \
    }