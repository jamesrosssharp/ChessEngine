/* vim: set et ts=4 sw=4: */

/*

ChessEngine : A chess engine written in C++ for SDL2

main.cpp: Entry point for chess engine

License: MIT License

Copyright 2023 J.R.Sharp

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>

#include <cstdint>

#include <unistd.h>

#define WINDOW_WIDTH 	854
#define WINDOW_HEIGHT 	854

#include "Renderer.h"
#include "UI.h"

int main(int argc, char** argv)
{

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0)
    {
        printf("Could not init SDL!\n");
        exit(EXIT_FAILURE);
    }

    std::uint32_t window_flags = SDL_WINDOW_OPENGL;
    SDL_Window *window = SDL_CreateWindow("ChessEngine", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
  
    SDL_GLContext Context = SDL_GL_CreateContext(window);

    (void)Context;

    Renderer r;
    UI       u(&r);

    r.resetBoard();

    bool running = true;
    bool full_screen = false;

    while (running)
    {
        SDL_Event Event;
        while (SDL_PollEvent(&Event))
        {
            if (Event.type == SDL_KEYDOWN)
            {
                switch (Event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        running = 0;
                        break;
                    case 'f':
                        full_screen = !full_screen;
                        if (full_screen)
                        {
                            SDL_SetWindowFullscreen(window, window_flags | SDL_WINDOW_FULLSCREEN_DESKTOP);
                        }
                        else
                        {
                            SDL_SetWindowFullscreen(window, window_flags);
                        }
                        break;
                    case 'a':
                        u.handleAKeyDown();
                        break;
                    case 'b':
                        u.handleBKeyDown();
                        break;
                    case SDLK_UP:
                        u.handleUpKeyDown();
                        break;
                    case SDLK_DOWN:
                        u.handleDownKeyDown();
                        break;
                    case SDLK_LEFT:
                        u.handleLeftKeyDown();
                        break;
                    case SDLK_RIGHT:
                        u.handleRightKeyDown();
                        break;
                    default:
                        break;
                }
            }
            else if (Event.type == SDL_KEYUP)
            {
                switch (Event.key.keysym.sym)
                {
                    default:
                        break;
                }
            }
            else if (Event.type == SDL_QUIT)
            {
                running = false;
            }
        }
    
        int w, h;

        SDL_GetWindowSize(window, &w, &h);

        r.renderScene(w, h);

        SDL_GL_SwapWindow(window);

        usleep(16000);

    }

    printf("Bye!\n");
    exit(EXIT_SUCCESS);
}
