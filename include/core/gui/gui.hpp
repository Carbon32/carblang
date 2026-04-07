#pragma once

#include "raylib.h"
#undef PI

#include "deps.hpp"

class CarbGUI
{
public:
    std::vector<std::function<void()>> draw_tasks;
    std::vector<std::function<void()>> update_tasks;

    std::string title_storage;
    const char *title;
    int width;
    int height;

    bool init = false;
    bool running = false;

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

    void set_window_config(const std::string &title, int width, int height)
    {
        this->title_storage = title;
        this->title = this->title_storage.c_str();

        this->width = width;
        this->height = height;
        this->init = true;
    }

    void handle_event_loop()
    {
        if (init && !running)
        {
            SetTraceLogLevel(LOG_NONE);
            InitWindow(width, height, title);
            running = true;
        }

        if (running)
        {
            while (!WindowShouldClose())
            {
                for (auto &task : update_tasks)
                    task();

                BeginDrawing();
                ClearBackground(RAYWHITE);

                for (auto &task : draw_tasks)
                    task();

                EndDrawing();
            }

            running = false;
            CloseWindow();
        }
    }
};

inline CarbGUI gui = CarbGUI();