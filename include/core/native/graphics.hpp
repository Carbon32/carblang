#define NATIVE_GUI_INIT                                                                                               \
    case NativeMethod::GUI_INIT:                                                                                      \
    {                                                                                                                 \
        if (args.size() != 3)                                                                                         \
            throw std::runtime_error("Use the correct arguments, init() expects 3 arguments (title, width, height)"); \
                                                                                                                      \
        if (!std::holds_alternative<std::string>(args[0]))                                                            \
            throw std::runtime_error("Use the correct arguments, init() title must be a string");                     \
                                                                                                                      \
        if (!std::holds_alternative<double>(args[1]) ||                                                               \
            !std::holds_alternative<double>(args[2]))                                                                 \
            throw std::runtime_error("Use the correct arguments, width/height must be numbers");                      \
                                                                                                                      \
        const std::string &title = std::get<std::string>(args[0]);                                                    \
        int width = static_cast<int>(std::get<double>(args[1]));                                                      \
        int height = static_cast<int>(std::get<double>(args[2]));                                                     \
                                                                                                                      \
        gui.set_window_config(title.c_str(), width, height);                                                          \
        push(nullptr);                                                                                                \
        break;                                                                                                        \
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

#define NATIVE_GUI_DELTA_TIME          \
    case NativeMethod::GUI_DELTA_TIME: \
    {                                  \
        float dt = GetFrameTime();     \
        push((double)dt);              \
        break;                         \
    }

#define NATIVE_GUI_IS_RUNNING          \
    case NativeMethod::GUI_IS_RUNNING: \
    {                                  \
        push(gui.is_running());        \
        break;                         \
    }

#define NATIVE_GUI_DRAW_TEXT                                                                    \
    case NativeMethod::GUI_DRAW_TEXT:                                                           \
    {                                                                                           \
        if (args.size() != 4)                                                                   \
            throw std::runtime_error("Use the correct arguments, draw_text(text, x, y, size)"); \
                                                                                                \
        if (!std::holds_alternative<std::string>(args[0]))                                      \
            throw std::runtime_error("Use the correct arguments, text must be string");         \
                                                                                                \
        std::string text = std::get<std::string>(args[0]);                                      \
        int x = (int)std::get<double>(args[1]);                                                 \
        int y = (int)std::get<double>(args[2]);                                                 \
        int size = (int)std::get<double>(args[3]);                                              \
                                                                                                \
        gui.add_draw_task([text, x, y, size]() { DrawText(text.c_str(), x, y, size, BLACK); }); \
                                                                                                \
        push(nullptr);                                                                          \
        break;                                                                                  \
    }

#define NATIVE_GUI_DRAW_TEXT_COLOR                                                                             \
    case NativeMethod::GUI_DRAW_TEXT_COLOR:                                                                    \
    {                                                                                                          \
        if (args.size() != 7)                                                                                  \
            throw std::runtime_error("Use the correct arguments, draw_text_color(text, x, y, size, r, g, b)"); \
                                                                                                               \
        if (!std::holds_alternative<std::string>(args[0]))                                                     \
            throw std::runtime_error("Use the correct arguments, text must be string");                        \
                                                                                                               \
        std::string text = std::get<std::string>(args[0]);                                                     \
        int x = (int)std::get<double>(args[1]);                                                                \
        int y = (int)std::get<double>(args[2]);                                                                \
        int size = (int)std::get<double>(args[3]);                                                             \
                                                                                                               \
        int r = (int)std::get<double>(args[4]);                                                                \
        int g = (int)std::get<double>(args[5]);                                                                \
        int b = (int)std::get<double>(args[6]);                                                                \
                                                                                                               \
        Color color = {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};                             \
                                                                                                               \
        gui.add_draw_task([text, x, y, size, color]() { DrawText(text.c_str(), x, y, size, color); });         \
                                                                                                               \
        push(nullptr);                                                                                         \
        break;                                                                                                 \
    }

#define NATIVE_GUI_SET_BACKGROUND_COLOR                                                     \
    case NativeMethod::GUI_SET_BACKGROUND_COLOR:                                            \
    {                                                                                       \
        if (args.size() != 3)                                                               \
            throw std::runtime_error("Use the correct arguments, set_background(r, g, b)"); \
                                                                                            \
        int r = (int)std::get<double>(args[0]);                                             \
        int g = (int)std::get<double>(args[1]);                                             \
        int b = (int)std::get<double>(args[2]);                                             \
                                                                                            \
        gui.r = r;                                                                          \
        gui.g = g;                                                                          \
        gui.b = b;                                                                          \
                                                                                            \
        push(nullptr);                                                                      \
        break;                                                                              \
    }

#define NATIVE_GUI_MEASURE_TEXT                                                                                  \
    case NativeMethod::GUI_MEASURE_TEXT:                                                                         \
    {                                                                                                            \
        if (args.size() != 2)                                                                                    \
            throw std::runtime_error("Use the correct arguments, measure_text(text, size) expects 2 arguments"); \
                                                                                                                 \
        if (!std::holds_alternative<std::string>(args[0]))                                                       \
            throw std::runtime_error("Use the correct arguments, first argument must be string");                \
                                                                                                                 \
        if (!std::holds_alternative<double>(args[1]))                                                            \
            throw std::runtime_error("Use the correct arguments, fecond argument must be number");               \
                                                                                                                 \
        const std::string &text = std::get<std::string>(args[0]);                                                \
        int size = (int)std::get<double>(args[1]);                                                               \
                                                                                                                 \
        int width = MeasureText(text.c_str(), size);                                                             \
                                                                                                                 \
        push((double)width);                                                                                     \
        break;                                                                                                   \
    }

#define NATIVE_GUI_DRAW_RECT                                                              \
    case NativeMethod::GUI_DRAW_RECT:                                                     \
    {                                                                                     \
        if (args.size() != 4)                                                             \
            throw std::runtime_error("Use the correct arguments, draw_rect(x, y, w, h)"); \
                                                                                          \
        int x = (int)std::get<double>(args[0]);                                           \
        int y = (int)std::get<double>(args[1]);                                           \
        int w = (int)std::get<double>(args[2]);                                           \
        int h = (int)std::get<double>(args[3]);                                           \
                                                                                          \
        gui.add_draw_task(DrawRectangle, x, y, w, h, BLUE);                               \
        push(nullptr);                                                                    \
        break;                                                                            \
    }

#define NATIVE_GUI_DRAW_RECT_ADV                                                                                       \
    case NativeMethod::GUI_DRAW_RECT_ADV:                                                                              \
    {                                                                                                                  \
        if (args.size() != 8)                                                                                          \
            throw std::runtime_error("Use the correct arguments, draw_rect_advanced(x, y, w, h, r, g, b, roundness)"); \
                                                                                                                       \
        int x = (int)std::get<double>(args[0]);                                                                        \
        int y = (int)std::get<double>(args[1]);                                                                        \
        int w = (int)std::get<double>(args[2]);                                                                        \
        int h = (int)std::get<double>(args[3]);                                                                        \
                                                                                                                       \
        int r = (int)std::get<double>(args[4]);                                                                        \
        int g = (int)std::get<double>(args[5]);                                                                        \
        int b = (int)std::get<double>(args[6]);                                                                        \
        float roundness = (float)std::get<double>(args[7]);                                                            \
                                                                                                                       \
        Color color = {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};                                     \
                                                                                                                       \
        gui.add_draw_task(DrawRectangleRounded,                                                                        \
                          Rectangle{(float)x, (float)y, (float)w, (float)h},                                           \
                          roundness,                                                                                   \
                          10,                                                                                          \
                          color);                                                                                      \
                                                                                                                       \
        push(nullptr);                                                                                                 \
        break;                                                                                                         \
    }

#define NATIVE_GUI_DRAW_CIRCLE                                                           \
    case NativeMethod::GUI_DRAW_CIRCLE:                                                  \
    {                                                                                    \
        if (args.size() != 3)                                                            \
            throw std::runtime_error("Use the correct arguments, draw_circle(x, y, r)"); \
                                                                                         \
        int x = (int)std::get<double>(args[0]);                                          \
        int y = (int)std::get<double>(args[1]);                                          \
        float r = (float)std::get<double>(args[2]);                                      \
                                                                                         \
        gui.add_draw_task(DrawCircle, x, y, r, RED);                                     \
        push(nullptr);                                                                   \
        break;                                                                           \
    }

#define NATIVE_GUI_LOAD_TEXTURE                                                         \
    case NativeMethod::GUI_LOAD_TEXTURE:                                                \
    {                                                                                   \
        if (args.size() != 1)                                                           \
            throw std::runtime_error("Use the correct arguments, load_texture(path)");  \
                                                                                        \
        if (!std::holds_alternative<std::string>(args[0]))                              \
            throw std::runtime_error("Use the correct arguments, path must be string"); \
                                                                                        \
        const std::string &path = std::get<std::string>(args[0]);                       \
                                                                                        \
        Texture2D tex = LoadTexture(path.c_str());                                      \
                                                                                        \
        if (tex.id == 0)                                                                \
            throw std::runtime_error("Failed to load texture");                         \
                                                                                        \
        auto texture_ptr = std::make_shared<Texture2D>(tex);                            \
                                                                                        \
        push(texture_ptr);                                                              \
        break;                                                                          \
    }

#define NATIVE_GUI_DRAW_TEXTURE                                                                 \
    case NativeMethod::GUI_DRAW_TEXTURE:                                                        \
    {                                                                                           \
        if (args.size() != 3)                                                                   \
            throw std::runtime_error("Use the correct arguments, draw_texture(texture, x, y)"); \
                                                                                                \
        std::cout << "Before get" << std::endl;                                                 \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                               \
        std::cout << "After get" << std::endl;                                                  \
        int x = (int)std::get<double>(args[1]);                                                 \
        int y = (int)std::get<double>(args[2]);                                                 \
                                                                                                \
        gui.add_draw_task([tex, x, y]() { DrawTexture(*tex, x, y, WHITE); });                   \
                                                                                                \
        push(nullptr);                                                                          \
        break;                                                                                  \
    }

#define NATIVE_GUI_DRAW_TEXTURE_SIZED                                                                       \
    case NativeMethod::GUI_DRAW_TEXTURE_SIZED:                                                              \
    {                                                                                                       \
        if (args.size() != 5)                                                                               \
            throw std::runtime_error("Use the correct arguments, draw_sized_texture(texture, x, y, w, h)"); \
                                                                                                            \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                                           \
        int x = (int)std::get<double>(args[1]);                                                             \
        int y = (int)std::get<double>(args[2]);                                                             \
        float w = (float)std::get<double>(args[3]);                                                         \
        float h = (float)std::get<double>(args[4]);                                                         \
                                                                                                            \
        gui.add_draw_task([tex, x, y, w, h]() {                                                       \
            Rectangle src = { 0, 0, (float)tex->width, (float)tex->height };                          \
            Rectangle dst = { (float)x, (float)y, w, h };                                             \
            Vector2 origin = { 0, 0 };                                                                \
                                                                                                      \
            DrawTexturePro(*tex, src, dst, origin, 0.0f, WHITE); });                                                         \
                                                                                                            \
        push(nullptr);                                                                                      \
        break;                                                                                              \
    }

#define NATIVE_GUI_DRAW_TEXTURE_FLIPPED                                                                                       \
    case NativeMethod::GUI_DRAW_TEXTURE_FLIPPED:                                                                              \
    {                                                                                                                         \
        if (args.size() != 7)                                                                                                 \
            throw std::runtime_error("Use the correct arguments, draw_flipped_texture(texture, x, y, w, h, flip_x, flip_y)"); \
                                                                                                                              \
        if (!std::holds_alternative<std::shared_ptr<Texture2D>>(args[0]))                                                     \
            throw std::runtime_error("Use the correct arguments, first argument must be texture");                            \
                                                                                                                              \
        auto tex = std::get<std::shared_ptr<Texture2D>>(args[0]);                                                             \
        int x = (int)std::get<double>(args[1]);                                                                               \
        int y = (int)std::get<double>(args[2]);                                                                               \
        float w = (float)std::get<double>(args[3]);                                                                           \
        float h = (float)std::get<double>(args[4]);                                                                           \
        bool flip_x = std::get<bool>(args[5]);                                                                                \
        bool flip_y = std::get<bool>(args[6]);                                                                                \
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
        DrawTexturePro(*tex, src, dst, origin, 0.0f, WHITE); });                                                           \
                                                                                                                              \
        push(nullptr);                                                                                                        \
        break;                                                                                                                \
    }

