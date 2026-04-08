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
        gui.start();              \
        push(nullptr);            \
        break;                    \
    }

#define NATIVE_GUI_TICK          \
    case NativeMethod::GUI_TICK: \
    {                            \
        gui.tick();              \
        push(gui.is_running());  \
        break;                   \
    }

#define NATIVE_GUI_IS_RUNNING          \
    case NativeMethod::GUI_IS_RUNNING: \
    {                                  \
        push(gui.is_running());        \
        break;                         \
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
            throw std::runtime_error("Use the correct arguments, gui.load_texture(path)"); \
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
        auto texture_ptr = std::make_shared<Texture2D>(tex);                               \
                                                                                           \
        push(texture_ptr);                                                                 \
        break;                                                                             \
    }

#define NATIVE_GUI_DRAW_TEXTURE                                                                     \
    case NativeMethod::GUI_DRAW_TEXTURE:                                                            \
    {                                                                                               \
        if (args.size() != 3)                                                                       \
            throw std::runtime_error("Use the correct arguments, gui.draw_texture(texture, x, y)"); \
                                                                                                    \
        std::cout << "Before get" << std::endl;                                                     \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                                   \
        std::cout << "After get" << std::endl;                                                      \
        int x = (int)std::get<double>(args[1]);                                                     \
        int y = (int)std::get<double>(args[2]);                                                     \
                                                                                                    \
        gui.add_draw_task([tex, x, y]() { DrawTexture(*tex, x, y, WHITE); });                       \
                                                                                                    \
        push(nullptr);                                                                              \
        break;                                                                                      \
    }

#define NATIVE_GUI_DRAW_TEXTURE_SIZED                                                                           \
    case NativeMethod::GUI_DRAW_TEXTURE_SIZED:                                                                  \
    {                                                                                                           \
        if (args.size() != 5)                                                                                   \
            throw std::runtime_error("Use the correct arguments, gui.draw_sized_texture(texture, x, y, w, h)"); \
                                                                                                                \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                                               \
        int x = (int)std::get<double>(args[1]);                                                                 \
        int y = (int)std::get<double>(args[2]);                                                                 \
        float w = (float)std::get<double>(args[3]);                                                             \
        float h = (float)std::get<double>(args[4]);                                                             \
                                                                                                                \
        gui.add_draw_task([tex, x, y, w, h]() {                                                       \
            Rectangle src = { 0, 0, (float)tex->width, (float)tex->height };                          \
            Rectangle dst = { (float)x, (float)y, w, h };                                             \
            Vector2 origin = { 0, 0 };                                                                \
                                                                                                      \
            DrawTexturePro(*tex, src, dst, origin, 0.0f, WHITE); });                                                             \
                                                                                                                \
        push(nullptr);                                                                                          \
        break;                                                                                                  \
    }

#define NATIVE_GUI_DRAW_TEXTURE_FLIPPED                                                                                           \
    case NativeMethod::GUI_DRAW_TEXTURE_FLIPPED:                                                                                  \
    {                                                                                                                             \
        if (args.size() != 7)                                                                                                     \
            throw std::runtime_error("Use the correct arguments, gui.draw_flipped_texture(texture, x, y, w, h, flip_x, flip_y)"); \
                                                                                                                                  \
        if (!std::holds_alternative<std::shared_ptr<Texture2D>>(args[0]))                                                         \
            throw std::runtime_error("First argument must be texture");                                                           \
                                                                                                                                  \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                                                                 \
        int x = (int)std::get<double>(args[1]);                                                                                   \
        int y = (int)std::get<double>(args[2]);                                                                                   \
        float w = (float)std::get<double>(args[3]);                                                                               \
        float h = (float)std::get<double>(args[4]);                                                                               \
        bool flip_x = std::get<bool>(args[5]);                                                                                    \
        bool flip_y = std::get<bool>(args[6]);                                                                                    \
                                                                                                                                  \
        gui.add_draw_task([tex, x, y, w, h, flip_x, flip_y]() {                   \
            Rectangle src = {                                                   \
                0,                                                              \
                0,                                                              \
                flip_x ? -(float)tex->width : (float)tex->width,                \
                flip_y ? -(float)tex->height : (float)tex->height               \
            };                                                                  \
                                                                                \
        Rectangle dst = { (float)x, (float)y, w, h };                       \
        Vector2 origin = { 0, 0 };                                          \
                                                                                \
        DrawTexturePro(*tex, src, dst, origin, 0.0f, WHITE); });                                                               \
                                                                                                                                  \
        push(nullptr);                                                                                                            \
        break;                                                                                                                    \
    }

#define NATIVE_GUI_MOUSE_PRESSED                                                          \
    case NativeMethod::GUI_MOUSE_PRESSED:                                                 \
    {                                                                                     \
        if (args.size() != 1)                                                             \
            throw std::runtime_error("Use the correct arguments, mouse_pressed(button)"); \
                                                                                          \
        if (!std::holds_alternative<double>(args[0]))                                     \
            throw std::runtime_error("Button must be number");                            \
                                                                                          \
        int button = (int)std::get<double>(args[0]);                                      \
                                                                                          \
        push(IsMouseButtonPressed(button));                                               \
        break;                                                                            \
    }

#define NATIVE_GUI_KEY_PRESSED                                                       \
    case NativeMethod::GUI_KEY_PRESSED:                                              \
    {                                                                                \
        if (args.size() != 1)                                                        \
            throw std::runtime_error("Use the correct arguments, key_pressed(key)"); \
                                                                                     \
        if (!std::holds_alternative<double>(args[0]))                                \
            throw std::runtime_error("Key must be number");                          \
                                                                                     \
        int key = (int)std::get<double>(args[0]);                                    \
                                                                                     \
        bool pressed = IsKeyPressed(key);                                            \
                                                                                     \
        push(pressed);                                                               \
        break;                                                                       \
    }

#define NATIVE_GUI_KEY_RELEASED                             \
    case NativeMethod::GUI_KEY_RELEASED:                    \
    {                                                       \
        if (args.size() != 1)                               \
            throw std::runtime_error("key_released(key)");  \
                                                            \
        if (!std::holds_alternative<double>(args[0]))       \
            throw std::runtime_error("key must be number"); \
                                                            \
        int key = (int)std::get<double>(args[0]);           \
                                                            \
        bool released = IsKeyReleased(key);                 \
                                                            \
        push(released);                                     \
        break;                                              \
    }

#define NATIVE_GUI_MOUSE_POS                      \
    case NativeMethod::GUI_MOUSE_POS:             \
    {                                             \
        Vector2 pos = GetMousePosition();         \
                                                  \
        auto array = std::make_shared<Array>();   \
        array->elements.push_back((double)pos.x); \
        array->elements.push_back((double)pos.y); \
                                                  \
        push(array);                              \
        break;                                    \
    }

#define NATIVE_GUI_CLEAR_TASKS    \
    case NativeMethod::GUI_CLEAR: \
    {                             \
        gui.draw_tasks.clear();   \
        gui.update_tasks.clear(); \
        push(nullptr);            \
        break;                    \
    }