#pragma once

#include "raylib.h"
#undef PI

struct TextElement
{
    std::string text;
    int x, y;
    int size;
    Color color;
};

#include "deps.hpp"

class CarbGUI
{
public:
    std::vector<std::function<void()>> draw_tasks;
    std::vector<std::function<void()>> update_tasks;

    std::string title_storage;
    std::filesystem::path resource_root;
    std::vector<std::shared_ptr<Texture2D>> textures;

    int width = 0;
    int height = 0;

    bool init = false;
    bool running = false;

    unsigned char r = 244;
    unsigned char g = 244;
    unsigned char b = 244;

    CarbGUI()
    {
        draw_tasks.reserve(64);
        update_tasks.reserve(16);
        textures.reserve(16);
    }

    CarbGUI(const CarbGUI &) = delete;
    CarbGUI &operator=(const CarbGUI &) = delete;
    CarbGUI(CarbGUI &&) = delete;
    CarbGUI &operator=(CarbGUI &&) = delete;

    ~CarbGUI()
    {
        close();
    }

    template <typename F, typename... Args>
    void add_draw_task(F &&f, Args &&...args)
    {
        draw_tasks.push_back(
            [func = std::forward<F>(f),
             ... captured_args = std::forward<Args>(args)]() mutable
            {
                func(captured_args...);
            });
    }

    template <typename F, typename... Args>
    void add_update_task(F &&f, Args &&...args)
    {
        update_tasks.push_back(
            [func = std::forward<F>(f),
             ... captured_args = std::forward<Args>(args)]() mutable
            {
                func(captured_args...);
            });
    }

    void set_window_config(const std::string &t, int w, int h)
    {
        if (running)
            throw std::runtime_error("Cannot change GUI configuration while it is running");
        if (w <= 0 || h <= 0)
            throw std::runtime_error("GUI window dimensions must be positive");

        title_storage = t;
        width = w;
        height = h;
        init = true;
    }

    void set_resource_root(const std::filesystem::path &source_path)
    {
        resource_root = source_path.empty() ? std::filesystem::path{} : source_path.parent_path();
    }

    std::filesystem::path resolve_resource_path(const std::string &path) const
    {
        std::filesystem::path resource_path(path);
        if (resource_path.is_absolute())
            return resource_path;

        std::error_code error;
        if (std::filesystem::is_regular_file(resource_path, error))
            return resource_path;

        error.clear();
        if (!resource_root.empty())
        {
            auto script_relative_path = resource_root / resource_path;
            if (std::filesystem::is_regular_file(script_relative_path, error))
                return script_relative_path;
            return script_relative_path;
        }

        return resource_path;
    }

    int checked_int(double value, const char *argument) const
    {
        if (!std::isfinite(value) ||
            value < static_cast<double>(std::numeric_limits<int>::min()) ||
            value >= -static_cast<double>(std::numeric_limits<int>::min()))
            throw std::runtime_error(std::string(argument) + " is outside the supported integer range");
        return static_cast<int>(value);
    }

    void start()
    {
        if (!init)
            throw std::runtime_error("GUI not initialized");

        if (!running)
        {
            SetTraceLogLevel(LOG_NONE);
            InitWindow(width, height, title_storage.c_str());
            if (!IsWindowReady())
                throw std::runtime_error("Failed to initialize GUI window");
            SetTargetFPS(60);
            running = true;
        }
    }

    void tick()
    {
        if (!running)
            return;

        if (WindowShouldClose())
        {
            close();
            return;
        }

        for (auto &task : update_tasks)
            task();

        BeginDrawing();
        ClearBackground(Color{r, g, b, 255});

        for (auto &task : draw_tasks)
            task();

        EndDrawing();

        draw_tasks.clear();
    }

    void track_texture(const std::shared_ptr<Texture2D> &texture)
    {
        if (texture)
            textures.push_back(texture);
    }

    void unload_texture(const std::shared_ptr<Texture2D> &texture)
    {
        if (!texture || texture->id == 0)
            return;
        if (!running)
            throw std::runtime_error("Cannot unload a texture after the GUI has closed");

        UnloadTexture(*texture);
        texture->id = 0;
        texture->width = 0;
        texture->height = 0;
        textures.erase(std::remove(textures.begin(), textures.end(), texture), textures.end());
    }

    void close()
    {
        if (!running)
            return;

        for (auto &texture : textures)
        {
            if (texture && texture->id != 0)
            {
                UnloadTexture(*texture);
                texture->id = 0;
                texture->width = 0;
                texture->height = 0;
            }
        }
        textures.clear();
        draw_tasks.clear();
        update_tasks.clear();
        CloseWindow();
        running = false;
    }

    bool is_running() const
    {
        return running;
    }
};

inline CarbGUI gui;