#define NATIVE_GUI_CREATE_TEXT                                                                                     \
    case NativeMethod::GUI_CREATE_TEXT:                                                                            \
    {                                                                                                              \
        if (args.size() != 7)                                                                                      \
            throw std::runtime_error("Use the correct arguments, create_text_element(text, x, y, size, r, g, b)"); \
                                                                                                                   \
        if (!std::holds_alternative<std::string>(args[0]))                                                         \
            throw std::runtime_error("Use the correct arguments, text must be string");                            \
                                                                                                                   \
        int r = (int)std::get<double>(args[4]);                                                                    \
        int g = (int)std::get<double>(args[5]);                                                                    \
        int b = (int)std::get<double>(args[6]);                                                                    \
        auto elem = std::make_shared<TextElement>();                                                               \
        elem->text = std::get<std::string>(args[0]);                                                               \
        elem->x = (int)std::get<double>(args[1]);                                                                  \
        elem->y = (int)std::get<double>(args[2]);                                                                  \
        elem->size = (int)std::get<double>(args[3]);                                                               \
        Color color = {(unsigned char)r, (unsigned char)g, (unsigned char)b, 255};                                 \
        elem->color = color;                                                                                       \
                                                                                                                   \
        gui.text_elements.push_back(elem);                                                                         \
                                                                                                                   \
        push(elem);                                                                                                \
        break;                                                                                                     \
    }

