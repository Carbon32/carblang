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
    std::vector<std::shared_ptr<TextElement>> text_elements;
    std::vector<std::function<void()>> draw_tasks;
    std::vector<std::function<void()>> update_tasks;

    std::string title_storage;
    const char *title = nullptr;

    int width = 0;
    int height = 0;

    bool init = false;
    bool running = false;

    unsigned char r = 244;
    unsigned char g = 244;
    unsigned char b = 244;

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
        title_storage = t;
        title = title_storage.c_str();
        width = w;
        height = h;
        init = true;
    }

    void start()
    {
        if (!init)
            throw std::runtime_error("GUI not initialized");

        if (!running)
        {
            SetTraceLogLevel(LOG_NONE);
            InitWindow(width, height, title);
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
            running = false;
            CloseWindow();
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

    bool is_running() const
    {
        return running;
    }
};

inline CarbGUI gui;