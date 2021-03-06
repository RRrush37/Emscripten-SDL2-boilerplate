#ifndef APP_HPP_
#define APP_HPP_
#pragma once

#include "basic_headers.hpp"
#include "./themes/front_page.hpp"
namespace game
{

class app
{
    bool should_continue {true};
    SDL_Window_ptr   window{nullptr, &SDL_DestroyWindow};
    SDL_Renderer_ptr renderer{nullptr, &SDL_DestroyRenderer};
    std::unique_ptr<fps>   game_fps{std::make_unique<fps>()};
    std::unique_ptr<theme> thm;

public:
    app()
    {
        if (error_code ec = SDL_Init(SDL_INIT_VIDEO); ec < 0)
            std::cout << "SDL_Init: " << SDL_GetError() << "\n";

        if (TTF_Init() == -1)
            std::cout << "TTF_Init: " << TTF_GetError() << "\n";

        {
            int flags   = IMG_INIT_PNG;
            int initted = IMG_Init(flags);
            if ((initted & flags) != flags)
                std::cout << "IMG_Init: " << IMG_GetError() << "\n";
        }

        SDL_Window   * wind = nullptr;
        SDL_Renderer * rend = nullptr;
        if (error_code ec = SDL_CreateWindowAndRenderer(WINDOW_WIDTH_PIXEL, WINDOW_HEIGHT_PIXEL,
                                                        0,
                                                        &wind, &rend); ec < 0)
            std::cout << SDL_GetError() << std::endl;
        window.reset(wind);
        renderer.reset(rend);

        if (error_code ec = SDL_SetRenderDrawColor(renderer.get(), 12, 199, 166, 255); ec < 0)
            std::cout << SDL_GetError() << std::endl;

        thm = std::make_unique<theme_types::front_page>(renderer.get(), &game_fps);
    }

    ~app() { TTF_Quit(); IMG_Quit(); SDL_Quit(); }

    void run()
    {
        if (not should_continue)
        {
#ifdef __EMSCRIPTEN__
            emscripten_cancel_main_loop();
#else
            std::exit(0);
#endif // __EMSCRIPTEN__
            return;
        }

        SDL_Event event;
        while (SDL_PollEvent (&event))
            handle_event(event);

        calculate();
        render();
        if (thm->is_finished())
            thm = thm->next();
    }

public:
    void handle_event(SDL_Event& event) { check_quit(event); thm->on_event(event); }

    void calculate()
    {
        game_fps->calculate();
        thm->calculate();
    }

    void render()
    {
        SDL_RenderClear(renderer.get());
        thm->render();
        SDL_RenderPresent(renderer.get());
    }

private:
    inline void check_quit(SDL_Event const & e)
    {
        if (e.type == SDL_QUIT)
            should_continue = false;
    }
};

} // namespace game
#endif // APP_HPP_