#define NATIVE_GUI_DRAW_TEXT_ELEMENT                                                                              \
    case NativeMethod::GUI_DRAW_TEXT_ELEMENT:                                                                     \
    {                                                                                                             \
        if (args.size() != 1)                                                                                     \
            throw std::runtime_error("Use the correct arguments, draw_text_element(element)");                    \
                                                                                                                  \
        auto elem = std::get<std::shared_ptr<TextElement>>(args[0]);                                              \
                                                                                                                  \
        gui.add_draw_task([elem]() { DrawText(elem->text.c_str(), elem->x, elem->y, elem->size, elem->color); }); \
                                                                                                                  \
        push(nullptr);                                                                                            \
        break;                                                                                                    \
    }

#define NATIVE_GUI_SET_TEXT                                          \
    case NativeMethod::GUI_SET_TEXT:                                 \
    {                                                                \
        auto elem = std::get<std::shared_ptr<TextElement>>(args[0]); \
        elem->text = std::get<std::string>(args[1]);                 \
        push(nullptr);                                               \
        break;                                                       \
    }

#define NATIVE_GUI_MOUSE_PRESSED                                                          \
    case NativeMethod::GUI_MOUSE_PRESSED:                                                 \
    {                                                                                     \
        if (args.size() != 1)                                                             \
            throw std::runtime_error("Use the correct arguments, mouse_pressed(button)"); \
                                                                                          \
        if (!std::holds_alternative<double>(args[0]))                                     \
            throw std::runtime_error("Use the correct arguments, button must be number"); \
                                                                                          \
        int button = (int)std::get<double>(args[0]);                                      \
                                                                                          \
        push(IsMouseButtonPressed(button));                                               \
        break;                                                                            \
    }

