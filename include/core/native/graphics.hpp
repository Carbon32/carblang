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

#define NATIVE_GUI_DRAW_TEXT                                                                        \
    case NativeMethod::GUI_DRAW_TEXT:                                                               \
    {                                                                                               \
        if (args.size() != 4)                                                                       \
            throw std::runtime_error("Use the correct arguments, gui_draw_text(text, x, y, size)"); \
                                                                                                    \
        if (!std::holds_alternative<std::string>(args[0]))                                          \
            throw std::runtime_error("Text must be string");                                        \
                                                                                                    \
        std::string text = std::get<std::string>(args[0]);                                          \
        int x = (int)std::get<double>(args[1]);                                                     \
        int y = (int)std::get<double>(args[2]);                                                     \
        int size = (int)std::get<double>(args[3]);                                                  \
                                                                                                    \
        gui.add_draw_task([text, x, y, size]() { DrawText(text.c_str(), x, y, size, BLACK); });     \
                                                                                                    \
        push(nullptr);                                                                              \
        break;                                                                                      \
    }

#define NATIVE_GUI_DRAW_RECT                                                                  \
    case NativeMethod::GUI_DRAW_RECT:                                                         \
    {                                                                                         \
        if (args.size() != 4)                                                                 \
            throw std::runtime_error("Use the correct arguments, gui_draw_rect(x, y, w, h)"); \
                                                                                              \
        int x = (int)std::get<double>(args[0]);                                               \
        int y = (int)std::get<double>(args[1]);                                               \
        int w = (int)std::get<double>(args[2]);                                               \
        int h = (int)std::get<double>(args[3]);                                               \
                                                                                              \
        gui.add_draw_task(DrawRectangle, x, y, w, h, BLUE);                                   \
        push(nullptr);                                                                        \
        break;                                                                                \
    }

#define NATIVE_GUI_DRAW_CIRCLE                                                               \
    case NativeMethod::GUI_DRAW_CIRCLE:                                                      \
    {                                                                                        \
        if (args.size() != 3)                                                                \
            throw std::runtime_error("Use the correct arguments, gui_draw_circle(x, y, r)"); \
                                                                                             \
        int x = (int)std::get<double>(args[0]);                                              \
        int y = (int)std::get<double>(args[1]);                                              \
        float r = (float)std::get<double>(args[2]);                                          \
                                                                                             \
        gui.add_draw_task(DrawCircle, x, y, r, RED);                                         \
        push(nullptr);                                                                       \
        break;                                                                               \
    }

#define NATIVE_GUI_LOAD_TEXTURE                                                            \
    case NativeMethod::GUI_LOAD_TEXTURE:                                                   \
    {                                                                                      \
        if (args.size() != 1)                                                              \
            throw std::runtime_error("Use the correct arguments, gui_load_texture(path)"); \
                                                                                           \
        if (!std::holds_alternative<std::string>(args[0]))                                 \
            throw std::runtime_error("Path must be string");                               \
                                                                                           \
        const std::string &path = std::get<std::string>(args[0]);                          \
                                                                                           \
        Texture2D tex = LoadTexture(path.c_str());                                         \
                                                                                           \
        if (tex.id == 0)                                                                   \
            throw std::runtime_error("Failed to load texture");                            \
                                                                                           \
        auto texture_ptr = std::shared_ptr<Texture2D>(                                     \
            new Texture2D(tex),                                                            \
            [](Texture2D *t)                                                               \
            {                                                                              \
                UnloadTexture(*t);                                                         \
                delete t;                                                                  \
            });                                                                            \
                                                                                           \
        push(texture_ptr);                                                                 \
        break;                                                                             \
    }

#define NATIVE_GUI_DRAW_TEXTURE                                                                 \
    case NativeMethod::GUI_DRAW_TEXTURE:                                                        \
    {                                                                                           \
        if (args.size() != 3)                                                                   \
            throw std::runtime_error("Use the correct arguments, gui_draw_texture(tex, x, y)"); \
                                                                                                \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                               \
        int x = (int)std::get<double>(args[1]);                                                 \
        int y = (int)std::get<double>(args[2]);                                                 \
                                                                                                \
        gui.add_draw_task(DrawTexture, *tex, x, y, WHITE);                                      \
        push(nullptr);                                                                          \
        break;                                                                                  \
    }

#define NATIVE_GUI_CLEAR_TASKS    \
    case NativeMethod::GUI_CLEAR: \
    {                             \
        gui.draw_tasks.clear();   \
        gui.update_tasks.clear(); \
        push(nullptr);            \
        break;                    \
    }