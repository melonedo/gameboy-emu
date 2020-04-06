
#include <cstdio>
#include <SDL.h>
#include "threads.h"
#include "../util/byte-type.h"
#include "../video/video.h"

namespace gameboy
{
  pthread_t window_thread;

  SDL_Window *pWindow;
  SDL_Renderer *pRenderer;

  bool init_window();

  void close_window();

  void refresh_screen();

  void *window_main(void *param)
  {
    begin_init();
    bool success = init_window();
    end_init(success);
    if (!success)
      return NULL;

    while (!program_ended)
    {
      refresh_screen();
      SDL_Event e;
      while (SDL_PollEvent(&e) != 0)
      {
        if (e.type == SDL_QUIT)
        {
          program_ended = true;
          break;
        }
        else if (e.type == SDL_KEYDOWN)
        {
          switch (e.key.keysym.sym)
          {
            case SDLK_UP:
            printf("up\n");
            break;

            case SDLK_DOWN:
            printf("down\n");
            break;

            case SDLK_LEFT:
            printf("left\n");
            break;

            case SDLK_RIGHT:
            printf("right\n");
            break;

          }
        }
      }
      SDL_Delay(15);
    }
    close_window();
    printf("Press enter to terminate console.\n");
    return NULL;
  }

  bool init_window()
  {
    //Initialize SDL
  	if(SDL_Init(SDL_INIT_VIDEO) < 0)
  	{
  		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
  		return false;
  	}

    //Set texture filtering to linear
    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    {
      printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    pWindow = SDL_CreateWindow(
      "Gameboy-emu", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      window_width, window_height, SDL_WINDOW_SHOWN);
    if(pWindow == NULL)
    {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError() );
      return false;
    }

    //Create renderer for window
    pRenderer = SDL_CreateRenderer(pWindow, -1,
      SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(pRenderer == NULL)
    {
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
      return false;
    }

    // Set the logical size to that of a gameboy screen
    if (SDL_RenderSetLogicalSize(pRenderer, screen_column_num, screen_row_num))
    {
      printf("Unable to set renderer logical size! SDL Error: %s\n", SDL_GetError());
      return false;
    }

    return true;
  }

  void close_window()
  {
  	//Destroy window
  	SDL_DestroyRenderer(pRenderer);
  	SDL_DestroyWindow(pWindow);
  	pWindow = NULL;
  	pRenderer = NULL;

  	//Quit SDL subsystems
  	SDL_Quit();
  }

  struct renderer_color_t
  {
    Uint8 r, g, b;
  } colors[4] = {
    // These four colors come from bgb
    {224, 248, 208},
    {136, 192, 112},
    { 52, 104,  86},
    {  8,  24,  32}
  };

  void refresh_screen()
  {
    for (int row = 0; row < screen_row_num; row++)
    {
      for (int col = 0; col < screen_column_num; col++)
      {
        color_t c = screen_buf[row][col + 8];
        SDL_SetRenderDrawColor(pRenderer,
          colors[c].r, colors[c].g, colors[c].b, 0xff);
        SDL_RenderDrawPoint(pRenderer, col, row);
      }
    }
    SDL_RenderPresent(pRenderer);
  }
};