#define NATIVE_GUI_MOUSE_RELEASED                                                          \
    case NativeMethod::GUI_MOUSE_RELEASED:                                                 \
    {                                                                                      \
        if (args.size() != 1)                                                              \
            throw std::runtime_error("Use the correct arguments, mouse_released(button)"); \
                                                                                           \
        if (!std::holds_alternative<double>(args[0]))                                      \
            throw std::runtime_error("Use the correct arguments, button must be number");  \
                                                                                           \
        int button = (int)std::get<double>(args[0]);                                       \
                                                                                           \
        push(IsMouseButtonReleased(button));                                               \
        break;                                                                             \
    }

#define NATIVE_GUI_KEY_PRESSED                                                         \
    case NativeMethod::GUI_KEY_PRESSED:                                                \
    {                                                                                  \
        if (args.size() != 1)                                                          \
            throw std::runtime_error("Use the correct arguments, key_pressed(key)");   \
                                                                                       \
        if (!std::holds_alternative<double>(args[0]))                                  \
            throw std::runtime_error("Use the correct arguments, key must be number"); \
                                                                                       \
        int key = (int)std::get<double>(args[0]);                                      \
                                                                                       \
        bool pressed = IsKeyPressed(key);                                              \
                                                                                       \
        push(pressed);                                                                 \
        break;                                                                         \
    }

#define NATIVE_GUI_GET_CHAR            \
    case NativeMethod::GUI_GET_CHAR:   \
    {                                  \
        int c = GetCharPressed();      \
                                       \
        if (c > 0)                     \
        {                              \
            std::string s(1, (char)c); \
            push(s);                   \
        }                              \
        else                           \
        {                              \
            push(nullptr);             \
        }                              \
                                       \
        break;                         \
    }

#define NATIVE_GUI_KEY_DOWN                                                       \
    case NativeMethod::GUI_KEY_DOWN:                                              \
    {                                                                             \
        if (args.size() != 1)                                                     \
            throw std::runtime_error("Use the correct arguments, key_down(key)"); \
                                                                                  \
        int key = (int)std::get<double>(args[0]);                                 \
                                                                                  \
        push(IsKeyDown(key));                                                     \
        break;                                                                    \
    }

#define NATIVE_GUI_KEY_RELEASED                                                        \
    case NativeMethod::GUI_KEY_RELEASED:                                               \
    {                                                                                  \
        if (args.size() != 1)                                                          \
            throw std::runtime_error("Use the correct arguments, key_released(key)");  \
                                                                                       \
        if (!std::holds_alternative<double>(args[0]))                                  \
            throw std::runtime_error("Use the correct arguments, key must be number"); \
                                                                                       \
        int key = (int)std::get<double>(args[0]);                                      \
                                                                                       \
        bool released = IsKeyReleased(key);                                            \
                                                                                       \
        push(released);                                                                \
        break;                                                                         \
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

#define NATIVE_GUI_CREATE_RECTANGLE                                                              \
    case NativeMethod::GUI_CREATE_RECTANGLE:                                                     \
    {                                                                                            \
        if (args.size() != 4)                                                                    \
            throw std::runtime_error("Use the correct arguments, create_rectangle(x, y, w, h)"); \
                                                                                                 \
        auto rect = std::make_shared<Array>();                                                   \
        rect->elements.push_back(args[0]);                                                       \
        rect->elements.push_back(args[1]);                                                       \
        rect->elements.push_back(args[2]);                                                       \
        rect->elements.push_back(args[3]);                                                       \
                                                                                                 \
        push(rect);                                                                              \
        break;                                                                                   \
    }

#define NATIVE_GUI_RECTANGLE_COLLISION                                                              \
    case NativeMethod::GUI_RECTANGLE_COLLISION:                                                     \
    {                                                                                               \
        if (args.size() != 2)                                                                       \
            throw std::runtime_error("Use the correct arguments, rectangle_collision(a, b)");       \
                                                                                                    \
        auto a = std::get<std::shared_ptr<Array>>(args[0]);                                         \
        auto b = std::get<std::shared_ptr<Array>>(args[1]);                                         \
                                                                                                    \
        if (a->elements.size() < 4 || b->elements.size() < 4)                                       \
            throw std::runtime_error("Use the correct arguments, rectangles must have 4 elements"); \
                                                                                                    \
        float ax = (float)std::get<double>(a->elements[0]);                                         \
        float ay = (float)std::get<double>(a->elements[1]);                                         \
        float aw = (float)std::get<double>(a->elements[2]);                                         \
        float ah = (float)std::get<double>(a->elements[3]);                                         \
                                                                                                    \
        float bx = (float)std::get<double>(b->elements[0]);                                         \
        float by = (float)std::get<double>(b->elements[1]);                                         \
        float bw = (float)std::get<double>(b->elements[2]);                                         \
        float bh = (float)std::get<double>(b->elements[3]);                                         \
                                                                                                    \
        bool collision =                                                                            \
            ax < bx + bw &&                                                                         \
            ax + aw > bx &&                                                                         \
            ay < by + bh &&                                                                         \
            ay + ah > by;                                                                           \
                                                                                                    \
        push(collision);                                                                            \
        break;                                                                                      \
    }

#define NATIVE_GUI_RECTANGLE_MOUSE_COLLISION                                                             \
    case NativeMethod::GUI_RECTANGLE_MOUSE_COLLISION:                                                    \
    {                                                                                                    \
        if (args.size() != 1)                                                                            \
            throw std::runtime_error("Use the correct arguments, rectangle_collision_with_mouse(rect)"); \
                                                                                                         \
        auto rect = std::get<std::shared_ptr<Array>>(args[0]);                                           \
                                                                                                         \
        if (rect->elements.size() < 4)                                                                   \
            throw std::runtime_error("Use the correct arguments, rectangle must have 4 elements");       \
                                                                                                         \
        float x = (float)std::get<double>(rect->elements[0]);                                            \
        float y = (float)std::get<double>(rect->elements[1]);                                            \
        float w = (float)std::get<double>(rect->elements[2]);                                            \
        float h = (float)std::get<double>(rect->elements[3]);                                            \
                                                                                                         \
        Vector2 mouse = GetMousePosition();                                                              \
                                                                                                         \
        bool inside =                                                                                    \
            mouse.x >= x && mouse.x <= x + w &&                                                          \
            mouse.y >= y && mouse.y <= y + h;                                                            \
                                                                                                         \
        push(inside);                                                                                    \
        break;                                                                                           \
    }

#define NATIVE_GUI_CLEAR_TASKS    \
    case NativeMethod::GUI_CLEAR: \
    {                             \
        gui.draw_tasks.clear();   \
        gui.update_tasks.clear(); \
        push(nullptr);            \
        break;                    \
    }